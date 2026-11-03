package com.oldmountain.dzm.admin;

import java.io.*;
import java.net.InetSocketAddress;
import java.nio.charset.StandardCharsets;
import java.security.MessageDigest;
import java.security.SecureRandom;
import java.util.*;
import java.util.concurrent.*;
import com.sun.net.httpserver.*;
import net.sf.json.JSONArray;
import net.sf.json.JSONObject;
import org.hibernate.Session;
import org.hibernate.Transaction;
import org.hibernate.SQLQuery;
import org.hibernate.Hibernate;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import com.oldmountain.dzm.DZM;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.message.OP;
import com.oldmountain.dzm.net.ObjectAccessor;
import com.oldmountain.dzm.services.RewardServices;
import com.oldmountain.dzm.util.Util;

/**
 * @brief 仅监听回环地址的管理 API，负责鉴权、预览、批量传书与持久化批次审计。
 * 发奖与玩家缓存刷新均排入原游戏主循环，批次记录和传书使用同一个数据库事务。
 */
public final class AdminServer implements HttpHandler {
    private static final Logger LOG = LoggerFactory.getLogger(AdminServer.class);
    private static final SecureRandom RANDOM = new SecureRandom();
    private final String instance;
    private final int port;
    private final Properties auth = new Properties();
    private final ConcurrentMap<String, Long> sessions = new ConcurrentHashMap<String, Long>();
    private final ConcurrentMap<String, Preview> previews = new ConcurrentHashMap<String, Preview>();
    private AdminCatalog catalog;
    private int failedLogins;
    private long blockedUntil;

    /** @brief 保存已确认的角色快照和附件，防止发放时重新选择目标。 */
    private static final class Preview {
        String owner;
        String target;
        String message;
        JSONArray items;
        List<Integer> ids;
        long expires;
    }

    /**
     * 读取当前部署的管理端口和实例标识。
     * @param config 部署生成的配置
     * @throws Exception 认证资源缺失
     */
    public AdminServer(Properties config) throws Exception {
        instance = config.getProperty("instanceId");
        port = Integer.parseInt(config.getProperty("port"));
        if (instance == null || !instance.matches("[a-f0-9]{32}")) throw new IllegalArgumentException("Invalid instance ID");
        try (InputStream in = AdminServer.class.getResourceAsStream("/admin-auth.properties")) {
            if (in == null) throw new IllegalStateException("Missing administrator authentication resource");
            auth.load(in);
        }
    }

    /**
     * 在游戏主循环创建独立审计表，不修改已有玩家表或重导初始数据库。
     * @throws Exception 模板读取或数据库初始化失败
     */
    public void initialize() throws Exception {
        catalog = new AdminCatalog();
        AdminRewards.AdminServerCatalogHolder.set(catalog);
        Session db = DZM.getEntityManager().getSessionFactory();
        Transaction tx = db.beginTransaction();
        try {
            db.createSQLQuery("CREATE TABLE IF NOT EXISTS nrb_admin_batches (batchId CHAR(32) PRIMARY KEY, "
                    + "details MEDIUMTEXT NOT NULL, createdAt INT NOT NULL) ENGINE=InnoDB DEFAULT CHARSET=utf8").executeUpdate();
            tx.commit();
        } catch (RuntimeException e) {
            tx.rollback();
            throw e;
        }
        LOG.info("initialize: administration catalog loaded, items={}", catalog.size());
        AccountService.initialize();
        PermanentActivities.initialize(catalog);
    }

    /** 开放本机接口；生命周期随游戏 JVM 结束。 @throws IOException 端口无法监听 */
    public void start() throws IOException {
        HttpServer server = HttpServer.create(new InetSocketAddress("127.0.0.1", port), 16);
        server.createContext("/admin/v1/", this);
        server.createContext("/account/v1/", new AccountHandler());
        server.setExecutor(new ThreadPoolExecutor(2, 4, 30, TimeUnit.SECONDS,
                new ArrayBlockingQueue<Runnable>(24), new ThreadPoolExecutor.AbortPolicy()));
        server.start();
    }

    /**
     * 处理受长度、会话和业务参数约束的管理请求。
     * @param exchange 本机 HTTP 请求
     * @throws IOException 客户端断开或响应写入失败
     */
    public void handle(final HttpExchange exchange) throws IOException {
        int status = 200;
        JSONObject response;
        try {
            if (!exchange.getRemoteAddress().getAddress().isLoopbackAddress()) throw new SecurityException("仅允许本机访问");
            final String route = exchange.getRequestURI().getPath().substring("/admin/v1/".length());
            if ("health".equals(route) && "GET".equals(exchange.getRequestMethod())) {
                response = (JSONObject) AdminTask.call(new Callable<Object>() {
                    public Object call() { pingDatabase(); return health(); }
                }, 5);
            } else {
                if (!"POST".equals(exchange.getRequestMethod())) throw new IllegalArgumentException("仅接受 JSON POST 请求");
                String contentType = exchange.getRequestHeaders().getFirst("Content-Type");
                if (contentType == null || !contentType.toLowerCase(Locale.ROOT).startsWith("application/json")) {
                    throw new IllegalArgumentException("请求必须使用 application/json");
                }
                final JSONObject request = readJson(exchange.getRequestBody());
                if ("login".equals(route)) {
                    response = login(request);
                } else {
                    final String token = requireSession(exchange);
                    if ("logout".equals(route)) {
                        sessions.remove(token);
                        response = ok();
                    } else {
                        response = (JSONObject) AdminTask.call(new Callable<Object>() {
                            public Object call() throws Exception { return dispatch(route, request, token); }
                        }, 120);
                    }
                }
            }
        } catch (Exception e) {
            Throwable cause = e instanceof ExecutionException ? e.getCause() : e;
            status = cause instanceof SecurityException ? 401 : cause instanceof IllegalArgumentException ? 400 : 503;
            response = new JSONObject();
            response.put("ok", false);
            response.put("message", status == 503 ? "服务未就绪或请求结果未确认；发奖后请按原批次号查询，勿另建批次重复发送" : cause.getMessage());
            if (status == 503) LOG.error("handle: administration request failed", cause);
            else LOG.warn("handle: administration request rejected, status={}", status);
        }
        byte[] bytes = response.toString().getBytes(StandardCharsets.UTF_8);
        exchange.getResponseHeaders().set("Content-Type", "application/json; charset=utf-8");
        exchange.getResponseHeaders().set("Cache-Control", "no-store");
        exchange.sendResponseHeaders(status, bytes.length);
        try (OutputStream output = exchange.getResponseBody()) { output.write(bytes); }
        finally { exchange.close(); }
    }

    private JSONObject health() {
        JSONObject value = ok();
        value.put("instanceId", instance);
        value.put("protocol", 1);
        value.put("accountAuth", 1);
        value.put("catalogCount", catalog.size());
        return value;
    }

    private void pingDatabase() {
        Session db = DZM.getEntityManager().getSessionFactory();
        Transaction tx = db.beginTransaction();
        try { db.createSQLQuery("SELECT 1").uniqueResult(); tx.commit(); }
        catch (RuntimeException e) { tx.rollback(); throw e; }
    }

    private synchronized JSONObject login(JSONObject request) throws Exception {
        long now = System.currentTimeMillis();
        if (now < blockedUntil) throw new SecurityException("登录失败次数过多，请稍后重试");
        byte[] inputHash = MessageDigest.getInstance("SHA-256").digest(request.optString("password").getBytes(StandardCharsets.UTF_8));
        if (!auth.getProperty("username").equals(request.optString("username"))
                || !MessageDigest.isEqual(hex(inputHash).getBytes(StandardCharsets.US_ASCII),
                        auth.getProperty("passwordSha256").getBytes(StandardCharsets.US_ASCII))) {
            if (++failedLogins >= 5) { blockedUntil = now + 60000; failedLogins = 0; }
            throw new SecurityException("账号或密码错误");
        }
        AdminTask.call(new Callable<Object>() { public Object call() { pingDatabase(); return null; } }, 5);
        failedLogins = 0;
        for (Map.Entry<String, Long> item : sessions.entrySet()) if (item.getValue() < now) sessions.remove(item.getKey());
        if (sessions.size() >= 16) throw new IllegalArgumentException("管理会话过多，请等待旧会话过期");
        String token = randomId() + randomId();
        sessions.put(token, now + 30 * 60 * 1000);
        JSONObject result = health();
        result.put("token", token);
        LOG.info("login: administrator session created");
        return result;
    }

    private String requireSession(HttpExchange exchange) {
        String header = exchange.getRequestHeaders().getFirst("Authorization");
        String token = header != null && header.startsWith("Bearer ") ? header.substring(7) : "";
        Long expires = sessions.get(token);
        if (expires == null || expires < System.currentTimeMillis()) {
            sessions.remove(token);
            throw new SecurityException("会话已失效，请重新登录");
        }
        return token;
    }

    private JSONObject dispatch(String route, JSONObject request, String owner) throws Exception {
        LOG.debug("dispatch: route={}", route);
        if ("catalog".equals(route)) {
            JSONObject result = ok(); result.put("items", catalog.list()); return result;
        }
        if ("players".equals(route)) return players(request.optString("name"));
        if ("preview".equals(route)) return preview(request, owner);
        if ("send".equals(route)) return send(request, owner);
        if ("batch".equals(route)) return batch(request.getString("batchId"));
        if ("cancel".equals(route)) return cancel(request.getString("batchId"));
        if ("history".equals(route)) return history();
        throw new IllegalArgumentException("未知管理接口");
    }

    private JSONObject players(String name) {
        if (name.length() > 64) throw new IllegalArgumentException("游戏名过长");
        Session db = DZM.getEntityManager().getSessionFactory();
        Transaction tx = db.beginTransaction();
        JSONArray found = new JSONArray();
        try {
            SQLQuery query = db.createSQLQuery("SELECT id,name,level FROM player WHERE name LIKE :name ORDER BY id LIMIT 100");
            // 使用参数绑定；结果只作候选展示，发奖时仍按完整游戏名精确匹配。
            query.setString("name", "%" + name + "%");
            for (Object row : query.list()) {
                Object[] values = (Object[]) row;
                JSONObject p = new JSONObject();
                p.put("id", values[0]); p.put("name", values[1]); p.put("level", values[2]); found.add(p);
            }
            tx.commit();
        } catch (RuntimeException e) { tx.rollback(); throw e; }
        JSONObject result = ok(); result.put("players", found); return result;
    }

    private JSONObject preview(JSONObject request, String owner) {
        final long now = System.currentTimeMillis();
        for (Map.Entry<String, Preview> p : previews.entrySet()) if (p.getValue().expires < now) previews.remove(p.getKey());
        if (previews.size() >= 100) throw new IllegalArgumentException("预览过多，请稍后重试");
        Preview p = new Preview();
        p.owner = owner; p.expires = now + 10 * 60 * 1000;
        String title = request.optString("title").trim();
        String message = request.optString("message").trim();
        if (title.length() == 0 || title.length() > 40 || message.length() > 140) throw new IllegalArgumentException("标题需 1–40 字，说明最多 140 字");
        p.message = title + (message.length() == 0 ? "" : "：" + message);
        // 数据库使用 utf8，拒绝不能存入旧三字节字符集的代理字符。
        for (int i = 0; i < p.message.length(); i++) if (Character.isSurrogate(p.message.charAt(i))) throw new IllegalArgumentException("标题或说明不支持表情字符");
        JSONArray requested = request.getJSONArray("items");
        if (requested.size() < 1 || requested.size() > 20) throw new IllegalArgumentException("每批请选择 1–20 种物品");
        p.items = new JSONArray();
        Set<String> keys = new HashSet<String>();
        int instances = 0;
        for (int i = 0; i < requested.size(); i++) {
            JSONObject input = requested.getJSONObject(i);
            String key = input.getString("key");
            if (!keys.add(key)) throw new IllegalArgumentException("同一物品请合并数量后发送");
            JSONObject item = catalog.attachment(key, input.getInt("quantity"));
            int type = item.getInt("t");
            if (type == 31000 || type == 41000 || type == 51000) instances += item.getInt("q");
            p.items.add(item);
        }
        if (instances > 100) throw new IllegalArgumentException("每名角色每批最多创建 100 件卡牌、装备或奥义");
        boolean all = request.optBoolean("all", false);
        String name = request.optString("name").trim();
        if (!all && (name.length() == 0 || name.length() > 64)) throw new IllegalArgumentException("请输入完整游戏名");
        p.ids = new ArrayList<Integer>();
        Session db = DZM.getEntityManager().getSessionFactory();
        Transaction tx = db.beginTransaction();
        try {
            SQLQuery q = db.createSQLQuery(all ? "SELECT id FROM player ORDER BY id" : "SELECT id FROM player WHERE name=:name ORDER BY id");
            if (!all) q.setString("name", name);
            for (Object id : q.list()) p.ids.add(((Number) id).intValue());
            tx.commit();
        } catch (RuntimeException e) { tx.rollback(); throw e; }
        if (p.ids.isEmpty()) throw new IllegalArgumentException("未找到游戏角色；尚未创建角色的平台账号不能领取物品");
        if (!all && p.ids.size() != 1) throw new IllegalArgumentException("游戏名存在重名，已拒绝发送，请先解决重名");
        p.target = all ? "全服现有角色" : name;
        String ticket = randomId();
        previews.put(ticket, p);
        JSONObject result = ok();
        result.put("ticket", ticket); result.put("players", p.ids.size());
        result.put("target", p.target); result.put("items", p.items);
        result.put("mailsPerPlayer", mailContents(p.items).size());
        return result;
    }

    private List<String> mailContents(JSONArray items) {
        List<String> groups = new ArrayList<String>();
        JSONArray part = new JSONArray();
        for (int i = 0; i < items.size(); i++) {
            JSONArray candidate = JSONArray.fromObject(part.toString()); candidate.add(items.get(i));
            // 原 rewards.rewardContent 只有 VARCHAR(255)，按长度拆信，避免截断附件。
            if (candidate.toString().length() > 250) { groups.add(part.toString()); part = new JSONArray(); }
            part.add(items.get(i));
        }
        if (!part.isEmpty()) groups.add(part.toString());
        return groups;
    }

    private JSONObject send(JSONObject request, String owner) {
        String batchId = checkedBatchId(request.getString("batchId"));
        JSONObject existing = findBatch(batchId);
        if (existing != null) return existing;
        Preview p = previews.get(request.getString("ticket"));
        if (p == null || !p.owner.equals(owner) || p.expires < System.currentTimeMillis()) throw new IllegalArgumentException("预览已过期，请重新预览并使用原批次号重试");
        List<String> contents = mailContents(p.items);
        JSONObject receipt = ok();
        receipt.put("found", true); receipt.put("batchId", batchId); receipt.put("target", p.target);
        receipt.put("players", p.ids.size()); receipt.put("mails", p.ids.size() * contents.size());
        receipt.put("items", p.items); receipt.put("message", p.message); receipt.put("time", Util.getServerTime());
        Session db = DZM.getEntityManager().getSessionFactory();
        Transaction tx = db.beginTransaction();
        try {
            db.createSQLQuery("INSERT INTO nrb_admin_batches(batchId,details,createdAt) VALUES(:id,:details,:time)")
                    .setString("id", batchId).setString("details", receipt.toString()).setInteger("time", Util.getServerTime()).executeUpdate();
            JSONObject info = new JSONObject(); info.put("nrb", 1); info.put("b", batchId);
            int inserted = 0;
            for (int offset = 0; offset < p.ids.size(); offset += 500) {
                StringBuilder ids = new StringBuilder();
                for (int i = offset; i < Math.min(offset + 500, p.ids.size()); i++) {
                    if (ids.length() > 0) ids.append(','); ids.append(p.ids.get(i));
                }
                for (String content : contents) {
                    SQLQuery q = db.createSQLQuery("INSERT INTO rewards(playerId,rewardType,rewardMsg,rewardInfo,rewardContent,status,addTime,rewardTime) "
                            + "SELECT id,7,:msg,:info,:content,0,:time,:time FROM player WHERE id IN (" + ids + ")");
                    q.setString("msg", p.message); q.setString("info", info.toString());
                    q.setString("content", content); q.setInteger("time", Util.getServerTime());
                    inserted += q.executeUpdate();
                }
            }
            if (inserted != receipt.getInt("mails")) throw new IllegalStateException("Target snapshot changed during dispatch");
            tx.commit();
        } catch (RuntimeException e) { tx.rollback(); throw e; }
        LOG.info("send: batch committed, batchId={}, players={}, mails={}", new Object[]{batchId, p.ids.size(), receipt.getInt("mails")});
        // 提交后刷新已加载玩家。推送失败不回滚已落库邮件，也不把成功批次报成失败。
        for (int id : p.ids) {
            Player player = ObjectAccessor.getOnlineUser(id);
            if (player == null) continue;
            player.setNeedSyncRewards(true);
            try {
                if (player.getSession() != null && player.getSession().isConnected()) {
                    player.sendPacket(OP.code.OPCODE_GET_USER_SYSMSG_INFORET_S_VALUE,
                            RewardServices.getSysReward(player).setServertime(Util.getServerTime()));
                }
            } catch (Exception e) { LOG.error("send: mailbox refresh failed, playerId=" + id, e); }
        }
        return receipt;
    }

    private JSONObject batch(String id) {
        JSONObject found = findBatch(checkedBatchId(id));
        if (found != null) return found;
        JSONObject result = ok(); result.put("found", false); return result;
    }

    private JSONObject cancel(String requestedId) {
        String id = checkedBatchId(requestedId);
        JSONObject found = findBatch(id);
        if (found != null) return found;
        JSONObject canceled = ok();
        canceled.put("found", true); canceled.put("canceled", true); canceled.put("batchId", id);
        canceled.put("players", 0); canceled.put("mails", 0); canceled.put("target", "取消未完成批次");
        canceled.put("time", Util.getServerTime());
        Session db = DZM.getEntityManager().getSessionFactory();
        Transaction tx = db.beginTransaction();
        try {
            // 取消与发奖由同一主循环串行处理，落库的取消记录阻止迟到请求再次发放。
            db.createSQLQuery("INSERT INTO nrb_admin_batches(batchId,details,createdAt) VALUES(:id,:details,:time)")
                    .setString("id", id).setString("details", canceled.toString()).setInteger("time", Util.getServerTime()).executeUpdate();
            tx.commit();
        } catch (RuntimeException e) { tx.rollback(); throw e; }
        LOG.info("cancel: pending batch canceled, batchId={}", id);
        return canceled;
    }

    private JSONObject findBatch(String id) {
        Session db = DZM.getEntityManager().getSessionFactory();
        Transaction tx = db.beginTransaction();
        try {
            // MariaDB 把 MEDIUMTEXT 报告为 LONGVARCHAR(-1)，旧 Hibernate 必须显式指定字符串映射。
            Object details = db.createSQLQuery("SELECT details FROM nrb_admin_batches WHERE batchId=:id")
                    .addScalar("details", Hibernate.STRING).setString("id", id).uniqueResult();
            tx.commit();
            return details == null ? null : JSONObject.fromObject(details.toString());
        } catch (RuntimeException e) { tx.rollback(); throw e; }
    }

    private JSONObject history() {
        JSONArray rows = new JSONArray();
        Session db = DZM.getEntityManager().getSessionFactory();
        Transaction tx = db.beginTransaction();
        try {
            for (Object details : db.createSQLQuery("SELECT details FROM nrb_admin_batches ORDER BY createdAt DESC,batchId DESC LIMIT 50")
                    .addScalar("details", Hibernate.STRING).list()) {
                rows.add(JSONObject.fromObject(details.toString()));
            }
            tx.commit();
        } catch (RuntimeException e) { tx.rollback(); throw e; }
        JSONObject result = ok(); result.put("batches", rows); return result;
    }

    private static String checkedBatchId(String id) {
        if (!id.matches("[a-f0-9]{32}")) throw new IllegalArgumentException("无效批次号");
        return id;
    }

    private static JSONObject readJson(InputStream input) throws IOException {
        ByteArrayOutputStream output = new ByteArrayOutputStream();
        byte[] buffer = new byte[4096]; int count;
        while ((count = input.read(buffer)) != -1) {
            if (output.size() + count > 65536) throw new IllegalArgumentException("请求过大");
            output.write(buffer, 0, count);
        }
        return JSONObject.fromObject(new String(output.toByteArray(), StandardCharsets.UTF_8));
    }

    private static JSONObject ok() { JSONObject result = new JSONObject(); result.put("ok", true); return result; }
    private static String randomId() { byte[] data = new byte[16]; RANDOM.nextBytes(data); return hex(data); }
    private static String hex(byte[] bytes) {
        StringBuilder out = new StringBuilder();
        for (byte b : bytes) out.append(String.format("%02x", b & 255));
        return out.toString();
    }
}

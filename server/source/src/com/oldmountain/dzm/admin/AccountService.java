package com.oldmountain.dzm.admin;

import java.nio.charset.StandardCharsets;
import java.security.MessageDigest;
import java.security.SecureRandom;
import java.util.LinkedHashMap;
import java.util.Locale;
import java.util.Map;
import javax.crypto.SecretKeyFactory;
import javax.crypto.spec.PBEKeySpec;
import net.sf.json.JSONObject;
import org.hibernate.Session;
import org.hibernate.Transaction;
import org.hibernate.Hibernate;
import org.hibernate.exception.ConstraintViolationException;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import com.oldmountain.dzm.DZM;

/**
 * @brief 三端共用的账号、密码摘要与持久化登录会话。
 * 使用独立账号表和稳定 UID 连接原有 mapping/角色存档；不接受设备游客身份。
 */
public final class AccountService {
    private static final Logger LOG = LoggerFactory.getLogger(AccountService.class);
    private static final SecureRandom RANDOM = new SecureRandom();
    /** 登录凭证有效期，单位：毫秒；恢复登录时续期。 */
    private static final long LIFETIME = 30L * 86400 * 1000;
    private static final int ITERATIONS = 120000;
    private static volatile boolean initialized;
    private static final Map<String, long[]> ATTEMPTS = new LinkedHashMap<String, long[]>();
    /** 串行创建/加载角色，避免同账号多端首次登录生成两个角色。 */
    public static final Object GAME_LOGIN_LOCK = new Object();

    private AccountService() { }

    /** 创建账号表和会话表；失败时保持登录关闭。 */
    public static void initialize() {
        Session db = DZM.getEntityManager().getSessionFactory();
        Transaction tx = db.beginTransaction();
        try {
            db.createSQLQuery("CREATE TABLE IF NOT EXISTS nrb_accounts ("
                    + "uid VARCHAR(50) CHARACTER SET ascii COLLATE ascii_bin PRIMARY KEY, "
                    + "username VARCHAR(32) CHARACTER SET ascii COLLATE ascii_bin NOT NULL UNIQUE, "
                    + "salt CHAR(32) NOT NULL, passwordHash CHAR(64) NOT NULL, iterations INT NOT NULL, "
                    + "createdAt BIGINT NOT NULL) ENGINE=InnoDB DEFAULT CHARSET=utf8").executeUpdate();
            db.createSQLQuery("CREATE TABLE IF NOT EXISTS nrb_account_sessions ("
                    + "tokenHash CHAR(64) CHARACTER SET ascii COLLATE ascii_bin PRIMARY KEY, "
                    + "uid VARCHAR(50) CHARACTER SET ascii COLLATE ascii_bin NOT NULL, "
                    + "expiresAt BIGINT NOT NULL, createdAt BIGINT NOT NULL, INDEX(uid), INDEX(expiresAt), "
                    + "FOREIGN KEY(uid) REFERENCES nrb_accounts(uid)) ENGINE=InnoDB DEFAULT CHARSET=utf8").executeUpdate();
            tx.commit();
            initialized = true;
            LOG.info("initialize: managed game accounts ready; guest login disabled");
        } catch (RuntimeException error) {
            tx.rollback();
            LOG.error("initialize: account tables unavailable", error);
            throw error;
        }
    }

    /**
     * 校验游戏包中的 UID 和凭证；数据库异常时拒绝登录。
     * @param uid 服务端分配的账号身份
     * @param token 登录凭证，禁止写入日志或玩家密码字段
     * @return 是否可加载该账号存档
     */
    public static boolean authenticate(String uid, String token) {
        if (!initialized || uid == null || !uid.matches("acct_[a-f0-9]{32}") || !validToken(token)) return false;
        try {
            Object[] row = session(token, false);
            return row != null && uid.equals(row[0]);
        } catch (RuntimeException error) {
            LOG.error("authenticate: account database unavailable", error);
            return false;
        }
    }

    /**
     * 处理受控的账号动作；密码计算在 HTTP 工作线程执行。
     * @param action register/login/resume/logout
     * @param request 有界 JSON 请求
     * @return 账号信息与凭证；不包含密码摘要
     * @throws Exception 账号、密码或数据库错误
     */
    public static JSONObject dispatch(String action, JSONObject request) throws Exception {
        if (!initialized) throw new IllegalStateException("账号服务尚未就绪，请先启动服务端");
        if ("resume".equals(action)) {
            String token = request.optString("token");
            Object[] row = session(token, true);
            if (row == null) throw new SecurityException("登录已过期，请重新输入账号密码");
            return result((String) row[0], (String) row[1], token);
        }
        if ("logout".equals(action)) {
            revoke(request.optString("token"));
            JSONObject result = new JSONObject(); result.put("ok", true); return result;
        }
        if (!"login".equals(action) && !"register".equals(action)) throw new IllegalArgumentException("未知账号接口");
        String username = username(request.optString("username"));
        String password = request.optString("password");
        if (password.length() < 6 || password.length() > 64) throw new IllegalArgumentException("密码长度须为 6–64 个字符");
        rateLimit(username);
        String uid;
        if ("register".equals(action)) {
            if (!password.equals(request.optString("confirmPassword"))) throw new IllegalArgumentException("两次输入的密码不一致");
            uid = register(username, password);
            LOG.info("dispatch: game account registered");
        } else {
            Object[] row = account(username);
            // 不存在的账号也计算摘要，避免直接暴露账号存在性与明显的耗时差异。
            String salt = row == null ? "00000000000000000000000000000000" : (String) row[1];
            int rounds = row == null ? ITERATIONS : ((Number) row[3]).intValue();
            byte[] computed = passwordHash(password, salt, rounds);
            if (row == null || !MessageDigest.isEqual(computed, unhex((String) row[2]))) {
                throw new SecurityException("账号或密码错误");
            }
            uid = (String) row[0];
            LOG.info("dispatch: game account password accepted");
        }
        return issue(uid, username);
    }

    private static String username(String value) {
        if (value == null || !value.matches("[A-Za-z0-9]{3,32}")) {
            throw new IllegalArgumentException("账号须为 3–32 位英文字母或数字，不区分大小写");
        }
        return value.toLowerCase(Locale.ROOT);
    }

    private static synchronized void rateLimit(String username) {
        long now = System.currentTimeMillis();
        String[] keys = {"all", "user:" + username};
        // 固定容量，避免公开注册接口用随机用户名耗尽服务器内存。
        java.util.Iterator<Map.Entry<String, long[]>> iterator = ATTEMPTS.entrySet().iterator();
        while (iterator.hasNext()) if (iterator.next().getValue()[0] <= now) iterator.remove();
        if (ATTEMPTS.size() >= 2048) throw new IllegalArgumentException("账号请求过多，请稍后重试");
        for (String key : keys) {
            long[] counter = ATTEMPTS.get(key);
            if (counter == null) { counter = new long[] {now + 60000, 0}; ATTEMPTS.put(key, counter); }
            if (++counter[1] > ("all".equals(key) ? 120 : 10)) {
                throw new IllegalArgumentException("登录或注册过于频繁，请一分钟后重试");
            }
        }
    }

    private static Object[] account(String username) {
        Session db = DZM.getEntityManager().getSessionFactory();
        Transaction tx = db.beginTransaction();
        try {
            // 旧 Hibernate 将未显式指定类型的 CHAR(32/64) 当成 Character，
            // 必须按完整字符串读取盐和摘要，避免已注册账号密码登录报错。
            Object[] row = (Object[]) db.createSQLQuery("SELECT uid,salt,passwordHash,iterations FROM nrb_accounts WHERE username=:name")
                    .addScalar("uid", Hibernate.STRING).addScalar("salt", Hibernate.STRING)
                    .addScalar("passwordHash", Hibernate.STRING).addScalar("iterations", Hibernate.INTEGER)
                    .setString("name", username).uniqueResult();
            tx.commit(); return row;
        } catch (RuntimeException error) { tx.rollback(); throw error; }
    }

    private static String register(String username, String password) throws Exception {
        String uid = "acct_" + random(16), salt = random(16);
        String hash = hex(passwordHash(password, salt, ITERATIONS));
        Session db = DZM.getEntityManager().getSessionFactory();
        Transaction tx = db.beginTransaction();
        try {
            db.createSQLQuery("INSERT INTO nrb_accounts(uid,username,salt,passwordHash,iterations,createdAt) "
                    + "VALUES(:uid,:name,:salt,:hash,:rounds,:now)")
                    .setString("uid", uid).setString("name", username).setString("salt", salt).setString("hash", hash)
                    .setInteger("rounds", ITERATIONS).setLong("now", System.currentTimeMillis()).executeUpdate();
            tx.commit(); return uid;
        } catch (ConstraintViolationException error) {
            tx.rollback(); throw new IllegalArgumentException("该账号已注册，请直接登录或换一个账号");
        } catch (RuntimeException error) { tx.rollback(); throw error; }
    }

    private static JSONObject issue(String uid, String username) throws Exception {
        String token = "nrb1_" + random(32);
        long now = System.currentTimeMillis();
        Session db = DZM.getEntityManager().getSessionFactory();
        Transaction tx = db.beginTransaction();
        try {
            db.createSQLQuery("DELETE FROM nrb_account_sessions WHERE expiresAt < :now").setLong("now", now).executeUpdate();
            db.createSQLQuery("INSERT INTO nrb_account_sessions(tokenHash,uid,expiresAt,createdAt) VALUES(:hash,:uid,:expires,:now)")
                    .setString("hash", digest(token)).setString("uid", uid).setLong("expires", now + LIFETIME)
                    .setLong("now", now).executeUpdate();
            // 最多保留 8 个设备会话；不会使另一端重复登录创建新角色。
            db.createSQLQuery("DELETE FROM nrb_account_sessions WHERE uid=:uid AND tokenHash<>:current AND tokenHash NOT IN "
                    + "(SELECT tokenHash FROM (SELECT tokenHash FROM nrb_account_sessions WHERE uid=:uid AND tokenHash<>:current "
                    + "ORDER BY createdAt DESC,tokenHash LIMIT 7) AS recent)")
                    .setString("uid", uid).setString("current", digest(token)).executeUpdate();
            tx.commit(); return result(uid, username, token);
        } catch (RuntimeException error) { tx.rollback(); throw error; }
    }

    private static Object[] session(String token, boolean extend) {
        if (!validToken(token)) return null;
        Session db = DZM.getEntityManager().getSessionFactory();
        Transaction tx = db.beginTransaction();
        try {
            long now = System.currentTimeMillis();
            Object[] row = (Object[]) db.createSQLQuery("SELECT a.uid,a.username FROM nrb_account_sessions s "
                    + "JOIN nrb_accounts a ON a.uid=s.uid WHERE s.tokenHash=:hash AND s.expiresAt>:now")
                    .addScalar("uid", Hibernate.STRING).addScalar("username", Hibernate.STRING)
                    .setString("hash", digest(token)).setLong("now", now).uniqueResult();
            if (row != null && extend) db.createSQLQuery("UPDATE nrb_account_sessions SET expiresAt=:expires WHERE tokenHash=:hash")
                    .setLong("expires", now + LIFETIME).setString("hash", digest(token)).executeUpdate();
            tx.commit(); return row;
        } catch (RuntimeException error) { tx.rollback(); throw error; }
    }

    private static void revoke(String token) {
        if (!validToken(token)) return;
        Session db = DZM.getEntityManager().getSessionFactory();
        Transaction tx = db.beginTransaction();
        try {
            db.createSQLQuery("DELETE FROM nrb_account_sessions WHERE tokenHash=:hash").setString("hash", digest(token)).executeUpdate();
            tx.commit();
        } catch (RuntimeException error) { tx.rollback(); throw error; }
    }

    private static JSONObject result(String uid, String username, String token) {
        JSONObject value = new JSONObject(); value.put("ok", true); value.put("uid", uid);
        value.put("username", username); value.put("token", token); value.put("expiresIn", LIFETIME / 1000);
        return value;
    }

    private static boolean validToken(String token) { return token != null && token.matches("nrb1_[a-f0-9]{64}"); }

    private static byte[] passwordHash(String password, String salt, int iterations) throws Exception {
        PBEKeySpec spec = new PBEKeySpec(password.toCharArray(), unhex(salt), iterations, 256);
        try { return SecretKeyFactory.getInstance("PBKDF2WithHmacSHA256").generateSecret(spec).getEncoded(); }
        finally { spec.clearPassword(); }
    }

    private static String digest(String value) {
        try { return hex(MessageDigest.getInstance("SHA-256").digest(value.getBytes(StandardCharsets.US_ASCII))); }
        catch (java.security.NoSuchAlgorithmException error) { throw new IllegalStateException(error); }
    }

    private static String random(int length) { byte[] data = new byte[length]; RANDOM.nextBytes(data); return hex(data); }
    private static String hex(byte[] value) {
        StringBuilder text = new StringBuilder(value.length * 2);
        for (byte b : value) text.append(String.format(Locale.ROOT, "%02x", b & 255));
        return text.toString();
    }
    private static byte[] unhex(String text) {
        byte[] bytes = new byte[text.length() / 2];
        for (int i = 0; i < bytes.length; i++) bytes[i] = (byte) Integer.parseInt(text.substring(i * 2, i * 2 + 2), 16);
        return bytes;
    }
}

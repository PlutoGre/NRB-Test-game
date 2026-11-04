package com.oldmountain.dzm.admin;

import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.security.MessageDigest;
import java.security.SecureRandom;
import java.text.SimpleDateFormat;
import java.util.*;
import java.util.concurrent.ConcurrentHashMap;
import net.sf.json.JSONArray;
import net.sf.json.JSONObject;
import org.hibernate.Hibernate;
import org.hibernate.Session;
import org.hibernate.Transaction;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import com.oldmountain.dzm.DZM;
import com.oldmountain.dzm.entity.*;
import com.oldmountain.dzm.message.AdventureInfo.OPAdventureInfoRet;
import com.oldmountain.dzm.message.OP;
import com.oldmountain.dzm.net.ObjectAccessor;
import com.oldmountain.dzm.net.packet.Packet;
import com.oldmountain.dzm.services.RewardServices;
import com.oldmountain.dzm.util.Util;

/**
 * @brief 运宝与养成扩展共用的角色账本、鉴权和事务入口。
 * 资产变动先落库再刷新缓存，同编号重试只读取原回执。
 */
public final class ExpansionSystems {
    public static final int REQUEST = 48003, RESPONSE = 48004;
    private static final Logger LOG = LoggerFactory.getLogger(ExpansionSystems.class);
    static final SecureRandom RANDOM = new SecureRandom();
    static final String[] MODES = {"transport", "equipment", "skills", "crew", "training", "cabin"};
    static JSONObject config;
    static AdminCatalog catalog;
    private static String configRevision;
    private static final Map<Integer, JSONObject> CACHE = new ConcurrentHashMap<Integer, JSONObject>();
    private static final Set<Integer> UNCERTAIN = Collections.newSetFromMap(new ConcurrentHashMap<Integer, Boolean>());

    private ExpansionSystems() { }

    /** 载入本地规则并创建可随数据库备份迁移的账本。 @param items 已验证的物品目录 @throws Exception 配置或数据库异常 */
    public static void initialize(AdminCatalog items) throws Exception {
        catalog = items;
        byte[] data = Files.readAllBytes(Paths.get("xml/expansionSystems.json"));
        config = JSONObject.fromObject(new String(data, StandardCharsets.UTF_8));
        StringBuilder digest = new StringBuilder();
        for (byte value : MessageDigest.getInstance("SHA-256").digest(data)) digest.append(String.format("%02x", value & 255));
        configRevision = digest.toString();
        JSONObject rule = config.getJSONObject("transport");
        bounded(rule, "durationSeconds", 5, 86400); bounded(rule, "dailyRuns", 1, 100);
        bounded(rule, "refreshGold", 0, 100000); bounded(rule, "finishGold", 1, 100000);
        validateRules();
        Session db = DZM.getEntityManager().getSessionFactory(); Transaction tx = db.beginTransaction();
        try {
            db.createSQLQuery("CREATE TABLE IF NOT EXISTS nrb_expansion (playerId INT NOT NULL PRIMARY KEY, state MEDIUMTEXT NOT NULL, revision INT NOT NULL, updatedAt INT NOT NULL) ENGINE=InnoDB DEFAULT CHARSET=utf8").executeUpdate();
            db.createSQLQuery("CREATE TABLE IF NOT EXISTS nrb_expansion_requests (playerId INT NOT NULL, requestId CHAR(32) CHARACTER SET ascii COLLATE ascii_bin NOT NULL, reply TEXT NOT NULL, createdAt INT NOT NULL, PRIMARY KEY(playerId,requestId)) ENGINE=InnoDB DEFAULT CHARSET=utf8").executeUpdate();
            List<Object[]> rows = db.createSQLQuery("SELECT playerId,state FROM nrb_expansion").addScalar("playerId",Hibernate.INTEGER).addScalar("state",Hibernate.STRING).list();
            CACHE.clear();
            for (Object[] row : rows) CACHE.put((Integer)row[0],JSONObject.fromObject((String)row[1]));
            tx.commit(); UNCERTAIN.clear();
        } catch (RuntimeException error) { tx.rollback(); throw error; }
        LOG.info("initialize: expansion systems ready, players={}, config={}", CACHE.size(), configRevision.substring(0,12));
    }

    /** 追加已启用功能的永久入口。 @param reply 原冒险列表 */
    public static void appendEntries(OPAdventureInfoRet.Builder reply) {
        for (int i=0;i<MODES.length;i++) if (config != null && config.getJSONObject(MODES[i]).optBoolean("enabled",false))
            reply.addAdventureItem(OPAdventureInfoRet.AdventureItem.newBuilder().setId(reply.getAdventureItemCount())
                .setAdventureId(9010+i).setLevelLimit(0).setBeginTime(0).setEndTime(Integer.MAX_VALUE).setVersion(1));
    }

    /** 提交状态不明时冻结旧缓存，避免定时保存覆盖已提交资产。 @param player 当前角色 @return 是否可以继续保存 */
    public static boolean ready(Player player) { return player == null || !UNCERTAIN.contains(player.getId()); }

    /** 只读访问已经提交的成长状态，用于原战斗重算。 @param playerId 角色编号 @return 已提交存档或空值 */
    public static JSONObject cached(int playerId) { return CACHE.get(playerId); }

    /**
     * 清理被旧玩法上阵、出售或消耗的船舱住客，防止战斗沿用失效助益。
     * @param player 当前角色
     * @param push 是否同时推送更新后的阵容；登录响应之前只重算存档
     * @return 是否允许继续处理请求
     */
    public static boolean synchronizeCabin(Player player,boolean push){
        if(player==null||config==null||!config.getJSONObject("cabin").optBoolean("enabled"))return true;
        JSONObject cached=CACHE.get(player.getId());if(!invalidCabin(player,cached))return true;
        synchronized(AccountService.GAME_LOGIN_LOCK){synchronized(player){
            Session db=DZM.getEntityManager().getSessionFactory();Transaction tx=db.beginTransaction();
            try{
                Object[] row=(Object[])db.createSQLQuery("SELECT state,revision FROM nrb_expansion WHERE playerId=:id FOR UPDATE")
                    .addScalar("state",Hibernate.STRING).addScalar("revision",Hibernate.INTEGER).setInteger("id",player.getId()).uniqueResult();
                if(row==null){tx.commit();return true;}
                JSONObject state=JSONObject.fromObject((String)row[0]);int revision=(Integer)row[1];
                if(invalidCabin(player,state)){
                    JSONObject seats=state.getJSONObject("cabin").getJSONObject("seats");
                    for(Object key:seats.keySet()){JSONObject seat=seats.getJSONObject(key.toString());if(seat.optInt("crew")>0&&!CabinGrowth.available(player,player.getDisciples().get(seat.optInt("crew"))))seat.put("crew",0);}
                    db.createSQLQuery("UPDATE nrb_expansion SET state=:state,revision=:revision,updatedAt=:now WHERE playerId=:id")
                        .setString("state",state.toString()).setInteger("revision",Math.addExact(revision,1)).setInteger("now",Util.getServerTime()).setInteger("id",player.getId()).executeUpdate();
                }
                tx.commit();CACHE.put(player.getId(),state);
                if(push)GrowthSystems.refresh(player);
                LOG.info("synchronizeCabin: invalid occupants removed, playerId={}",player.getId());return true;
            }catch(RuntimeException error){
                try{if(tx.isActive())tx.rollback();}catch(RuntimeException rollback){LOG.error("synchronizeCabin: rollback failed",rollback);}
                LOG.error("synchronizeCabin: cannot restore cabin state, playerId="+player.getId(),error);return false;
            }
        }}
    }
    private static boolean invalidCabin(Player player,JSONObject state){
        if(state==null||!state.containsKey("cabin")||!state.getJSONObject("cabin").containsKey("seats"))return false;
        JSONObject seats=state.getJSONObject("cabin").getJSONObject("seats");
        for(Object key:seats.keySet()){int id=seats.getJSONObject(key.toString()).optInt("crew");if(id>0&&!CabinGrowth.available(player,player.getDisciples().get(id)))return true;}return false;
    }

    /** 处理主循环内已鉴权的请求。 @param packet 原游戏连接的数据包 */
    public static void handle(Packet packet) {
        JSONObject request = null, reply; Player player = null;
        try {
            if (packet.data.array().length > 4096) throw new IllegalArgumentException("请求过大");
            request = JSONObject.fromObject(new String(packet.data.array(),StandardCharsets.UTF_8));
            String mode = request.getString("mode"); rules(mode);
            player = ObjectAccessor.getOnlineUser((Integer)packet.session.getAttribute("playerid"));
            if (player == null || player.getSession()!=packet.session || !player.getPuid().equals(packet.session.getAttribute("nrb.account.uid"))) throw new IllegalArgumentException("请重新登录游戏");
            synchronized (AccountService.GAME_LOGIN_LOCK) { synchronized (player) {
                if (!ready(player)) throw new IllegalStateException("角色资产正在恢复，请重新登录");
                reply = dispatch(player,request);
            } }
        } catch (IllegalArgumentException error) { reply = failure(request,error.getMessage(),false); }
        catch (Exception error) {
            LOG.error("handle: expansion request failed, playerId="+(player==null?0:player.getId()),error);
            reply = failure(request,"操作结果尚未确认，请点击刷新核对",true);
        }
        Packet response = new Packet(RESPONSE,reply.toString().getBytes(StandardCharsets.UTF_8)); response.data.flip(); packet.session.write(response);
    }

    private static JSONObject dispatch(Player player, JSONObject request) {
        String action = request.optString("action","info"), nonce = request.optString("requestId"), mode=request.getString("mode");
        boolean mutate = !"info".equals(action);
        if (!nonce.matches("[a-f0-9]{32}")) throw new IllegalArgumentException("无效请求编号");
        Session db=DZM.getEntityManager().getSessionFactory(); Transaction tx=db.beginTransaction();
        boolean attempted=false; Context context=null; JSONObject state; int revision; JSONObject result=null;
        try {
            db.createSQLQuery("INSERT IGNORE INTO nrb_expansion(playerId,state,revision,updatedAt) VALUES(:id,'{}',0,:now)")
                .setInteger("id",player.getId()).setInteger("now",Util.getServerTime()).executeUpdate();
            Object[] row=(Object[])db.createSQLQuery("SELECT state,revision FROM nrb_expansion WHERE playerId=:id FOR UPDATE")
                .addScalar("state",Hibernate.STRING).addScalar("revision",Hibernate.INTEGER).setInteger("id",player.getId()).uniqueResult();
            state=JSONObject.fromObject((String)row[0]); revision=(Integer)row[1];
            context=new Context(player,state,db,nonce);
            if (mutate) {
                String saved=(String)db.createSQLQuery("SELECT reply FROM nrb_expansion_requests WHERE playerId=:id AND requestId=:nonce")
                    .addScalar("reply",Hibernate.STRING).setInteger("id",player.getId()).setString("nonce",nonce).uniqueResult();
                if(saved!=null) { result=JSONObject.fromObject(saved); result.put("replayed",true); }
                else {
                    if(!configRevision.equals(request.optString("configRevision"))) throw new IllegalArgumentException("规则已更新，请刷新");
                    if(request.optInt("revision",-1)!=revision) throw new IllegalArgumentException("存档已改变，请刷新后重试");
                    execute(mode,action,request,context);
                    context.writeWallet(); revision=Math.addExact(revision,1);
                    db.createSQLQuery("UPDATE nrb_expansion SET state=:state,revision=:revision,updatedAt=:now WHERE playerId=:id")
                        .setString("state",state.toString()).setInteger("revision",revision).setInteger("now",Util.getServerTime()).setInteger("id",player.getId()).executeUpdate();
                    result=context.result;
                    db.createSQLQuery("INSERT INTO nrb_expansion_requests(playerId,requestId,reply,createdAt) VALUES(:id,:nonce,:reply,:now)")
                        .setInteger("id",player.getId()).setString("nonce",nonce).setString("reply",result.toString()).setInteger("now",Util.getServerTime()).executeUpdate();
                }
            }
            attempted=true; tx.commit();
        } catch(RuntimeException error) {
            try {if(tx.isActive())tx.rollback();}catch(RuntimeException rollback){LOG.error("dispatch: rollback failed",rollback);}
            if(attempted) {UNCERTAIN.add(player.getId()); LOG.error("dispatch: commit outcome unknown; restart server to reload authoritative assets, playerId="+player.getId(),error);}
            throw error;
        }
        CACHE.put(player.getId(),state);
        context.apply();
        if (mutate) {
            player.setNeedSyncRewards(true);
            try {
                player.sendPacket(OP.code.OPCODE_GET_USER_SYSMSG_INFORET_S_VALUE,RewardServices.getSysReward(player).setServertime(Util.getServerTime()));
            } catch(Exception error) { LOG.warn("dispatch: mailbox refresh deferred, playerId="+player.getId(),error); }
            if(!"transport".equals(mode)) GrowthSystems.refresh(player);
            LOG.info("dispatch: mode={}, action={}, playerId={}, replay={}",new Object[]{mode,action,player.getId(),result.optBoolean("replayed",false)});
        }
        JSONObject reply=new JSONObject(); reply.put("ok",true); reply.put("mode",mode); reply.put("owner",player.getPuid());
        reply.put("requestId",nonce); reply.put("revision",revision); reply.put("configRevision",configRevision);
        reply.put("goldcoins",player.getGoldCoins()); reply.put("silvercoins",String.valueOf(player.getSilvercoins())); reply.put("serverTime",Util.getServerTime());
        reply.put("title",rules(mode).getString("name")); reply.put("state",state); reply.put("result",result);
        reply.put("view",view(mode,request,context));
        return reply;
    }

    static JSONObject rules(String mode) {
        if(!Arrays.asList(MODES).contains(mode) || config==null || !config.getJSONObject(mode).optBoolean("enabled",false)) throw new IllegalArgumentException("此功能暂未开放");
        return config.getJSONObject(mode);
    }
    static JSONObject object(JSONObject parent,String key) {
        if(!parent.containsKey(key)) parent.put(key,new JSONObject()); return parent.getJSONObject(key);
    }
    static int bounded(JSONObject object,String key,int min,int max) {
        int value=object.getInt(key); if(value<min||value>max) throw new IllegalArgumentException("配置数值超出范围："+key); return value;
    }
    static JSONObject action(JSONArray actions,String label,String action,int id) {
        JSONObject item=new JSONObject();item.put("label",label);item.put("action",action);item.put("id",id);item.put("enabled",true);actions.add(item);
        // JSONArray.add 会复制 JSONObject，后续 choice / enabled 必须写入数组内实例。
        return actions.getJSONObject(actions.size()-1);
    }
    static String day() { SimpleDateFormat format=new SimpleDateFormat("yyyy-MM-dd");format.setTimeZone(TimeZone.getTimeZone("Asia/Shanghai"));return format.format(new Date()); }
    private static void validateRules(){
        JSONObject transport=config.getJSONObject("transport");int weights=0;
        for(String key:new String[]{"qualityWeights","silverRewards","toolRewards","goldRewards"})if(transport.getJSONArray(key).size()!=3)throw new IllegalArgumentException("运输品质必须有三档："+key);
        for(int i=0;i<3;i++){
            int weight=transport.getJSONArray("qualityWeights").getInt(i);if(weight<0||weight>100000)throw new IllegalArgumentException("品质权重无效");weights=Math.addExact(weights,weight);
            catalog.attachment("property:1002",transport.getJSONArray("silverRewards").getInt(i));catalog.attachment("tool:1502001",transport.getJSONArray("toolRewards").getInt(i));
            int gold=transport.getJSONArray("goldRewards").getInt(i);if(gold<0)throw new IllegalArgumentException("运输钻石奖励无效");if(gold>0)catalog.attachment("property:1001",gold);
        }
        if(weights==0)throw new IllegalArgumentException("品质总权重必须大于零");
        for(String mode:new String[]{"equipment","skills","crew","training","cabin"}){
            JSONObject r=config.getJSONObject(mode);if(!r.optBoolean("enabled"))continue;
            for(Object key:r.keySet()){
                // 道具模板 ID 与费用、倍率的取值范围不同，药剂 1502001 是合法 ID。
                Object value=r.get(key.toString());if(value instanceof Number){long number=((Number)value).longValue();long max="toolId".equals(key.toString())?Integer.MAX_VALUE:1000000;if(number<0||number>max)throw new IllegalArgumentException("养成配置超出范围："+mode+"."+key);}
            }
            for(String cap:new String[]{"maxRank","maxTemper","maxSuper","maxHull","maxSeat"})if(r.containsKey(cap))bounded(r,cap,1,100);
            if("equipment".equals(mode)){JSONArray recipes=r.getJSONArray("recipes");if(recipes.isEmpty()||recipes.size()>12)throw new IllegalArgumentException("工坊配方数量无效");for(int i=0;i<recipes.size();i++)catalog.attachment("equip:"+recipes.getInt(i),1);bounded(r,"craftShards",1,1000000);bounded(r,"decomposeShards",1,10000);}
            if("skills".equals(mode)){JSONArray choices=r.getJSONArray("inbornChoices");if(choices.isEmpty()||choices.size()>12)throw new IllegalArgumentException("天生奥义候选数量无效");for(int i=0;i<choices.size();i++)catalog.attachment("skill:"+choices.getInt(i),1);}
            if("crew".equals(mode)){bounded(r,"refundPercent",0,100);bounded(r,"superRequires",0,r.getInt("maxRank"));}
            if("training".equals(mode)){bounded(r,"toolCount",1,1000);bounded(r,"potentialCost",1,1000);bounded(r,"maxGain",1,100);catalog.attachment("tool:"+r.getInt("toolId"),r.getInt("toolCount"));validateVector(r,"attributeUnits");}
            if("cabin".equals(mode)){bounded(r,"slots",1,7);if(r.getJSONArray("ships").size()!=2)throw new IllegalArgumentException("默认船体需两艘");validateVector(r,"hullBonus");}
        }
    }
    private static void validateVector(JSONObject rule,String field){JSONArray values=rule.getJSONArray(field);if(values.size()!=4)throw new IllegalArgumentException("四维属性配置无效："+field);for(int i=0;i<4;i++)if(values.getInt(i)<0||values.getInt(i)>10000)throw new IllegalArgumentException("属性配置超出范围："+field);}
    private static JSONObject failure(JSONObject request,String message,boolean uncertain) {
        JSONObject out=new JSONObject();out.put("ok",false);out.put("uncertain",uncertain);out.put("message",message);
        if(request!=null){out.put("mode",request.optString("mode"));out.put("requestId",request.optString("requestId"));} return out;
    }

    private static void execute(String mode,String action,JSONObject request,Context ctx) {
        if("transport".equals(mode)) transport(action,ctx); else GrowthSystems.execute(mode,action,request,ctx);
    }
    private static JSONObject view(String mode,JSONObject request,Context ctx) {
        JSONObject out=new JSONObject();JSONArray lines=new JSONArray(), actions=new JSONArray();
        if("transport".equals(mode)) {
            JSONObject s=object(ctx.state,"transport"),rule=rules(mode);int used=day().equals(s.optString("day"))?s.optInt("runs"):0;
            int quality=s.optInt("quality",1),end=s.optInt("end"),now=Util.getServerTime();
            lines.add("每日运输 "+used+" / "+rule.getInt("dailyRuns")+"　品质 "+quality+" / 3");
            lines.add("三档奖励：贝里 "+rule.getJSONArray("silverRewards")+"；潜能药剂 "+rule.getJSONArray("toolRewards")+"；钻石 "+rule.getJSONArray("goldRewards")+"。");
            JSONArray weights=rule.getJSONArray("qualityWeights");int total=weights.getInt(0)+weights.getInt(1)+weights.getInt(2);
            lines.add("品质刷新概率：普通 "+(weights.getInt(0)*100.0/total)+"%、精良 "+(weights.getInt(1)*100.0/total)+"%、珍稀 "+(weights.getInt(2)*100.0/total)+"%。离线计时继续。");
            if(end>0) {
                lines.add(end>now?"运输中，剩余 "+(end-now)+" 秒":"运宝完成，可以领取传书奖励");
                action(actions,"领取运输奖励","claim",0).put("enabled",now>=end);
                if(end>now) action(actions,"立即完成（"+rule.getInt("finishGold")+" 钻石）","finish",0);
            }else {
                action(actions,"刷新品质（"+rule.getInt("refreshGold")+" 钻石）","refresh",0);
                action(actions,"开始运输（"+rule.getInt("durationSeconds")+" 秒）","start",0).put("enabled",used<rule.getInt("dailyRuns"));
            }
        } else GrowthSystems.view(mode,request,ctx,lines,actions);
        // 兼容旧版文字列表，同时提供新版界面的对象、数值预览和舱位快照。
        out.put("presentation",ExpansionPresentation.snapshot(mode,request,ctx,actions));
        out.put("lines",lines);out.put("actions",actions);return out;
    }
    private static void transport(String action,Context ctx) {
        JSONObject s=object(ctx.state,"transport"),rule=rules("transport");int now=Util.getServerTime(),end=s.optInt("end"),quality=s.optInt("quality",1);
        if("refresh".equals(action)) {
            if(end!=0)throw new IllegalArgumentException("运输中不能刷新品质"); ctx.gold(rule.getInt("refreshGold"));
            JSONArray weights=rule.getJSONArray("qualityWeights");int roll=RANDOM.nextInt(weights.getInt(0)+weights.getInt(1)+weights.getInt(2));s.put("quality",roll<weights.getInt(0)?1:roll<weights.getInt(0)+weights.getInt(1)?2:3);ctx.message("品质已刷新为 "+s.getInt("quality"));
        }else if("start".equals(action)) {
            if(end!=0)throw new IllegalArgumentException("请先领取上一次运宝奖励");
            int used=day().equals(s.optString("day"))?s.optInt("runs"):0;
            if(used>=rule.getInt("dailyRuns"))throw new IllegalArgumentException("今日运输次数已用完");
            s.put("day",day());s.put("runs",used+1);s.put("quality",quality);s.put("end",Math.addExact(now,rule.getInt("durationSeconds")));ctx.message("运输已开始，倒计时在离线后继续");
        }else if("finish".equals(action)) {
            if(end<=now)throw new IllegalArgumentException("当前没有需要加速的运输");ctx.gold(rule.getInt("finishGold"));s.put("end",now);ctx.message("运输已完成，请领取奖励");
        }else if("claim".equals(action)) {
            if(end==0||end>now)throw new IllegalArgumentException("运输尚未完成");
            JSONArray items=new JSONArray();items.add(catalog.attachment("property:1002",rule.getJSONArray("silverRewards").getInt(quality-1)));items.add(catalog.attachment("tool:1502001",rule.getJSONArray("toolRewards").getInt(quality-1)));
            int gold=rule.getJSONArray("goldRewards").getInt(quality-1);if(gold>0)items.add(catalog.attachment("property:1001",gold));
            ctx.mail("运宝奖励",items);s.put("end",0);s.put("quality",1);s.put("completed",s.optInt("completed")+1);ctx.message("运输奖励已发送至传书");
        }else throw new IllegalArgumentException("未知运输操作");
    }

    /** @brief 单次请求的数据库操作和提交后缓存刷新计划。 */
    static final class Context {
        final Player player;final JSONObject state;final Session db;final String nonce;
        final JSONObject result=new JSONObject();final List<Runnable> updates=new ArrayList<Runnable>();
        int sys,recharge;long silver;
        Context(Player p,JSONObject s,Session d,String n){player=p;state=s;db=d;nonce=n;sys=p.getSysgoldcoins();recharge=p.getRechargegoldcoins();silver=p.getSilvercoins();}
        void message(String value){result.put("message",value);}
        void gold(int amount){if(amount<0||amount>(long)sys+recharge)throw new IllegalArgumentException("钻石不足");int first=Math.min(sys,amount);sys-=first;recharge-=amount-first;}
        void silver(long amount){if(amount<0||silver<amount)throw new IllegalArgumentException("贝里不足");silver-=amount;}
        void writeWallet(){db.createSQLQuery("UPDATE player SET sysgoldcoins=:s,rechargegoldcoins=:r,silvercoins=:b WHERE id=:id").setInteger("s",sys).setInteger("r",recharge).setLong("b",silver).setInteger("id",player.getId()).executeUpdate();}
        void apply(){player.setSysgoldcoins(sys);player.setRechargegoldcoins(recharge);player.setSilvercoins(silver);for(Runnable update:updates)update.run();}
        void mail(String title,JSONArray items){
            Reward mail=new Reward();mail.setPlayerId(player.getId());mail.setRewardType(7);mail.setRewardMsg(title);
            JSONObject info=new JSONObject();info.put("nrb",1);info.put("e","ex");info.put("r",nonce.substring(0,16));
            mail.setRewardInfoFromJson(info);mail.setRewardContentFromJson(items);mail.setStatus(0);mail.setAddTime(Util.getServerTime());mail.setRewardTime(Util.getServerTime());db.save(mail);
        }
    }
}

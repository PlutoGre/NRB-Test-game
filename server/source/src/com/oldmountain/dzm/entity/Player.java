package com.oldmountain.dzm.entity;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;

import com.oldmountain.dzm.configEntity.SkillBookNpcStats;

import net.sf.json.JSONArray;
import net.sf.json.JSONObject;

import com.oldmountain.dzm.DZM;
import com.oldmountain.dzm.net.ObjectAccessor;
import com.oldmountain.dzm.util.BattleUtil;
import com.oldmountain.dzm.util.HandbookUtil;
import com.oldmountain.dzm.util.Util;

public class Player extends ClientSession{
	private int id;//游戏id
	private String puid;//平台id
	private String password;//密码
	private String name;//门派名称
	private int level;//等级
	private int exp;//玩家经验
	private int powerbytime;//按时间回复体力
	private int powerbychicken;//吃鸡回复的体力
	private int poweraddtime;//上次体力恢复时间
	private int eatfoodtime;//上次使用享美食的时间
	private int maxpower;//体力上限
	private int todaychickennum;//今日已使用的叫花鸡数量
	private int todaybuychickennum;//今日已购买的恢复体力药品数量
	private int eatchickentime;//上次使用叫花鸡时间
	private int vitalitybytime;//按时间恢复的元气
	private int vitalitybypellet;//吃养元丹恢复的元气
	private int vitalityaddtime;//上次元气恢复时间
	private int maxvitality;//元气上限
	private int todaypelletnum;//今日已使用的养元丹数量
	private int todaybuypelletnum;//今日已购买的恢复元气药品数量
	private int eatpellettime;//上次使用养元丹时间
	private int viplevel;//vip等级
	private int rechargenum;//累计已充值数量
	private long silvercoins;//玩家银两数
	private int sysgoldcoins;//系统赠送给玩家的元宝数
	private int rechargegoldcoins;//玩家自己充值获得的元宝
	private int todayleavetimes;//今日十里剩余次数
	private int tentime;//十里找一上次时间
	private int hundredtime;//百里找一上次时间
	private int wanlitime;//万里挑一上次时间
	private int tutorialstep;//新手向导步骤
	private int registertime;//玩家注册时间
	private int origin;//玩家是从哪个平台来的
	private int momentum;//玩家的势
	private String battleids;//玩家的阵容信息
	private String setting;//玩家的设置信息
	private JSONObject firstgain;//玩家是否第一次获取魂魄啥的
	private int lastlogin;//玩家上次登录时间
	private int lastVerifyTime;//玩家上次操作时间
	private Handbook handbook;//玩家的图鉴信息
	private HashMap<Integer, Disciple> disciples;//玩家的弟子信息
	private HashMap<Integer, Equip> equips;//玩家装备信息
	private HashMap<Integer, Skill> skills;//玩家的技能信息
	private HashMap<Integer, Bag> bags;//玩家的背包物品
	private Career career;//玩家的江湖信息
	private HashMap<Integer, Soul> souls;//玩家的魂魄信息
	private HashMap<Integer, Battle> battlearray;//玩家的阵容信息
	private List<Integer> discipleitems;//已经拥有的所有弟子的itemid
	private UserStats userStats;//用户的行为状态
	private TeamBuffer teamBuffer;//用户的掌门决信息
    private AdventureFightStats fightStats;//血战到底信息
    private RelationShip relationShip;//玩家恩怨信息
    //阵容数量->不存入数据库，玩家在升级后进行调整和往内存写入数据时
    private int battleCount;
    private Title userTitle;
    private Challenge userChallenge;//用户的论剑信息

    private HashMap<Integer,Reward> rewards;//血战排行奖励和系统补偿奖励
    private boolean  isNeedSyncRewards=true;
    private boolean isDirty;//判断玩家数据是否需要更新到服务器
    private HashMap<String, HashSet<Integer>> needUpdateObj = new HashMap<String, HashSet<Integer>>();
    private HashMap<String, HashSet<Integer>> needAddObj = new HashMap<String, HashSet<Integer>>();
    private HashMap<Integer, SkillBook> skillBooks;//玩家的技能残章表
    private HashMap<Integer, SysMsgInfo> sysMsgInfos;//玩家的传书信息
    
    private int invitedid;//邀请此玩家的玩家id
    private InviteStatus inviteStatus;//用户邀请的用户提供的状态
    
    private DareWorldBoss worldBoss;//用户当天攻击世界boss信息
    private int instructionFinishTime;//新手引导完成时间
    
    private int leaguaid;//玩家的公会ID
    private int applyLeaguaNum;//已经申请联盟的个数
    private String deviceId;//设备号
    private String platform;//来源平台
    private ActivityInfo activityInfo;//玩家活动相关信息
    


    private SkillBookNpcStats skillBookNpcStats;
	public Player() {
		this.name = "aaa";
		this.level = 1;
		this.exp = 60;
		this.powerbytime = 30;
		this.powerbychicken = 0;
		this.maxpower = 30;
		this.vitalitybytime = 12;
		this.vitalitybypellet = 0;
		this.maxvitality = 12;
		this.viplevel = 3;        //// 默认VIP3 = 3
		this.rechargenum = 0;
		this.silvercoins = 1000L;
		this.tutorialstep = 0;
		this.sysgoldcoins = 30;
		this.rechargegoldcoins = 0;
		this.eatfoodtime = 0;
		this.todaychickennum = 0;
		this.eatchickentime = 0;
		this.todaypelletnum = 0;
		this.eatpellettime = 0;
		this.tentime = this.hundredtime = this.wanlitime = 0;
		this.todayleavetimes = 5;
		this.origin = this.lastlogin = this.momentum = 0;
		this.handbook = new Handbook();
		this.disciples = new HashMap<Integer, Disciple>();
		this.equips = new HashMap<Integer, Equip>();
		this.skills = new HashMap<Integer, Skill>();
		this.souls = new HashMap<Integer, Soul>();
		this.bags = new HashMap<Integer, Bag>();
		this.discipleitems = new ArrayList<Integer>();
		this.userStats = new UserStats();
		this.battleids = "";
		this.setting = "{\"music\":1,\"sound\":1,\"chat\":1}";
		this.firstgain = new JSONObject();
		this.isDirty = false;
		this.needUpdateObj = new HashMap<String, HashSet<Integer>>();
        this.needAddObj = new HashMap<String, HashSet<Integer>>();
        this.rewards = new HashMap<Integer, Reward>();
        this.isNeedSyncRewards=true;
        this.skillBooks= new HashMap<Integer, SkillBook>();
        this.skillBookNpcStats=new SkillBookNpcStats();
        this.sysMsgInfos=new HashMap<Integer, SysMsgInfo>();
        
        this.invitedid = 0;
        this.instructionFinishTime=0;
        this.leaguaid = 0;
	}
	
	public String getFirstgain(){
		return firstgain.toString();
	}
	
	public void setFirstgain(String firstgain){
		this.firstgain = JSONObject.fromObject(firstgain);
	}
	
	public void setFirstGainByType(String type){
		this.firstgain.put(type, 1);
	}

	public int getLastVerifyTime(){
		return lastVerifyTime;
	}
	
	public void setLastVerifyTime(int time){
		this.lastVerifyTime = time;
	}
	
	public String getPuid() {
		return puid;
	}

	public void setPuid(String puid) {
		this.puid = puid;
		this.isDirty = true;
	}

	public String getPassword() {
		return password;
	}

	public void setPassword(String password) {
		this.password = password;
		this.isDirty = true;
	}

	public int getId() {
		return id;
	}

	public void setId(int id) {
		this.id = id;
		this.isDirty = true;
	}
	
	public int getLevel(){
		return level;
	}
	
	public void setLevel(int level)
    {
		this.level = level;
        battleCount=BattleUtil.getBattleCountByLevel(level);
		this.isDirty = true;
	}
	
	public int getExp(){
		return exp;
	}
	
	public void setExp(int exp){
		this.exp = exp;
		this.isDirty = true;
	}
	
	public int getPowerbytime(){
		return powerbytime;
	}
	
	public void setPowerbytime(int power){
		this.powerbytime = power;
		this.isDirty = true;
	}
	
	public int getPowerbychicken(){
		return powerbychicken;
	}
	
	public void setPowerbychicken(int power){
		this.powerbychicken = power;
		this.isDirty = true;
	}
	
	public int getEatfoodtime(){
		return eatfoodtime;
	}
	
	public void setEatfoodtime(int time){
		this.eatfoodtime = time;
		this.isDirty = true;
	}
	
	public int getPoweraddtime(){
		return poweraddtime;
	}
	
	public void setPoweraddtime(int time){
		this.poweraddtime = time;
		this.isDirty = true;
	}
	
	public int getMaxpower(){
		return maxpower;
	}
	
	public void setMaxpower(int maxPower){
		this.maxpower = maxPower;
		this.isDirty = true;
	}
	
	public int getTodaychickennum(){
		return todaychickennum;
	}
	
	public void setTodaychickennum(int num){
		this.todaychickennum = num;
		this.isDirty = true;
	}
	
	public int getEatchickentime(){
		return eatchickentime;
	}
	
	public void setEatchickentime(int time){
		this.eatchickentime = time;
		this.isDirty = true;
	}
	
	public int getVitalitybytime(){
		return vitalitybytime;
	}
	
	public void setVitalitybytime(int vitality){
		this.vitalitybytime = vitality;
		this.isDirty = true;
	}
	
	public int getVitalitybypellet(){
		return vitalitybypellet;
	}
	
	public void setVitalitybypellet(int vitality){
		this.vitalitybypellet = vitality;
		this.isDirty = true;
	}
	
	public int getVitalityaddtime(){
		return vitalityaddtime;
	}
	
	public void setVitalityaddtime(int time){
		this.vitalityaddtime = time;
		this.isDirty = true;
	}
	
	public int useVitality(int count){
		if (this.vitalitybytime + this.vitalitybypellet < count) {
			return 0;
		}
		if (this.vitalitybytime >= count) {
			this.vitalitybytime -= count;
		}
		else {
			this.vitalitybytime = 0;
			this.vitalitybypellet = this.vitalitybypellet - count + this.vitalitybytime;
		}
		this.setLastVerifyTime(Util.getServerTime());
		return this.vitalitybytime + this.vitalitybypellet;
	}
	
	public int getMaxvitality(){
		return maxvitality;
	}
	
	public void setMaxvitality(int maxvitality){
		this.maxvitality = maxvitality;
		this.isDirty = true;
	}
	
	public int getViplevel(){
		return viplevel;
	}
	
	public void setViplevel(int vip_level){
		this.viplevel = vip_level;
		this.isDirty = true;
	}
	
	public int getRechargenum(){
		return rechargenum;
	}
	
	public void setRechargenum(int num){
		this.rechargenum = num;
		this.isDirty = true;
	}
	
	public long getSilvercoins(){
		return silvercoins;
	}
	
	public void setSilvercoins(long silver_coins){
		this.silvercoins = silver_coins;
		this.isDirty = true;
	}
	
	public int getSysgoldcoins(){
		return sysgoldcoins;
	}
	
	public void setSysgoldcoins(int gold_coins){
		this.sysgoldcoins = gold_coins;
		this.isDirty = true;
	}
	
	public int getRechargegoldcoins(){
		return rechargegoldcoins;
	}

    public int getGoldCoins()
    {
        return getSysgoldcoins()+getRechargegoldcoins();
    }

    public void consumeGold(int _gold)
    {
        this.sysgoldcoins-=_gold;
        if(this.sysgoldcoins<0)
        {
             this.rechargegoldcoins+=this.sysgoldcoins;
             this.sysgoldcoins=0;
        }
        this.isDirty = true;
    }

    public void increaseSysGoldCoins(int _gold)
    {
        if(_gold>0)
        {
            this.sysgoldcoins+=_gold;
        }
        this.isDirty = true;
    }

    public void increaseSilverCoins(int _silver)
    {
        if(_silver>0)
        {
            this.silvercoins+=_silver;
        }
        this.isDirty = true;
    }

    public void increaseExp(int _exp)
    {
        if(_exp>0)
        {
            this.exp+=_exp;
        }
        this.isDirty = true;
    }
	
	public void setRechargegoldcoins(int gold_coins){
		this.rechargegoldcoins = gold_coins;
		this.isDirty = true;
	}
	
	public int getRegistertime(){
		return registertime;
	}
	
	public void setRegistertime(int time){
		this.registertime = time;
		this.isDirty = true;
	}
	
	public int getTutorialstep(){
		return tutorialstep;
	}
	
	public void setTutorialstep(int step){
		this.tutorialstep = step;
		this.isDirty = true;
	}
	
	public int getTentime(){
		return tentime;
	}
	
	public void setTentime(int time){
		this.tentime = time;
		this.isDirty = true;
	}
	
	public int getHundredtime(){
		return hundredtime;
	}
	
	public void setHundredtime(int time){
		this.hundredtime = time;
		this.isDirty = true;
	}
	
	public int getWanlitime(){
		return wanlitime;
	}
	
	public void setWanlitime(int time){
		this.wanlitime = time;
		this.isDirty = true;
	}
	
	public int getTodayleavetimes(){
		return todayleavetimes;
	}
	
	public void setTodayleavetimes(int num){
		this.todayleavetimes = num;
		this.isDirty = true;
	}
	
	public Handbook getHandbook(){
		return handbook;
	}
	
	public void setHandbook(Handbook handbook){
		this.handbook = handbook;
	}
	
	public void addOneHandbook(Handbook handbook){
		DZM.getEntityManager().createSync(handbook);
		this.handbook = handbook;
	}
	
	public void addOrUpdateHandbook(Handbook handbook){
		if(this.handbook != null){
			this.handbook = handbook;
		}else{
			DZM.getEntityManager().createSync(handbook);
			this.handbook = handbook;
		}
	}
	
	public void addDisciple(int discipleId, int showType){
		int id = this.getId();
		Handbook handbook = this.handbook;
		
		String discipleStr = this.handbook.getDiscipleInfo();
		
		JSONObject rewardContent = JSONObject.fromObject(this.handbook.getDiscipleInfo());
		rewardContent.put(discipleId, showType);
		this.handbook.setDiscipleInfo(rewardContent.toString());
		addNeedUpdate("handbooks", this.getId());
	}

	public void addEquip(int equipId, int showType){
		JSONObject rewardContent = JSONObject.fromObject(this.handbook.getEquipInfo());
		rewardContent.put(equipId, showType);
		this.handbook.setEquipInfo(rewardContent.toString());
		addNeedUpdate("handbooks", this.getId());
	}

	public void addSkill(int skillId, int showType){
		JSONObject rewardContent = JSONObject.fromObject(this.handbook.getSkillInfo());
		rewardContent.put(skillId, showType);
		this.handbook.setSkillInfo(rewardContent.toString());
		addNeedUpdate("handbooks", this.getId());
	}

	public void updateOneHandbook(Handbook handbook){
//		this.handbook.put(handbook.getPlayerId(),handbook);
//		addNeedUpdate("handbook",handbook.getPlayerId());
	}
	
	public void removeOneHandbook(Handbook handbook){
//		this.handbook.remove(handbook.getPlayerId());
//		DZM.getEntityManager().deleteSync(handbook);
	}
	
	public HashMap<Integer, Disciple> getDisciples(){
		return disciples;
	}
	
	public void setDisciples(HashMap<Integer, Disciple> disciples){
		this.disciples = disciples;
	}
	
	public void addOneDisciple(Disciple disciple){
		HandbookUtil.gotDisciple(this, disciple.getItemid());//获得船员时添加图鉴信息
		DZM.getEntityManager().createSync(disciple);
		this.discipleitems.add(disciple.getItemid());
		this.disciples.put(disciple.getId(), disciple);
	}
	
	public void updateOneDisciple(Disciple disciple){
		this.disciples.put(disciple.getId(), disciple);
		addNeedUpdate("disciple",disciple.getId());
	}
	
	public void removeOneDisciple(Disciple disciple){
		Skill skill = this.skills.get(disciple.getSkill1());
		this.removeOneSkill(skill);
		this.disciples.remove(disciple.getId());
        removeDiscipleItemByItemId(disciple.getItemid());
		DZM.getEntityManager().deleteSync(disciple);
	}
	private void removeDiscipleItemByItemId(int itemId)
    {
		/**
		 * 特殊处理一下，有可能一个itemid会在里面加重
		 */
       int size = this.discipleitems.size();
       for(int i=0;i<size;)
       {
           if(this.discipleitems.get(i)==itemId)
           {
               this.discipleitems.remove(i);
               size = this.discipleitems.size();
               continue;
           }
           i++;
       }
    }
	public HashMap<Integer, Equip> getEquips(){
		return equips;
	}
	
	public void setEquips(HashMap<Integer, Equip> equips){
		this.equips = equips;
	}
	
	public void addOneEquip(Equip equip){
		HandbookUtil.gotEquip(this, equip.getItemid());//获得装备时添加图鉴信息
		DZM.getEntityManager().createSync(equip);
		this.equips.put(equip.getId(), equip);
	}
	
	public void updateOneEquip(Equip equip){
		this.equips.put(equip.getId(),equip);
		addNeedUpdate("equip", equip.getId());
	}
	
	public void removeOneEquip(Equip equip){
		this.equips.remove(equip.getId());
		DZM.getEntityManager().deleteSync(equip);
	}
	
	public HashMap<Integer, Skill> getSkills(){
		return skills;
	}
	
	public void setSkills(HashMap<Integer, Skill> skills){
		this.skills = skills;
	}
	
	public void addOneSkill(Skill skill){
		HandbookUtil.gotSkill(this, skill.getItemid());//获得技能时添加图鉴信息
		DZM.getEntityManager().createSync(skill);
		this.skills.put(skill.getId(), skill);
	}
	
	public void updateOneSkill(Skill skill){
		this.skills.put(skill.getId(), skill);
		addNeedUpdate("skill", skill.getId());
	}
	
	public void removeOneSkill(Skill skill){
		this.skills.remove(skill.getId());
		DZM.getEntityManager().deleteSync(skill);
	}
	
	public Career getCareer(){
		return career;
	}
	
	public void setCareer(Career career){
		this.career = career;
		addNeedUpdate("career", career.getPlayerid());
	}
	
	public int getOrigin(){
		return origin;
	}
	
	public void setOrigin(int origin){
		this.origin = origin;
		this.isDirty = true;
	}
	
	public HashMap<Integer, Bag> getBags() {
		return bags;
	}
	
	public void setBags(HashMap<Integer, Bag> bags){
		this.bags = bags;
	}
	
	public void addOneBag(Bag bag){
		DZM.getEntityManager().createSync(bag);
		this.bags.put(bag.getItemid(), bag);
	}
	
	public void updateOneBag(Bag bag){
		this.bags.put(bag.getItemid(), bag);
		addNeedUpdate("bag", bag.getItemid());
	}
	
	public int getTodaypelletnum(){
		return todaypelletnum;
	}
	
	public void setTodaypelletnum(int num){
		this.todaypelletnum = num;
		this.isDirty = true;
	}
	
	public int getEatpellettime(){
		return eatpellettime;
	}
	
	public void setEatpellettime(int time){
		this.eatpellettime = time;
		this.isDirty = true;
	}
	
	public String getKey() {
		return "player" + id;
	}
	
	public String getName(){
		return name;
	}
	
	public void setName(String name){
		this.name = name;
		this.isDirty = true;
	}
	
	public HashMap<Integer, Soul> getSouls() {
		return souls;
	}
	
	public void setSouls(HashMap<Integer, Soul>souls){
		this.souls = souls;
	}
	
	public void addOneSoul(Soul soul){
		HandbookUtil.gotSoul(this, soul.getItemid());//获取魂魄时添加图鉴信息
		DZM.getEntityManager().createSync(soul);
		this.souls.put(soul.getItemid(), soul);
	}
	
	public void updateOneSoul(Soul soul){
		this.souls.put(soul.getItemid(), soul);
		addNeedUpdate("soul", soul.getItemid());
	}
	
	public HashMap<Integer, Battle> getBattlearray(){
		return battlearray;
	}
	
	public void setBattlearray(HashMap<Integer, Battle> battleArray){
		this.battlearray = battleArray;
	}
	
	public void addOneBattle(Battle battle){
		DZM.getEntityManager().createSync(battle);
		this.battlearray.put(battle.getId(), battle);
	}
	
	public void updateOneBattle(Battle battle){
		this.battlearray.put(battle.getId(), battle);
		addNeedUpdate("battle", battle.getId());
	}

    public HashMap<Integer, Reward> getRewards()
    {
        return rewards;
    }

    public void setRewards(HashMap<Integer, Reward> rewards)
    {
        this.rewards = rewards;
    }

    public void updateOneReward(Reward reward)
    {
        this.rewards.put(reward.getId(),reward);
        addNeedUpdate("rewards",reward.getId());
    }

    public boolean isNeedSyncRewardsToMap()
    {
        return isNeedSyncRewards;
    }

    public void setNeedSyncRewards(boolean needSyncRewards)
    {
        isNeedSyncRewards = needSyncRewards;
    }

    public int getLastlogin(){
		return lastlogin;
	}
	
	public void setLastlogin(int time){
		this.lastlogin = time;
		this.isDirty = true;
	}
	
	public int getMomentum(){
		return momentum;
	}
	
	public void setMomentum(int momentum){
		this.momentum = momentum;
		this.isDirty = true;
	}
	
	public String getBattleids(){
		return battleids;
	}
	
	public void setBattleids(String ids){
		this.battleids = ids;
		this.isDirty = true;
	}
	
	public List<Integer> getDiscipleitems(){
		return discipleitems;
	}
	
	public void setDiscipleitems(List<Integer> items){
		this.discipleitems = items;
		this.isDirty = true;
	}
	
	public UserStats getUserStats(){
		return userStats;
	}
	
	public void setUserStats(UserStats stats){
		this.userStats = stats;
		addNeedUpdate("stats", stats.getPlayerid());
		this.isDirty = true;
	}
	
	public String getSetting(){
		return setting;
	}
	
	public void setSetting(String setting){
		this.setting = setting;
		this.isDirty = true;
	}
	
	public TeamBuffer getTeamBuffer(){
		return teamBuffer;
	}
	
	public void setTeamBuffer(TeamBuffer teamBuffer){
		this.teamBuffer = teamBuffer;
		addNeedUpdate("teambuffer", teamBuffer.getPlayerid());
	}

    public AdventureFightStats getFightStats()
    {
        if(fightStats==null)
        {
            fightStats=new AdventureFightStats();
        }
        return fightStats;
    }

    public void setFightStats(AdventureFightStats fightStats) {
        this.fightStats = fightStats;
    }

    public int getBattleCount() {
        return battleCount;
    }

    public void setBattleCount(int battleCount)
    {
        this.battleCount = battleCount;
    }
    
    public Title getTitle(){
    	return userTitle;
    }
    
    public void setTitle(Title title){
    	this.userTitle = title;
    	addNeedUpdate("title", title.getPlayerid());
    }
    
    public Challenge getChallenge(){
    	return userChallenge;
    }
    
    public boolean getIsDirty(){
    	return isDirty;
    }
    
    public void setIsDirty(boolean isDirty){
    	this.isDirty = isDirty;
    }
    
    public HashMap<String, HashSet<Integer>> getNeedUpdate(){
    	return needUpdateObj;
    }
    
    public void addNeedUpdate(String type,int id){
    	if (needUpdateObj.containsKey(type)) {
			needUpdateObj.get(type).add(id);
		}
    	else {
			HashSet<Integer> needUpdateHashSet = new HashSet<Integer>();
			needUpdateHashSet.add(id);
			needUpdateObj.put(type, needUpdateHashSet);
		}
    	this.isDirty = true;
    }
    
    public void clearNeedUpdate(){
    	this.isDirty = false;
    	this.needUpdateObj = new HashMap<String, HashSet<Integer>>();
    }

    public HashMap<String, HashSet<Integer>> getNeedAdd(){
        return needAddObj;
    }

    public void addNeedAdd(String type,int id)
    {
        if (needAddObj.containsKey(type))
        {
            needAddObj.get(type).add(id);
        }
        else
        {
            HashSet<Integer> needAddHashSet = new HashSet<Integer>();
            needAddHashSet.add(id);
            needAddObj.put(type, needAddHashSet);
        }
        this.isDirty = true;
    }

    public void clearNeedAdd()
    {
        this.isDirty = false;
        this.needAddObj = new HashMap<String, HashSet<Integer>>();
    }
    
    public void setChallenge(Challenge challenge){
    	this.userChallenge = challenge;
    	addNeedUpdate("challenge", challenge.getPlayerid());
    }
    
	public void increaseLevel(int _level){
		if(_level>0){
			this.level+=_level;
		}
	}
	
	public void increaseVitalityBySys(int _vitality){
			this.vitalitybypellet+=_vitality;
	}
	
	public void increasePowerBySys(int _power){
			this.powerbychicken+=_power;
	}
    public void decreasePowerBySys(int _power){
        if(_power>0){
            this.powerbychicken-=_power;
            if(this.powerbychicken<0)
            {
                this.powerbytime+=this.powerbychicken;
                this.powerbychicken=0;
            }
        }
    }
    
    public boolean resetName(String name){
    	boolean status = false;
		
		if (!ObjectAccessor.getExistNames().contains(name)) {
			String oldName = this.name;
			
			ObjectAccessor.getExistNames().remove(oldName);
			ObjectAccessor.getExistNames().add(name);
			
			this.name = name;
			
			Challenge challenge = this.userChallenge;
			if (challenge != null) {
				challenge.setName(name);
				this.setChallenge(challenge);
			}
			status = true;
		}
		
		return status;
    }

    public void addOneSkillBook(SkillBook skillBook)
    {
    	HandbookUtil.gotSkillBook(this, skillBook.getSkillId());//获得残障时添加图鉴信息
        this.skillBooks.put(skillBook.getSkillId(), skillBook);
    }

    public void updateOneSkillBook(SkillBook skillBook)
    {
        this.skillBooks.put(skillBook.getSkillId(), skillBook);
        addNeedUpdate("skillBook", skillBook.getSkillId());
    }

    public HashMap<Integer, SkillBook> getSkillBooks()
    {
        return skillBooks;
    }

    public void setSkillBooks(HashMap<Integer, SkillBook> skillBooks)
    {
        this.skillBooks = skillBooks;
    }

    public SkillBookNpcStats getSkillBookNpcStats() {
        return skillBookNpcStats;
    }

    public void setSkillBookNpcStats(SkillBookNpcStats skillBookNpcStats) {
        this.skillBookNpcStats = skillBookNpcStats;
    }

    public void addOneSysMsgInfo(SysMsgInfo sysMsgInfo)
    {
        this.sysMsgInfos.put(sysMsgInfo.getId(), sysMsgInfo);
        addNeedAdd("sysMsgInfo", sysMsgInfo.getId());
    }

    public void updateOneSysMsgInfo(SysMsgInfo sysMsgInfo)
    {
        this.sysMsgInfos.put(sysMsgInfo.getId(), sysMsgInfo);
        addNeedUpdate("sysMsgInfo", sysMsgInfo.getId());
    }


    public void updateFightEndStats()
    {
        addNeedUpdate("fightEnd",this.getId());
    }

    public HashMap<Integer, SysMsgInfo> getSysMsgInfos()
    {
        return sysMsgInfos;
    }

    public void setSysMsgInfos(HashMap<Integer, SysMsgInfo> sysMsgInfos)
    {
        this.sysMsgInfos = sysMsgInfos;
    }
	
	public int getTodaybuychickennum() {
		return todaybuychickennum;
	}
    
    public void setTodaybuychickennum(int num) {
    	this.todaybuychickennum = num;
    }
    
    public int getTodaybuypelletnum(){
    	return todaybuypelletnum;
    }
    
    public void setTodaybuypelletnum(int num){
    	this.todaybuypelletnum = num;
    }
    
    public int getInvitedid(){
    	return invitedid;
    }
    
    public void setInvitedid(int id){
    	this.invitedid = id;
    	this.isDirty = true;
    }
    
    public InviteStatus getInviteStatus(){
    	return inviteStatus;
    }
    
    public void setInviteStatus(InviteStatus status){
    	this.inviteStatus = status;
    }
    
    public DareWorldBoss getWorldBoss(){
    	if (this.worldBoss == null) {
			this.worldBoss = new DareWorldBoss();
			this.worldBoss.setPlayerid(id);
			this.worldBoss.setLastverifytime(Util.getServerTime() - 60);
		}
    	return this.worldBoss;
    }
    
    public void setWorldBoss(DareWorldBoss worldBoss){
    	this.worldBoss = worldBoss;
    }

	public int getInstructionFinishTime() {
		return instructionFinishTime;
	}

	public void setInstructionFinishTime(int instructionFinishTime) {
		this.instructionFinishTime = instructionFinishTime;
		this.isDirty=true;
	}


	public RelationShip getRelationShip() {
		return relationShip;
	}

	public void setRelationShip(RelationShip relationShip) {
		this.relationShip = relationShip;
		addNeedUpdate("relationship", relationShip.getPlayerid());
		this.isDirty = true;
	}

	
	public int getLeaguaid(){
		return leaguaid;
	}
	
	public void setLeaguaid(int id){
		this.leaguaid = id;
		this.isDirty = true;
	}

    public int getApplyLeaguaNum(){
    	return applyLeaguaNum;
    }
    
    public void setApplyLeaguaNum(int applyLeaguaNum){
    	this.applyLeaguaNum = applyLeaguaNum;
    }
    
    public String getDeviceId(){
    	return deviceId;
    }
    
    public void setDeviceId(String deviceId){
    	this.deviceId = deviceId;
    }
    
    public String getPlatform(){
    	return platform;
    }
    
    public void setPlatform(String platform){
    	this.platform = platform;
    }

	public ActivityInfo getActivityInfo() {
		return activityInfo;
	}

	public void setActivityInfo(ActivityInfo activityInfo) {
		this.activityInfo = activityInfo;
		addNeedUpdate("activityInfo", activityInfo.getPlayerid());
		this.isDirty = true;
	}
    
}

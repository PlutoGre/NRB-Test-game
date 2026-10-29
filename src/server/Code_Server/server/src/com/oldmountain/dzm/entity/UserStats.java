package com.oldmountain.dzm.entity;

import org.apache.commons.lang.StringUtils;

import net.sf.json.JSONObject;

public class UserStats {
	private int playerid;//
	private int firstbaitime;//百里首刷时间
	private int firstwantime;//万里首刷时间
	private String levelupreward;//新手升级的奖励领取情况
	private String buygifts;//已购买过的礼包
	private int discipleLucky;//当前抽弟子累积
	private int nextDisciple;//下次高级奖池ID
	private StringBuffer buyGiftsBuffer;
	private int logincount;//上次连续登陆的天数
	private int getreward;//是否已经获取奖励
	private String loginreward;//连续登陆奖励
    private int visitRewardCount;//亲吻女帝领奖累计次数
    private int visitTimes;//累计连续亲吻次数
    private int lastVisitTime;//上次亲吻女帝时间
    private int hasVisited;//今天是否亲过女帝
    private int luckOne;//抽宝箱运气值1
    private int luckTwo;//抽宝箱运气值2
    private int treasureWorth;//抽宝箱抽取额度
    private int ameatfood;//12点吃叫花鸡状态
    private int pmeatfood;//18点吃叫花鸡状态
    private int challengetimes;//今日剩余论剑使用次数
    private JSONObject adContinueInfo;//封测奇遇连续登陆活动信息
    private float luckPool;//抽宝箱累计运气奖池
    private String cdkeyTypes; //已领取过的cdkey的type,以","分隔
    private int sysMsgTime;//系统消息读取的最后时间
    private int pieceskilltimes;//拼合残障的次数
    private int getRebate;//领取充值返利
    private int chargeHeap;//充值返利累积充值金额(RMB)
    private int chargeTerm;//充值返利的期数
    private int fortuneComeTimes;//财神到活动的已摇奖次数
    private int giveSoulTerm;//抽船员送影子活动id
    private int giveSoulHeap2;//送影子活动百万中抽中阶段判定ID的次数
    private	int giveSoulHeap3;//送影子活动亿万中抽中阶段判定ID的次数
    private int giveSoulDifferTimes2;//送影子活动百万中连续没抽中保底id的次数
    private int giveSoulDifferTimes3;//送影子活动亿万中连续没抽中保底id的次数
    private int fightEndTimes;	//玩家参加血战的次数
    private int luckyStarTermid;	//幸运星活动的id
    private int luckyStarDropTimes1;	//本次活动期间幸运星1的掉落次数
    private int luckyStarDropTimes2;	//本次活动期间幸运星2的掉落次数
    private int luckyStayHeap1;	//幸运星活动时金宝箱的抽取次数累积
    private int luckyStayHeap2;	//幸运星活动时银宝箱的抽取次数累积
    private int challengeActTermId;//论剑兑奖活动id
    private int highestRankInAct;//论剑活动期间达到过的最高排名
    private int nextQuickCareerTime;	//下次连闯时间
    private int hasGotFriendInfo;	//是否已经获取过恩怨信息1-已获得 0-未获得
    private String deepWaterLastTimes;//深海探宝活动剩余次数
    private String deepWaterHeap;//深海探宝活动累积额度
    private int deepWaterTermId;//玩家深海探宝活动的termid
    private int dwNextRefreshTime;//深海探宝活动下次刷新时间

    public UserStats(){
		this.firstbaitime = 0;
		this.firstwantime = 0;
		this.buygifts = "";
		this.discipleLucky = 0;
		this.nextDisciple = 3;
		this.logincount = 1;
		this.getreward = 0;
		this.loginreward = "";
        this.visitRewardCount = 0;
        this.visitTimes = 0;
        this.lastVisitTime = (int) (System.currentTimeMillis()/1000);
        this.hasVisited = 0;
		this.buyGiftsBuffer = new StringBuffer();
		this.levelupreward = "";

        this.luckOne = 600;
        this.luckTwo = 600;
        this.treasureWorth = 0;
        this.luckPool = 0.0f;
        this.ameatfood = 0;
        this.pmeatfood = 0;
        this.challengetimes = 0;
        this.adContinueInfo=new JSONObject();
        
        this.pieceskilltimes = 0;
        this.getRebate=0;
        this.chargeHeap=0;
        this.chargeTerm=0;
        this.fortuneComeTimes=0;
        this.luckyStarTermid=0;
        this.luckyStarDropTimes1=0;
        this.luckyStarDropTimes2=0;
        this.luckyStayHeap1=0;
        this.luckyStayHeap2=0;
        this.challengeActTermId=0;
        this.highestRankInAct=0;
        this.hasGotFriendInfo=0;
        this.deepWaterTermId=0;
        this.deepWaterHeap="{}";
        this.deepWaterLastTimes="{}";
        this.dwNextRefreshTime=0;
	}
	
	public int getPlayerid(){
		return playerid;
	}
	
	public void setPlayerid(int id){
		this.playerid = id;
	}
	
	public int getFirstbaitime(){
		return firstbaitime;
	}
	
	public void setFirstbaitime(int time){
		this.firstbaitime = time;
	}
	
	public int getFirstwantime(){
		return firstwantime;
	}
	
	public void setFirstwantime(int time){
		this.firstwantime = time;
	}
	
	public String getBuygifts()
    {
		//return buygifts;
		return this.buyGiftsBuffer.toString();
	}
	
	public void setBuygifts(String gifts)
    {
		this.buygifts = gifts;
        this.buyGiftsBuffer.append(this.buygifts);
	}

    public void increaseBuyGifts(String gifts)
    {
        this.buyGiftsBuffer.append(gifts).append(",");
    }
	
    public int getBuyGiftsTimesById(int toolId)
    {
        int count=0;
        try
        {
            int i = 0;
            String str=String.valueOf(toolId);
            while((i=this.buyGiftsBuffer.indexOf(str, i))!=-1)
            {
                count++;
                i+=str.length()+1;
            }
        }
        catch (Exception e)
        {
            count=0;
        }
        return count;
    }
	public String getLevelupreward()
    {
		return levelupreward;
	}

	public void setLevelupreward(String levelUpReward)
    {
		this.levelupreward = levelUpReward;
	}

    /**
     * 领取升级奖励
     * @param id
     */
    public void rewardLevelUpReward(int id)
    {
        this.levelupreward+=id+",";
    }
	
	public int getDiscipleLucky(){
		return discipleLucky;
	}
	
	public void setDiscipleLucky(int lucky){
		this.discipleLucky = lucky;
	}
	
	public int getNextDisciple() {
		return nextDisciple;
	}
	
	public void setNextDisciple(int id){
		this.nextDisciple = id;
	}

	public int getLogincount(){
		return logincount;
	}
	
	public void setLogincount(int count){
		this.logincount = count;
	}
	
	public int getGetreward(){
		return getreward;
	}
	
	public void setGetreward(int isGet){
		this.getreward = isGet;
	}
	
	public String getLoginreward(){
        return loginreward;
    }

    public void setLoginreward(String reward){
        this.loginreward = reward;
    }

    public int getVisitRewardCount() {
        return visitRewardCount;
    }

    public void setVisitRewardCount(int visitRewardCount) {
        this.visitRewardCount = visitRewardCount;
    }

    public int getVisitTimes() {
        return visitTimes;
    }

    public void setVisitTimes(int visitTimes) {
        this.visitTimes = visitTimes;
    }

    public int getLastVisitTime() {
        return lastVisitTime;
    }

    public void setLastVisitTime(int lastVisitTime) {
        this.lastVisitTime = lastVisitTime;
    }

    public int getHasVisited() {
        return hasVisited;
    }

    public void setHasVisited(int hasVisited) {
        this.hasVisited = hasVisited;
    }
    public int getLuckOne() {
        return luckOne;
    }

    public void setLuckOne(int luckOne) {
        this.luckOne = luckOne;
    }

    public int getLuckTwo() {
        return luckTwo;
    }

    public void setLuckTwo(int luckTwo) {
        this.luckTwo = luckTwo;
    }

    public int getTreasureWorth() {
        return treasureWorth;
    }

    public void setTreasureWorth(int treasureWorth) {
        this.treasureWorth = treasureWorth;
    }
    
    public int getAmeatfood(){
    	return ameatfood;
    }
    
    public void setAmeatfood(int status){
    	this.ameatfood = status;
    }
    
    public int getPmeatfood(){
    	return pmeatfood;
    }
    
    public void setPmeatfood(int status){
    	this.pmeatfood = status;
    }

    public int getChallengetimes(){
    	return challengetimes;
    }
    
    public void setChallengetimes(int times){
    	this.challengetimes = times;
    }

    public String getAdContinueInfo()
    {
        return adContinueInfo.toString();
    }

    public void setAdContinueInfo(String adContinueInfo)
    {
        this.adContinueInfo =JSONObject.fromObject(adContinueInfo);
    }

    public JSONObject getAdventureContinueInfoJson()
    {
        return adContinueInfo;
    }

    public void setAdContinueInfoFromJson(JSONObject js)
    {
        this.adContinueInfo=js;
    }
    public float getLuckPool() {
        return luckPool;
    }

    public void setLuckPool(float luckPool) {
        this.luckPool = luckPool;
    }
    
    public String getCdkeyTypes() {
		return cdkeyTypes;
	}

	public void setCdkeyTypes(String cdkeyTypes) {
		this.cdkeyTypes = cdkeyTypes;
	}

	public void addCdkeyType(String cdkeyType){
    	if(StringUtils.isBlank(this.cdkeyTypes))
    	{
    		this.cdkeyTypes=cdkeyType;
    	}
    	else 
    	{
			this.cdkeyTypes=this.cdkeyTypes+","+cdkeyType;
		}
    }

    public int getSysMsgTime() {
        return sysMsgTime;
    }

    public void setSysMsgTime(int sysMsgTime) {
        this.sysMsgTime = sysMsgTime;
    }
    
    public int getPieceskilltimes(){
    	return pieceskilltimes;
    }
    
    public void setPieceskilltimes(int times){
    	this.pieceskilltimes = times;
    }
    
    public int getGetRebate() {
		return getRebate;
	}

	public void setGetRebate(int getRebate) {
		this.getRebate = getRebate;
	}

	public void addPieceskilltimes(int times){
    	this.pieceskilltimes += times;
    }

	public int getChargeHeap() {
		return chargeHeap;
	}

	public void setChargeHeap(int chargeHeap) {
		this.chargeHeap = chargeHeap;
	}

	public int getChargeTerm() {
		return chargeTerm;
	}

	public void setChargeTerm(int chargeTerm) {
		this.chargeTerm = chargeTerm;
	}

	public int getFortuneComeTimes() {
		return fortuneComeTimes;
	}

	public void setFortuneComeTimes(int fortuneComeTimes) {
		this.fortuneComeTimes = fortuneComeTimes;
	}

	public int getGiveSoulTerm() {
		return giveSoulTerm;
	}

	public void setGiveSoulTerm(int giveSoulTerm) {
		this.giveSoulTerm = giveSoulTerm;
	}

	public int getGiveSoulHeap3() {
		return giveSoulHeap3;
	}

	public void setGiveSoulHeap3(int giveSoulHeap3) {
		this.giveSoulHeap3 = giveSoulHeap3;
	}

	public int getGiveSoulHeap2() {
		return giveSoulHeap2;
	}

	public void setGiveSoulHeap2(int giveSoulHeap2) {
		this.giveSoulHeap2 = giveSoulHeap2;
	}

	public int getGiveSoulDifferTimes2() {
		return giveSoulDifferTimes2;
	}

	public void setGiveSoulDifferTimes2(int giveSoulDifferTimes2) {
		this.giveSoulDifferTimes2 = giveSoulDifferTimes2;
	}

	public int getGiveSoulDifferTimes3() {
		return giveSoulDifferTimes3;
	}

	public void setGiveSoulDifferTimes3(int giveSoulDifferTimes3) {
		this.giveSoulDifferTimes3 = giveSoulDifferTimes3;
	}

	public int getFightEndTimes() {
		return fightEndTimes;
	}

	public void setFightEndTimes(int fightEndTimes) {
		this.fightEndTimes = fightEndTimes;
	}

	public int getLuckyStarTermid() {
		return luckyStarTermid;
	}

	public void setLuckyStarTermid(int luckyStarTermid) {
		this.luckyStarTermid = luckyStarTermid;
	}

	public int getLuckyStarDropTimes1() {
		return luckyStarDropTimes1;
	}

	public void setLuckyStarDropTimes1(int luckyStarDropTimes1) {
		this.luckyStarDropTimes1 = luckyStarDropTimes1;
	}

	public int getLuckyStarDropTimes2() {
		return luckyStarDropTimes2;
	}

	public void setLuckyStarDropTimes2(int luckyStarDropTimes2) {
		this.luckyStarDropTimes2 = luckyStarDropTimes2;
	}

	public int getLuckyStayHeap1() {
		return luckyStayHeap1;
	}

	public void setLuckyStayHeap1(int luckyStayHeap1) {
		this.luckyStayHeap1 = luckyStayHeap1;
	}

	public int getLuckyStayHeap2() {
		return luckyStayHeap2;
	}

	public void setLuckyStayHeap2(int luckyStayHeap2) {
		this.luckyStayHeap2 = luckyStayHeap2;
	}

	public int getChallengeActTermId() {
		return challengeActTermId;
	}

	public void setChallengeActTermId(int challengeActTermId) {
		this.challengeActTermId = challengeActTermId;
	}

	public int getHighestRankInAct() {
		return highestRankInAct;
	}

	public void setHighestRankInAct(int highestRankInAct) {
		this.highestRankInAct = highestRankInAct;
	}

	public int getNextQuickCareerTime() {
		return nextQuickCareerTime;
	}

	public void setNextQuickCareerTime(int nextQuickCareerTime) {
		this.nextQuickCareerTime = nextQuickCareerTime;
	}

	public int getHasGotFriendInfo() {
		return hasGotFriendInfo;
	}

	public void setHasGotFriendInfo(int hasGotFriendInfo) {
		this.hasGotFriendInfo = hasGotFriendInfo;
	}
	
	public String getDeepWaterLastTimes() {
		return deepWaterLastTimes;
	}

	public void setDeepWaterLastTimes(String deepWaterLastTimes) {
		this.deepWaterLastTimes = deepWaterLastTimes;
	}

	public String getDeepWaterHeap() {
		return deepWaterHeap;
	}

	public void setDeepWaterHeap(String deepWaterHeap) {
		this.deepWaterHeap = deepWaterHeap;
	}

	public int getDeepWaterTermId() {
		return deepWaterTermId;
	}

	public void setDeepWaterTermId(int deepWaterTermId) {
		this.deepWaterTermId = deepWaterTermId;
	}

	public int getDwNextRefreshTime() {
		return dwNextRefreshTime;
	}

	public void setDwNextRefreshTime(int dwNextRefreshTime) {
		this.dwNextRefreshTime = dwNextRefreshTime;
	}
	
}

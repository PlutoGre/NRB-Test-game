package com.oldmountain.dzm.entity;

import com.oldmountain.dzm.message.FightEndInfo;
import com.oldmountain.dzm.services.FightEndRankingServices;
import com.oldmountain.dzm.util.AdventureFightEndUtil;
import com.oldmountain.dzm.util.TimeUtil;
import com.oldmountain.dzm.util.Util;
import net.sf.json.JSONArray;
import net.sf.json.JSONObject;

import java.util.HashMap;

/*
 * Created with IntelliJ IDEA.
 * User: dylan
 * Date: 13-4-2
 * Time: 上午10:06
 * To change this template use File | Settings | File Templates.
 */
public class AdventureFightStats
{
    private int id;
    private int playerId;

    //当前战斗面板
    private int battlePanelType;
    private int barrierCount;
    //已进行战役统计
    private int battleAmount;
    //昨日通关关卡
    private int yestBarrierLV;
    //昨日获得评分
    private int yestObtainStar;
    //今日最大攻关等级
    private int maxBarrierLV;
    //今日最多得星数
    private int maxObtainStar;
    //当前剩余得星数量
    private int currSurplusStar;
    //当前所在关卡，非通过关卡等级
    private int currBarrierLV;
    //当前获得评分
    private int currObtainStar;
    //阶段性得星最多详细信息
    private JSONObject obtainStarDetail;
    //属性加成信息
    private JSONObject proAdditionInfo;
    //属性选择面板配置信息
    private JSONObject additionSelDetail;
    //阶段性奖励领取过的额外奖励信息
    private JSONObject extraRewardDetailInfo;
    //阶段性领取奖励配置
    private JSONArray extraRewardConf;
    //队伍信息
    private JSONObject teamConfInfo;
    //最后更新时间
    private int lastUpdateTime;
    //昨日加成比例
    private int yestAddition;
    //当前阶段性得星
    private int _currStepObtainStar;
    //下一次需要生成选择奖励属性的关卡等级
    private int _nextSelBarrierLevel;
    //下一次需要进行阶段性奖励数据的关卡等级
    private int _nextResetBarrierLevel;
    //上次加成变更类型，如无则无需重新计算阵容属性
    private int _addType;
    //下次同步时间,用于同步数据库数据信息
    private int _nextSyncTime;
    //上一次操作时的时期时间
    private int lastOprTime;
    //最后一次生成每5关卡获得额外奖励的配置值
    private int _lastGetExtraRewardStar;
    //每日数据重置时间，重置后时间调整为下一次时间
    private int nextResetTime;
    //克隆玩家数据信息
    private HashMap<Integer, Battle> battlearray;//玩家的阵容信息
    //预测今日排名
    private int calculateTodayRanking;
    private int continueInRankAmount=0;
    private int isNewStatus=1;
    public AdventureFightStats()
    {
        this.yestAddition=-1;
        this.playerId=-1;
        this.barrierCount=-1;
        this.battlePanelType=0;
        this.battleAmount=0;
        this.yestBarrierLV=0;
        this.yestObtainStar=0;
        this.maxBarrierLV=0;
        this.maxObtainStar=0;
        this.currSurplusStar=0;
        this.currBarrierLV=1;
        this.currObtainStar=0;
        this._currStepObtainStar =0;
        this._addType=0;
        this.obtainStarDetail=new JSONObject();
        this.extraRewardDetailInfo=new JSONObject();
        this.proAdditionInfo=new JSONObject();
        this.additionSelDetail=new JSONObject();
        this.teamConfInfo=new JSONObject();
        this.extraRewardConf=new JSONArray();
        this._nextResetBarrierLevel=AdventureFightEndUtil.getInstance().getNextResetStepBarrier(currBarrierLV);
        this._nextSelBarrierLevel=AdventureFightEndUtil.getInstance().getNextAdditionAddSelBarrier(currBarrierLV);
        this.nextResetTime=TimeUtil.getNextDayMinTime();
        this.calculateTodayRanking=0;
        this.continueInRankAmount=-1;
        this.isNewStatus=1;
    }

    public int getContinueInRankAmount()
    {
        if(continueInRankAmount==-1)
        {
            this.continueInRankAmount=FightEndRankingServices.YestInRankAmountByPlayer(playerId, barrierCount);
        }
        return continueInRankAmount;
    }

    public void setContinueInRankAmount(int continueInRankAmount)
    {
        this.continueInRankAmount = continueInRankAmount;
    }

    public int getLastUpdateTime()
    {
        return lastUpdateTime;
    }

    public void setLastUpdateTime(int lastUpdateTime)
    {
        this.lastUpdateTime = lastUpdateTime;
        this.getNextSyncTime();
    }

    public int getNextSyncTime()
    {
        return this._nextSyncTime;
    }

    public int getId() {
        return id;
    }

    public void setId(int id) {
        this.id = id;
    }

    public int getPlayerId() {
        return playerId;
    }

    public void setPlayerId(int playerId)
    {
        this.playerId = playerId;
    }

    public int getBarrierCount() {
        return barrierCount;
    }

    public void setBarrierCount(int barrierCount)
    {
        this.barrierCount = barrierCount;
    }

    public int getBattlePanelType() {
        return battlePanelType;
    }

    public void setBattlePanelType(int battlePanelType)
    {
        this.battlePanelType = battlePanelType;
    }

    public int getBattleAmount() {
        return battleAmount;
    }

    public void setBattleAmount(int battleAmount) {
        this.battleAmount = battleAmount;
    }

    public int getYestBarrierLV() {
        return yestBarrierLV;
    }

    public void setYestBarrierLV(int yestBarrierLV) {
        this.yestBarrierLV = yestBarrierLV;
    }

    public int getYestObtainStar() {
        return yestObtainStar;
    }

    public void setYestObtainStar(int yestObtainStar) {
        this.yestObtainStar = yestObtainStar;
    }

    public int getMaxBarrierLV()
    {
        return maxBarrierLV;
    }

    public void setMaxBarrierLV(int maxBarrierLV) {
        this.maxBarrierLV = maxBarrierLV;
    }

    public int getMaxObtainStar() {
        return maxObtainStar;
    }

    public void setMaxObtainStar(int maxObtainStar) {
        this.maxObtainStar = maxObtainStar;
    }

    public int getCurrBarrierLV() {
        return currBarrierLV;
    }

    public void setCurrBarrierLV(int currBarrierLV)
    {
        this.currBarrierLV = currBarrierLV;
        this._nextResetBarrierLevel=AdventureFightEndUtil.getInstance().getNextResetStepBarrier(currBarrierLV);
        this._nextSelBarrierLevel=AdventureFightEndUtil.getInstance().getNextAdditionAddSelBarrier(currBarrierLV);
    }

    public int getCurrObtainStar() {
        return currObtainStar;
    }

    public void setCurrObtainStar(int currObtainStar)
    {
        this.currObtainStar = currObtainStar;
    }

    public int getCurrSurplusStar() {
        return currSurplusStar;
    }

    public void setCurrSurplusStar(int currSurplusStar) {
        this.currSurplusStar = currSurplusStar;
    }

    public String getObtainStarDetail() {
        return obtainStarDetail.toString();
    }

    public void setObtainStarDetail(String obtainStarDetail)
    {
        this.obtainStarDetail=JSONObject.fromObject(obtainStarDetail);
    }

    public String getProAdditionInfo()
    {
        return proAdditionInfo.toString();
    }

    public void setProAdditionInfo(String proAdditionInfo)
    {
        this.proAdditionInfo=JSONObject.fromObject(proAdditionInfo);
    }

    public String getAdditionSelDetail()
    {
        return additionSelDetail.toString();
    }

    public JSONObject getAdditionSelDetailJson()
    {
        return additionSelDetail;
    }

    public void setAdditionSelDetail(String additionSelDetail)
    {
        this.additionSelDetail=JSONObject.fromObject(additionSelDetail);
    }

    public void setAdditionSelDetailFromJson(JSONObject js)
    {
        this.additionSelDetail=js;
    }

    public String getExtraRewardDetailInfo()
    {
        return extraRewardDetailInfo.toString();
    }

    public JSONObject getExtraRewardDetailInfoJson()
    {
        return extraRewardDetailInfo;
    }

    public void setExtraRewardDetailInfo(String additionSelDetail)
    {
        this.extraRewardDetailInfo=JSONObject.fromObject(additionSelDetail);
    }


    public String getExtraRewardConf()
    {
        return extraRewardConf.toString();
    }

    public JSONArray getExtraRewardConfJsonArray()
    {
        return extraRewardConf;
    }

    public void setExtraRewardConf(String jsonStr)
    {
        this.extraRewardConf=JSONArray.fromObject(jsonStr);
    }

   public void setExtraRewardConfFromJsonArr(JSONArray ja)
   {
       this.extraRewardConf=ja;
   }



    public String getTeamConfInfo()
    {
        return teamConfInfo.toString();
    }

    public JSONObject getTeamConfJson()
    {
        return this.teamConfInfo;
    }

    public void setTeamConfInfo(String teamConfInfo)
    {
        this.teamConfInfo=JSONObject.fromObject(teamConfInfo);
    }

    public void setTeamConfFromJson(JSONObject conf)
    {
        this.teamConfInfo=conf;
    }

    public int getLastGetExtraRewardStar()
    {
        return _lastGetExtraRewardStar;
    }

    public void setLastGetExtraRewardStar(int _lastGetExtraRewardStar)
    {
        this._lastGetExtraRewardStar = _lastGetExtraRewardStar;
    }


    public int getLastOprTime()
    {
        return lastOprTime;
    }

    public void setLastOprTime(int lastOprTime)
    {
        this.lastOprTime = lastOprTime;
    }

    public int getNextResetTime() {
        return nextResetTime;
    }

    public void setNextResetTime(int nextResetTime) {
        this.nextResetTime = nextResetTime;
    }

    public int getCalculateTodayRanking() {
        return calculateTodayRanking;
    }

    public void setCalculateTodayRanking(int calculateTodayRanking)
    {
        this.calculateTodayRanking = calculateTodayRanking;
    }

    public int getNewStatus() {
        return isNewStatus;
    }

    public void setNewStatus(int newStatus) {
        isNewStatus = newStatus;
    }

    /**
     * 获取昨日加成值
     * @return
     */
    public int getYestAddition()
    {
        if(this.yestAddition==-1)
        {
            this.yestAddition=AdventureFightEndUtil.getYestAdditionRateByStar(this.yestObtainStar);
        }
        return this.yestAddition;
    }

    /**
     * 添加阶段性领取额外奖励的最大值,记录本次关卡领取过的额外奖励最大值
     */
    public void increaseExtraRewardInfo()
    {
        this.extraRewardDetailInfo.put(this.currBarrierLV-1,getLastGetExtraRewardStar());
    }

    /**
     * 获得阶段性已经领取额外奖励最大值
     * @return
     */
    public int getMaxExtraStarInfoByBarrier()
    {
        int lv= this.currBarrierLV-1;
        if(this.extraRewardDetailInfo.containsKey(String.valueOf(this.currBarrierLV-1)))
        {
            return this.extraRewardDetailInfo.getInt(String.valueOf(lv));
        }
        else
        {
            return 0;
        }
    }

    /**
     * 每日剩余战斗次数
     * @return
     */
    public int getSurplusDailyBarrierCount()
    {
        return AdventureFightEndUtil.DAILY_BARRIER_COUNT-getBattleAmount();
    }

    /**
     * 阶段性领取奖励计算总结得星
     */
    public void calcStepReward()
    {
        if((this.currBarrierLV-1)==_nextResetBarrierLevel)
        {
            //记录每5关得星最高记录信息
            int _stepStar=getCurrStepLevelMaxObtainStar();
            if(_stepStar< _currStepObtainStar)
            {
                obtainStarDetail.put((currBarrierLV-1), _currStepObtainStar);
            }

            //清空每5关卡获得评分
            _currStepObtainStar =0;
            _nextResetBarrierLevel=AdventureFightEndUtil.getInstance().getNextResetStepBarrier(currBarrierLV);
        }
        getNextPanelType(true);
    }

    /**
     * 获得星
     * @param _obtainStar
     */
    private void increaseObtainStar(int _obtainStar)
    {
        //将战斗得星增加到当前得星内
        this.currObtainStar+=_obtainStar;
        //将战斗得星添加到剩余得星内
        this.currSurplusStar+=_obtainStar;
        //当前血战得星是否为今天最大得星
        if(this.currObtainStar>this.maxObtainStar)
        {
            this.maxObtainStar=this.currObtainStar;
        }
        if(this.currBarrierLV>this.maxBarrierLV)
        {
            this.maxBarrierLV=this.currBarrierLV-1;
        }
        //记录阶段性关卡得星信息
        _currStepObtainStar +=_obtainStar;
    }

    /**
     * 属性加成扣除星
     * @param index
     */
    public void deductObtainStarByIndex(int index)
    {
        int _star=0;
        try
        {
            _star=AdventureFightEndUtil.getDeducteStarByIndex(index);
        }
        catch (Exception e)
        {
            _star=0;
        }
        this.currSurplusStar-=_star;
        if(this.currSurplusStar<=0)
        {
            this.currSurplusStar=0;
        }
        this._nextSelBarrierLevel=AdventureFightEndUtil.getInstance().getNextAdditionAddSelBarrier(this.currBarrierLV);
    }

    /**
     * 添加血战临时加成属性
     * @param index
     */
    public void addObtainTempAdditionByIndex(int index)
    {
        int type=0;
        if(additionSelDetail.containsKey(String.valueOf(index)))
        {
            type=additionSelDetail.getInt(String.valueOf(index));
            if(proAdditionInfo.containsKey(String.valueOf(type)))
            {
                int _add=proAdditionInfo.getInt(String.valueOf(type));
                if(_add!=0)
                {
                    _addType=type;
                    proAdditionInfo.put(type,_add+AdventureFightEndUtil.getAdditionRateByIndex(index));
                }
                else
                {
                    proAdditionInfo.put(type,AdventureFightEndUtil.getAdditionRateByIndex(index));
                }
            }
            else
            {
                proAdditionInfo.put(type,AdventureFightEndUtil.getAdditionRateByIndex(index));
            }
        }
    }

    /**
     * 添加临时属性信息
     * @param index
     */
    public void increaseTempAdditionByIndex(int index)
    {
        addObtainTempAdditionByIndex(index);
        deductObtainStarByIndex(index);
    }
    /**
     * 添加昨日属性加成信息，通过属性类型
     * @param type
     */
    public void increaseYestAdditionByType(int type)
    {
        battleAmount++;
        proAdditionInfo=new JSONObject();
        proAdditionInfo.put(type,getYestAddition());
    }

    public void increaseBattleAmount()
    {
        battleAmount++;
        proAdditionInfo=new JSONObject();
    }

    /**
     * 获得阶段性得星数量
     * @return
     */
    public int getStepObtainStar()
    {
        return this._currStepObtainStar;
    }

    /**
     * 本次加成类型，根据本次加成调整我方血战信息部分属性数值信息，不全量更新
     * @return
     */
    public int getAddAdditionType()
    {
        return _addType;
    }

    /**
     * 获取临时加成值
     * @param type
     * @return
     */
    public int getAdditionRateByType(int type)
    {
        int _addValue=0;
        if(proAdditionInfo.containsKey(String.valueOf(type)))
        {
            _addValue=proAdditionInfo.getInt(String.valueOf(type));
        }
        return _addValue;
    }

    /**
     * 是否展现奖励面板
     * @return
     */
    public boolean isRewardPanel()
    {
        return (currBarrierLV-1) == _nextResetBarrierLevel;
    }

    /**
     * 获取当前超过上次阶段奖励星级数据信息
     * @return
     */
    public int getCurrStepRewardStar()
    {
        int _stepStar=getCurrStepLevelMaxObtainStar();
        if(_stepStar< _currStepObtainStar)
        {
            return _currStepObtainStar-_stepStar;
        }
        return 0;
    }

    /**
     * 获取阶段性关卡最大得星数
     * @return
     */
    public int getCurrStepLevelMaxObtainStar()
    {
        int _stepStar=0;
        if(obtainStarDetail.containsKey(String.valueOf(currBarrierLV-1)))
        {
            _stepStar=obtainStarDetail.getInt(String.valueOf(currBarrierLV-1));
        }
        return  _stepStar;
    }

    /**
     * 获取阶段性关卡最大得星数
     * @return
     */
    public int getCurrStepLevelMaxObtainStarByCurrLv()
    {
        int _stepStar=0;
        if(obtainStarDetail.containsKey(String.valueOf(currBarrierLV-1)))
        {
            _stepStar=obtainStarDetail.getInt(String.valueOf(currBarrierLV-1));
        }
        return  _stepStar;
    }

    /**
     * 闯关成功
     * @param obtainStar
     */
    public void passBarrier(int obtainStar)
    {
        this.currBarrierLV+=1;
        increaseObtainStar(obtainStar);
    }

    /**
     * 闯关失败->重置闯关数据信息
     */
    public void failBarrier()
    {
        System.out.println("玩家"+playerId+"攻打 第："+currBarrierLV+"关失败，重置关卡信息！");
        resetBarrierConf();
    }

    /**
     * 重置关卡配置等信息
     */
    public void resetBarrierConf()
    {
        currSurplusStar=0;
        currBarrierLV=1;
        currObtainStar=0;
        _currStepObtainStar =0;
        _addType=0;
        proAdditionInfo=new JSONObject();
        additionSelDetail=new JSONObject();
        teamConfInfo=new JSONObject();
        extraRewardConf=new JSONArray();
        _nextResetBarrierLevel=AdventureFightEndUtil.getInstance().getNextResetStepBarrier(currBarrierLV);
        _nextSelBarrierLevel=AdventureFightEndUtil.getInstance().getNextAdditionAddSelBarrier(currBarrierLV);
    }

    /**
     * 新的日期后重置为新的一个数据，并将数据记录到昨日信息中
     */
    public void resetBarrierByNextDay()
    {
        this.battleAmount=0;
        this.yestBarrierLV=this.maxBarrierLV;
        this.yestObtainStar=this.maxObtainStar;
        this.yestAddition=-1;
        getYestAddition();
        this.maxBarrierLV=0;
        this.maxObtainStar=0;
        this.calculateTodayRanking=0;
        this.obtainStarDetail=new JSONObject();
        this.extraRewardDetailInfo=new JSONObject();
        this.isNewStatus=1;
        resetBarrierConf();
        this.continueInRankAmount=FightEndRankingServices.YestInRankAmountByPlayer(playerId, barrierCount);
    }

    /**
     * 是否为奖励展现面板
     * @return
     */
    public boolean isSelAdditionPanel()
    {
        return (this.currBarrierLV - 1) == this._nextSelBarrierLevel;
    }
    /**
     * 获取下一次血战面板类型
     * @return
     */
    public void getNextPanelType(boolean isPass)
    {
        if(isPass)
        {
            if(isSelAdditionPanel())
            {
                this.battlePanelType= FightEndInfo.OPFightEndInfoRet.PanelType.StepAddAttr_VALUE;
                return ;
            }
            else if(isRewardPanel())
            {
                this.battlePanelType= FightEndInfo.OPFightEndInfoRet.PanelType.RewardPanel_VALUE;
                return ;
            }
            else
            {
                this.battlePanelType= FightEndInfo.OPFightEndInfoRet.PanelType.TeamPanel_VALUE;
                return ;
            }
        }
        else
        {
            this.battlePanelType=FightEndInfo.OPFightEndInfoRet.PanelType.MainPanel_VALUE;
        }
    }

    /**
     * 检测是否跨天
     */
    public boolean checkIsAcrossDay()
    {
        int serverTime=Util.getServerTime();
        if(serverTime>this.nextResetTime)
        {
            //同步排行榜数据
            FightEndRankingServices.checkNextDay(serverTime);
            resetBarrierByNextDay();
            this.nextResetTime=TimeUtil.getNextDayMinTime();
            return true;
        }
        return false;
    }

    /**
     * 设置下次同步时间
     */
    public void setNextSyncTime()
    {
        if(this._nextSyncTime==0)
        {
            this._nextSyncTime=this.getLastOprTime();
        }
        this.lastUpdateTime=this._nextSyncTime;
        this._nextSyncTime=AdventureFightEndUtil.getInstance().getNextSyncTime();
    }

    /**
     * 克隆玩家信息内的阵容信息
     * @param player
     */
    public void cloneBattleArray(Player player)
    {
        for (Integer integer : player.getBattlearray().keySet())
        {
            battlearray.put(integer,player.getBattlearray().get(integer).clone());
            calcBattlePropForTemp(integer);
        }
    }

    /**
     * 计算玩家血战临时数据加成信息
     * @param index
     */
    public void calcBattlePropForTemp(int index)
    {
       if(index!=-1)
       {
           Battle battle=battlearray.get(index);
           battle.setHealth(battle.getHealth()*getAdditionRateByType(FightEndInfo.OPFightEndInfoRet.Addition_Type.HP_VALUE));
           battle.setAttack(battle.getHealth()*getAdditionRateByType(FightEndInfo.OPFightEndInfoRet.Addition_Type.ATTATCK_VALUE));
           battle.setDefence(battle.getHealth()*getAdditionRateByType(FightEndInfo.OPFightEndInfoRet.Addition_Type.DEFENSE_VALUE));
           battle.setRp(battle.getHealth()*getAdditionRateByType(FightEndInfo.OPFightEndInfoRet.Addition_Type.WILL_VALUE));
       }
       else
       {
           if(battlearray!=null)
           {
               for (Integer integer : battlearray.keySet())
               {
                   Battle battle=battlearray.get(integer);
                   battle.setHealth(battle.getHealth()*getAdditionRateByType(FightEndInfo.OPFightEndInfoRet.Addition_Type.HP_VALUE));
                   battle.setAttack(battle.getHealth()*getAdditionRateByType(FightEndInfo.OPFightEndInfoRet.Addition_Type.ATTATCK_VALUE));
                   battle.setDefence(battle.getHealth()*getAdditionRateByType(FightEndInfo.OPFightEndInfoRet.Addition_Type.DEFENSE_VALUE));
                   battle.setRp(battle.getHealth()*getAdditionRateByType(FightEndInfo.OPFightEndInfoRet.Addition_Type.WILL_VALUE));
               }
           }

       }
    }
}

package com.oldmountain.dzm.services;

import com.oldmountain.dzm.entity.AdventureFightStats;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.message.FightEndInfo.OPFightEndInfoRet;
import com.oldmountain.dzm.util.AdventureFightEndUtil;
import com.oldmountain.dzm.util.BagUtil;
import com.oldmountain.dzm.util.UserToolConst;
import com.oldmountain.dzm.util.Util;
import net.sf.json.JSONArray;
import net.sf.json.JSONObject;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

/*
 * Created with IntelliJ IDEA.
 * User: dylan
 * Date: 13-4-2
 * Time: 下午8:56
 * To change this template use File | Settings | File Templates.
 */
public class AdventureFightStatsServices
{
    private static final Logger logger2 = LoggerFactory.getLogger("fightEndInfo");
    public static OPFightEndInfoRet.Builder getFightEndInfo(Player player,int panelType,boolean isGenrate,boolean isNewStats)
    {
        OPFightEndInfoRet.Builder _fightStatsBuilder=OPFightEndInfoRet.newBuilder();
        AdventureFightStats _adventureFightStats=player.getFightStats();
        _fightStatsBuilder.setPanelType(OPFightEndInfoRet.PanelType.valueOf(panelType));
        _adventureFightStats.setBattlePanelType(Integer.parseInt(String.valueOf(panelType)));
        player.getFightStats().setBarrierCount(player.getBattleCount());
        //血战排行每5分钟更新一次,
        //昨天血战信息
        _fightStatsBuilder.setYestFightInfo(getYestStats(player));
        //今日血战信息
        _fightStatsBuilder.setTodayFightInfo(getTodayStats(player));
        if(panelType==OPFightEndInfoRet.PanelType.TeamPanel_VALUE)
        { //FightTeamInfo 队伍信息
            _fightStatsBuilder.setTeamInfo(getTeamInfoByBarrierAndBattle(player,isGenrate));
        }
        else if(panelType==OPFightEndInfoRet.PanelType.StepAddAttr_VALUE)
        { //StepAdditionInfo 阶段性奖励信息
            _fightStatsBuilder.setStepAdditionInfo(getStepSelAddition(player,isGenrate));
        }
        else if(panelType==OPFightEndInfoRet.PanelType.RewardPanel_VALUE)
        {//optional RewardInfo 阶段性领取奖励配置信息
            _fightStatsBuilder.setRewardInfo(getRewardInfo(player,isGenrate));
        }
        int serverTime=Util.getServerTime();
        _adventureFightStats.setLastOprTime(serverTime);
        if(isNewStats)
        {
            _adventureFightStats.setNextSyncTime();
            DaoManager.getAdventureFightDao().addAdventureFightStats(player);
        }
        else
        {
            _adventureFightStats.setNextSyncTime();
            player.updateFightEndStats();
        }
        _fightStatsBuilder.setIsNewStatus(_adventureFightStats.getNewStatus());
        _fightStatsBuilder.setTodayRanking(player.getFightStats().getCalculateTodayRanking());
        _fightStatsBuilder.setContinueInRankAmount(player.getFightStats().getContinueInRankAmount());
        _fightStatsBuilder.setStatus(1);
        _fightStatsBuilder.setServertime(Util.getServerTime());
        return _fightStatsBuilder;
    }

    /**
     * 获得阶段性关卡奖励配置
     * @param player
     * @return
     */
    public static OPFightEndInfoRet.RewardInfo.Builder getRewardInfo(Player player,boolean isGenerate)
    {
        if(isGenerate||player.getFightStats().getExtraRewardConf().length()<5)
        {
            return generateRewardInfo(player);
        }
        OPFightEndInfoRet.RewardInfo.Builder rewardInfo=OPFightEndInfoRet.RewardInfo.newBuilder();
        AdventureFightStats fightStats=player.getFightStats();
        JSONArray ja=fightStats.getExtraRewardConfJsonArray();
        for(int i=0;i<ja.size();++i)
        {
            OPFightEndInfoRet.RewardInfo.RewardItem.Builder rewardItem=OPFightEndInfoRet.RewardInfo.RewardItem.newBuilder();
            rewardItem.setRewardtype(ja.getJSONObject(i).getInt("type"));
            rewardItem.setRewardid(ja.getJSONObject(i).getInt("itemId"));
            rewardItem.setRewardquantity(ja.getJSONObject(i).getInt("quantity"));
            rewardInfo.addRewardItem(rewardItem);
        }
        return  rewardInfo;
    }

    /**
     * 生成阶段性关卡奖励配置
     * @param player
     * @return
     */
    public static OPFightEndInfoRet.RewardInfo.Builder generateRewardInfo(Player player)
    {
        OPFightEndInfoRet.RewardInfo.Builder rewardInfo=OPFightEndInfoRet.RewardInfo.newBuilder();
        AdventureFightStats fightStats=player.getFightStats();
        //超出部分的得星数据信息
        int extraSilver=fightStats.getCurrStepRewardStar()*AdventureFightEndUtil.getInstance().getSilverByBarrierAndBattle(fightStats.getCurrBarrierLV()-1,player.getBattleCount());
        int extraEnd=AdventureFightEndUtil.getInstance().getExtraStarIdByStar(fightStats.getStepObtainStar());
        //记录额外奖励配置的max编号
        fightStats.setLastGetExtraRewardStar(extraEnd);
        int extraStar=fightStats.getMaxExtraStarInfoByBarrier();
        JSONArray ja=AdventureFightEndUtil.getInstance().getExtraRewardByBarrierStar(fightStats.getCurrBarrierLV()-1,extraStar,extraEnd);
        boolean addSilver=false;
        for(int i=0;i<ja.size();++i)
        {
            OPFightEndInfoRet.RewardInfo.RewardItem.Builder rewardItem=OPFightEndInfoRet.RewardInfo.RewardItem.newBuilder();
            if(ja.getJSONObject(i).getInt("itemId")==UserToolConst.USER_PROPERTY_SILVER_COINS)
            {
                addSilver=true;
                ja.getJSONObject(i).put("quantity",ja.getJSONObject(i).getInt("quantity")+extraSilver);
            }
            rewardItem.setRewardtype(ja.getJSONObject(i).getInt("type"));
            rewardItem.setRewardid(ja.getJSONObject(i).getInt("itemId"));
            rewardItem.setRewardquantity(ja.getJSONObject(i).getInt("quantity"));
            rewardInfo.addRewardItem(rewardItem);
        }
        if(!addSilver&&extraSilver>0)
        {
            OPFightEndInfoRet.RewardInfo.RewardItem.Builder rewardItem=OPFightEndInfoRet.RewardInfo.RewardItem.newBuilder();
            JSONObject _ja=new JSONObject();
            _ja.put("itemId",UserToolConst.USER_PROPERTY_SILVER_COINS);
            _ja.put("type",UserToolConst.USER_PROPERTY);
            _ja.put("quantity",extraSilver);
            rewardItem.setRewardtype(UserToolConst.USER_PROPERTY);
            rewardItem.setRewardid(UserToolConst.USER_PROPERTY_SILVER_COINS);
            rewardItem.setRewardquantity(extraSilver);
            rewardInfo.addRewardItem(rewardItem);
            ja.add(_ja);
        }
        fightStats.increaseExtraRewardInfo();
        fightStats.setExtraRewardConfFromJsonArr(ja);
        return  rewardInfo;
    }

    /**
     * 生成血战临时加成选择信息
     * @param player
     * @return
     */
   public static OPFightEndInfoRet.StepAdditionInfo.Builder generateStepSelAddition(Player player)
   {
       OPFightEndInfoRet.StepAdditionInfo.Builder stepAdditionInfo=OPFightEndInfoRet.StepAdditionInfo.newBuilder();
       int[] additionArr=AdventureFightEndUtil.getInstance().generateAdditionArr().clone();
       JSONObject stepAdditionSelConfig=new JSONObject();
       for(int i=1;i<additionArr.length+1;i++)
       {
           try
           {
               int percente=AdventureFightEndUtil.getAdditionRateByIndex(i);
               int requireStar=AdventureFightEndUtil.getDeducteStarByIndex(i);

               OPFightEndInfoRet.StepAdditionInfo.StepAdditionItem.Builder stepAdditionItem=OPFightEndInfoRet.StepAdditionInfo.StepAdditionItem.newBuilder();
               stepAdditionItem.setId(i);
               stepAdditionItem.setType(OPFightEndInfoRet.Addition_Type.valueOf(additionArr[i-1]));
               stepAdditionItem.setPercentage(percente);
               stepAdditionItem.setRequireStar(requireStar);
               stepAdditionInfo.addStepAddItem(stepAdditionItem);
               stepAdditionSelConfig.put(i,additionArr[i-1]);
           }
           catch (Exception e)
           {
               System.out.println("generateStepSelAddition:"+e.getMessage());
           }
       }
       //将生成的属性加成数据添加到配置信息中
       player.getFightStats().setAdditionSelDetailFromJson(stepAdditionSelConfig);
       return stepAdditionInfo;
   }

    /**
     * 获取血战临时加成选择信息
     * @param player
     * @return
     */
    public static OPFightEndInfoRet.StepAdditionInfo.Builder getStepSelAddition(Player player,boolean isGenrate)
    {
        OPFightEndInfoRet.StepAdditionInfo.Builder stepAdditionInfo=OPFightEndInfoRet.StepAdditionInfo.newBuilder();
        if(isGenrate||player.getFightStats().getAdditionSelDetailJson().toString().length()<5)
        {
             return generateStepSelAddition(player);
        }
        else
        {
            for(int i=1;i<=3;i++)
            {
                try
                {
                    OPFightEndInfoRet.StepAdditionInfo.StepAdditionItem.Builder stepAdditionItem=OPFightEndInfoRet.StepAdditionInfo.StepAdditionItem.newBuilder();
                    stepAdditionItem.setId(i);
                    stepAdditionItem.setType(OPFightEndInfoRet.Addition_Type.valueOf(player.getFightStats().getAdditionSelDetailJson().getInt(String.valueOf(i))));
                    stepAdditionItem.setPercentage(AdventureFightEndUtil.getAdditionRateByIndex(i));
                    stepAdditionItem.setRequireStar(AdventureFightEndUtil.getDeducteStarByIndex(i));
                    stepAdditionInfo.addStepAddItem(stepAdditionItem);
                }
                catch (Exception e)
                {
                  System.out.println("getStepSelAddition:"+e.getMessage());
                }
            }
            return stepAdditionInfo;
        }

    }

    /**
     * 昨日血战数据信息
     * @param player
     * @return
     */
    public static OPFightEndInfoRet.PassBarrierInfo.Builder getYestStats(Player player)
    {
        //optional todayFightInfo 今天血战信息
        OPFightEndInfoRet.PassBarrierInfo.Builder yestFightInfo=OPFightEndInfoRet.PassBarrierInfo.newBuilder();
        yestFightInfo.setMaxPassBarrier(player.getFightStats().getYestBarrierLV());
        yestFightInfo.setMaxObtainStar(player.getFightStats().getYestObtainStar());
        yestFightInfo.setAddition(player.getFightStats().getYestAddition());
        return yestFightInfo;
    }

    /**
     * 今日血战数据信息
     * @param player
     * @return
     */
    public static OPFightEndInfoRet.PassBarrierInfo.Builder getTodayStats(Player player)
    {
        //optional todayFightInfo 今天血战信息
        OPFightEndInfoRet.PassBarrierInfo.Builder todayFightInfo=OPFightEndInfoRet.PassBarrierInfo.newBuilder();
        todayFightInfo.setAttackCount(AdventureFightEndUtil.DAILY_BARRIER_COUNT);
        todayFightInfo.setSurplusAttackCount(player.getFightStats().getSurplusDailyBarrierCount());
        todayFightInfo.setCurrBarrier(player.getFightStats().getCurrBarrierLV());
        todayFightInfo.setCurrObtainStar(player.getFightStats().getCurrObtainStar());
        todayFightInfo.setMaxPassBarrier(player.getFightStats().getMaxBarrierLV());
        todayFightInfo.setMaxObtainStar(player.getFightStats().getMaxObtainStar());
        todayFightInfo.setStepObtainStar(player.getFightStats().getStepObtainStar());
        todayFightInfo.setExceedObtainStar(player.getFightStats().getCurrStepLevelMaxObtainStarByCurrLv());
        todayFightInfo.setCurrSurplusStar(player.getFightStats().getCurrSurplusStar());
        todayFightInfo.setAddition(player.getFightStats().getYestAddition());
        todayFightInfo.setTodayAddition(getAdditionStats(player));
        return todayFightInfo;
    }


    /**
     * 属性加成信息
     * @param player
     * @return
     */
    public static  OPFightEndInfoRet.TotalAdditionStatInfo.Builder getAdditionStats(Player player)
    {
        OPFightEndInfoRet.TotalAdditionStatInfo.Builder additionStatInfo=OPFightEndInfoRet.TotalAdditionStatInfo.newBuilder();
        additionStatInfo.setHp(player.getFightStats().getAdditionRateByType(OPFightEndInfoRet.Addition_Type.HP_VALUE));
        additionStatInfo.setAttack(player.getFightStats().getAdditionRateByType(OPFightEndInfoRet.Addition_Type.ATTATCK_VALUE));
        additionStatInfo.setDefense(player.getFightStats().getAdditionRateByType(OPFightEndInfoRet.Addition_Type.DEFENSE_VALUE));
        additionStatInfo.setWill(player.getFightStats().getAdditionRateByType(OPFightEndInfoRet.Addition_Type.WILL_VALUE));
        return  additionStatInfo;
    }


    /**
     * 生成血战队伍信息
     * @param player
     * @param barrier
     * @param battle
     * @return
     */
    private  static OPFightEndInfoRet.FightTeamInfo.Builder generateTeamInfoByBarrierAndBattle(Player player,int barrier,int battle)
    {
        OPFightEndInfoRet.FightTeamInfo.Builder teamInfo=OPFightEndInfoRet.FightTeamInfo.newBuilder();
        JSONObject teamConfig=new JSONObject();
        AdventureFightStats fightStats=player.getFightStats();
        int keyId=barrier<=3?0:AdventureFightEndUtil.getInstance().generateToolKeyId();
        int keyIndex=(int)(Math.random()*3+1);
        for(int i=1;i<=3;i++)
        {
            try
            {
                int npcId=AdventureFightEndUtil.getInstance().generateNpcIdByBattleAndPos(battle,i);
                int[] battleInfo=AdventureFightEndUtil.getInstance().generateMemberCountByBattleAndPos(battle,i);
                String[] oppontetBattleNpcId=AdventureFightEndUtil.getInstance().getOpponentMemberId(npcId,battleInfo[1]);
                JSONObject team=new JSONObject();
                team.put("ni",npcId);
                team.put("s",battleInfo[0]);
                team.put("o",battleInfo[1]);
                team.put("n",oppontetBattleNpcId);
                team.put("m",oppontetBattleNpcId[0]);
                if(keyId!=0)
                {
                   if(keyIndex==i)
                   {
                      team.put("k",keyId);
                   }
                   else
                   {
                       team.put("k",0);
                   }
                }
                else
                {
                    team.put("k",0);
                }
                OPFightEndInfoRet.FightTeamInfo.FightTeamItem.Builder teamItem=OPFightEndInfoRet.FightTeamInfo.FightTeamItem.newBuilder();

                teamItem.setId(i);
                teamItem.setMemberId(team.getInt("m"));
                teamItem.setSelfMember(team.getInt("s"));
                teamItem.setOpponentMember(team.getInt("o"));
                teamItem.setRewardKeyId(team.getInt("k"));
                teamInfo.addTeamItem(teamItem);
                teamConfig.put(i,team);
            }
            catch (Exception e)
            {
                 System.out.println("generateTeamInfoByBarrierAndBattle:"+e.getMessage());
            }
        }
        fightStats.setTeamConfFromJson(teamConfig);
        return teamInfo;
    }

    /**
     * 获取teamInfo
     * @param player
     * @param generate
     * @return
     */
    public static OPFightEndInfoRet.FightTeamInfo.Builder getTeamInfoByBarrierAndBattle(Player player,boolean generate)
    {
        OPFightEndInfoRet.FightTeamInfo.Builder teamInfo=OPFightEndInfoRet.FightTeamInfo.newBuilder();
        AdventureFightStats fightStats=player.getFightStats();
        int barrier=fightStats.getCurrBarrierLV();
        int battle= player.getBattleCount();
        if(fightStats.getTeamConfInfo().length()<5||generate)
        {
            return generateTeamInfoByBarrierAndBattle(player,barrier,battle);
        }
        else
        {
            for(int i=1;i<=3;i++)
            {
                try
                {
                    JSONObject team=fightStats.getTeamConfJson().getJSONObject(String.valueOf(i));
                    OPFightEndInfoRet.FightTeamInfo.FightTeamItem.Builder teamItem=OPFightEndInfoRet.FightTeamInfo.FightTeamItem.newBuilder();
                    teamItem.setId(i);
                    teamItem.setMemberId(team.getInt("m"));
                    teamItem.setSelfMember(team.getInt("s"));
                    teamItem.setOpponentMember(team.getInt("o"));
                    teamItem.setRewardKeyId(team.getInt("k"));
                    teamInfo.addTeamItem(teamItem);
                }
                catch (Exception e)
                {
                     System.out.println("getTeamInfoByBarrierAndBattle:"+e.getMessage());
                }
            }
            return  teamInfo;
        }
    }

}

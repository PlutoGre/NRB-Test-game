package com.oldmountain.dzm.packethandler;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;
import java.util.Random;

import org.apache.mina.common.IoSession;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.google.protobuf.InvalidProtocolBufferException;
import com.oldmountain.dzm.entity.Battle;
import com.oldmountain.dzm.entity.BattleArrayObject;
import com.oldmountain.dzm.entity.Career;
import com.oldmountain.dzm.entity.CareerXmlTemplate;
import com.oldmountain.dzm.entity.CharacterResult;
import com.oldmountain.dzm.entity.Disciple;
import com.oldmountain.dzm.entity.LuckyStarXmlTemplate;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.entity.QuickCareerConfig;
import com.oldmountain.dzm.entity.Result;
import com.oldmountain.dzm.entity.UserStats;
import com.oldmountain.dzm.message.OP;
import com.oldmountain.dzm.message.ErrorReporter.OPErrorReporter;
import com.oldmountain.dzm.message.QuickCareerBattle.OPQuickCareerBattle;
import com.oldmountain.dzm.message.QuickCareerBattle.OPQuickCareerBattleRet;
import com.oldmountain.dzm.message.QuickCareerBattle.OPQuickCareerBattleRet.Builder;
import com.oldmountain.dzm.message.UserBattle.OPUserBattle;
import com.oldmountain.dzm.message.UserBattleArray.OPUserBattleArrayRet;
import com.oldmountain.dzm.message.UserDiscipleInfo.OPUserDiscipleInfoRet;
import com.oldmountain.dzm.net.ObjectAccessor;
import com.oldmountain.dzm.net.packet.Packet;
import com.oldmountain.dzm.util.BagUtil;
import com.oldmountain.dzm.util.BattleUtil;
import com.oldmountain.dzm.util.DiscipleUtil;
import com.oldmountain.dzm.util.TitleUtil;
import com.oldmountain.dzm.util.UserToolConst;
import com.oldmountain.dzm.util.Util;
import com.oldmountain.dzm.util.XMLTemplateService;

public class QuickCareerBattleHandle {
	private static final Logger logger = LoggerFactory.getLogger("BattleArrayInfo");
	private static final Logger drop_log = LoggerFactory.getLogger("RewardDropActivate");
	public static void quickCareer(Packet packet,IoSession session){
		OPQuickCareerBattle params = null;
		OPQuickCareerBattleRet.Builder builder = OPQuickCareerBattleRet.newBuilder();
		OPErrorReporter.Builder errorBuilder = OPErrorReporter.newBuilder();
		
		try {
			params = OPQuickCareerBattle.parseFrom(packet.data.array());
		} catch (InvalidProtocolBufferException e) {
			e.printStackTrace();
			return;
		}
		int opponentID = params.getOpponentID();
		int stageId=params.getStageId();
		int version=params.getVersion();
		int battleTimes=0;
		int type=params.getType().getNumber();
		int current_time = Util.getServerTime();
		
		int playerId = (Integer)session.getAttribute("playerid");
		Player player = ObjectAccessor.getOnlineUser(playerId);
		if(params.hasBattleTimes()){
			battleTimes=params.getBattleTimes();
			if(battleTimes>10){
				errorBuilder.setErrorid(OP.code.OPCODE_QUICK_CAREER_BATTLERET_S_VALUE);
				errorBuilder.setErrormsg("times large then 10");
				player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errorBuilder);
				return;
			}
		}
		logger.info("quick career battle handler received: {}, {}, {}, {}, {}, {}, {}", new Object[]{playerId , version , type , opponentID , stageId ,battleTimes});
		if (type != OPUserBattle.Type.CARRER_VALUE) {
			errorBuilder.setErrorid(OP.code.OPCODE_QUICK_CAREER_BATTLERET_S_VALUE);
			errorBuilder.setErrormsg("type error");
			player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errorBuilder);
			return;
		}
		
		QuickCareerConfig quickCareerConfig=XMLTemplateService.getQuickCareerConfig();
		if(player.getViplevel()<quickCareerConfig.getVipLimit()){
			errorBuilder.setErrorid(OP.code.OPCODE_QUICK_CAREER_BATTLERET_S_VALUE);
			errorBuilder.setErrormsg("viplevel is not enough to do quickCareer");
			player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errorBuilder);
			return;
		}
		int costGold=0;
		//如果传入消耗金币 
		if(params.hasCostGold()){
			if(player.getViplevel()<quickCareerConfig.getVipResetLimit()){
				errorBuilder.setErrorid(OP.code.OPCODE_QUICK_CAREER_BATTLERET_S_VALUE);
				errorBuilder.setErrormsg("viplevel is not enough to reset cd");
				player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errorBuilder);
				return;
			}
			costGold=params.getCostGold();
			resetColdTime(player, costGold ,quickCareerConfig ,builder ,packet ,session);
			builder.setVersion(version);
			builder.setServertime(Util.getServerTime());
			player.sendPacket(OP.code.OPCODE_QUICK_CAREER_BATTLERET_S_VALUE, builder);
			logger.info("quick career reset cold time received: {}, {}, {}, {}, {}, {}, {}", new Object[]{playerId , opponentID , type ,builder.getBackwardsTimes()  ,builder.getUserGolds()});
			return;
			
		}
		Career career=player.getCareer();
		String careerValuate=career.getPointevaluate();
		Map<String, Integer> careerValueMap=Util.jsonToMap(careerValuate);
		String satgeid=String.valueOf(stageId);
		if(!careerValueMap.containsKey(satgeid)){
			errorBuilder.setErrorid(OP.code.OPCODE_QUICK_CAREER_BATTLERET_S_VALUE);
			errorBuilder.setErrormsg("can not do quick career");
			player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errorBuilder);
			return;
		}
		//初始化玩家幸运星活动的相关数据
    	Util.initLuckyStarActivit(current_time, player);
		//每轮航行增加的冷却时间基数
		int basecoldTime=quickCareerConfig.getBasecoldTime();	
		UserStats userStats=player.getUserStats();
		int nextQuickCareerTime=userStats.getNextQuickCareerTime();
		int backWardsTimes=nextQuickCareerTime-current_time;
		if(backWardsTimes>0){
			errorBuilder.setErrorid(OP.code.OPCODE_QUICK_CAREER_BATTLERET_S_VALUE);
            errorBuilder.setErrormsg("backWardsTimes is not finish");
            player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errorBuilder);
            return;
		}
		/********************************************************************************************************/
		Map<String,Integer> countMap = Util.jsonToMap(career.getPointcount());
        if(player.getPowerbytime()+player.getPowerbychicken()<battleTimes)
        {
            errorBuilder.setErrorid(OP.code.OPCODE_QUICK_CAREER_BATTLERET_S_VALUE);
            errorBuilder.setErrormsg("power not enough");
            player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errorBuilder);
            return;
        }
        CareerXmlTemplate careerInfo = XMLTemplateService.getCareerTemplate(stageId);	
        if (countMap.containsKey(String.valueOf(stageId)) && (countMap.get(String.valueOf(stageId))+battleTimes) > careerInfo.getCountLimit()) {
        	errorBuilder.setErrorid(OP.code.OPCODE_QUICK_CAREER_BATTLERET_S_VALUE);
            errorBuilder.setErrormsg("over count limit");
            player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errorBuilder);
            return;
		}
        //TODO
        
        logger.info("todo battle: {}, {}, {}, {}", new Object[]{battleTimes , careerInfo.getCountLimit() , player.getPowerbytime()+player.getPowerbychicken() ,basecoldTime});
        try {
	        for(int j=0;j<battleTimes;j++){
				BattleArrayObject userBattleInfo = BattleUtil.getUserBattleArrayObject(player);
				ArrayList<CharacterResult> characterResults = new ArrayList<CharacterResult>();
				Result result = new Result();
				int fightResult=2;
				result.setWinlose(fightResult);
				boolean careerGotItem = false;
				if (fightResult <= 2) {//战斗胜利
					int gainCoins = careerInfo.getGainCoins();
					int gainExp = careerInfo.getGainExp();
					int gainUserExp = XMLTemplateService.getUserLevelConfig(player.getLevel()).getCareerXp();
					
					if (countMap.containsKey(String.valueOf(stageId))) {
						int newCount = countMap.get(String.valueOf(stageId)) + 1;
						countMap.put(String.valueOf(stageId), newCount);
					}
					else {
						countMap.put(String.valueOf(stageId), 1);
					}
					career.setPointcount(Util.mapToString(countMap));
					String pointeValuate = career.getPointevaluate();
					Map<String,Integer> valuateMap = Util.jsonToMap(pointeValuate);
					String stage = String.valueOf(stageId);
					
					result.setGotCoin(gainCoins);
					result.setGotExp(gainUserExp);
					
					if (!valuateMap.containsKey(stage) || valuateMap.get(stage) < 3 - fightResult) {
						valuateMap.put(stage, 3 - fightResult);
					}
					if (stageId == career.getCurrentpoint()) {
						
						career.setCurrentpoint(careerInfo.getUnlockId());
						if (career.getCurrentchapter() != careerInfo.getUnlockId()/100) {
							career.setCurrentchapter(careerInfo.getUnlockId()/100);
						}
					}
					career.setPointevaluate(Util.mapToString(valuateMap));
					player.setCareer(career);
					ArrayList<Integer> dropRates = careerInfo.getDropRates();
					Random random = new Random();
					int a = random.nextInt(100) + 1;
					int index = dropRates.size();
					for (int i = 0; i < dropRates.size(); i++) {
						if (a <= dropRates.get(i)) {
							index = i;
							break;
						}
					}
					if (index < dropRates.size()) {
						int[] drops = careerInfo.getDropItems().get(index);
						ArrayList<Integer> rewards = Util.addRewards(player, drops);
						result.setGotItemType(rewards.get(0));
						result.setGotItemId(rewards.get(1));
						result.setGotItemCount(rewards.get(2));
						careerGotItem = true;
					}
					//检测称号
					TitleUtil.checkNewTitle(player, 1, stageId);
		
					//弟子属性变化
					ArrayList<Battle> userBattles = userBattleInfo.getBattlearrays();
					for (int i = 0; i < userBattles.size(); i++) {
						CharacterResult characterResult = new CharacterResult();
						Battle battle = userBattles.get(i);
						if (battle.getDiscipleid() == 0) {
							continue;
						}
						Disciple disciple = player.getDisciples().get(battle.getDiscipleid());
						int addLevel = Util.getlevelByExp(disciple, gainExp);
						if (addLevel > 0) {//弟子升级
							OPUserDiscipleInfoRet.DiscipleInfo.Builder discipleBuilder = DiscipleUtil.buildDiscipleBuilder(disciple);
							builder.addDisciple(discipleBuilder);
							characterResult.setIsUpgrade(true);
							BattleUtil.resetBattleProperty(battle, disciple,player,null,0);
							OPUserBattleArrayRet.BattleArrayInfo.Builder battleBuilder = OPUserBattleArrayRet.BattleArrayInfo.newBuilder();
							BattleUtil.buildBattleBuilder(battleBuilder, battle, i);
							builder.addBattleArrayInfo(battleBuilder);
							player.updateOneBattle(battle);
						}
						else {
							characterResult.setIsUpgrade(false);
						}
						characterResult.setItemid(disciple.getItemid());
						characterResult.setGainExp(gainExp);
						characterResult.setLevel(disciple.getLevel());
						characterResults.add(characterResult);
						player.updateOneDisciple(disciple);
					}
					
					player.setSilvercoins(player.getSilvercoins() + gainCoins);
					Util.getUserLevelByExp(player, gainUserExp);
				}
			
			if (careerGotItem == false) {
				HashMap<String, ArrayList<Integer>> usePowerRewardConfig = XMLTemplateService.getUsePowerReward();
				ArrayList<Integer> rates = usePowerRewardConfig.get("rates");
				int index = rates.size();
				Random random = new Random();
				int rand = random.nextInt(1000) + 1;
				for (int i = 0; i < rates.size(); i++) {
					if (rand <= rates.get(i)) {
						index = i;
						break;
					}
				}
				
				if (index < rates.size()) {
					ArrayList<Integer> types = usePowerRewardConfig.get("types");
					ArrayList<Integer> ids = usePowerRewardConfig.get("ids");
					ArrayList<Integer> quantitys = usePowerRewardConfig.get("quantities");
					int dropType = types.get(index);
					int dropId = ids.get(index);
					int quantity = quantitys.get(index);
					int [] drops = Util.getRewardIdByType(dropType, dropId, quantity, player.getLevel(), 2,careerInfo.getChapterId());
					/**********************************************幸运星掉落判定**********************************************************/
					int itemId=Util.getActivityTermId(Util.getServerMillTime(), XMLTemplateService.getluckyStarTimeConfigs());
					LuckyStarXmlTemplate luckyStarXmlTemplate1=XMLTemplateService.getLuckyStarXmlTemplate(1);
					LuckyStarXmlTemplate luckyStarXmlTemplate2=XMLTemplateService.getLuckyStarXmlTemplate(2);
					//活动开启判定
					if(itemId!=0){
						if(drops[0]== luckyStarXmlTemplate1.getItemId()){
							int dropTimes=userStats.getLuckyStarDropTimes1();
							//如果达到掉落次数上限,则不掉落
							if(dropTimes>=luckyStarXmlTemplate1.getMaxDropCount()){
								drops[0]=0;
							}
							else{
								userStats.setLuckyStarDropTimes1(userStats.getLuckyStarDropTimes1()+1);
							}
						}
						if(drops[0]== luckyStarXmlTemplate2.getItemId()){
							int dropTimes=userStats.getLuckyStarDropTimes2();
							//如果达到掉落次数上限,则不掉落
							if(dropTimes>=luckyStarXmlTemplate2.getMaxDropCount()){
								drops[0]=0;
							}
							else{
								userStats.setLuckyStarDropTimes2(userStats.getLuckyStarDropTimes2()+1);
							}
						}
					}
					else 
					{
						//不再活动时间内,则不会掉落(避免配置中存在幸运星掉落的可能性)
						if(drops[0]== luckyStarXmlTemplate1.getItemId() || drops[0]== luckyStarXmlTemplate2.getItemId()){
							drops[0]=0;
						}
					}
					
					/***********************************************幸运星掉落判定*********************************************************/
					if (drops[0] != 0) {
						int[] reward = new int[]{dropType,drops[0],drops[1]};
						ArrayList<Integer> rewards = Util.addRewards(player, reward);
						result.setGotItemType(rewards.get(0));
						result.setGotItemId(rewards.get(1));
						result.setGotItemCount(rewards.get(2));
						careerGotItem = true;
						int termId=Util.getActivityTermId(current_time, XMLTemplateService.getDropTimeConfigs());
						ArrayList<Integer> dropConfig=XMLTemplateService.getDropCountConfig();
						//如果掉落的
						if(termId!=0 && dropType==dropConfig.get(0) && rewards.get(1)== dropConfig.get(1)){
							drop_log.info("record for  drop count:------{},{},{},{},{}", new Object[]{playerId ,Util.getServerTime() ,dropType,rewards.get(1),rewards.get(2)});
						}
		                //添加宝箱运气池的运气值
		                if(rewards.get(0)==0||rewards.get(0)==1)
		                {
		                    if(UserToolConst.getType(rewards.get(1))==UserToolConst.TREASURE_TOOL)
		                    {
		                        BagUtil.getInstance().addLuckPool(player,rewards.get(1),rewards.get(2), XMLTemplateService.getSysBasicConfig().getCareerLuckyProportion());
		                    }
		                }
					}
				}
			}
			result.setCharacterResults(characterResults);
			builder.addResult(UserBattleHandler.getResult(player,result));
        }	
        } catch (Exception e) {
        	logger.info("quickCareer Exception : {}, {}, {}", new Object[]{e.getMessage() , stageId });
		}
        player.decreasePowerBySys(battleTimes);
		player.setLastVerifyTime(Util.getServerTime());
		userStats.setNextQuickCareerTime(current_time+battleTimes*basecoldTime);
		player.setUserStats(userStats);
		builder.setUserPower(player.getPowerbychicken() + player.getPowerbytime());		
		builder.setVersion(version);
		builder.setStats(0);
		builder.setUserGolds(player.getGoldCoins());
		builder.setBackwardsTimes(userStats.getNextQuickCareerTime()-current_time);
		builder.setServertime(Util.getServerTime());
		logger.info("quickCareer send packet : {}, {}, {}", new Object[]{ player.getGoldCoins(),player.getUserStats().getNextQuickCareerTime() ,userStats.getNextQuickCareerTime()-current_time });
		player.sendPacket(OP.code.OPCODE_QUICK_CAREER_BATTLERET_S_VALUE, builder);
		
		
		
	}
	private static void resetColdTime(Player player , int costGold ,QuickCareerConfig quickCareerConfig, Builder builder, Packet packet, IoSession session){
		OPErrorReporter.Builder errorBuilder = OPErrorReporter.newBuilder();
		int current_time=Util.getServerTime();
		UserStats userStats=player.getUserStats();
		int nextQuickCareerTime=userStats.getNextQuickCareerTime();
		int backwardsTime=nextQuickCareerTime-current_time;
		double backwards_time=backwardsTime;
		int divisor=quickCareerConfig.getDivisor();
		if(backwardsTime<=0){
			builder.setBackwardsTimes(0);
		}
		int basePrice=quickCareerConfig.getBasePrice();
//		int realCost=(int) Math.ceil((basePrice*backwardsTime)/(divisor*60));
		int realCost=((int)Math.ceil(backwards_time/(60*divisor)))*basePrice;
		logger.info("reset cold time: {}, {}, {}", new Object[]{backwardsTime , realCost , costGold});
		if(costGold-realCost<10 && realCost-costGold<10){
			if(player.getGoldCoins()>=costGold){
				player.consumeGold(costGold);
				userStats.setNextQuickCareerTime(current_time);
				builder.setBackwardsTimes(0);
				builder.setStats(1);
			}
			else{
				errorBuilder.setErrorid(OP.code.OPCODE_QUICK_CAREER_BATTLERET_S_VALUE);
	            errorBuilder.setErrormsg("over count limit");
	            player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errorBuilder);
	            return;
			}
		}
		else{
			builder.setBackwardsTimes(backwardsTime);
			builder.setStats(2);
		}
		player.setUserStats(userStats);
		builder.setUserGolds(player.getGoldCoins());
		logger.info("reset cold time return: {}, {}, {}", new Object[]{userStats , player.getGoldCoins() , backwardsTime});
	}
}

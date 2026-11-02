package com.oldmountain.dzm.packethandler;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Random;

import com.oldmountain.dzm.entity.*;
import com.oldmountain.dzm.message.SkillBookInfo.*;
import com.oldmountain.dzm.services.SkillBookServices;
import com.oldmountain.dzm.util.*;

import org.apache.mina.common.IoSession;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.google.protobuf.InvalidProtocolBufferException;
import com.oldmountain.dzm.message.OP;
import com.oldmountain.dzm.message.ErrorReporter.OPErrorReporter;
import com.oldmountain.dzm.message.UserBattle.OPUserBattle;
import com.oldmountain.dzm.message.UserBattle.OPUserBattleRet;
import com.oldmountain.dzm.message.UserBattleArray.OPUserBattleArrayRet;
import com.oldmountain.dzm.message.UserDiscipleInfo.OPUserDiscipleInfoRet;
import com.oldmountain.dzm.message.UserEquipInfo.OPUserEquipInfoRet;
import com.oldmountain.dzm.message.UserSkillInfo.OPUserSkillInfoRet;
import com.oldmountain.dzm.message.UserSoulInfo.OPUserSoulInfoRet;
import com.oldmountain.dzm.message.UserToolInfo.OPUserToolInfoRet;
import com.oldmountain.dzm.net.ObjectAccessor;
import com.oldmountain.dzm.net.packet.Packet;

public class UserBattleHandler {
	private static final Logger logger2 = LoggerFactory.getLogger("BattleArrayInfo");
	private static final Logger drop_log = LoggerFactory.getLogger("RewardDropActivate");
	public static void getBattleResult(Packet packet,IoSession session){
		OPUserBattle params = null;
		OPUserBattleRet.Builder builder = OPUserBattleRet.newBuilder();
		OPErrorReporter.Builder errorBuilder = OPErrorReporter.newBuilder();
		
		try {
			params = OPUserBattle.parseFrom(packet.data.array());
		} catch (InvalidProtocolBufferException e) {
			e.printStackTrace();
			return;
		}
		
		int version = params.getVersion();
		int stageId = params.getStage();
		int opponentID = params.getOpponentID();
		int type = params.getType().getNumber();
		if (version != 1) {
			return;
		}
		
		int playerId = (Integer)session.getAttribute("playerid");
		Player player = ObjectAccessor.getOnlineUser(playerId);
		logger2.info("user battle handler received: {}, {}, {}", new Object[]{playerId , version , type , opponentID , stageId});
		long current_time = Util.getServerMillTime();
		
		//初始化玩家幸运星活动的相关数据
    	Util.initLuckyStarActivit(current_time, player);
		BattleArrayObject userBattleInfo = BattleUtil.getUserBattleArrayObject(player);
		
		BattleArrayObject enemyBattleInfo = new BattleArrayObject();
		ArrayList<Float> skillRates = null;
		if (type != OPUserBattle.Type.CARRER_VALUE) {
			errorBuilder.setErrorid(OP.code.OPCODE_USER_BATTLERET_S_VALUE);
			errorBuilder.setErrormsg("type error");
			player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errorBuilder);
			logger2.info("Error Reporter: user battle handler : {}, {}, {}", new Object[]{playerId});
			return;
		}
        if(player.getPowerbytime()+player.getPowerbychicken()<=0)
        {
            errorBuilder.setErrorid(OP.code.OPCODE_USER_BATTLERET_S_VALUE);
            errorBuilder.setErrormsg("power not enough");
            player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errorBuilder);
            return;
        }
        player.decreasePowerBySys(1);

        Career career = player.getCareer();
        
        CareerXmlTemplate careerInfo = XMLTemplateService.getCareerTemplate(stageId);
        
        Map<String,Integer> countMap = Util.jsonToMap(career.getPointcount());
        if (countMap.containsKey(String.valueOf(stageId)) && countMap.get(String.valueOf(stageId)) >= careerInfo.getCountLimit()) {
        	errorBuilder.setErrorid(OP.code.OPCODE_USER_BATTLERET_S_VALUE);
            errorBuilder.setErrormsg("over count limit");
            player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errorBuilder);
            return;
		}
        
		enemyBattleInfo = careerInfo.getBattleInfo();
		skillRates = careerInfo.getSkillRates();
		
		Fight fight = new Fight();
		fight.doFight(userBattleInfo, enemyBattleInfo,skillRates);
		
		Round round1 = fight.getRound1();
		Round round2 = fight.getRound2();
		FinalRound round3 = fight.getFinalRound();
		int fightResult = fight.getResult();
		ArrayList<CharacterResult> characterResults = new ArrayList<CharacterResult>();
		
		Result result = new Result();
		result.setWinlose(fightResult);
		UserStats userStats=player.getUserStats();
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
			if ((!valuateMap.containsKey(stage) && fightResult == 0) || (valuateMap.containsKey(stage) && valuateMap.get(stage) != 3 && fightResult == 0)) {//首次达到三星
				gainCoins *= 3;
				gainExp *= 3;
				gainUserExp *= 3;
				valuateMap.put(stage, 3 - fightResult);
			}
			
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
				int itemId=Util.getActivityTermId(current_time, XMLTemplateService.getluckyStarTimeConfigs());
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
					logger2.info("lucky star drop: {}, {}, {}, {}, {}", new Object[]{ userStats.getLuckyStarTermid() , userStats.getLuckyStayHeap1() , userStats.getLuckyStayHeap2(),userStats.getLuckyStarDropTimes1(),userStats.getLuckyStarDropTimes2()});
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
		
		player.setLastVerifyTime(Util.getServerTime());
		
		result.setCharacterResults(characterResults);
		builder.setRound1(getRoundBuilder(round1));
		builder.setRound2(getRoundBuilder(round2));
		builder.setRound3(getFinalRound(round3));
		builder.setResult(getResult(player,result));
		builder.setTeamArchievements(fight.getTeamBuilder());
		builder.setUserPower(player.getPowerbychicken() + player.getPowerbytime());
		
		builder.setVersion(version);
		builder.setServertime(Util.getServerTime());
		player.setUserStats(userStats);
		player.sendPacket(OP.code.OPCODE_USER_BATTLERET_S_VALUE, builder);
		long current_time_2 = Util.getServerMillTime();
		long taken_time=current_time_2-current_time;
		logger2.info("user battle handler ret send: {}, {}, {}", new Object[]{playerId ,taken_time});
	}
	
	public static OPUserBattleRet.Round.Builder getRoundBuilder(Round round){
		OPUserBattleRet.Round.Builder roundBuilder = OPUserBattleRet.Round.newBuilder();
		OPUserBattleRet.Character.Builder characterBuilder = OPUserBattleRet.Character.newBuilder();
		OPUserBattleRet.Action.Builder actionBuilder = OPUserBattleRet.Action.newBuilder();
		OPUserBattleRet.Damage.Builder damageBuilder = OPUserBattleRet.Damage.newBuilder();
		List<Characters> characters = round.getCharacters();
		List<Actions> actions = round.getActions();
		for (int i = 0; i < characters.size(); i++) {
			Characters character = characters.get(i);
			characterBuilder.setPosition(character.getPosition());
			characterBuilder.setResourceID(character.getItemid());
			characterBuilder.setLife(character.getLife());
			characterBuilder.setPower(character.getPower());
			characterBuilder.setLevel(character.getLevel());
			roundBuilder.addCharacters(characterBuilder);
		}
		
		for (int i = 0; i < actions.size(); i++) {
			actionBuilder = OPUserBattleRet.Action.newBuilder();
			Actions action = actions.get(i);
			if (action.getType() == 0) {
				actionBuilder.setType(OPUserBattleRet.Action.Type.ATTACK);
			}
			else {
				actionBuilder.setType(OPUserBattleRet.Action.Type.SKILL);
				actionBuilder.setSkillID(action.getSkillId());
			}
			actionBuilder.setPositionID(action.getPosition());
			List<Damage> damages = action.getDamages();
			for (int j = 0; j < damages.size(); j++) {
				Damage damage = damages.get(j);
				damageBuilder.setPosition(damage.getPosition());
				damageBuilder.setHarmValue(damage.getHarmValue());
				actionBuilder.addDamage(damageBuilder);
			}
			roundBuilder.addActions(actionBuilder);
		}
		
		roundBuilder.setGotTo(round.getGotTo());
		return roundBuilder;
	}
	
	public static OPUserBattleRet.FinalRound.Builder getFinalRound(FinalRound finalRound){
		OPUserBattleRet.FinalRound.Builder roundBuilder = OPUserBattleRet.FinalRound.newBuilder();
		OPUserBattleRet.Character.Builder characterBuilder = OPUserBattleRet.Character.newBuilder();
		List<Characters> characters = finalRound.getCharacters();
		for (int i = 0; i < characters.size(); i++) {
			Characters character = characters.get(i);
			characterBuilder.setPosition(character.getPosition());
			characterBuilder.setResourceID(character.getItemid());
			characterBuilder.setLife(character.getLife());
			characterBuilder.setPower(character.getPower());
			characterBuilder.setLevel(character.getLevel());
			roundBuilder.addCharacters(characterBuilder);
		}
		roundBuilder.setWin(finalRound.getWin());
		roundBuilder.setGotTo(finalRound.getGotto());
		return roundBuilder;
	}
	
	public static OPUserBattleRet.Result.Builder getResult(Player player,Result result){
		OPUserBattleRet.Result.Builder resultBuilder = OPUserBattleRet.Result.newBuilder();
		OPUserBattleRet.Result.CharacterResult.Builder charecterResultBuilder = OPUserBattleRet.Result.CharacterResult.newBuilder();
		
		switch (result.getWinlose()) {
			case 0:
				resultBuilder.setWinlose(OPUserBattleRet.Result.WINLOSE.BIGWIN);
				break;
			case 1:
				resultBuilder.setWinlose(OPUserBattleRet.Result.WINLOSE.WIN);
				break;
			case 2:
				resultBuilder.setWinlose(OPUserBattleRet.Result.WINLOSE.JUSTWIN);
				break;
			case 3:
				resultBuilder.setWinlose(OPUserBattleRet.Result.WINLOSE.JUSTLOSE);
				break;
			case 4:
				resultBuilder.setWinlose(OPUserBattleRet.Result.WINLOSE.LOSE);
				break;
			case 5:
				resultBuilder.setWinlose(OPUserBattleRet.Result.WINLOSE.BIGLOSE);
				break;
		}
		resultBuilder.setGotCoin(result.getGotCoin());
		resultBuilder.setGotExp(result.getGotExp());
		resultBuilder.setGotGold(result.getGotGold());
		
		List<CharacterResult> characterResults = result.getCharacterResults();
		for (int i = 0; i < characterResults.size(); i++) {
			CharacterResult characterResult = characterResults.get(i);
			charecterResultBuilder.setResourceID(characterResult.getItemid());
			charecterResultBuilder.setGainExp(characterResult.getGainExp());
			charecterResultBuilder.setLevel(characterResult.getLevel());
			charecterResultBuilder.setIsUpgarade(characterResult.getIsUpgrade());
			resultBuilder.addCharacterResults(charecterResultBuilder);
		}
		
		if (result.getGotItemType() >= 0) {//有获取到物品
			OPUserBattleRet.Result.GotItemResult.Builder itemBuilder = OPUserBattleRet.Result.GotItemResult.newBuilder();
			itemBuilder.setCount(result.getGotItemCount());
			switch (result.getGotItemType()) {
				case 0:
					OPUserToolInfoRet.ToolInfo.Builder toolBuilder = OPUserToolInfoRet.ToolInfo.newBuilder();
					itemBuilder.setType(OPUserBattleRet.Result.GotItemResult.ITEMTYPE.TOOL);
					Bag tool = player.getBags().get(result.getGotItemId());
					toolBuilder.setId(tool.getId());
					toolBuilder.setItemid(tool.getItemid());
					toolBuilder.setCount(tool.getCount());
					itemBuilder.setTool(toolBuilder);
					break;
				case 1:
					OPUserToolInfoRet.ToolInfo.Builder toolUseBuilder = OPUserToolInfoRet.ToolInfo.newBuilder();
					itemBuilder.setType(OPUserBattleRet.Result.GotItemResult.ITEMTYPE.TOOL_USE);
					Bag toolUse = player.getBags().get(result.getGotItemId());
					toolUseBuilder.setId(toolUse.getId());
					toolUseBuilder.setItemid(toolUse.getItemid());
					toolUseBuilder.setCount(toolUse.getCount());
					itemBuilder.setTool(toolUseBuilder);
					break;
				case 2:
					itemBuilder.setType(OPUserBattleRet.Result.GotItemResult.ITEMTYPE.EQUIPMENT);
					Equip equip = player.getEquips().get(result.getGotItemId());
					OPUserEquipInfoRet.EquipInfo.Builder equipBuilder = EquipUtil.buildReturn(equip);
					itemBuilder.setEquip(equipBuilder);
					break;
				case 3:
					itemBuilder.setType(OPUserBattleRet.Result.GotItemResult.ITEMTYPE.SKILL);
					Skill skill = player.getSkills().get(result.getGotItemId());
					OPUserSkillInfoRet.SkillInfo.Builder skillBuilder = SkillUtil.buildSkillBuilder(skill);
					itemBuilder.setSkill(skillBuilder);
					break;
				case 4:
					itemBuilder.setType(OPUserBattleRet.Result.GotItemResult.ITEMTYPE.SOUL);
					Soul soul = player.getSouls().get(result.getGotItemId());
					OPUserSoulInfoRet.SoulInfo.Builder soulBuilder = SoulUtil.buildSoulBuilder(soul);
					itemBuilder.setSoul(soulBuilder);
					break;
                case 5:
                    //获取残章
                    itemBuilder.setType(OPUserBattleRet.Result.GotItemResult.ITEMTYPE.SKILLBOOK);
                    SkillBook skillBook = player.getSkillBooks().get(XMLTemplateService.getSkillIdBySkillBookId(result.getGotItemId()));
                    OPGetSkillBookInfoRet.SkillBookPartItem.Builder skillBookBuiler = OPGetSkillBookInfoRet.SkillBookPartItem.newBuilder();
                    skillBookBuiler.setPartId(result.getGotItemId());
                    skillBookBuiler.setSkillId(skillBook.getSkillId());
                    skillBookBuiler.setCount(skillBook.getSkillBookCountBySkillBookId(result.getGotItemId()));
                    itemBuilder.setSkillbook(skillBookBuiler);
                    break;
                case 6:
                    //获取技能拼合机会
                    itemBuilder.setType(OPUserBattleRet.Result.GotItemResult.ITEMTYPE.SKILLPIECE);
                    SkillBook skillBookItem = player.getSkillBooks().get(result.getGotItemId());
                    OPGetSkillBookInfoRet.SkillBookPartItem.Builder bookBuiler = OPGetSkillBookInfoRet.SkillBookPartItem.newBuilder();
                    bookBuiler.setPartId(0);
                    bookBuiler.setSkillId(skillBookItem.getSkillId());
                    bookBuiler.setCount(skillBookItem.getPieceChance());
                    itemBuilder.setSkillbook(bookBuiler);
                    break;
			}
			resultBuilder.setGotItem(itemBuilder);
		}
		
		resultBuilder.setCanViewTeam(result.getCanViewTeam());
		return resultBuilder;
	}
}

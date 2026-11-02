package com.oldmountain.dzm.packethandler;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;
import org.apache.mina.common.IoSession;
import com.google.protobuf.InvalidProtocolBufferException;
import com.oldmountain.dzm.DZM;
import com.oldmountain.dzm.entity.Battle;
import com.oldmountain.dzm.entity.BattleArrayObject;
import com.oldmountain.dzm.entity.CareerXmlTemplate;
import com.oldmountain.dzm.entity.Challenge;
import com.oldmountain.dzm.entity.CharacterResult;
import com.oldmountain.dzm.entity.Disciple;
import com.oldmountain.dzm.entity.DiscipleXmlTemplate;
import com.oldmountain.dzm.entity.Equip;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.entity.Result;
import com.oldmountain.dzm.entity.Skill;
import com.oldmountain.dzm.message.OP;
import com.oldmountain.dzm.message.Instruction.OPInstruction;
import com.oldmountain.dzm.message.Instruction.OPInstructionRet;
import com.oldmountain.dzm.message.UserBattle.OPUserBattleRet;
import com.oldmountain.dzm.message.UserDiscipleInfo.OPUserDiscipleInfoRet;
import com.oldmountain.dzm.message.UserSkillInfo.OPUserSkillInfoRet;
import com.oldmountain.dzm.net.ObjectAccessor;
import com.oldmountain.dzm.net.packet.Packet;
import com.oldmountain.dzm.util.BattleUtil;
import com.oldmountain.dzm.util.DiscipleUtil;
import com.oldmountain.dzm.util.Fight;
import com.oldmountain.dzm.util.SkillUtil;
import com.oldmountain.dzm.util.Util;
import com.oldmountain.dzm.util.XMLTemplateService;

public class InstructionHandler {
	public static final ArrayList<Integer> selectDisciplePool = new ArrayList<Integer>(Arrays.asList(53,65,59));
	public static final ArrayList<Integer> firstWanliPool = new ArrayList<Integer>(Arrays.asList(75,77,78,84,87,90));
	
	public static void doInstruction(Packet packet,IoSession session){
		OPInstruction params = null;
		OPInstructionRet.Builder builder = OPInstructionRet.newBuilder();
		builder.setVersion(1);
		
		try {
			params = OPInstruction.parseFrom(packet.data.array());
		} catch (InvalidProtocolBufferException e) {
			e.printStackTrace();
			return;
		}
		
		int version = params.getVersion();
		if (version != 1) {
			//TODO 版本控制
		}
		String name = params.getName();
		
		int selectDisciple = params.getDiscipleSelectedItemID();
		int discipleId = selectDisciplePool.get(selectDisciple - 1);
		
		int playerId = (Integer)session.getAttribute("playerid");
		Player player = ObjectAccessor.getOnlineUser(playerId);
		
		if (name.equals("") || name == null || name.length() > 7) {
			builder.setNamePossible(false);
			player.sendPacket(OP.code.OPCODE_INSTRUCTIONRET_S_VALUE,builder);
			return;
		}
		
		ArrayList<String> existNames = ObjectAccessor.getExistNames();
		if (existNames.contains(name)) {
			builder.setNamePossible(false);
			player.sendPacket(OP.code.OPCODE_INSTRUCTIONRET_S_VALUE,builder);
			return;
		}
		
		builder.setNamePossible(true);
		player.setName(name);
		ObjectAccessor.addToExistName(name);
		
		HashMap<Integer, Disciple>userDisciples = player.getDisciples();
		HashMap<Integer, Equip> userEquips = player.getEquips();
		
		Iterator<Map.Entry<Integer, Equip>> equipIterator = userEquips.entrySet().iterator();
		Equip equip = new Equip();
		while (equipIterator.hasNext()) {
			Map.Entry<Integer, Equip> entry = equipIterator.next();
			Equip e = entry.getValue();
			if (e.getItemid() == 32) {
				equip = e;
				break;
			}
		}
		
		if (equip.getId() == 0) {
			System.out.println(1);
		}
		
		Disciple disciple = new Disciple();
		Disciple randDisciple = new Disciple();
		
		if (userDisciples.size() > 1) {
			HashMap<Integer, Disciple> disciples = player.getDisciples();
			Iterator<Map.Entry<Integer, Disciple>> iterator = disciples.entrySet().iterator();
			while (iterator.hasNext()) {
				Map.Entry<Integer, Disciple> entry = iterator.next();
				Disciple value = entry.getValue();
				if (selectDisciplePool.contains(value.getItemid())) {
					OPUserDiscipleInfoRet.DiscipleInfo.Builder selectDiscipleBuilder = DiscipleUtil.buildDiscipleBuilder(value);
					OPUserSkillInfoRet.SkillInfo.Builder selectSkillBuilder = SkillUtil.buildSkillBuilder(player.getSkills().get(value.getSkill1()));
					builder.setDiscipleSelected(selectDiscipleBuilder);
					builder.setSelectedSkill(selectSkillBuilder);
					disciple = value;
				}
				else {
					OPUserDiscipleInfoRet.DiscipleInfo.Builder randDiscipleBuilder = DiscipleUtil.buildDiscipleBuilder(value);
					OPUserSkillInfoRet.SkillInfo.Builder randSkillBuilder = SkillUtil.buildSkillBuilder(player.getSkills().get(value.getSkill1()));
					builder.setDiscipleRandom(randDiscipleBuilder);
					builder.setRandomSkill(randSkillBuilder);
					randDisciple = value;
				}
			}
		}
		else {
			disciple = DiscipleUtil.newDisciple(player, discipleId,5,54);
			OPUserDiscipleInfoRet.DiscipleInfo.Builder selectDiscipleBuilder = DiscipleUtil.buildDiscipleBuilder(disciple);
			builder.setDiscipleSelected(selectDiscipleBuilder);
			int poolSize = firstWanliPool.size();
			int rand = (int)(Math.random() * poolSize);
			int randDiscipleId = firstWanliPool.get(rand);
			randDisciple = DiscipleUtil.newDisciple(player, randDiscipleId,1,30);
			OPUserDiscipleInfoRet.DiscipleInfo.Builder randDiscipleBuilder = DiscipleUtil.buildDiscipleBuilder(randDisciple);
			player.setWanlitime(Util.getServerTime());
			
			builder.setDiscipleRandom(randDiscipleBuilder);
			//设置阵容的基本属性
			DiscipleXmlTemplate selectDiscipleInfo = XMLTemplateService.getDiscipleTemplate(discipleId);
			String fates = selectDiscipleInfo.getFates();
			String[] fatesArray = fates.split(",");
			String fateStatus = "0";
			for (int i = 1; i < fatesArray.length; i++) {
				fateStatus += ",0"; 
			}
			
			String[] battleIds = player.getBattleids().split(",");
			HashMap<Integer, Battle> userBattles = player.getBattlearray();
			
			int firstId = Integer.parseInt(battleIds[0]);
			Battle firstBattle = userBattles.get(firstId);
			firstBattle.setDiscipleid(disciple.getId());
			firstBattle.setItemid(disciple.getItemid());
			firstBattle.setLevel(disciple.getLevel());
			firstBattle.setAttack(disciple.getAttackToInt());
			firstBattle.setDefence(disciple.getDefenceToInt());
			firstBattle.setHealth(disciple.getHealthToInt());
			firstBattle.setRp(disciple.getRpToInt());
			Skill defaultSkill = player.getSkills().get(disciple.getSkill1());
			builder.setSelectedSkill(SkillUtil.buildSkillBuilder(defaultSkill));
			firstBattle.setDefaultskill(defaultSkill.getId());
			firstBattle.setDefaultskilltype(defaultSkill.getType());
			firstBattle.setDefaultskillitem(defaultSkill.getItemid());
			firstBattle.setDefaultskillquantity(defaultSkill.getQuantity());
			//设置阵容的缘分信息
			firstBattle.setFates(fates);
			firstBattle.setFatesstatus(fateStatus);
			
			
			defaultSkill.setBattleid(firstId);
			disciple.setBattleid(firstId);
			
			player.updateOneDisciple(disciple);
			player.updateOneSkill(defaultSkill);
			
			
			
			DiscipleXmlTemplate randDiscipleInfo = XMLTemplateService.getDiscipleTemplate(randDiscipleId);
			String randFates = randDiscipleInfo.getFates();
			String[] randFatesArray = randFates.split(",");
			String randFateStatus = "0";
			for (int i = 1; i < randFatesArray.length; i++) {
				randFateStatus += ",0"; 
			}
			int secondId = Integer.parseInt(battleIds[1]);
			equip.setBattleid(secondId);
			
			Battle secondBattle = userBattles.get(secondId);
			secondBattle.setEquip1(equip.getId());
			secondBattle.setDiscipleid(randDisciple.getId());
			secondBattle.setItemid(randDisciple.getItemid());
			secondBattle.setLevel(randDisciple.getLevel());
			secondBattle.setAttack(randDisciple.getAttackToInt() + equip.getAttackToInt());
			secondBattle.setDefence(randDisciple.getDefenceToInt());
			secondBattle.setHealth(randDisciple.getHealthToInt());
			secondBattle.setRp(randDisciple.getRpToInt());
			Skill secondDefaultSkill = player.getSkills().get(randDisciple.getSkill1());
			builder.setRandomSkill(SkillUtil.buildSkillBuilder(secondDefaultSkill));
			secondBattle.setDefaultskill(secondDefaultSkill.getId());
			secondBattle.setDefaultskilltype(secondDefaultSkill.getType());
			secondBattle.setDefaultskillitem(secondDefaultSkill.getItemid());
			secondBattle.setDefaultskillquantity(secondDefaultSkill.getQuantity());
			
			//设置阵容的缘分信息
			secondBattle.setFates(randFates);
			secondBattle.setFatesstatus(randFateStatus);
			
			secondDefaultSkill.setBattleid(secondId);
			randDisciple.setBattleid(secondId);
			player.updateOneEquip(equip);
			player.updateOneDisciple(randDisciple);
			player.updateOneSkill(secondDefaultSkill);
			
			ArrayList<Integer> battleDiscipleItems = new ArrayList<Integer>();
			battleDiscipleItems.add(discipleId);
			battleDiscipleItems.add(randDiscipleId);
			
			BattleUtil.resetBattleProperty(firstBattle, disciple, player, battleDiscipleItems, 1);
			BattleUtil.resetBattleProperty(secondBattle, randDisciple, player, battleDiscipleItems, 1);
			
			player.updateOneBattle(firstBattle);
			player.updateOneBattle(secondBattle);
			BattleUtil.getUserBattleArrayObject(player).getBattlearrays().set(0, firstBattle);
			BattleUtil.getUserBattleArrayObject(player).getBattlearrays().set(1, secondBattle);
			
			//创建论剑信息
//			Challenge challenge = new Challenge();
//			challenge.setLevel(1);
//			challenge.setName(name);
//			challenge.setPlayerid(playerId);
//			challenge.setRank(playerId + 108);
//			challenge.setHighestrank(playerId + 108);
//			DZM.getEntityManager().createSync(challenge);
//			player.setChallenge(challenge);
		}
		BattleArrayObject userBattleArrayInfo = BattleUtil.getUserBattleArrayObject(player);
		CareerXmlTemplate careerXmlTemplate = XMLTemplateService.getCareerTemplate(102);
		Fight fight = new Fight();
		fight.doFight(userBattleArrayInfo, careerXmlTemplate.getBattleInfo(),null);
		
		OPUserBattleRet.Builder battleBuilder = OPUserBattleRet.newBuilder();
		battleBuilder.setServertime(Util.getServerTime());
		battleBuilder.setVersion(1);
		battleBuilder.setRound1(UserBattleHandler.getRoundBuilder(fight.getRound1()));
		battleBuilder.setRound2(UserBattleHandler.getRoundBuilder(fight.getRound2()));
		battleBuilder.setRound3(UserBattleHandler.getFinalRound(fight.getFinalRound()));
		
		ArrayList<CharacterResult> characterResults = new ArrayList<CharacterResult>();
		CharacterResult characterResult = new CharacterResult();
		characterResult.setGainExp(30);
		characterResult.setIsUpgrade(false);
		characterResult.setItemid(disciple.getItemid());
		characterResult.setLevel(disciple.getLevel());
		characterResults.add(characterResult);
		
		characterResult = new CharacterResult();
		characterResult.setGainExp(30);
		characterResult.setIsUpgrade(false);
		characterResult.setItemid(randDisciple.getItemid());
		characterResult.setLevel(randDisciple.getLevel());
		characterResults.add(characterResult);
		
		
		Result result = new Result();
		result.setWinlose(fight.getResult());
		result.setGotCoin(171);
		result.setGotExp(30);
		result.setGotItemType(2);
		result.setGotItemId(equip.getId());
		result.setGotItemCount(1);
		result.setCharacterResults(characterResults);
		
		player.setTutorialstep(1);
		player.setInstructionFinishTime(Util.getServerTime());
		battleBuilder.setResult(UserBattleHandler.getResult(player, result));
		battleBuilder.setTeamArchievements(fight.getTeamBuilder());
		
		builder.setBattle2(battleBuilder);
		
		player.setLastVerifyTime(Util.getServerTime());
		
		player.sendPacket(OP.code.OPCODE_INSTRUCTIONRET_S_VALUE, builder);
	}
	
	public static void doClientGot(Packet packet,IoSession session){
		int playerId = (Integer)session.getAttribute("playerid");
		Player player = ObjectAccessor.getOnlineUser(playerId);
		
		player.setTutorialstep(1);
	}
}

package com.oldmountain.dzm.packethandler;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

import org.apache.mina.common.IoSession;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.google.protobuf.InvalidProtocolBufferException;
import com.oldmountain.dzm.entity.Battle;
import com.oldmountain.dzm.entity.Disciple;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.entity.Skill;
import com.oldmountain.dzm.entity.SkillXmlTemplate;
import com.oldmountain.dzm.message.OP;
import com.oldmountain.dzm.message.ErrorReporter.OPErrorReporter;
import com.oldmountain.dzm.message.UpgradeSkill.OPUpgradeSkill;
import com.oldmountain.dzm.message.UpgradeSkill.OPUpgradeSkillRet;
import com.oldmountain.dzm.message.UserBattleArray.OPUserBattleArrayRet;
import com.oldmountain.dzm.net.ObjectAccessor;
import com.oldmountain.dzm.net.packet.Packet;
import com.oldmountain.dzm.util.BattleUtil;
import com.oldmountain.dzm.util.Util;
import com.oldmountain.dzm.util.XMLTemplateService;

public class UpgradeSkillHandler {
	private static final Logger logger2 = LoggerFactory.getLogger("SkillInfo");
	public static void upgradeSkill(Packet packet,IoSession session){
		OPUpgradeSkill params = null;
		OPUpgradeSkillRet.Builder builder = OPUpgradeSkillRet.newBuilder();
		OPErrorReporter.Builder errBuilder = OPErrorReporter.newBuilder();
		
		try {
			params = OPUpgradeSkill.parseFrom(packet.data.array());
		} catch (InvalidProtocolBufferException e) {
			e.printStackTrace();
			return;
		}
		
		int skillId = params.getId();
		int count = params.getKidCount();
		
		int playerId = (Integer)session.getAttribute("playerid");
		Player player = ObjectAccessor.getOnlineUser(playerId);
		int id=player.getId();
		logger2.info("upgrade skill handler received: {}, {}, {}", new Object[]{id , skillId});
		long current_time = Util.getServerMillTime();
		if (count <= 0 || count > 5) {
			errBuilder.setErrorid(OP.code.OPCODE_UPGRADE_SKILLRET_S_VALUE);
			errBuilder.setErrormsg("no skill send");
			player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errBuilder);
			logger2.info("Error Reporter send: upgrade skill handler: {}, {}, {}", new Object[]{id});
			return;
		}
		
		List<Integer> ids = params.getKidList();
		
		if (ids.contains(skillId)) {
			errBuilder.setErrorid(OP.code.OPCODE_UPGRADE_SKILLRET_S_VALUE);
			errBuilder.setErrormsg("skill id exist in kill array");
			player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errBuilder);
			logger2.info("Error Reporter send: upgrade skill handler: {}, {}, {}", new Object[]{id});
			return;
		}
		
		HashMap<Integer, Skill> userSkills = player.getSkills();
		
		if (!userSkills.containsKey(skillId)) {
			errBuilder.setErrorid(OP.code.OPCODE_UPGRADE_SKILLRET_S_VALUE);
			errBuilder.setErrormsg("skill not exist");
			player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errBuilder);
			logger2.info("Error Reporter send: upgrade skill handler: {}, {}, {}", new Object[]{id});
			return;
		}
		
		Skill skill = userSkills.get(skillId);
		
		SkillXmlTemplate skillInfo = XMLTemplateService.getSkillTemplate(skill.getItemid());
		
		int qualityExp = 5-skillInfo.getQuality();
		
		int costCoins = (int)( qualityExp * 100 * Math.pow(skill.getLevel(), 2));
		
		if (player.getSilvercoins() < costCoins) {
			errBuilder.setErrorid(OP.code.OPCODE_UPGRADE_SKILLRET_S_VALUE);
			errBuilder.setErrormsg("coins not enough");
			player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errBuilder);
			logger2.info("Error Reporter send: upgrade skill handler: {}, {}, {}", new Object[]{id});
			return;
		}
		
		int addExp = 0;
		for (int i = 0; i < count; i++) {
			int sId = ids.get(i);
			if (!userSkills.containsKey(sId)) {
				errBuilder.setErrorid(OP.code.OPCODE_UPGRADE_SKILLRET_S_VALUE);
				errBuilder.setErrormsg("swallow skill not exist");
				player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errBuilder);
				logger2.info("Error Reporter send: upgrade skill handler: {}, {}, {}", new Object[]{id});
				return;
			}
			Skill kSkill = userSkills.get(sId);
			if (kSkill.getIsdefaultskill() == 1) {
				errBuilder.setErrorid(OP.code.OPCODE_UPGRADE_SKILLRET_S_VALUE);
				errBuilder.setErrormsg("skill is default skill");
				player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errBuilder);
				logger2.info("Error Reporter send: upgrade skill handler: {}, {}, {}", new Object[]{id});
				return;
			}
			
			if (kSkill.getBattleid() != 0) {
				int battleId = kSkill.getBattleid();
				Battle battle = player.getBattlearray().get(battleId);
				if (battle != null && (battle.getSkill1() == sId || battle.getSkill2() == sId )) {
					errBuilder.setErrorid(OP.code.OPCODE_UPGRADE_SKILLRET_S_VALUE);
					errBuilder.setErrormsg("skill is in battle");
					player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errBuilder);
					logger2.info("Error Reporter send: upgrade skill handler: {}, {}, {}", new Object[]{id});
					return;
				}
			}
		}
		
		for (int i = 0; i < count; i++) {
			int sId = ids.get(i);
			Skill kSkill = userSkills.get(sId);
			SkillXmlTemplate info = XMLTemplateService.getSkillTemplate(kSkill.getItemid());
			addExp += (5-info.getQuality());
			player.removeOneSkill(kSkill);
			builder.addKid(sId);
		}
		
		int newExp = skill.getConsume() + addExp;
		int newLevel = (int) Math.floor(Math.log(newExp/qualityExp + 2)/Math.log(2));
		skill.setConsume(newExp);
		
		if (newLevel > skill.getLevel()) {
			skill.setLevel(newLevel);
			skill.setQuantity(skill.getQuantity() + skillInfo.getGrowth());
			
			if (skill.getBattleid() != 0) {
				int battleid = skill.getBattleid();
				String battleids = player.getBattleids();
				String[] battleArray = battleids.split(",");
				for (int i = 0; i < battleArray.length; i++) {
					if (battleid == Integer.parseInt(battleArray[i])) {
						Battle battle = player.getBattlearray().get(battleid);
						if(battle.getDefaultskill()==skillId){
							battle.setDefaultskillquantity(skill.getQuantity());
						}
						else if(battle.getSkill1()==skillId){
							battle.setSkill1quantity(skill.getQuantity());
						}
						else if(battle.getSkill2()==skillId){
							battle.setSkill2quantity(skill.getQuantity());
						}
						Disciple disciple = player.getDisciples().get(battle.getDiscipleid());
						BattleUtil.resetBattleProperty(battle, disciple,player,null,0);
						OPUserBattleArrayRet.BattleArrayInfo.Builder battleBuilder= OPUserBattleArrayRet.BattleArrayInfo.newBuilder();
						BattleUtil.buildBattleBuilder(battleBuilder, battle,i);
						builder.setBattle(battleBuilder);
						player.updateOneBattle(battle);
						BattleUtil.getUserBattleArrayObject(player).getBattlearrays().set(i, battle);
						break;
					}
				}
			}
			
			if (newLevel % 5 == 0 && skillInfo.getQuality() == 1) {
				ArrayList<Integer> broParams = new ArrayList<Integer>(1);
				broParams.add(skillInfo.getId());
				broParams.add(newLevel);
				BoardCastInfoHandler.addBoardCase(BoardCastInfoHandler.UPGRADESKILL_TYPE, player.getName(), broParams);
			}
		}
		
		player.updateOneSkill(skill);
		player.setSilvercoins(player.getSilvercoins() - costCoins);
		player.setLastVerifyTime(Util.getServerTime());
		
		
		OPUpgradeSkillRet.SkillInfo.Builder builder2 = OPUpgradeSkillRet.SkillInfo.newBuilder();
		builder2.setId(skillId);
		builder2.setLevel(skill.getLevel());
		builder2.setQuantity(skill.getQuantity());
		builder2.setConsume(skill.getConsume());
		
		builder.setSkill(builder2);
		builder.setServertime(Util.getServerTime());
		builder.setSilvercoins(player.getSilvercoins());
		
		player.sendPacket(OP.code.OPCODE_UPGRADE_SKILLRET_S_VALUE, builder);
		long current_time_2 = Util.getServerMillTime();
		long taken_time=current_time_2-current_time;
		logger2.info("upgrade skill handler ret send: {}, {}, {}", new Object[]{id ,taken_time});
	}
}

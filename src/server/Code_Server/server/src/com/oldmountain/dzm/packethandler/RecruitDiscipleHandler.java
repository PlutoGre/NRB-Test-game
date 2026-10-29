package com.oldmountain.dzm.packethandler;

import java.util.HashMap;

import org.apache.mina.common.IoSession;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.google.protobuf.InvalidProtocolBufferException;
import com.oldmountain.dzm.entity.Disciple;
import com.oldmountain.dzm.entity.DiscipleXmlTemplate;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.entity.Skill;
import com.oldmountain.dzm.entity.SkillXmlTemplate;
import com.oldmountain.dzm.entity.Soul;
import com.oldmountain.dzm.message.OP;
import com.oldmountain.dzm.message.ErrorReporter.OPErrorReporter;
import com.oldmountain.dzm.message.RecruitDisciple.OPRecruitDisciple;
import com.oldmountain.dzm.message.RecruitDisciple.OPRecruitDiscipleRet;
import com.oldmountain.dzm.message.UserDiscipleInfo.OPUserDiscipleInfoRet;
import com.oldmountain.dzm.message.UserSkillInfo.OPUserSkillInfoRet;
import com.oldmountain.dzm.message.UserSoulInfo.OPUserSoulInfoRet;
import com.oldmountain.dzm.net.ObjectAccessor;
import com.oldmountain.dzm.net.packet.Packet;
import com.oldmountain.dzm.util.DiscipleUtil;
import com.oldmountain.dzm.util.SkillUtil;
import com.oldmountain.dzm.util.SoulUtil;
import com.oldmountain.dzm.util.Util;
import com.oldmountain.dzm.util.XMLTemplateService;

public class RecruitDiscipleHandler {
	private static final Logger logger2 = LoggerFactory.getLogger("DiscipleInfo");
	public static void recruitDisciple(Packet packet,IoSession session){
		OPRecruitDisciple params = null;
		OPRecruitDiscipleRet.Builder builder = OPRecruitDiscipleRet.newBuilder();
		
		
		try {
			params = OPRecruitDisciple.parseFrom(packet.data.array());
		} catch (InvalidProtocolBufferException e) {
			e.printStackTrace();
			return;
		}
		
		int id = params.getId();
		
		int playerId = (Integer)session.getAttribute("playerid");
		Player player = ObjectAccessor.getOnlineUser(playerId);
		int playid=player.getId();
		logger2.info("recruit disciple handler received: {}, {}, {}", new Object[]{playid , id});
		long current_time = Util.getServerMillTime();
		HashMap<Integer, Soul> userSouls = player.getSouls();
		
		if (!userSouls.containsKey(id)) {
			OPErrorReporter.Builder errBuilder = OPErrorReporter.newBuilder();
			errBuilder.setErrorid(OP.code.OPCODE_RECRUIT_DISCIPLERET_S_VALUE);
			errBuilder.setErrormsg("no soul!");
			player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errBuilder);
			return ;
		}
		
		Soul soul = userSouls.get(id);
		
		DiscipleXmlTemplate info = XMLTemplateService.getDiscipleTemplate(id);
		
		int needCount = 100;
		switch (info.getQuality()) {
			case 1:
				needCount = 30;
				break;
			case 2:
				needCount = 15;
				break;
			case 3:
				needCount = 5;
				break;
			case 4:
				needCount = 1;
				break;
		}
		
		if (soul.getCount() < needCount) {
			OPErrorReporter.Builder errBuilder = OPErrorReporter.newBuilder();
			errBuilder.setErrorid(OP.code.OPCODE_RECRUIT_DISCIPLERET_S_VALUE);
			errBuilder.setErrormsg("soul count not enough!");
			player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errBuilder);
			return ;
		}
		
		if (player.getDiscipleitems().contains(id)) {
			OPErrorReporter.Builder errBuilder = OPErrorReporter.newBuilder();
			errBuilder.setErrorid(OP.code.OPCODE_RECRUIT_DISCIPLERET_S_VALUE);
			errBuilder.setErrormsg("already has this disciple!");
			player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errBuilder);
			return ;
		}
		
		soul.setCount(soul.getCount() - needCount);
		player.updateOneSoul(soul);
		player.setLastVerifyTime(Util.getServerTime());
		
		OPUserSoulInfoRet.SoulInfo.Builder soulBuilder = SoulUtil.buildSoulBuilder(soul);
		builder.setSoul(soulBuilder);
		Disciple disciple = DiscipleUtil.newDisciple(player, id, 1, 0);
		Skill skill = player.getSkills().get(disciple.getSkill1());
		OPUserSkillInfoRet.SkillInfo.Builder skillBuilder = SkillUtil.buildSkillBuilder(skill);
		builder.setSkill(skillBuilder);
		OPUserDiscipleInfoRet.DiscipleInfo.Builder builder2 = DiscipleUtil.buildDiscipleBuilder(disciple);
		builder.setDisciple(builder2);
		
		builder.setServertime(Util.getServerTime());
		
		player.sendPacket(OP.code.OPCODE_RECRUIT_DISCIPLERET_S_VALUE, builder);
		
		long current_time_2 = Util.getServerMillTime();
		long taken_time=current_time_2-current_time;
		logger2.info("recruit disciple handler ret send: {}, {}, {}", new Object[]{playid ,taken_time});
	}
	
	
}

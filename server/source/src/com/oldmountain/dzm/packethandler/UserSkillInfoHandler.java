package com.oldmountain.dzm.packethandler;

import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;

import org.apache.mina.common.IoSession;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.google.protobuf.InvalidProtocolBufferException;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.entity.Skill;
import com.oldmountain.dzm.message.OP;
import com.oldmountain.dzm.message.UserSkillInfo.OPUserSkillInfo;
import com.oldmountain.dzm.message.UserSkillInfo.OPUserSkillInfoRet;
import com.oldmountain.dzm.net.ObjectAccessor;
import com.oldmountain.dzm.net.packet.Packet;
import com.oldmountain.dzm.util.Util;

/**
 * 获取用户技能信息列表
 * @author dragon
 *
 */

public class UserSkillInfoHandler {
	private static final Logger logger2 = LoggerFactory.getLogger("SkillInfo");
	public static void getUserSkillInfo(Packet packet,IoSession session){
		OPUserSkillInfo params = null;
		OPUserSkillInfoRet.Builder builder = OPUserSkillInfoRet.newBuilder();
		
		try {
			params = OPUserSkillInfo.parseFrom(packet.data.array());
		} catch (InvalidProtocolBufferException e) {
			e.printStackTrace();
			return;
		}
		
		int version = params.getVersion();
		
		if (version != 1) {
			return ;
		}
		
		int playerId = (Integer)session.getAttribute("playerid");
		Player player = ObjectAccessor.getOnlineUser(playerId);
		logger2.info("user skill info handler received: {}, {}, {}", new Object[]{playerId , version});
		long current_time = Util.getServerMillTime();
		int time = Util.getServerTime();
		builder.setServertime(time);
		
		HashMap<Integer, Skill> skills = player.getSkills();
		OPUserSkillInfoRet.SkillInfo.Builder builder2 = OPUserSkillInfoRet.SkillInfo.newBuilder();
		
		if (params.hasId()) {
			int id = params.getId();
			if (skills.containsKey(id)) {
				Skill value = skills.get(id);
				builder2.setId(value.getId());
				builder2.setItemid(value.getItemid());
				builder2.setLevel(value.getLevel());
				builder2.setQuantity(value.getQuantity());
				builder2.setConsume(value.getConsume());
				builder2.setIsborn(value.getIsdefaultskill());
				builder.addSkill(builder2);
			}
		}
		else {
			Iterator<Map.Entry<Integer, Skill>> iterator = skills.entrySet().iterator();
			while (iterator.hasNext()) {
				Map.Entry<Integer, Skill> entry	= iterator.next();
				Skill value = entry.getValue();
				
				builder2.setId(value.getId());
				builder2.setItemid(value.getItemid());
				builder2.setLevel(value.getLevel());
				builder2.setQuantity(value.getQuantity());
				builder2.setConsume(value.getConsume());
				builder2.setIsborn(value.getIsdefaultskill());
				
				builder.addSkill(builder2);
			}
		}
		player.setLastVerifyTime(time);
		player.sendPacket(OP.code.OPCODE_GET_USER_SKILL_INFORET_S_VALUE, builder);
		
		long current_time_2 = Util.getServerMillTime();
		long taken_time=current_time_2-current_time;
		logger2.info("user skill info handler ret send: {}, {}, {}", new Object[]{playerId ,taken_time});
	}
}

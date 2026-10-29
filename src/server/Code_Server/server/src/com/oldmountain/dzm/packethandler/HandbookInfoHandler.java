package com.oldmountain.dzm.packethandler;

import java.util.HashMap;
import java.util.List;
import java.util.Map;

import net.sf.json.JSONObject;

import org.apache.mina.common.IoSession;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.google.protobuf.InvalidProtocolBufferException;
import com.oldmountain.dzm.entity.Disciple;
import com.oldmountain.dzm.entity.Equip;
import com.oldmountain.dzm.entity.Handbook;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.entity.Skill;
import com.oldmountain.dzm.entity.SkillBook;
import com.oldmountain.dzm.entity.Soul;
import com.oldmountain.dzm.message.OP;
import com.oldmountain.dzm.message.HandbookInfo.OPHandbookInfo;
import com.oldmountain.dzm.message.HandbookInfo.OPHandbookInfoRet;
import com.oldmountain.dzm.net.ObjectAccessor;
import com.oldmountain.dzm.net.packet.Packet;
import com.oldmountain.dzm.util.HandbookUtil;
import com.oldmountain.dzm.util.Util;

/**
 * 获取用户弟子列表信息
 * @author dragon
 *
 */
public class HandbookInfoHandler {
	private static final Logger log = LoggerFactory.getLogger(LoginHandler.class);
	public static void getHandbookInfo(Packet packet,IoSession session){
		OPHandbookInfo params = null;
		OPHandbookInfoRet.Builder builder = OPHandbookInfoRet.newBuilder();
		
		try {
			params = OPHandbookInfo.parseFrom(packet.data.array());
		} catch (InvalidProtocolBufferException e) {
			e.printStackTrace();
			return;
		}
		
		int version = params.getVersion();
		
		if (version != 1) {
			//TODO 版本控制
		}
		
		int uuid = (Integer)session.getAttribute("playerid");
		Player player = ObjectAccessor.getOnlineUser(uuid);
		int id=player.getId();
		log.info("handbook info handler received: {}, {}, {}", new Object[]{id , version});
		long current_time = Util.getServerMillTime();
		/*********************玩家首次登入图鉴系统,将其已有信息加入图鉴中**************************/
		HandbookUtil.checkFirstVisit(player);
		
		/*********************玩家首次登入图鉴系统,将其已有信息加入图鉴中**************************/
		int time = Util.getServerTime();
		player.setLastVerifyTime(time);
		player.sendPacket(OP.code.OPCODE_HANDBOOK_INFORET_S_VALUE, HandbookUtil.buildHandbookBuilder(player.getHandbook()));

		
		long current_time_2 = Util.getServerMillTime();
		long taken_time=current_time_2-current_time;
		log.info("handbook info handler ret send: {}, {}, {}", new Object[]{id ,taken_time});
						  
	}
	
}

package com.oldmountain.dzm.packethandler;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

import org.apache.mina.common.IoSession;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.google.protobuf.InvalidProtocolBufferException;
import com.oldmountain.dzm.entity.Disciple;
import com.oldmountain.dzm.entity.DiscipleXmlTemplate;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.entity.Soul;
import com.oldmountain.dzm.message.OP;
import com.oldmountain.dzm.message.ErrorReporter.OPErrorReporter;
import com.oldmountain.dzm.message.UpgradeDisciple.OPUpgradeDisciple;
import com.oldmountain.dzm.message.UpgradeDisciple.OPUpgradeDiscipleRet;
import com.oldmountain.dzm.net.ObjectAccessor;
import com.oldmountain.dzm.net.packet.Packet;
import com.oldmountain.dzm.util.Util;
import com.oldmountain.dzm.util.XMLTemplateService;

public class UpgradeDiscipleHandler {
	private static final Logger logger2 = LoggerFactory.getLogger("DiscipleInfo");
	public static void upgradeDisciple(Packet packet,IoSession session){
		OPUpgradeDisciple params = null;
		OPUpgradeDiscipleRet.Builder builder = OPUpgradeDiscipleRet.newBuilder();
		
		try {
			params = OPUpgradeDisciple.parseFrom(packet.data.array());
		} catch (InvalidProtocolBufferException e) {
			e.printStackTrace();
			return;
		}
		
		int discipleId = params.getId();
		int playerId = (Integer)session.getAttribute("playerid");
		
		Player player = ObjectAccessor.getOnlineUser(playerId);
		int id=player.getId();
		logger2.info("upgrade disciple handler received: {}, {}, {}", new Object[]{id , discipleId});

		long current_time = Util.getServerMillTime();
		HashMap<Integer, Disciple> userDisciples = player.getDisciples();
		HashMap<Integer, Soul> userSouls = player.getSouls();
		
		if (!userDisciples.containsKey(discipleId)) {
			OPErrorReporter.Builder errBuilder = OPErrorReporter.newBuilder();
			errBuilder.setErrorid(OP.code.OPCODE_UPGRADE_DISCIPLERET_S_VALUE);
			errBuilder.setErrormsg("disciple not exist");
			player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errBuilder);
			return;
		}
		
		Disciple disciple = userDisciples.get(discipleId);
		int itemId = disciple.getItemid();
		if (!userSouls.containsKey(itemId)) {
			OPErrorReporter.Builder errBuilder = OPErrorReporter.newBuilder();
			errBuilder.setErrorid(OP.code.OPCODE_UPGRADE_DISCIPLERET_S_VALUE);
			errBuilder.setErrormsg("soul not exist");
			player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errBuilder);
			return;
		}
		
		if (disciple.getUpgradelevel() >= 10) {
			OPErrorReporter.Builder errBuilder = OPErrorReporter.newBuilder();
			errBuilder.setErrorid(OP.code.OPCODE_UPGRADE_DISCIPLERET_S_VALUE);
			errBuilder.setErrormsg("disciple already upgread for 10 times");
			player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errBuilder);
			return;
		}
		
		Soul soul = userSouls.get(itemId);
		
		List<Integer> data = getUpgradeNeedCount(itemId, disciple.getUpgradelevel());
		if (soul.getCount() < data.get(0)) {
			OPErrorReporter.Builder errBuilder = OPErrorReporter.newBuilder();
			errBuilder.setErrorid(OP.code.OPCODE_UPGRADE_DISCIPLERET_S_VALUE);
			errBuilder.setErrormsg("soul count not enough");
			player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errBuilder);
			return;
		}
		
		soul.setCount(soul.getCount() - data.get(0));
		
		disciple.setPotentiality(disciple.getPotentiality() + data.get(1));
		disciple.setUpgradelevel(disciple.getUpgradelevel() + 1);
		player.updateOneSoul(soul);
		player.updateOneDisciple(disciple);
		player.setLastVerifyTime(Util.getServerTime());
		
		builder.setId(discipleId);
		builder.setUpgradelevel(disciple.getUpgradelevel());
		builder.setPotentiality(disciple.getPotentiality());
		builder.setServertime(Util.getServerTime());
		builder.setSoulcount(soul.getCount());
		
		player.sendPacket(OP.code.OPCODE_UPGRADE_DISCIPLERET_S_VALUE, builder);
		long current_time_2 = Util.getServerMillTime();
		long taken_time=current_time_2-current_time;
		logger2.info("upgrade disciple handler ret send: {}, {}, {}", new Object[]{id ,taken_time});
	}
	
	public static List<Integer> getUpgradeNeedCount(int itemid,int upgradelevel){
		DiscipleXmlTemplate discipleInfo = XMLTemplateService.getDiscipleTemplate(itemid);
		int quality = discipleInfo.getQuality();
		int init = 1;
		int incre = 1;
		int addPoten = 0;
		switch (quality) {
			case 1:
				init = 45;
				incre = 15;
				addPoten = 75;
				break;
			case 2:
				init = 30;
				incre = 15;
				addPoten = 60;
				break;
			case 3:
				init = 15;
				incre = 10;
				addPoten = 45;
				break;
			case 4:
				init = 5;
				incre = 10;
				addPoten = 30;
				break;
			default:
				break;
		}
		List<Integer> data = new ArrayList<Integer>();
		data.add(init + upgradelevel * incre);
		data.add(addPoten);
		return data;
	}
}

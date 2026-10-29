package com.oldmountain.dzm.packethandler;

import java.util.HashMap;
import java.util.List;

import org.apache.mina.common.IoSession;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.google.protobuf.InvalidProtocolBufferException;
import com.oldmountain.dzm.entity.Equip;
import com.oldmountain.dzm.entity.EquipXmlTemplate;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.message.OP;
import com.oldmountain.dzm.message.ErrorReporter.OPErrorReporter;
import com.oldmountain.dzm.message.SellEquip.OPSellEquip;
import com.oldmountain.dzm.message.SellEquip.OPSellEquipRet;
import com.oldmountain.dzm.net.ObjectAccessor;
import com.oldmountain.dzm.net.packet.Packet;
import com.oldmountain.dzm.util.Util;
import com.oldmountain.dzm.util.XMLTemplateService;

public class SellEquipHandler {
	private static final Logger logger2 = LoggerFactory.getLogger("EquipInfo");
	public static void sellEquip(Packet packet,IoSession session){
		OPSellEquip params = null;
		OPSellEquipRet.Builder builder = OPSellEquipRet.newBuilder();
		OPErrorReporter.Builder errBuilder = OPErrorReporter.newBuilder();
		try {
			params = OPSellEquip.parseFrom(packet.data.array());
		} catch (InvalidProtocolBufferException e) {
			e.printStackTrace();
			return;
		}
		
		int playerId = (Integer)session.getAttribute("playerid");
		Player player = ObjectAccessor.getOnlineUser(playerId);
		int playerid=player.getId();
		logger2.info("sell equip handler received: {}, {}, {}", new Object[]{playerid});
		long current_time = Util.getServerMillTime();
		int count = params.getIdCount();
		if (count <= 0) {
			errBuilder.setErrorid(OP.code.OPCODE_SELL_EQUIPRET_S_VALUE);
			errBuilder.setErrormsg("no equip send");
			player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errBuilder);
			logger2.info("Error Reporter send: sell equip handler: {}, {}, {}", new Object[]{playerid});
			return;
		}
		
		List<Integer> ids = params.getIdList();
		
		
		HashMap<Integer, Equip> userEquips = player.getEquips();
		int sellCoins = 0;
		for (int i = 0; i < count; i++) {
			int id = ids.get(i);
			if (!userEquips.containsKey(id)) {
				errBuilder.setErrorid(OP.code.OPCODE_SELL_EQUIPRET_S_VALUE);
				errBuilder.setErrormsg("equip not exist");
				player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errBuilder);
				logger2.info("Error Reporter send: sell equip handler: {}, {}, {}", new Object[]{playerid});
				return;
			}
		}
		
		for (int i = 0; i < count; i++) {
			int id = ids.get(i);
			Equip equip = userEquips.get(id);
			sellCoins += getSellCoins(equip.getItemid(), equip.getLevel());
			player.removeOneEquip(equip);
			builder.addIds(id);
		}
		player.setSilvercoins(player.getSilvercoins() + sellCoins);
		player.setLastVerifyTime(Util.getServerTime());
		
		builder.setSilvercoins(player.getSilvercoins());
		builder.setServertime(Util.getServerTime());
		
		player.sendPacket(OP.code.OPCODE_SELL_EQUIPRET_S_VALUE, builder);
		long current_time_2 = Util.getServerMillTime();
		long taken_time=current_time_2-current_time;
		logger2.info("sell equip handler ret send: {}, {}, {}", new Object[]{playerid ,taken_time});
	}
	
	public static int getSellCoins(int itemId,int level){
		EquipXmlTemplate info = XMLTemplateService.getEquipTemplate(itemId);
		float defaultNum = info.getQuantity();
		float base = 1;
		switch (info.getQuality()) {
			case 1:
				base = 5.25f;
				break;
			case 2:
				base = 4.41f;
				break;
			case 3:
				base = 3.21f;
				break;
			case 4:
				base = 1.63f;
				break;
		}
		
		return (int) Math.round(defaultNum * base + info.getGrowth() * (level - 1) * 0.875) * 20;
	}
}

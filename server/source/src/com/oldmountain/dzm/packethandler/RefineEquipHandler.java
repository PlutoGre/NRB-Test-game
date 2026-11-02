package com.oldmountain.dzm.packethandler;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

import org.apache.mina.common.IoSession;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.google.protobuf.InvalidProtocolBufferException;
import com.oldmountain.dzm.entity.Bag;
import com.oldmountain.dzm.entity.Equip;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.message.OP;
import com.oldmountain.dzm.message.RefineEquip.OPRefineEquip;
import com.oldmountain.dzm.message.RefineEquip.OPRefineEquipRet;
import com.oldmountain.dzm.message.RefineEquip.OPRefineEquip.Meterial;
import com.oldmountain.dzm.net.ObjectAccessor;
import com.oldmountain.dzm.net.packet.Packet;
import com.oldmountain.dzm.util.EquipUtil;
import com.oldmountain.dzm.util.Util;

public class RefineEquipHandler {
	private static final Logger logger2 = LoggerFactory.getLogger("EquipInfo");
	public static void doRefineEquip(Packet packet,IoSession session){
		OPRefineEquip params = null;
		OPRefineEquipRet.Builder builder = OPRefineEquipRet.newBuilder();
		
		try {
			params = OPRefineEquip.parseFrom(packet.data.array());
		} catch (InvalidProtocolBufferException e) {
			e.printStackTrace();
			return;
		}
		
		int equipId = params.getId();
		int count = params.getUsemeterialCount();
		
		if (count <= 0) {
			//TODO 参数不正确
		}
		List<Meterial> meterials = params.getUsemeterialList();
		int playerId = (Integer)session.getAttribute("playerid");
		Player player = ObjectAccessor.getOnlineUser(playerId);
		int playerid=player.getId();
		logger2.info("refine equip handler received: {}, {}, {}", new Object[]{playerid , equipId});
		int current_time = Util.getServerTime();
		HashMap<Integer, Equip> userEquips = player.getEquips();
		
		if (!userEquips.containsKey(equipId)) {
			//TODO 装备不存在
		}
		
		Equip equip = userEquips.get(equipId);
		HashMap<Integer, Bag> userBags = player.getBags();
		int addScore = 0;
		for (int i = 0; i < count; i++) {
			Meterial meterial = meterials.get(i);
			int id = meterial.getId();
			int need_count = meterial.getCount();
			if (!userBags.containsKey(id) || userBags.get(id).getCount() < need_count) {
				//TODO 材料不存在，或者数量不够，认定为前端作弊
				return ;
			}
			addScore = EquipUtil.getRefineMeterialXp(equip.getItemid());
			userBags.get(id).setCount(userBags.get(id).getCount() - need_count);
		}
		equip.setRefinexp(equip.getRefinexp() + addScore);
		int newRefineLevel = EquipUtil.getRefineLevelByXp(equip.getRefinexp());
		if (newRefineLevel > equip.getRefinelevel()) {
			equip.setRefinelevel(newRefineLevel);
			
			if (newRefineLevel % 10 == 0) {
				ArrayList<Integer> broParams = new ArrayList<Integer>(1);
				broParams.add(equip.getItemid());
				broParams.add(newRefineLevel);
				BoardCastInfoHandler.addBoardCase(BoardCastInfoHandler.REFINEEQUIP_TYPE, player.getName(), broParams);
			}
		}
		userEquips.put(equipId, equip);
		player.setEquips(userEquips);
		player.setBags(userBags);
		
		builder.setServertime(current_time);
		OPRefineEquipRet.EquipInfo.Builder builder2 = OPRefineEquipRet.EquipInfo.newBuilder();
		builder2.setId(equipId);
		builder2.setItemid(equip.getItemid());
		builder2.setType(equip.getType());
		builder2.setLevel(equip.getLevel());
		builder2.setRefinexp(equip.getRefinexp());
		builder2.setRefinelevel(equip.getRefinelevel());
		builder2.setAttack(equip.getAttackToInt());
		builder2.setDefence(equip.getDefenceToInt());
		builder2.setHealth(equip.getHealthToInt());
		builder2.setBattleid(equip.getBattleid());
		
		builder.setEquip(builder2);
		
		player.sendPacket(OP.code.OPCODE_REFINE_EQUIPRET_S_VALUE, builder);
		int current_time_2 = Util.getServerTime();
		int taken_time=current_time_2-current_time;
		logger2.info("refine equip handler ret send: {}, {}, {}", new Object[]{playerid ,taken_time});
	}
}

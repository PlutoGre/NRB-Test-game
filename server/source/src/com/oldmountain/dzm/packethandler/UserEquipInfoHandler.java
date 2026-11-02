package com.oldmountain.dzm.packethandler;

import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;

import com.oldmountain.dzm.util.EquipUtil;
import net.sf.json.JSONArray;
import org.apache.mina.common.IoSession;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.google.protobuf.InvalidProtocolBufferException;
import com.oldmountain.dzm.entity.Equip;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.message.OP;
import com.oldmountain.dzm.message.UserEquipInfo.OPUserEquipInfoRet;
import com.oldmountain.dzm.message.UserEquipInfo.OPUserEquipleInfo;
import com.oldmountain.dzm.net.ObjectAccessor;
import com.oldmountain.dzm.net.packet.Packet;
import com.oldmountain.dzm.util.Util;

/**
 * 获取用户装备列表信息
 * @author dragon
 *
 */
public class UserEquipInfoHandler {
	private static final Logger logger2 = LoggerFactory.getLogger("EquipInfo");
	public static void getUserEquipInfo(Packet packet,IoSession session){
		OPUserEquipleInfo params = null;
		OPUserEquipInfoRet.Builder builder = OPUserEquipInfoRet.newBuilder();
		
		try {
			params = OPUserEquipleInfo.parseFrom(packet.data.array());
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
		logger2.info("user equip info handler received: {}, {}, {}", new Object[]{playerId , version});
		long current_time = Util.getServerMillTime();
		int time = Util.getServerTime();
		builder.setServertime(time);
		
		HashMap<Integer, Equip> equips = player.getEquips();
		OPUserEquipInfoRet.EquipInfo.Builder builder2 = OPUserEquipInfoRet.EquipInfo.newBuilder();
		
		if (params.hasId()) {
			int id = params.getId();
			if (equips.containsKey(id)) {
				Equip value = equips.get(id);
				
				builder2.setId(value.getId());
				builder2.setItemid(value.getItemid());
				builder2.setLevel(value.getLevel());
				builder2.setRefinelevel(value.getRefinelevel());
				
				OPUserEquipInfoRet.EquipInfo.PropertyInfo.Builder builder3 = OPUserEquipInfoRet.EquipInfo.PropertyInfo.newBuilder();
				
				if (value.getAttack() != 0) {
					builder3.setType(1);
					builder3.setNum(value.getAttackToInt());
				}
				builder2.addPInfo(builder3);
				if (value.getDefence() != 0) {
					builder3.setType(2);
					builder3.setNum(value.getDefenceToInt());
				}
				builder2.addPInfo(builder3);
				if (value.getHealth() != 0) {
					builder3.setType(3);
					builder3.setNum(value.getHealthToInt());
				}
				builder2.addPInfo(builder3);
                //builder2.setStoneinlaidinfo(value.getDiamondInfo().replaceAll("%","_"));
                JSONArray _stoneArr=value.getStoneJsonArr();
                for(int i=0;i<_stoneArr.size();++i)
                {
                    builder2.addStoneInfo(EquipUtil.getStoneInfoByStoneJson(_stoneArr.getJSONObject(i),i+1));
                }
                builder2.setBuffvalue(value.getStoneAdditionValue());
				builder.addEquip(builder2);
			}
		}
		else {
			Iterator<Map.Entry<Integer, Equip>> iterator = equips.entrySet().iterator();
			while (iterator.hasNext()) {
				Map.Entry<Integer, Equip> entry = iterator.next();
				Equip value = entry.getValue();
				builder2 = OPUserEquipInfoRet.EquipInfo.newBuilder();
				builder2.setId(value.getId());
				builder2.setItemid(value.getItemid());
				builder2.setLevel(value.getLevel());
				builder2.setRefinexp(value.getRefinexp());
				builder2.setRefinelevel(value.getRefinelevel());
				
				OPUserEquipInfoRet.EquipInfo.PropertyInfo.Builder builder3 = OPUserEquipInfoRet.EquipInfo.PropertyInfo.newBuilder();
				
				if (value.getAttack() != 0) {
					builder3.setType(1);
					builder3.setNum(value.getAttackToInt());
					builder2.addPInfo(builder3);
				}
				
				if (value.getDefence() != 0) {
					builder3.setType(2);
					builder3.setNum(value.getDefenceToInt());
					builder2.addPInfo(builder3);
				}
				
				if (value.getHealth() != 0) {
					builder3.setType(3);
					builder3.setNum(value.getHealthToInt());
					builder2.addPInfo(builder3);
				}
                //builder2.setStoneinlaidinfo(value.getDiamondInfo().replaceAll("%","_"));
                JSONArray _stoneArr=value.getStoneJsonArr();
                for(int i=0;i<_stoneArr.size();++i)
                {
                    builder2.addStoneInfo(EquipUtil.getStoneInfoByStoneJson(_stoneArr.getJSONObject(i), i + 1));
                }
                builder2.setBuffvalue(value.getStoneAdditionValue());
				builder.addEquip(builder2);
			}
		}
		player.setLastVerifyTime(time);
		player.sendPacket(OP.code.OPCODE_GET_USER_EQUIP_INFORET_S_VALUE, builder);
		
		long current_time_2 = Util.getServerMillTime();
		long taken_time=current_time_2-current_time;
		logger2.info("user equip info handler ret send: {}, {}, {}", new Object[]{playerId ,taken_time});
	}
}

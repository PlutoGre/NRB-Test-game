package com.oldmountain.dzm.packethandler;

import java.util.HashMap;

import org.apache.mina.common.IoSession;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.google.protobuf.InvalidProtocolBufferException;
import com.oldmountain.dzm.entity.Battle;
import com.oldmountain.dzm.entity.Disciple;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.message.OP;
import com.oldmountain.dzm.message.DealTrainStatus.OPDealTrainStatus;
import com.oldmountain.dzm.message.DealTrainStatus.OPDealTrainStatusRet;
import com.oldmountain.dzm.message.UserBattleArray.OPUserBattleArrayRet;
import com.oldmountain.dzm.net.ObjectAccessor;
import com.oldmountain.dzm.net.packet.Packet;
import com.oldmountain.dzm.util.BattleUtil;
import com.oldmountain.dzm.util.Util;

public class DealTrainStatusHandler {
	private static final Logger logger2 = LoggerFactory.getLogger("DiscipleInfo");
	public static void dealTrainStatus(Packet packet,IoSession session){
		OPDealTrainStatus params = null;
		OPDealTrainStatusRet.Builder builder = OPDealTrainStatusRet.newBuilder();
		
		try {
			params = OPDealTrainStatus.parseFrom(packet.data.array());
		} catch (InvalidProtocolBufferException e) {
			e.printStackTrace();
			return;
		}
		
		int id = params.getId();
		int type = params.getType();
		
		int playerId = (Integer)session.getAttribute("playerid");
		Player player = ObjectAccessor.getOnlineUser(playerId);
		int playerid=player.getId();
		logger2.info("deal train status handler received: {}, {}, {}", new Object[]{id , type});
		long current_time = Util.getServerMillTime();
		int time = Util.getServerTime();
		
		HashMap<Integer, Disciple> userDisciples = player.getDisciples();
		Disciple disciple = userDisciples.get(id);
		HashMap<String, HashMap<String, Integer>> trainStatus = disciple.getTrainstatus();
		if (!trainStatus.containsKey("minus")) {
			//TODO 没有这个状态，请求错误
			return ;
		}
		HashMap<String, Integer> minus = trainStatus.get("minus");
		HashMap<String, Integer> add = trainStatus.get("add");
		
		OPDealTrainStatusRet.PropertyInfo.Builder builder2 = OPDealTrainStatusRet.PropertyInfo.newBuilder();
		switch (type){
			case 1://接受
				int minusPro = minus.get("pro");
				int minusNum = minus.get("num");
				int addPro = add.get("pro");
				int addNum = add.get("num");
				
				int delPoten = addNum - minusNum;
				builder2.setType(minusPro);
				switch (minusPro) {
					case 1:
						disciple.setAttack(disciple.getAttack() - minusNum);
						builder2.setNum(disciple.getAttackToInt());
						break;
					case 2:
						disciple.setDefence(disciple.getDefence() - minusNum);
						builder2.setNum(disciple.getDefenceToInt());
						break;
					case 3:
						disciple.setHealth(disciple.getHealth() - minusNum);
						builder2.setNum(disciple.getHealthToInt());
						break;
					case 4:
						disciple.setRp(disciple.getRp() - minusNum);
						builder2.setNum(disciple.getRpToInt());
						break;
					default:
						break;
				}
				builder.addPro(builder2);
				
				builder2.setType(addPro);
				switch (addPro) {
					case 1:
						disciple.setAttack(disciple.getAttack() + addNum);
						builder2.setNum(disciple.getAttackToInt());
						break;
					case 2:
						disciple.setDefence(disciple.getDefence() + addNum);
						builder2.setNum(disciple.getDefenceToInt());
						break;
					case 3:
						disciple.setHealth(disciple.getHealth() + addNum);
						builder2.setNum(disciple.getHealthToInt());
						break;
					case 4:
						disciple.setRp(disciple.getRp() + addNum);
						builder2.setNum(disciple.getRpToInt());
						break;
					default:
						break;
				}
				builder.addPro(builder2);
				
				disciple.setPotentiality(disciple.getPotentiality() - delPoten);
				
				break;
			case 2://放弃
				break;
		}
		
		disciple.setTrainstatus(new HashMap<String, HashMap<String,Integer>>());
		
		player.updateOneDisciple(disciple);
		player.setLastVerifyTime(time);
		
		if (disciple.getBattleid() != 0) {
			int battleid = disciple.getBattleid();
			String battleids = player.getBattleids();
			String[] battleArray = battleids.split(",");
			for (int i = 0; i < battleArray.length; i++) {
				if (battleid == Integer.parseInt(battleArray[i])) {
					Battle battle = player.getBattlearray().get(battleid);
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
		
		builder.setId(id);
		builder.setPotentiality(disciple.getPotentiality());
		builder.setServertime(time);
		player.sendPacket(OP.code.OPCODE_DEAL_TRAIN_STATUSRET_S_VALUE, builder);
		long current_time_2 = Util.getServerMillTime();
		long taken_time=current_time_2-current_time;
		logger2.info("deal train status handler ret send: {}, {}, {}", new Object[]{playerid ,taken_time});
	}
}

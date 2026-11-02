package com.oldmountain.dzm.packethandler;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Random;

import org.apache.mina.common.IoSession;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.google.protobuf.InvalidProtocolBufferException;
import com.oldmountain.dzm.entity.Bag;
import com.oldmountain.dzm.entity.Disciple;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.message.OP;
import com.oldmountain.dzm.message.ErrorReporter.OPErrorReporter;
import com.oldmountain.dzm.message.TrainDisciple.OPTrainDisciple;
import com.oldmountain.dzm.message.TrainDisciple.OPTrainDiscipleRet;
import com.oldmountain.dzm.net.ObjectAccessor;
import com.oldmountain.dzm.net.packet.Packet;
import com.oldmountain.dzm.util.Util;

public class TrainDiscipleHandler {
	private static final Logger logger2 = LoggerFactory.getLogger("DiscipleInfo");
	private static final Logger logger_goldCost = LoggerFactory.getLogger("GoldCost");
	public static void trainDisciple(Packet packet,IoSession session){
		OPTrainDisciple params = null;
		OPTrainDiscipleRet.Builder builder = OPTrainDiscipleRet.newBuilder();
		
		try {
			params = OPTrainDisciple.parseFrom(packet.data.array());
		} catch (InvalidProtocolBufferException e) {
			e.printStackTrace();
			return;
		}
		
		int id = params.getId();
		int type = params.getType();
		
		int playerId = (Integer)session.getAttribute("playerid");
		Player player = ObjectAccessor.getOnlineUser(playerId);
		int playerid=player.getId();
		logger2.info("train disciple handler received: {}, {}, {}", new Object[]{playerid , id , type});
		long current_time = Util.getServerMillTime();
		int time = Util.getServerTime();
		HashMap<Integer, Disciple> userDisciples = player.getDisciples();
		if (!userDisciples.containsKey(id)) {
			OPErrorReporter.Builder errBuilder = OPErrorReporter.newBuilder();
			errBuilder.setErrorid(OP.code.OPCODE_TRAIN_DISCIPLERET_S_VALUE);
			errBuilder.setErrormsg("disciple not exist");
			player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errBuilder);
			return;
		}
		
		HashMap<Integer, Bag> userBags = player.getBags();
		
		Bag bag = userBags.get(1502001);
		
		Disciple disciple = userDisciples.get(id);
		int potenLeft = disciple.getPotentiality();
		int minus = 0;
		int add = 0;
		int costGold = 0;
		int costPellet = 0;
		int userGold=player.getGoldCoins();
		Random random = new Random();
		
		List<Integer> properties = new ArrayList<Integer>();
		properties.add(1);
		properties.add(2);
		properties.add(3);
		properties.add(4);
		int minusProRand = random.nextInt(4);
		int minusPro = properties.get(minusProRand);
		properties.remove(minusProRand);
		int addProRand = random.nextInt(3);
		int addPro = properties.get(addProRand);
		int costId=0;
		switch (type) {
			case 1://普通一次
				if (bag.getCount() < 5) {
					OPErrorReporter.Builder errBuilder = OPErrorReporter.newBuilder();
					errBuilder.setErrorid(OP.code.OPCODE_TRAIN_DISCIPLERET_S_VALUE);
					errBuilder.setErrormsg("pellet not enough");
					player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errBuilder);
					return;
				}
				minus = 1 + random.nextInt(3);
				add = minus + 1 + random.nextInt(3);
				add = add > potenLeft?potenLeft+minus:add;
				costPellet = 5;
				break;
			case 2://精心一次
				if (bag.getCount() < 5) {
					OPErrorReporter.Builder errBuilder = OPErrorReporter.newBuilder();
					errBuilder.setErrorid(OP.code.OPCODE_TRAIN_DISCIPLERET_S_VALUE);
					errBuilder.setErrormsg("pellet not enough");
					player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errBuilder);
					return;
				}
				if (player.getSysgoldcoins() + player.getRechargegoldcoins() < 1) {
					OPErrorReporter.Builder errBuilder = OPErrorReporter.newBuilder();
					errBuilder.setErrorid(OP.code.OPCODE_TRAIN_DISCIPLERET_S_VALUE);
					errBuilder.setErrormsg("gold not enough");
					player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errBuilder);
					return;
				}
				minus = 1 + random.nextInt(3);
				add = minus + 3 + random.nextInt(4);
				add = add > potenLeft?potenLeft+minus:add;
				costGold = 10;
				costPellet = 5;
				costId=1001;
				break;
			case 3://普通十次
				if (bag.getCount() < 50) {
					OPErrorReporter.Builder errBuilder = OPErrorReporter.newBuilder();
					errBuilder.setErrorid(OP.code.OPCODE_TRAIN_DISCIPLERET_S_VALUE);
					errBuilder.setErrormsg("pellet not enough");
					player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errBuilder);
					return;
				}
				int minusRandom1=1 + random.nextInt(3);
				minus = minusRandom1 * 10;
				add = (minusRandom1 + 1 + random.nextInt(3)) * 10;
				add = (add - minus)> potenLeft?potenLeft+minus:add;
				costPellet = 50;
				break;
			case 4://精心十次
				if (bag.getCount() < 50) {
					OPErrorReporter.Builder errBuilder = OPErrorReporter.newBuilder();
					errBuilder.setErrorid(OP.code.OPCODE_TRAIN_DISCIPLERET_S_VALUE);
					errBuilder.setErrormsg("pellet not enough");
					player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errBuilder);
					return;
				}
				if (player.getSysgoldcoins() + player.getRechargegoldcoins() < 10) {
					OPErrorReporter.Builder errBuilder = OPErrorReporter.newBuilder();
					errBuilder.setErrorid(OP.code.OPCODE_TRAIN_DISCIPLERET_S_VALUE);
					errBuilder.setErrormsg("gold not enough");
					player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errBuilder);
					return;
				}
				int minusRandom2=1 + random.nextInt(3);
				minus = minusRandom2 * 10;
				add = (minusRandom2 + 3 + random.nextInt(4)) * 10;
				add = (add - minus)> potenLeft?potenLeft+minus:add;
				costGold = 88;  /// 精心10次修改为88
				costPellet = 50;
				costId=1002;
				break;
			default:
				OPErrorReporter.Builder errBuilder = OPErrorReporter.newBuilder();
				errBuilder.setErrorid(OP.code.OPCODE_TRAIN_DISCIPLERET_S_VALUE);
				errBuilder.setErrormsg("param type wrong");
				player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errBuilder);
				return;
		}
		if(minusPro==1 && disciple.getAttackToInt()-minus<0){
			int attack=disciple.getAttackToInt();
			add=add-(minus-attack);
			minus=attack;
		}
		else if(minusPro==2 && disciple.getDefenceToInt()-minus<0){
			int defence=disciple.getDefenceToInt();
			add=add-(minus-defence);
			minus=defence;
		}
		else if(minusPro==3 && disciple.getHealthToInt()-minus<0){
			int health=disciple.getHealthToInt();
			add=add-(minus-(health-1));
			minus=health-1;
		}
		else if(minusPro==4 && disciple.getRpToInt()-minus<0){
			int rp=disciple.getRpToInt();
			add=add-(minus-rp);
			minus=rp;
		}
		
		if (player.getGoldCoins() < costGold) {
			OPErrorReporter.Builder errBuilder = OPErrorReporter.newBuilder();
			errBuilder.setErrorid(OP.code.OPCODE_TRAIN_DISCIPLERET_S_VALUE);
			errBuilder.setErrormsg("gold coins not enough");
			player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errBuilder);
			return;
		}
		
		player.consumeGold(costGold);
		bag.setCount(bag.getCount() - costPellet);
		
		HashMap<String, HashMap<String, Integer>> status = new HashMap<String, HashMap<String, Integer>>();
		
		HashMap<String, Integer> info = new HashMap<String, Integer>();
		info.put("pro", minusPro);
		info.put("num", minus);
		status.put("minus", info);
		
		info = new HashMap<String, Integer>();
		info.put("pro", addPro);
		info.put("num", add);
		status.put("add",info);
		
		disciple.setTrainstatus(status);
		player.getDisciples().put(id, disciple);
		player.updateOneBag(bag);
		
		player.setLastVerifyTime(time);
		
		OPTrainDiscipleRet.TrainStatus.Builder builder2 = OPTrainDiscipleRet.TrainStatus.newBuilder();
		builder2.setPro(minusPro);
		builder2.setNum(minus);
		builder.setMinus(builder2);
		
		builder2.setPro(addPro);
		builder2.setNum(add);
		builder.setAdd(builder2);
		builder.setId(id);
		builder.setServertime(time);
		builder.setGoldcoins(player.getSysgoldcoins() + player.getRechargegoldcoins());
		player.sendPacket(OP.code.OPCODE_TRAIN_DISCIPLERET_S_VALUE, builder);
		
		long current_time_2 = Util.getServerMillTime();
		long taken_time=current_time_2-current_time;
		logger2.info("train disciple handler ret send: {}, {}, {}", new Object[]{playerid ,taken_time});
		logger_goldCost.info("Gold Cost Info: {} , {} , {} , {} , {}, {} , {}",new Object[]{ Util.getServerTime() , player.getId() , costId , 1 , userGold , player.getGoldCoins() , costGold});
	}
}

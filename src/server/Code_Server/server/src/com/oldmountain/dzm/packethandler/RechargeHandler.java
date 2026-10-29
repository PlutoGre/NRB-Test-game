package com.oldmountain.dzm.packethandler;

import java.util.HashMap;
import java.util.List;

import org.apache.mina.common.IoSession;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.google.protobuf.InvalidProtocolBufferException;
import com.oldmountain.dzm.DZM;
import com.oldmountain.dzm.entity.Bag;
import com.oldmountain.dzm.entity.Mapping;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.entity.PlayerFrom;
import com.oldmountain.dzm.entity.Recharge;
import com.oldmountain.dzm.entity.RechargeXmlTemplate;
import com.oldmountain.dzm.message.OP;
import com.oldmountain.dzm.message.Recharge.OPRecharge;
import com.oldmountain.dzm.message.Recharge.OPRechargeRet;
import com.oldmountain.dzm.message.ShopPurchase.OPShopPurchaseCheckRet;
import com.oldmountain.dzm.message.UserToolInfo.OPUserToolInfoRet;
import com.oldmountain.dzm.net.ObjectAccessor;
import com.oldmountain.dzm.net.packet.Packet;
import com.oldmountain.dzm.services.ActivityServices;
import com.oldmountain.dzm.util.TitleUtil;
import com.oldmountain.dzm.util.Util;
import com.oldmountain.dzm.util.XMLTemplateService;

public class RechargeHandler {
	private static final Logger recharge_log = LoggerFactory.getLogger("RechargeInfo");
	public static void recharge(Packet packet,IoSession session){
		OPRecharge params = null;
		OPRechargeRet.Builder builder = OPRechargeRet.newBuilder();
		OPShopPurchaseCheckRet.Builder builder2 = OPShopPurchaseCheckRet.newBuilder();
		try {
			params = OPRecharge.parseFrom(packet.data.array());
		} catch (InvalidProtocolBufferException e) {
			e.printStackTrace();
		}
		
		String ConsumeStreamId = params.getConsumeStreamId();
		String CooOrderSerial = params.getCooOrderSerial();
		String Uin = params.getUin();
		String requestGoodsId = params.getGoodsId();
		String[] requestGoodsIdArray = requestGoodsId.split("\\.");
		int GoodsId = Integer.parseInt(requestGoodsIdArray[0]);
		int GoodsCount = params.getGoodsCount();
		String CreateTime = params.getCreateTime();
		Mapping mapping = ObjectAccessor.getMapping(Uin);
		if (mapping == null) {
			List<Object> mappings = DZM.getEntityManager().limitQuery("from Mapping where puid = ?", 0, 1, Uin);
			if (mappings.size() == 1) {
				mapping = (Mapping)mappings.get(0);
			}
		}
		if (mapping == null) {
			builder.setStatus(2);
			Packet pt = new Packet(OP.code.OPCODE_RECHARGERET_S_VALUE, builder.build().toByteArray());
			session.write(pt);
			return;
		}
		int playerId = mapping.getGameid();
		Player player = ObjectAccessor.getOnlineUser(playerId);
		int id=player.getId();
		recharge_log.info("recharge handler received: {}, {}, {}, {}, {}, {}, {}", new Object[]{id , ConsumeStreamId , CooOrderSerial , Uin , GoodsId , GoodsCount , CreateTime});
		long current_time = Util.getServerMillTime();
		
		if (player == null) {
			List<Object> list = DZM.getEntityManager().limitQuery("from Player where id = ?", 0, 1, playerId);
			if (list.size() == 1) {
				player = (Player)list.get(0);
			}
			List<Object> from = DZM.getEntityManager().limitQuery("from PlayerFrom where playerid=?", 0, 1, playerId);
			if (from.size() == 1) {
				PlayerFrom fromInfo = (PlayerFrom)from.get(0);
				player.setPlatform(fromInfo.getPlatform());
				player.setDeviceId(fromInfo.getDeviceid());
			}
		}
		
		if (player == null) {
			builder.setStatus(3);
			Packet pt = new Packet(OP.code.OPCODE_RECHARGERET_S_VALUE, builder.build().toByteArray());
			session.write(pt);
			return;
		}
		
		HashMap<Integer, RechargeXmlTemplate> rechargeXml = null;
		String platName = null;
		if (params.hasPlatform()) {
			platName = params.getPlatform().toLowerCase();
			try {
				rechargeXml = XMLTemplateService.getRechargeConfigByPlayform(platName);
			} catch (Exception e) {
			}
		}
		else {
			rechargeXml = XMLTemplateService.getAllRecharges();
		}
		
		if (rechargeXml == null || !rechargeXml.containsKey(GoodsId)) {
			builder.setStatus(3);
			Packet pt = new Packet(OP.code.OPCODE_RECHARGERET_S_VALUE, builder.build().toByteArray());
			session.write(pt);
			return;
		}
		
		RechargeXmlTemplate rechargeConfig = rechargeXml.get(GoodsId);
		int totalAmount = 0;
		int amount = rechargeConfig.getAmount();
		int addAmount = rechargeConfig.getAddNum();
		totalAmount = amount + addAmount;
		int isFirstPay = 0;
		if (player.getRechargenum() == 0) {
			//// 双倍奖励的地方
			totalAmount = amount * 2 + addAmount;
			isFirstPay = 1;
			Bag bag = new Bag();
			bag.setItemid(3504001);
			bag.setPlayerid(playerId);
			bag.setCount(1);
			bag.setCreatetime(Util.getServerTime());
			player.addOneBag(bag);
			
			OPUserToolInfoRet.ToolInfo.Builder toolBuilder = OPUserToolInfoRet.ToolInfo.newBuilder();
			toolBuilder.setId(bag.getId());
			toolBuilder.setItemid(bag.getItemid());
			toolBuilder.setCount(bag.getCount());
			builder2.setTool(toolBuilder);
			
		}
		
		player.setRechargegoldcoins(player.getRechargegoldcoins() + totalAmount * GoodsCount);
		player.setRechargenum(player.getRechargenum() + rechargeConfig.getCostMoney() * GoodsCount);
		int vipLevel = 0;
		if (platName != null) {
			//vipLevel = getVipLevelByNumAndPlatform(player.getRechargenum(),platName);
			
			vipLevel = getVipLevelByNum(player.getRechargenum());
		}
		else {
			vipLevel = getVipLevelByNum(player.getRechargenum());
		}
		if (vipLevel > player.getViplevel()) {
			player.setViplevel(vipLevel);
			TitleUtil.checkNewTitle(player, 2, vipLevel);
		}
//		DZM.getEntityManager().updateSync(player);
		
		Recharge recharge = new Recharge();
		recharge.setCooorderserial(CooOrderSerial);
		recharge.setConsumestreamid(ConsumeStreamId);
		recharge.setUid(Uin);
		recharge.setPlayerid(playerId);
		recharge.setGoodsid(GoodsId);
		recharge.setGoodscount(GoodsCount);
		recharge.setGoodscost(rechargeConfig.getCostMoney());
		recharge.setAmount(amount);
		recharge.setAddnum(addAmount);
		recharge.setIsfirstpay(isFirstPay);
		recharge.setRequesttime(CreateTime);
		recharge.setCreatetime(Util.getServerTime());
		DZM.getEntityManager().createSync(recharge);
		
		player.setLastVerifyTime(Util.getServerTime());
		
		builder.setStatus(1);
//		Packet pt = new Packet(OP.code.OPCODE_RECHARGERET_S_VALUE, builder.build().toByteArray());
//		session.write(pt);
		
		//邀请奖励部分
		if (player.getInvitedid() != 0) {
			UserInvitedHandler.updateInviteStatus(player.getInvitedid(), 4, amount);
			if (isFirstPay == 1) {
				UserInvitedHandler.updateInviteStatus(player.getInvitedid(), 3, 1);
			}
		}
		
		
		builder2.addCooOrderSerial(CooOrderSerial);
		builder2.setVersion(1);
		builder2.setTotalMoney(player.getGoldCoins());
		builder2.setVipLevel(player.getViplevel());
		builder2.setRechargenum(player.getRechargenum());
		try {
			player.sendPacket(OP.code.OPCODE_GET_SHOP_PURCHASE_RETURN_S_VALUE, builder2);
		} catch (Exception e) {
			e.printStackTrace();
		}
		//充值返利累积
		ChargeRebateHandler.addChargeHeap(packet, session , rechargeConfig.getCostMoney() , player ,Util.getServerTime());
		long current_time_2 = Util.getServerMillTime();
		long taken_time=current_time_2-current_time;
		recharge_log.info("recharge handler ret send: shopPurchaseCheckRet : {}, {}, {}, {}, {}, {}, {}, {}, {}", new Object[]{id , taken_time , isFirstPay , Util.getServerTime() , GoodsId , GoodsCount , player.getRegistertime(),player.getPlatform(),player.getDeviceId()});
		//女神回馈开启判定,活动开启则充值时增加对应额度的鲜花
		int godness_termId=ActivityServices.getTermIdById(ActivityServices.GODDESS_REBATE, current_time_2);
		if(godness_termId!=0){
			GoddessRebateHandler.addFlower(packet, session,rechargeConfig.getCostMoney(),player , godness_termId,current_time_2);
		}
		
	}
	
	public static int getVipLevelByNum(int num){
//		int viplevel = 0;
//		if (num >= 60000) {
//			viplevel = 13;
//		}
//		else if (num >= 30000) {
//			viplevel = 12;
//		}
//		else if (num >= 15000) {
//			viplevel = 11;
//		}
//		else if (num >= 8000){
//			viplevel = 10;
//		}
//		else if (num >= 5000) {
//			viplevel = 9;
//		}
//		else if (num >= 2000) {
//			viplevel = 8;
//		}
//		else if (num >= 1000) {
//			viplevel = 7;
//		}
//		else if (num >= 648) {
//			viplevel = 6;
//		}
//		else if (num >= 328) {
//			viplevel = 5;
//		}
//		else if (num >= 198) {
//			viplevel = 4;
//		}
//		else if (num >= 42) {
//			viplevel = 3;
//		}
//		else if (num >= 30) {
//			viplevel = 2;
//		}
//		else if (num >= 6){
//			viplevel = 1;
//		}
		/// 又是TMD写死的代码，VIP充值调整。
		int viplevel = 0;
		if (num >= 2000) {
			viplevel = 13;
		}
		else if (num >= 1500) {
			viplevel = 12;
		}
		else if (num >= 1200) {
			viplevel = 11;
		}
		else if (num >= 1000){
			viplevel = 10;
		}
		else if (num >= 800) {
			viplevel = 9;
		}
		else if (num >= 500) {
			viplevel = 8;
		}
		else if (num >= 350) {
			viplevel = 7;
		}
		else if (num >= 200) {
			viplevel = 6;
		}
		else if (num >= 100) {
			viplevel = 5;
		}
		else if (num >= 50) {
			viplevel = 4;
		}
		else if (num >= 3) {
			viplevel = 3;
		}
		else if (num >= 2) {
			viplevel = 2;
		}
		else if (num >= 1){
			viplevel = 1;
		}
		return viplevel;
	}
	
	private static int getVipLevelByNumAndPlatform(int num,String platform){
		int base = 1;
		if (platform == "uc") {
			base = 10;
		}
		
		int viplevel = 0;
		if (num >= 60000 * base) {
			viplevel = 13;
		}
		else if (num >= 30000 * base) {
			viplevel = 12;
		}
		else if (num >= 15000 * base) {
			viplevel = 11;
		}
		else if (num >= 8000 * base){
			viplevel = 10;
		}
		else if (num >= 5000 * base) {
			viplevel = 9;
		}
		else if (num >= 2000 * base) {
			viplevel = 8;
		}
		else if (num >= 1000 * base) {
			viplevel = 7;
		}
		else if (num >= 648 * base) {
			viplevel = 6;
		}
		else if (num >= 328 * base) {
			viplevel = 5;
		}
		else if (num >= 198 * base) {
			viplevel = 4;
		}
		else if (num >= 42 * base) {
			viplevel = 3;
		}
		else if (num >= 30 * base) {
			viplevel = 2;
		}
		else if (num >= 6 * base){
			viplevel = 1;
		}
		return viplevel;
	}
}

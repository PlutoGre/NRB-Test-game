package com.oldmountain.dzm.packethandler;

import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;

import org.apache.mina.common.IoSession;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.google.protobuf.InvalidProtocolBufferException;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.entity.RechargeXmlTemplate;
import com.oldmountain.dzm.message.OP;
import com.oldmountain.dzm.message.ErrorReporter.OPErrorReporter;
import com.oldmountain.dzm.message.ShopList.OPShopList;
import com.oldmountain.dzm.message.ShopList.OPShopListRet;
import com.oldmountain.dzm.net.ObjectAccessor;
import com.oldmountain.dzm.net.packet.Packet;
import com.oldmountain.dzm.util.Util;
import com.oldmountain.dzm.util.XMLTemplateService;

public class GetShopListHandler {
	private static final Logger log = LoggerFactory.getLogger(LoginHandler.class);
	private static final Logger logger2 = LoggerFactory.getLogger("ShopInfo");
	public static void getShopList(Packet packet,IoSession session){
		OPShopList params = null;
		OPShopListRet.Builder builder = OPShopListRet.newBuilder();
		
		try {
			params = OPShopList.parseFrom(packet.data.array());
		} catch (InvalidProtocolBufferException e) {
			e.printStackTrace();
			return ;
		}
		
		int version = params.getVersion();
		
		if (version != 1) {
			
		}
		int playerId = (Integer)session.getAttribute("playerid");
		Player player = ObjectAccessor.getOnlineUser(playerId);
		int playerid=player.getId();
		log.info("get shop list handler received: {}, {}, {}", new Object[]{playerid , version});
		logger2.info("get shop list handler received: {}, {}, {}", new Object[]{playerid , version});
		long current_time = Util.getServerMillTime();
		int time = Util.getServerTime();
		HashMap<Integer, RechargeXmlTemplate> rechargeXml = null;
		if (params.hasPlatname()) {
			String platform = params.getPlatname().toLowerCase();
			try {
				rechargeXml = XMLTemplateService.getRechargeConfigByPlayform(platform);
			} catch (Exception e) {
				e.printStackTrace();
				return;
			}
		}
		else {
			rechargeXml = XMLTemplateService.getAllRecharges();
		}
		
		if (rechargeXml == null) {
			OPErrorReporter.Builder errBuilder = OPErrorReporter.newBuilder();
			errBuilder.setErrorid(OP.code.OPCODE_GET_SHOP_ITEM_LISTRET_S_VALUE);
			errBuilder.setErrormsg("config not exist");
			player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errBuilder);
			return;
		}
		
		Iterator<Map.Entry<Integer, RechargeXmlTemplate>> iterator = rechargeXml.entrySet().iterator();
		while (iterator.hasNext()) {
			OPShopListRet.ShopItem.Builder builder2 = OPShopListRet.ShopItem.newBuilder();
			Map.Entry<Integer, RechargeXmlTemplate> entry = iterator.next();
			int id = entry.getKey();
			RechargeXmlTemplate value = entry.getValue();
			builder2.setProductID(id + ".haizeiwang.youai");
			builder2.setProductName("钻石" + value.getAmount());
			builder2.setProductPrice(value.getCostMoney());
			builder2.setAmount(value.getAmount());
			builder2.setAddAmount(value.getAddNum());
			builder.addItemList(builder2);
		}
		
		player.setLastVerifyTime(time);
		builder.setVersion(version);
		
		
		player.sendPacket(OP.code.OPCODE_GET_SHOP_ITEM_LISTRET_S_VALUE, builder);
		long current_time_2 = Util.getServerMillTime();
		long taken_time=current_time_2-current_time;
		log.info("get shop list handler ret send: {}, {}, {}", new Object[]{playerid ,taken_time});
		logger2.info("get shop list handler ret send: {}, {}, {}", new Object[]{playerid ,taken_time});
	}
}

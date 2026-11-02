package com.oldmountain.dzm.packethandler;

import java.net.HttpURLConnection;
import java.net.URL;
import java.net.URLConnection;
import java.util.List;

import org.apache.mina.common.IoSession;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.google.protobuf.InvalidProtocolBufferException;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.message.OP;
import com.oldmountain.dzm.message.ShopPurchase.OPShopPurchaseCheck;
import com.oldmountain.dzm.message.ShopPurchase.OPShopPurchaseCheckRet;
import com.oldmountain.dzm.net.ObjectAccessor;
import com.oldmountain.dzm.net.packet.Packet;
import com.oldmountain.dzm.util.Util;

public class ShopPurchaseCheckHandler {
	private static final Logger logger2 = LoggerFactory.getLogger("ShopInfo");
	public static void shopPurchaseCheck(Packet packet,IoSession session){
		OPShopPurchaseCheck params = null;
		OPShopPurchaseCheckRet.Builder builder = OPShopPurchaseCheckRet.newBuilder();
		
		try {
			params = OPShopPurchaseCheck.parseFrom(packet.data.array());
		} catch (InvalidProtocolBufferException e) {
			e.printStackTrace();
			return;
		}
		
		int version = params.getVersion();
		if (version != 1) {
			//TODO 版本控制
		}
		int playerId = (Integer)session.getAttribute("playerid");
		Player player = ObjectAccessor.getOnlineUser(playerId);
		int id=player.getId();
		logger2.info("shop purchase check handler received: {}, {}, {}", new Object[]{id , version});
		long current_time = Util.getServerMillTime();
		int time = Util.getServerTime();
		
		int count = params.getCooOrderSerialCount();
		if (count <= 0) {
			return;
		}
		
		List<String> serials = params.getCooOrderSerialList();
		
		for (int i = 0; i < count; i++) {
			String CooOrderSerial = serials.get(i);
			String urlString = "http://http://127.0.0.1/protocolbuf_php/recharge/QueryOrderResult.php?CooOrderSerial=" + CooOrderSerial;
			try {
				URL url = new URL(urlString);
				URLConnection URLconnection = url.openConnection();  
				HttpURLConnection httpConnection = (HttpURLConnection)URLconnection;
				int responseCode = httpConnection.getResponseCode();
				if (responseCode == HttpURLConnection.HTTP_OK) { 
					
				}
				
			} catch (Exception e) {
				e.printStackTrace();
			}
			builder.addCooOrderSerial(serials.get(i));
		}
		
		player.setLastVerifyTime(time);
		
		builder.setVersion(version);
		
		builder.setTotalMoney(player.getSysgoldcoins() + player.getRechargegoldcoins());
		
		player.sendPacket(OP.code.OPCODE_GET_SHOP_PURCHASE_RETURN_S_VALUE, builder);
		long current_time_2 = Util.getServerMillTime();
		long taken_time=current_time_2-current_time;
		logger2.info("shop purchase check handler ret send: {}, {}, {}", new Object[]{id ,taken_time});
	}
}

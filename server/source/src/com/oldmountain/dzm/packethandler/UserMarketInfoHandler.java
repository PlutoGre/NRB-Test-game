package com.oldmountain.dzm.packethandler;

import org.apache.mina.common.IoSession;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.message.OP;
import com.oldmountain.dzm.message.UserMarketInfo.OPUserMarketInfo;
import com.oldmountain.dzm.message.UserMarketInfo.OPUserMarketInfoRet;
import com.oldmountain.dzm.net.ObjectAccessor;
import com.oldmountain.dzm.net.packet.Packet;
import com.oldmountain.dzm.util.Util;

public class UserMarketInfoHandler {
	private static final Logger logger2 = LoggerFactory.getLogger("MarketInfo");
	public static void getUserMarketInfo(Packet packet,IoSession session){
		OPUserMarketInfo params = null;
		OPUserMarketInfoRet.Builder builder = OPUserMarketInfoRet.newBuilder();
		
		try {
			params = OPUserMarketInfo.parseFrom(packet.data.array());
		} catch (Exception e) {
			// TODO: handle exception
		}
		
		int version = params.getVersion();
		if (version != 1) {
			//TODO 版本控制
		}
		
		int playerId = (Integer)session.getAttribute("playerid");
		Player player = ObjectAccessor.getOnlineUser(playerId);
		logger2.info("user market info handler received: {}, {}, {}", new Object[]{playerId , version});
		long startTime = Util.getServerMillTime();
		int current_time = Util.getServerTime();
		builder.setTenleavetime(player.getTodayleavetimes());
        
		int time = player.getHundredtime() + 24 * 3600 - current_time;
		time = time < 0?0:time;
		builder.setHundredtime(time);
		time = player.getWanlitime() + 3 * 24 * 3600 - current_time;
		time = time < 0?0:time;
		builder.setWanlitime(time);
		time = player.getTentime() + 10 * 60 - current_time;
		time = time < 0?0:time;
		builder.setTentime(time);
		builder.setTencost(10);
		builder.setHundredcost(100);
		builder.setWanlicost(300);
		builder.setServertime(current_time);
		
		String buyGifts = player.getUserStats().getBuygifts();
		if( !buyGifts.equals("") && buyGifts != null){
			String[] gifts = buyGifts.split(",");
			for (int i = 0; i < gifts.length; i++) {
				builder.addGotGiftID(Integer.parseInt(gifts[i]));
			}
		}
		
		
		if (player.getRechargenum() == 0) {
			builder.setFirstPay(true);
		}
		else {
			builder.setFirstPay(false);
		}
		
		if (player.getUserStats().getFirstbaitime() == 0) {
			builder.setFirstBaili(true);
		}
		else {
			builder.setFirstBaili(false);
		}
		
		if (player.getUserStats().getFirstwantime() == 0) {
			builder.setFirstWanli(true);
		}
		else {
			builder.setFirstWanli(false);
		}
		int termId=Util.getGiveSoulTermId(startTime);
		builder.setGivesoulstatus(termId!=0);
		
		player.setLastVerifyTime(current_time);
		
		player.sendPacket(OP.code.OPCODE_GET_USER_MARKET_INFORET_S_VALUE, builder);
		long current_time_2 = Util.getServerMillTime();
		long taken_time=current_time_2-startTime;
		logger2.info("user market info handler ret send: {}, {}, {}", new Object[]{playerId ,taken_time});
	}
}

package com.oldmountain.dzm.packethandler;

import java.util.HashMap;

import com.oldmountain.dzm.entity.ToolsXmlTemplate;
import com.oldmountain.dzm.message.ErrorReporter;
import com.oldmountain.dzm.util.UserToolConst;
import com.oldmountain.dzm.util.XMLTemplateService;
import org.apache.mina.common.IoSession;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.google.protobuf.InvalidProtocolBufferException;
import com.oldmountain.dzm.entity.Bag;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.message.OP;
import com.oldmountain.dzm.message.AddVitality.OPAddVitality;
import com.oldmountain.dzm.message.AddVitality.OPAddVitalityRet;
import com.oldmountain.dzm.message.ErrorReporter.OPErrorReporter;
import com.oldmountain.dzm.net.ObjectAccessor;
import com.oldmountain.dzm.net.packet.Packet;
import com.oldmountain.dzm.util.Util;

public class AddVitalityHandler {
	private static final Logger log = LoggerFactory.getLogger(LoginHandler.class);
	private static final Logger logger2 = LoggerFactory.getLogger("UserBasicInfo");
	private static final Logger logger_goldCost = LoggerFactory.getLogger("GoldCost");
	public static void addVitality(Packet packet,IoSession session){
		OPAddVitality params = null;
		OPAddVitalityRet.Builder builder = OPAddVitalityRet.newBuilder();
		
		try {
			params = OPAddVitality.parseFrom(packet.data.array());
		} catch (InvalidProtocolBufferException e) {
			e.printStackTrace();
		}
		
		int typeid = params.getTypeid();
		int uuid = (Integer)session.getAttribute("playerid");
		Player player = ObjectAccessor.getOnlineUser(uuid);
		int id=player.getId();
		log.info("add vitality handler received: {}, {}, {}", new Object[]{id , typeid});
		logger2.info("add vitality handler received: {}, {}, {}", new Object[]{id , typeid});
		int current_time = Util.getServerTime();
		
		switch (typeid){
            case 3:
            	int dayLimit = UserToolConst.vipLvToolLimit.get(player.getViplevel());
            	if (player.getTodaypelletnum() >= dayLimit) {
                    // 判断是否今日已不能使用海鲜饭
                    OPErrorReporter.Builder eBuilder = OPErrorReporter.newBuilder();
                    eBuilder.setErrorid(OP.code.OPCODE_ADD_VITALITYRET_S_VALUE);
                    eBuilder.setErrormsg("CANT EAT ANY MORE");
                    player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, eBuilder);
                    logger2.info("Error Reporter send: use tool handler : {}, {}, {}", new Object[]{id});
                    return ;
                }
                ToolsXmlTemplate tm= XMLTemplateService.getToolTemplate(UserToolConst.JISUZHEN);
                int cost = tm.getPrice() * (player.getTodaybuypelletnum() + 1);
                if(player.getGoldCoins() >= cost)
                {
                	int userGold=player.getGoldCoins();
                    player.consumeGold(cost);
                    player.setVitalitybypellet(player.getVitalitybypellet() + 8);
                    player.setTodaypelletnum(player.getTodaypelletnum() + 1);
                    player.setTodaybuypelletnum(player.getTodaybuypelletnum() + 1);
                    player.setEatpellettime(current_time);
                    builder.setStatus(301);
                    builder.setServertime(current_time);
                    builder.setVitality(player.getVitalitybypellet() + player.getVitalitybytime());
                    logger_goldCost.info("Gold Cost Info: {} , {} , {} , {} , {}, {} , {}",new Object[]{ Util.getServerTime() , player.getId() , 1006 , 1 , userGold , player.getGoldCoins() , cost});
                }
                else
                {
                    // 元宝数量不够
                    ErrorReporter.OPErrorReporter.Builder eBuilder = ErrorReporter.OPErrorReporter.newBuilder();
                    eBuilder.setErrorid(OP.code.OPCODE_ADD_VITALITYRET_S_VALUE);
                    eBuilder.setErrormsg("GOLD NOT ENOUGH!");
                    player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, eBuilder);
                    logger2.info("Add ArenaNum Gold Not Enough {}, {}, {}", new Object[]{player.getId()});
                }
                break;

		}
		
		player.sendPacket(OP.code.OPCODE_ADD_VITALITYRET_S_VALUE, builder);
		int current_time_2 = Util.getServerTime();
		int taken_time=current_time_2-current_time;
		log.info("add vitality handler ret send: {}, {}, {}", new Object[]{id ,taken_time});
		logger2.info("add vitality handler ret send: {}, {}, {}", new Object[]{id ,taken_time});
	}
}

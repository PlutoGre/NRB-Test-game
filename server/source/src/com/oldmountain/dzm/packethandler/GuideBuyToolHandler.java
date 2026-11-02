package com.oldmountain.dzm.packethandler;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import org.apache.commons.lang.StringUtils;
import org.apache.mina.common.IoSession;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.google.protobuf.InvalidProtocolBufferException;
import com.oldmountain.dzm.DZM;
import com.oldmountain.dzm.entity.Bag;
import com.oldmountain.dzm.entity.Cdkey;
import com.oldmountain.dzm.entity.CdkeyXmlTemplate;
import com.oldmountain.dzm.entity.Equip;
import com.oldmountain.dzm.entity.GuideConfigerXmlTemplate;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.entity.Skill;
import com.oldmountain.dzm.entity.Soul;
import com.oldmountain.dzm.message.CdkeyReward;
import com.oldmountain.dzm.message.GuideBuyTool;
import com.oldmountain.dzm.message.OP;
import com.oldmountain.dzm.message.UserEquipInfo;
import com.oldmountain.dzm.message.CdkeyReward.OPCdkeyRewardRet;
import com.oldmountain.dzm.message.ContinueLogin.OPContinueLoginRet;
import com.oldmountain.dzm.message.ErrorReporter.OPErrorReporter;
import com.oldmountain.dzm.message.UserRewardInfo.OPRewardInfoRet;
import com.oldmountain.dzm.net.ObjectAccessor;
import com.oldmountain.dzm.net.packet.Packet;
import com.oldmountain.dzm.services.BagToolServices;
import com.oldmountain.dzm.services.EquipServices;
import com.oldmountain.dzm.util.BagUtil;
import com.oldmountain.dzm.util.EquipUtil;
import com.oldmountain.dzm.util.SkillUtil;
import com.oldmountain.dzm.util.SoulUtil;
import com.oldmountain.dzm.util.UserToolConst;
import com.oldmountain.dzm.util.Util;
import com.oldmountain.dzm.util.XMLTemplateService;

public class GuideBuyToolHandler {
	private static final Logger log = LoggerFactory.getLogger(GuideBuyToolHandler.class);
	private static final Logger logger_goldCost = LoggerFactory.getLogger("GoldCost");
	public static void buyTool(Packet packet, IoSession session) {	
		GuideBuyTool.OPGuideBuyTool params = null;
		GuideBuyTool.OPGuideBuyToolRet.Builder builder = GuideBuyTool.OPGuideBuyToolRet.newBuilder();
		OPErrorReporter.Builder errorBuilder = OPErrorReporter.newBuilder();
		try{
			params = GuideBuyTool.OPGuideBuyTool.parseFrom(packet.data.array());
		}catch (InvalidProtocolBufferException e) {
			e.printStackTrace();
			return;
		}
		int version = params.getVersion();
		int typeid = params.getTypeid();
		
		int uuid = (Integer)session.getAttribute("playerid");
        Player player = ObjectAccessor.getOnlineUser(uuid);
        int playerid=player.getId();
        int status=1;	//操作结果状态码
        int gold_before=player.getGoldCoins();
        log.info("Guide Buy Tool Handler received: {}, {}, {}", new Object[]{ playerid , version , typeid});
        long current_time = Util.getServerMillTime();
        Map<String,Integer> firstGain = Util.jsonToMap(player.getFirstgain());
        GuideConfigerXmlTemplate gt=XMLTemplateService.getGuideConfigerXmlTemplate(typeid);
        int price=gt.getPrice();
        int count=gt.getCount();
        String type=gt.getType();
        if(firstGain.containsKey(type) && firstGain.get(type)==2){
        	errorBuilder.setErrorid(OP.code.OPCODE_GUIDE_BUY_TOOLRET_S_VALUE);
			errorBuilder.setErrormsg("have already get it");
			player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errorBuilder);
			log.info("Error Reporter send: guide buy tool handler: {}, {}, {}", new Object[]{player.getId()});
			return;
        }
        if(player.getGoldCoins()<price){
        	status=2;
        }
        else
        {
        	switch (typeid) {
        	case 1:
        		//装备
        		int itemId = gt.getItemId();
        		firstGain.put(type, 2);
        		player.consumeGold(price);
        		UserEquipInfo.OPUserEquipInfoRet.EquipInfo.Builder equip_builder=EquipServices.createEquitAlone(player, itemId);
        		builder.addEquipInfo(equip_builder);
        		//时间，用户ID，类型ID，个数，消费之前多少钱，消费之后多少钱，消费多少钱
        		logger_goldCost.info("Gold Cost Info: {} , {} , {} , {} , {}, {} , {}",new Object[]{ Util.getServerTime() , player.getId() , 1011 , count , gold_before , player.getGoldCoins() , price});
        		break;
        	case 2:
        		//经验
        		int exp=gt.getCount();
        		firstGain.put(type, 2);
        		player.consumeGold(price);
        		Util.getUserLevelByExp(player, exp);
       			builder.setExp(exp);
       			logger_goldCost.info("Gold Cost Info: {} , {} , {} , {} , {}, {} , {}",new Object[]{ Util.getServerTime() , player.getId() , 1010 , count , gold_before , player.getGoldCoins() , price});
        		break;
        	default:
        		break;
        	}
        }
       	player.setFirstgain(Util.mapToString(firstGain));
        player.setLastVerifyTime(Util.getServerTime());
        builder.setGoldcoins(player.getGoldCoins());
        builder.setSilvercoins(player.getSilvercoins());
        builder.setServertime(Util.getServerTime());
        builder.setStatus(status);
        player.sendPacket(OP.code.OPCODE_GUIDE_BUY_TOOLRET_S_VALUE, builder);// 发送返回包
		long current_time_2 = Util.getServerMillTime();
		long taken_time=current_time_2-current_time;
		log.info("Guide Buy Tool Handler Ret send: {}, {}, {}", new Object[]{playerid , taken_time , status});
	}

}

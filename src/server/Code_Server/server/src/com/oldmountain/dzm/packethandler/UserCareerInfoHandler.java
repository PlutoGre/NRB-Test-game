package com.oldmountain.dzm.packethandler;

import java.util.Iterator;
import java.util.Map;

import org.apache.mina.common.IoSession;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.google.protobuf.InvalidProtocolBufferException;
import com.oldmountain.dzm.entity.Career;
import com.oldmountain.dzm.entity.CareerXmlTemplate;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.entity.UserStats;
import com.oldmountain.dzm.message.OP;
import com.oldmountain.dzm.message.ErrorReporter.OPErrorReporter;
import com.oldmountain.dzm.message.ResetCareerCount.OPResetCareerCount;
import com.oldmountain.dzm.message.ResetCareerCount.OPResetCareerCountRet;
import com.oldmountain.dzm.message.UserCareerInfo.OPUserCareerInfo;
import com.oldmountain.dzm.message.UserCareerInfo.OPUserCareerInfoRet;
import com.oldmountain.dzm.net.ObjectAccessor;
import com.oldmountain.dzm.net.packet.Packet;
import com.oldmountain.dzm.util.Util;
import com.oldmountain.dzm.util.XMLTemplateService;

public class UserCareerInfoHandler {
	private static final Logger logger2 = LoggerFactory.getLogger("UserBasicInfo");
	private static final Logger logger_goldCost = LoggerFactory.getLogger("GoldCost");
	public static void getUserCareerInfo(Packet packet,IoSession session){
		OPUserCareerInfo params = null;
		OPUserCareerInfoRet.Builder builder = OPUserCareerInfoRet.newBuilder();
		
		try {
			params = OPUserCareerInfo.parseFrom(packet.data.array());
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
		logger2.info("user career info handler received: {}, {}, {}", new Object[]{playerId , version});
		long current_time = Util.getServerMillTime();
		int time = Util.getServerTime();
		Career userCareer = player.getCareer();
		
		Map<String,Integer> chapteraddpower = Util.jsonToMap(userCareer.getChapteraddpower());
		Map<String,Integer> pointevaluate = Util.jsonToMap(userCareer.getPointevaluate());
		Map<String,Integer> pointcount = Util.jsonToMap(userCareer.getPointcount());
		
		Iterator<Map.Entry<String,Integer>> iterator = chapteraddpower.entrySet().iterator();
		OPUserCareerInfoRet.Status.Builder builder2 = OPUserCareerInfoRet.Status.newBuilder();
		
		while (iterator.hasNext()) {
			Map.Entry<String, Integer> entry = iterator.next();
			int idx = Integer.parseInt(entry.getKey());
			int value = entry.getValue();
			
			builder2.setId(idx);
			builder2.setValue(value);
			
			builder.addChapteraddpower(builder2);
		}
		
		iterator = pointevaluate.entrySet().iterator();
		
		while (iterator.hasNext()) {
			Map.Entry<String, Integer> entry = iterator.next();
			int idx = Integer.parseInt(entry.getKey());
			int value = entry.getValue();
			
			builder2.setId(idx);
			builder2.setValue(value);
			
			builder.addPointevaluate(builder2);
		}
		
		iterator = pointcount.entrySet().iterator();
		
		while (iterator.hasNext()) {
			Map.Entry<String, Integer> entry = iterator.next();
			int idx = Integer.parseInt(entry.getKey());
			int value = entry.getValue();
			
			builder2.setId(idx);
			builder2.setValue(value);
			
			builder.addPointcount(builder2);
		}
		
		player.setLastVerifyTime(time);
		builder.setPlayerid(userCareer.getPlayerid());
		builder.setCurrentchapter(userCareer.getCurrentchapter());
		builder.setCurrentpoint(userCareer.getCurrentpoint());
		builder.setChecktime(userCareer.getChecktime());
		UserStats userStats=player.getUserStats();
		//返回玩家连闯功能倒计时的信息
		int nextQuickCareerTime=userStats.getNextQuickCareerTime();
		int backwardsTimes=0;
		if(nextQuickCareerTime==0){
			userStats.setNextQuickCareerTime(time);
			player.setUserStats(userStats);
		}
		else{
			backwardsTimes=nextQuickCareerTime-time;
			if(backwardsTimes<0){
				backwardsTimes=0;
			}
		}
		builder.setBackwardsTime(backwardsTimes);
		
		player.sendPacket(OP.code.OPCODE_GET_USER_CAREER_INFORET_S_VALUE, builder);
		long current_time_2 = Util.getServerMillTime();
		long taken_time=current_time_2-current_time;
		logger2.info("user career info handler ret send: {}, {}, {}", new Object[]{playerId ,taken_time});
	}
	
	public static void resetCount(Packet packet,IoSession session){
		OPResetCareerCount params = null;
		OPResetCareerCountRet.Builder builder = OPResetCareerCountRet.newBuilder();
		OPErrorReporter.Builder errorBuilder = OPErrorReporter.newBuilder();
		try {
			params = OPResetCareerCount.parseFrom(packet.data.array());
		} catch (InvalidProtocolBufferException e) {
			e.printStackTrace();
			return;
		}
		int version = params.getVersion();
		if (version != 1) {
			
		}
		
		int id = params.getId();
		int status=1;
		String idsString = String.valueOf(id);
		int playerId = (Integer)session.getAttribute("playerid");
		Player player = ObjectAccessor.getOnlineUser(playerId);
		logger2.info("reset career count handler received: {}, {}, {}", new Object[]{playerId , version , id});
		long current_time = Util.getServerMillTime();
		Career career = player.getCareer();
		
		String counts = career.getPointcount();
		
		CareerXmlTemplate careerInfo = XMLTemplateService.getCareerTemplate(id);
		int costId=1003;
		int userGold=player.getGoldCoins();
		int consumeGold = 5;
		if (careerInfo.getBoosType() == 2) {
			consumeGold = 10;
			costId=1004;
		}
		if(userGold<consumeGold){
            status=2;
		}
		else{
			Map<String, Integer> countsMap = Util.jsonToMap(counts);
			if (countsMap.containsKey(idsString)) {
				countsMap.remove(idsString);
			}
			career.setPointcount(Util.mapToString(countsMap));
			player.setCareer(career);
			player.consumeGold(consumeGold);
		}
		
		player.setLastVerifyTime(Util.getServerTime());
		
		
		builder.setStatus(status);
		builder.setGold(player.getGoldCoins());
		player.sendPacket(OP.code.OPCODE_RESET_CAREER_COUNTRET_S_VALUE, builder);
		long current_time_2 = Util.getServerMillTime();
		long taken_time=current_time_2-current_time;
		logger2.info("reset career count handler ret send: {}, {}, {}", new Object[]{playerId ,taken_time});
		logger_goldCost.info("Gold Cost Info: {} , {} , {} , {} , {}, {} , {}",new Object[]{ Util.getServerTime() , player.getId() , costId , 1 , userGold , player.getGoldCoins() , consumeGold});
	}
}

package com.oldmountain.dzm.packethandler;

import org.apache.mina.common.IoSession;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.google.protobuf.InvalidProtocolBufferException;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.entity.Title;
import com.oldmountain.dzm.message.OP;
import com.oldmountain.dzm.message.TitleInfo.OPTitleInfo;
import com.oldmountain.dzm.message.TitleInfo.OPTitleInfoRet;
import com.oldmountain.dzm.net.ObjectAccessor;
import com.oldmountain.dzm.net.packet.Packet;
import com.oldmountain.dzm.util.Util;

public class TitleInfoHandler {
	private static final Logger logger2 = LoggerFactory.getLogger("TitleInfo");
	public static void getTitleInfo(Packet packet,IoSession session){
		OPTitleInfo params = null;
		OPTitleInfoRet.Builder builder = OPTitleInfoRet.newBuilder();
		OPTitleInfoRet.TitleItem.Builder titleBuilder = OPTitleInfoRet.TitleItem.newBuilder();
		
		try {
			params = OPTitleInfo.parseFrom(packet.data.array());
		} catch (InvalidProtocolBufferException e) {
			e.printStackTrace();
		}
		
		int version = params.getVersion();
		if (version != 1) {
			//TODO 版本控制
		}
		
		int playerId = (Integer)session.getAttribute("playerid");
		Player player = ObjectAccessor.getOnlineUser(playerId);
		int id=player.getId();
		logger2.info("title info handler received: {}, {}, {}", new Object[]{id , version});
		long current_time = Util.getServerMillTime();
		Title userTitle = player.getTitle();
		
		if (userTitle.getCareertypeid() != 0) {
			titleBuilder.setTitleID(userTitle.getCareertypeid());
			builder.addTitles(titleBuilder);
		}
		if (userTitle.getViptypeid() != 0) {
			titleBuilder = OPTitleInfoRet.TitleItem.newBuilder();
			titleBuilder.setTitleID(userTitle.getViptypeid());
			builder.addTitles(titleBuilder);
		}
		if (userTitle.getChallengetypeid() != 0) {
			titleBuilder = OPTitleInfoRet.TitleItem.newBuilder();
			titleBuilder.setTitleID(userTitle.getChallengetypeid());
			builder.addTitles(titleBuilder);
		}
		if (userTitle.getChiptypeid() != 0) {
			titleBuilder = OPTitleInfoRet.TitleItem.newBuilder();
			titleBuilder.setTitleID(userTitle.getChiptypeid());
			builder.addTitles(titleBuilder);
		}
		if (userTitle.getFighttypeid() != 0) {
			titleBuilder = OPTitleInfoRet.TitleItem.newBuilder();
			titleBuilder.setTitleID(userTitle.getFighttypeid());
			builder.addTitles(titleBuilder);
		}
		if (userTitle.getBattletypeid() != 0) {
			titleBuilder = OPTitleInfoRet.TitleItem.newBuilder();
			titleBuilder.setTitleID(userTitle.getBattletypeid());
			builder.addTitles(titleBuilder);
		}
		if (userTitle.getFriendtypeid() != 0) {
			titleBuilder = OPTitleInfoRet.TitleItem.newBuilder();
			titleBuilder.setTitleID(userTitle.getFriendtypeid());
			builder.addTitles(titleBuilder);
		}
		if (userTitle.getFightrankid() != 0) {
			titleBuilder = OPTitleInfoRet.TitleItem.newBuilder();
			titleBuilder.setTitleID(userTitle.getFightrankid());
			titleBuilder.setGetTime(userTitle.getFightranktime());
			builder.addTitles(titleBuilder);
		}
		if (userTitle.getBattlerankid() != 0) {
			titleBuilder = OPTitleInfoRet.TitleItem.newBuilder();
			titleBuilder.setTitleID(userTitle.getBattlerankid());
			titleBuilder.setGetTime(userTitle.getBattleranktime());
			builder.addTitles(titleBuilder);
		}
		
		player.setLastVerifyTime(Util.getServerTime());
		
		builder.setServertime(Util.getServerTime());
		player.sendPacket(OP.code.OPCODE_GET_TITLE_INFORET_S_VALUE, builder);
		long current_time_2 = Util.getServerMillTime();
		long taken_time=current_time_2-current_time;
		logger2.info("title info handler ret send: {}, {}, {}", new Object[]{id ,taken_time});
	}
}

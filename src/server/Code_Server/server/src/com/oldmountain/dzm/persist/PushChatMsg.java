package com.oldmountain.dzm.persist;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;

import org.apache.mina.common.IoSession;
import com.oldmountain.dzm.entity.ChatEntity;
import com.oldmountain.dzm.message.OP;
import com.oldmountain.dzm.message.ChatPush.OPChatPushRet;
import com.oldmountain.dzm.net.packet.Packet;
import com.oldmountain.dzm.packethandler.ChatSendMsgInfoHandler;
import com.oldmountain.dzm.util.Util;

public class PushChatMsg implements Runnable {
	public static HashMap<Integer, OPChatPushRet.Builder> builds = null;
	
	public void run() {
		try {
			while (true) {
				pushMsg();
				Thread.sleep(2000);
			}
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
	
	public static void pushMsg(){
		HashMap<Integer, HashMap<Integer,IoSession>> inPageUser = ChatSendMsgInfoHandler.getInPageUser();
		Object[] keyArrs = inPageUser.keySet().toArray();
		buildBuilderForGuild(keyArrs);
		Iterator<Map.Entry<Integer, HashMap<Integer,IoSession>>> userArr = inPageUser.entrySet().iterator();
		while (userArr.hasNext()) {
			Map.Entry<Integer, HashMap<Integer,IoSession>> guildUser = userArr.next();
			int guidId = guildUser.getKey();
			OPChatPushRet.Builder builder = builds.get(guidId);
			if (builder.getMsgsCount() > 0) {
				Packet packet = new Packet(OP.code.OPCODE_CHAT_PUSHRET_S_VALUE, builds.get(guidId).build().toByteArray());
				HashMap<Integer,IoSession> sessions = guildUser.getValue();
				Iterator<Map.Entry<Integer, IoSession>> sessionsArr = sessions.entrySet().iterator();
				IoSession session = null;
				while (sessionsArr.hasNext()) {
					Map.Entry<Integer, IoSession> sessionEntry = sessionsArr.next();
					session = sessionEntry.getValue();
					session.write(packet);	
				}
			}
		}
	}
	
	public static void buildBuilderForGuild(Object[] guilds){
		builds = new HashMap<Integer, OPChatPushRet.Builder>();
		if (guilds.length != 0) {
			OPChatPushRet.Builder baseBuilder = OPChatPushRet.newBuilder();
			baseBuilder.setServertime(Util.getServerTime());
			ArrayList<ChatEntity> worldMsg = ChatSendMsgInfoHandler.popWorldChatMsg();
			for (int i = 0; i < worldMsg.size(); i++) {
				ChatEntity chat = worldMsg.get(i);
				OPChatPushRet.MsgInfo.Builder msgBuilder = OPChatPushRet.MsgInfo.newBuilder();
				msgBuilder.setPlayerid(chat.getPlayerId());
				msgBuilder.setViplevel(chat.getVipLevel());
				msgBuilder.setName(chat.getName());
				msgBuilder.setMsg(chat.getMsg());
				msgBuilder.setSendTime(chat.getSendTime());
				msgBuilder.setType(0);
				msgBuilder.setId(chat.getId());
				baseBuilder.addMsgs(msgBuilder);
			}
			HashMap<Integer, ArrayList<ChatEntity>> guildMsg = ChatSendMsgInfoHandler.popGuildChatMsg();
			for (int i = 0; i < guilds.length; i++) {
				int guildId = (Integer)guilds[i];
				if (!guildMsg.containsKey(guildId)) {
					builds.put(guildId, baseBuilder);
					continue;
				}
				ArrayList<ChatEntity> guildChat = guildMsg.get(guildId);
				OPChatPushRet.Builder newBuilder = baseBuilder.clone();
				for (int j = 0; j < guildChat.size(); j++) {
					ChatEntity chat = guildChat.get(i);
					OPChatPushRet.MsgInfo.Builder msgBuilder = OPChatPushRet.MsgInfo.newBuilder();
					msgBuilder.setPlayerid(chat.getPlayerId());
					msgBuilder.setViplevel(chat.getVipLevel());
					msgBuilder.setName(chat.getName());
					msgBuilder.setMsg(chat.getMsg());
					msgBuilder.setSendTime(chat.getSendTime());
					msgBuilder.setType(1);
					msgBuilder.setId(chat.getId());
					newBuilder.addMsgs(msgBuilder);
				}
				builds.put(guildId, newBuilder);
			}
		}
	}
}
package com.oldmountain.dzm.packethandler;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.Map.Entry;

import org.apache.mina.common.IoSession;
import com.google.protobuf.InvalidProtocolBufferException;
import com.oldmountain.dzm.entity.Bag;
import com.oldmountain.dzm.entity.ChatEntity;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.message.OP;
import com.oldmountain.dzm.message.ChatPush.OPChatPushRet;
import com.oldmountain.dzm.message.ChatSendMsgInfo.OPChatSendMsgInfo;
import com.oldmountain.dzm.message.ChatSendMsgInfo.OPChatSendMsgInfoRet;
import com.oldmountain.dzm.message.ChatSendMsgInfo.OPEnterChatPageRet;
import com.oldmountain.dzm.message.ChatSendMsgInfo.OPQuitChatPageRet;
import com.oldmountain.dzm.message.ErrorReporter.OPErrorReporter;
import com.oldmountain.dzm.message.UserToolInfo.OPUserToolInfoRet;
import com.oldmountain.dzm.net.ObjectAccessor;
import com.oldmountain.dzm.net.packet.Packet;
import com.oldmountain.dzm.util.Util;

/**
 * 
 */
public class ChatSendMsgInfoHandler
{
	private static HashMap<Integer, HashMap<Integer,IoSession>> inPageSessionMap = new HashMap<Integer, HashMap<Integer,IoSession>>();
	private static ArrayList<ChatEntity> worldChatInfo = new ArrayList<ChatEntity>();
	private static HashMap<Integer, ArrayList<ChatEntity>> guildChatInfo = new HashMap<Integer, ArrayList<ChatEntity>>();
	private static HashMap<Integer, ChatEntity> chatEntityPool =new HashMap<Integer, ChatEntity>();
	private static int history_index=0;
	
 	public static void enterPage(Packet packet,IoSession session){
 		int playerId = (Integer)session.getAttribute("playerid");
 		Player player = ObjectAccessor.getOnlineUser(playerId);
 		int guildId = player.getLeaguaid();
 		if (!inPageSessionMap.containsKey(guildId)) {
			HashMap<Integer,IoSession> guildSessions = new HashMap<Integer, IoSession>();
			guildSessions.put(playerId, session);
			inPageSessionMap.put(guildId, guildSessions);
		}
 		else {
			inPageSessionMap.get(guildId).put(playerId, session);
		}
 		/*******************刚进入玩家添加信息展示**************************/
 		OPChatPushRet.Builder baseBuilder = OPChatPushRet.newBuilder();
 		baseBuilder.setServertime(Util.getServerTime());
 		Iterator<Entry<Integer, ChatEntity>> entry_iterator=chatEntityPool.entrySet().iterator();
 		while(entry_iterator.hasNext()){
 			Entry<Integer, ChatEntity> entry=entry_iterator.next();
 			ChatEntity chat = entry.getValue();
 			OPChatPushRet.MsgInfo.Builder msgBuilder = OPChatPushRet.MsgInfo.newBuilder();
 			msgBuilder.setPlayerid(chat.getPlayerId());
 			msgBuilder.setViplevel(chat.getVipLevel());
 			msgBuilder.setName(chat.getName());
 			msgBuilder.setMsg(chat.getMsg());
 			msgBuilder.setSendTime(chat.getSendTime());
 			msgBuilder.setType(chat.getType());
 			msgBuilder.setId(chat.getId());
 			baseBuilder.addMsgs(msgBuilder);
 			Packet msg_packet = new Packet(OP.code.OPCODE_CHAT_PUSHRET_S_VALUE, baseBuilder.build().toByteArray());
 			session.write(msg_packet);
 		}
 		OPEnterChatPageRet.Builder builder = OPEnterChatPageRet.newBuilder();
 		builder.setStatus(1);
 		player.sendPacket(OP.code.OPCODE_ENTER_CHAT_PAGERET_S_VALUE, builder);
 	}
 	
 	public static void leavePage(Packet packet,IoSession session){
 		int playerId = (Integer)session.getAttribute("playerid");
 		Player player = ObjectAccessor.getOnlineUser(playerId);
 		int guildId = player.getLeaguaid();
 		if (inPageSessionMap.containsKey(guildId) && inPageSessionMap.get(guildId).containsKey(playerId)) {
			inPageSessionMap.get(guildId).remove(playerId);
		}
 		OPQuitChatPageRet.Builder builder = OPQuitChatPageRet.newBuilder();
 		builder.setStatus(1);
 		player.sendPacket(OP.code.OPCODE_QUIT_CHAT_PAGERET_S_VALUE, builder);
 	}
 	
	public static void sendChatInfo(Packet packet, IoSession session)
    {
        OPChatSendMsgInfo params = null;
        OPChatSendMsgInfoRet.Builder builder = OPChatSendMsgInfoRet.newBuilder();
        OPErrorReporter.Builder errBuilder = OPErrorReporter.newBuilder();
		try
        {
			params = OPChatSendMsgInfo.parseFrom(packet.data.array());
		}
        catch (InvalidProtocolBufferException e)
        {
			e.printStackTrace();
			return;
		}
        
        builder.setServertime(Util.getServerTime());
		
		String msg=params.getMsg();
		int type = params.getType();//type=0是世界说话；type=1是公会喊话
        int playerId = (Integer)session.getAttribute("playerid");
        Player player = ObjectAccessor.getOnlineUser(playerId);
        ChatEntity chat = new ChatEntity();
        chat.setPlayerId(playerId);
        chat.setName(player.getName());
        chat.setVipLevel(player.getViplevel());
        chat.setMsg(msg);
        chat.setSendTime(Util.getServerTime());
        chat.setId(history_index);
        if (type == 0) {
        	if (!player.getBags().containsKey(1101002)) {
				errBuilder.setErrorid(OP.code.OPCODE_CHAT_SEND_MSGRET_S_VALUE);
				errBuilder.setErrormsg("tool not enough");
				player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errBuilder);
				return;
			}
			Bag tools = player.getBags().get(1101002);
			if (tools.getCount() < 1) {
				errBuilder.setErrorid(0);
				errBuilder.setErrormsg("tool not enough");
				player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errBuilder);
				return;
			}
			chat.setType(0);
			tools.setCount(tools.getCount() - 1);
			player.updateOneBag(tools);
			getWorldChatInfo().add(chat);
			
			OPUserToolInfoRet.ToolInfo.Builder toolBuilder = OPUserToolInfoRet.ToolInfo.newBuilder();
			toolBuilder.setId(tools.getId());
			toolBuilder.setItemid(tools.getItemid());
			toolBuilder.setCount(tools.getCount());
			builder.setTool(toolBuilder);
		}
        else if (type == 1) {
			int guildId = player.getLeaguaid();
			if (guildId == 0) {
				errBuilder.setErrorid(OP.code.OPCODE_CHAT_SEND_MSGRET_S_VALUE);
				errBuilder.setErrormsg("no guild found");
				player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errBuilder);
				return;
			}
			
			if (!getGuildChatInfo().containsKey(guildId)) {
				ArrayList<ChatEntity> chatInfo = new ArrayList<ChatEntity>();
				getGuildChatInfo().put(guildId, chatInfo);
			}
			chat.setType(1);
			getGuildChatInfo().get(guildId).add(chat);
		}
        /****************将最近的30条信息放入容器中*******************/
        if(chatEntityPool.size()>=30){
        	Iterator<Integer> chatPool_set=chatEntityPool.keySet().iterator();
        	int min_index=-1;
        	while(chatPool_set.hasNext()){
        		int index=chatPool_set.next();
        		if(min_index==-1){
        			min_index=index;
        		}
        		else{
        			min_index=(min_index<index)?min_index:index;
        		}
        	}
        	if(min_index!=-1){
        		chatEntityPool.remove(min_index);
        	}
        	history_index++;
        	if(history_index>=10000000){
        		history_index=0;
        	}
        	chatEntityPool.put(history_index, chat);
        }
        else{
        	history_index++;
        	if(history_index>=10000000){
        		history_index=0;
        	}
        	chatEntityPool.put(history_index, chat);
        }
        
        /***********************************/
        
        player.sendPacket(OP.code.OPCODE_CHAT_SEND_MSGRET_S_VALUE, builder);
	}
	
	public static synchronized ArrayList<ChatEntity> getWorldChatInfo(){
		return worldChatInfo;
	}
	
	public static synchronized HashMap<Integer, ArrayList<ChatEntity>> getGuildChatInfo(){
		return guildChatInfo;
	}
	
	public static HashMap<Integer, HashMap<Integer,IoSession>> getInPageUser(){
		return inPageSessionMap;
	}
	
	public static ArrayList<ChatEntity> popWorldChatMsg(){
		ArrayList<ChatEntity> chatInfo = getWorldChatInfo();
		worldChatInfo = new ArrayList<ChatEntity>();
		return chatInfo;
	}
	
	public static HashMap<Integer, ArrayList<ChatEntity>> popGuildChatMsg(){
		HashMap<Integer, ArrayList<ChatEntity>> chatInfo = getGuildChatInfo();
		guildChatInfo = new HashMap<Integer, ArrayList<ChatEntity>>();
		return chatInfo;
	}
}

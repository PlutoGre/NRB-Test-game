package com.oldmountain.dzm.packethandler;

import java.util.List;

import org.apache.mina.common.ByteBuffer;
import org.apache.mina.common.IoSession;

import com.google.protobuf.InvalidProtocolBufferException;
import com.oldmountain.dzm.DZM;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.message.BackStageFetchUserInfo.OPBackStageFetchUserInfo;
import com.oldmountain.dzm.message.BackStageFetchUserInfo.OPBackStageFetchUserInfoRet;
import com.oldmountain.dzm.message.OP;
import com.oldmountain.dzm.net.ObjectAccessor;
import com.oldmountain.dzm.net.packet.Packet;
import com.oldmountain.dzm.util.Util;

public class BackStageFetchUserInfoHandler {
	public static void doFetchUserInfo(Packet packet,IoSession session){
		OPBackStageFetchUserInfo params = null;
		OPBackStageFetchUserInfoRet.Builder builder = OPBackStageFetchUserInfoRet.newBuilder();
		
		for(int i=0;i<packet.data.array().length;++i){
			packet.data.array()[i]=(byte) (packet.data.array()[i] ^ 0xA5);
		}
		
		
		try {
			params = OPBackStageFetchUserInfo.parseFrom(packet.data.array());
		} catch (InvalidProtocolBufferException e) {
			e.printStackTrace();
			return;
		}
		
		int version = params.getVersion();
		String name = params.getName();
		
		Player player;
		List<Object> listbyName = DZM.getEntityManager().limitQuery("from Player where name = ?", 0, 1, name);
		if (listbyName.size() == 1) {
			player = (Player)listbyName.get(0);
		}
		else {
			player = new Player();
		}
		int userid = player.getId();

		
		
		try{
			player = ObjectAccessor.getOnlineUser(userid);
			//如果该用户已经加载，取内存中的，否则，取数据库中的
			if(player != null){
			}else{
				List<Object> list = DZM.getEntityManager().limitQuery("from Player where id = ?", 0, 1, userid);
				if (list.size() == 1) {
					player = (Player)list.get(0);
				}
				else {
					player = new Player();
				}
			}
			
			builder.setServertime(Util.getServerTime());
			builder.setUserid(player.getId());
			builder.setName(player.getName());
			builder.setRegistertime(player.getRegistertime());
			builder.setLastlogin(player.getLastlogin());
			builder.setLevel(player.getLevel());
			builder.setSilvercoins(player.getSilvercoins());
			builder.setGoldcoins(player.getGoldCoins());
			
			
			
			System.out.print(player.getId());
			System.out.print(player.getName());
			System.out.print(player.getRegistertime());
			System.out.print(player.getLastlogin());
			System.out.print(player.getLevel());
			System.out.print(player.getSilvercoins());
			System.out.print(player.getSysgoldcoins()+player.getRechargegoldcoins());

		}catch (Exception e){
			e.printStackTrace();
		}
		
		if (version != 1) {
			return;
		}
		

		
		
		Packet pt = new Packet(OP.code.OPCODE_HEARTBEATRET_S_VALUE, builder.build().toByteArray());
		
		ByteBuffer datasrc = pt.data;
		int size = datasrc.limit();
		ByteBuffer data = ByteBuffer.allocate(size);
		
		datasrc.rewind();
		
		for(int i=0;i<size;++i)
		{
			byte info = (byte) (datasrc.get() ^ 0xA5);
			data.put(info);
		}
		
		data.rewind();
		pt.data = data;
		
		session.write(pt);
		return;
	}
}

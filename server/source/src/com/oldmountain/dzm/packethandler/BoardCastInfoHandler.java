package com.oldmountain.dzm.packethandler;

import java.util.ArrayList;

import org.apache.mina.common.IoSession;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.google.protobuf.InvalidProtocolBufferException;
import com.oldmountain.dzm.entity.BroadCast;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.message.OP;
import com.oldmountain.dzm.message.BoardCastInfo.OPBoardCastInfo;
import com.oldmountain.dzm.message.BoardCastInfo.OPBoardCastInfoRet;
import com.oldmountain.dzm.net.ObjectAccessor;
import com.oldmountain.dzm.net.packet.Packet;
import com.oldmountain.dzm.util.Util;

/**
 * 广播信息包
 * 
 */
public class BoardCastInfoHandler {
	private static final Logger log = LoggerFactory.getLogger(BoardCastInfoHandler.class);
	
	public static final int DEALCHIPS_type = 1;
	public static final int OPENCASE_type = 2;
	public static final int CANBAI_TYPE = 3;
	public static final int RECRUITDISCIPLE_TYPE = 4;
	public static final int UPGRADESKILL_TYPE = 5;
	public static final int REFINEEQUIP_TYPE = 6;

	public static void getBoardCast(Packet packet, IoSession session)
    {
        OPBoardCastInfo opBoardCastInfo = null;
        OPBoardCastInfoRet.Builder builder = OPBoardCastInfoRet.newBuilder();
		try
        {
			opBoardCastInfo = OPBoardCastInfo.parseFrom(packet.data.array());
		}
        catch (InvalidProtocolBufferException e)
        {
			e.printStackTrace();
			return;
		}
		
		int version = opBoardCastInfo.getVersion();//version default = 1;

        int playerId = (Integer)session.getAttribute("playerid");
        Player player = ObjectAccessor.getOnlineUser(playerId);
        log.info("board cast info handler received: {}, {}, {}", new Object[]{playerId , version});
        long current_time = Util.getServerMillTime();
        
        ArrayList<BroadCast> broadCasts = ObjectAccessor.getSystemBro();
        int size = broadCasts.size();
        int count = 0;
        for(int i=size-1;i >=0 ;i--)
        {
        	if (count >= 30) {
				break;
			}
            OPBoardCastInfoRet.BoardCast.Builder sysMsgInfo=OPBoardCastInfoRet.BoardCast.newBuilder();
            BroadCast broadCast = broadCasts.get(i);
            sysMsgInfo.setType(broadCast.getType());
            sysMsgInfo.setName(broadCast.getName());
            ArrayList<Integer> params = broadCast.getParams();
            for (int j = 0; j < params.size(); j++) {
				sysMsgInfo.addParams(params.get(j));
			}
            builder.addBoardCastInfo(sysMsgInfo);
            count ++;
        }
        builder.setServertime(1000);
        player.setLastVerifyTime(Util.getServerTime());
        
		player.sendPacket(OP.code.OPCODE_GET_BOARDCAST_INFORET_S_VALUE, builder);// 发送返回包
		long current_time_2 = Util.getServerMillTime();
		long taken_time=current_time_2-current_time;
		log.info("board cast info handler ret send: {}, {}, {}", new Object[]{playerId ,taken_time});
	}
	
	public static void addBoardCase(int type,String name,ArrayList<Integer> params){
		BroadCast broadCast = new BroadCast();
		broadCast.setType(type);
		broadCast.setName(name);
		broadCast.setParams(params);
		ObjectAccessor.addSystemBro(broadCast);
	}

}

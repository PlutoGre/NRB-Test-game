package com.oldmountain.dzm.packethandler;

import com.google.protobuf.InvalidProtocolBufferException;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.message.ChangeName.*;
import com.oldmountain.dzm.message.ErrorReporter.OPErrorReporter;
import com.oldmountain.dzm.message.OP;
import com.oldmountain.dzm.net.ObjectAccessor;
import com.oldmountain.dzm.net.packet.Packet;
import com.oldmountain.dzm.services.BagToolServices;
import com.oldmountain.dzm.util.Util;
import org.apache.mina.common.IoSession;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

/**
 * 留言包
 * 1， 解析数据包
 * 2， 保存留言数据
 * 3， 推送给相关玩家数据信息
 * 4， 下发留言成功
 * 
 *
 * @author lvjc
 */
public class ChangeNameHandler
{
	private static final Logger logger2 = LoggerFactory.getLogger("ChatInfo");
	private static final int TOOLID = 1101001;
	public static void changeName(Packet packet, IoSession session)
    {
        OPChangeName opChangeName = null;
		try
        {
			opChangeName = OPChangeName.parseFrom(packet.data.array());
		}
        catch (InvalidProtocolBufferException e)
        {
			e.printStackTrace();
			return;
		}
		
		String name=opChangeName.getName();
		int version = opChangeName.getVersion();//version default = 1;
        int uuid = (Integer)session.getAttribute("playerid");
        Player player = ObjectAccessor.getOnlineUser(uuid);
        
        if (name.equals("") || name == null || name.length() > 7) {
        	OPErrorReporter.Builder errBuilder = OPErrorReporter.newBuilder();
        	errBuilder.setErrorid(OP.code.OPCODE_CHANE_NAMERET_S_VALUE);
        	errBuilder.setErrormsg("name error");
			player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE,errBuilder);
			return;
		}
        
        int id=player.getId();
        logger2.info("ChangeNameHandler changeName handler received: {}, {}, {}", new Object[]{id , version , name});
        long current_time = Util.getServerMillTime();
        int time = Util.getServerTime();
        OPChangeNameRet.Builder builder = OPChangeNameRet.newBuilder();
        int status=0;
        if(player.getBags().containsKey(TOOLID))
        {
            if(player.getBags().get(TOOLID).getCount()<=0)
            {
                status=1;
            }
            else
            {
                if(!player.resetName(name))
                {
                    status=2;
                }
                else
                {
                    Util.decreaseBagTool(player,TOOLID,1);
                    builder.addToolInfo(BagToolServices.buildToolBuilder(player.getBags().get(TOOLID))) ;
                }
            }
        }
        else
        {
            status=1;
        }
        
        player.setLastVerifyTime(time);
        
        long current_time_2 = Util.getServerMillTime();
        builder.setServertime(time);
        builder.setStatus(status);

		player.sendPacket(OP.code.OPCODE_CHANE_NAMERET_S_VALUE, builder);// 发送返回包
		long taken_time=current_time_2-current_time;
		logger2.info("ChangeNameHandler changeName handler ret send: {}, {}, {}", new Object[]{id ,taken_time});
	}

}

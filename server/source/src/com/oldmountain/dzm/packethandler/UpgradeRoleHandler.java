package com.oldmountain.dzm.packethandler;

import org.apache.mina.common.IoSession;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.google.protobuf.InvalidProtocolBufferException;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.message.OP;
import com.oldmountain.dzm.message.UpgradeRole.OPUpgradeRole;
import com.oldmountain.dzm.message.UpgradeRole.OPUpgradeRoleRet;
import com.oldmountain.dzm.net.ObjectAccessor;
import com.oldmountain.dzm.net.packet.Packet;
import com.oldmountain.dzm.util.Util;

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
public class UpgradeRoleHandler
{
	private static final Logger log = LoggerFactory.getLogger(UpgradeRoleHandler.class);
	private static final Logger logger2 = LoggerFactory.getLogger("UserBasicInfo");
	public static void upgradeRole(Packet packet, IoSession session)
    {
        OPUpgradeRole opDisposeFriendsInfo = null;
		try
        {
			opDisposeFriendsInfo = OPUpgradeRole.parseFrom(packet.data.array());
		}
        catch (InvalidProtocolBufferException e)
        {
			e.printStackTrace();
			return;
		}

		int version = opDisposeFriendsInfo.getVersion();//version default = 1;
        int uuid = (Integer)session.getAttribute("playerid");
        Player player = ObjectAccessor.getOnlineUser(uuid);
        int id=player.getId();
        log.info("upgrade role handler received: {}, {}, {}", new Object[]{id , version});
        logger2.info("upgrade role handler received: {}, {}, {}", new Object[]{id , version});
        int current_time = Util.getServerTime();
        OPUpgradeRoleRet.Builder builder = OPUpgradeRoleRet.newBuilder();
		builder.setStatus(1);
        builder.setServertime((int) System.currentTimeMillis());
        builder.setLevel(1);
		player.sendPacket(OP.code.OPCODE_UPGRADE_ROLERET_S_VALUE, builder);// 发送返回包
		int current_time_2 = Util.getServerTime();
		int taken_time=current_time_2-current_time;
		log.info("upgrade role handler ret send: {}, {}, {}", new Object[]{id ,taken_time});
		logger2.info("upgrade role handler ret send: {}, {}, {}", new Object[]{id ,taken_time});
	}

}

package com.oldmountain.dzm.packethandler;

import java.util.ArrayList;
import java.util.Map;

import com.google.protobuf.InvalidProtocolBufferException;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.message.ChangeSetting.*;
import com.oldmountain.dzm.message.ErrorReporter.OPErrorReporter;
import com.oldmountain.dzm.message.OP;
import com.oldmountain.dzm.net.ObjectAccessor;
import com.oldmountain.dzm.net.packet.Packet;
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
 * TODO 重复登陆的处理， 下发玩家数据信息.
 * 
 * @author lvjc
 */
public class ChangeSettingHandler
{
	private static final Logger logger2 = LoggerFactory.getLogger("UserInfo");
	public static void changeSetting(Packet packet, IoSession session)
    {
        OPChangeSetting params = null;
		try
        {
			params = OPChangeSetting.parseFrom(packet.data.array());
		}
        catch (InvalidProtocolBufferException e)
        {
			e.printStackTrace();
			return;
		}
        int ret = params.getOprRet();
        int version = params.getVersion();//version default = 1;
        String type = params.getType();
        int uuid = (Integer)session.getAttribute("playerid");
        Player player = ObjectAccessor.getOnlineUser(uuid);
        int id=player.getId();
        logger2.info("change setting handler received: {}, {}, {}", new Object[]{id , version , type , ret});
        long current_time = Util.getServerMillTime();
        ArrayList<String> typeArray = new ArrayList<String>();
        typeArray.add("music");
        typeArray.add("sound");
        typeArray.add("chat");
        
        if (!typeArray.contains(type)) {
			OPErrorReporter.Builder errorBuilder = OPErrorReporter.newBuilder();
			errorBuilder.setErrorid(OP.code.OPCODE_CHANGE_SETTINGRET_S_VALUE);
			errorBuilder.setErrormsg("type error");
			player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errorBuilder);
			logger2.info("Error Reporter send: change setting handler: {}, {}, {}", new Object[]{id});
			return;
		}
        
        Map<String, Integer> setting = Util.jsonToMap(player.getSetting());
        
        setting.put(type, ret);
        
        String settingString = Util.mapToString(setting);
        
        player.setSetting(settingString);
        player.setLastVerifyTime(Util.getServerTime());
        
        OPChangeSettingRet.Builder builder = OPChangeSettingRet.newBuilder();
		builder.setStatus(1);
        builder.setServertime(Util.getServerTime());

		player.sendPacket(OP.code.OPCODE_CHANGE_SETTINGRET_S_VALUE, builder);// 发送返回包
		long current_time_2 = Util.getServerMillTime();
		long taken_time=current_time_2-current_time;
		logger2.info("change setting handler ret send: {}, {}, {}", new Object[]{id ,taken_time});
	}

}

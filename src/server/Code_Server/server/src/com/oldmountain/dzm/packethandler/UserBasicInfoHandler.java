package com.oldmountain.dzm.packethandler;

import java.util.Map;

import com.oldmountain.dzm.services.ActivateVersionService;
import com.oldmountain.dzm.services.SysMsgInfoServices;
import net.sf.json.JSONObject;
import org.apache.mina.common.IoSession;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.google.protobuf.InvalidProtocolBufferException;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.message.OP;
import com.oldmountain.dzm.message.UserBasicInfo.OPUserBasicInfo;
import com.oldmountain.dzm.message.UserBasicInfo.OPUserBasicInfoRet;
import com.oldmountain.dzm.net.ObjectAccessor;
import com.oldmountain.dzm.net.packet.Packet;
import com.oldmountain.dzm.util.Util;

/**
 * 获取用户基础信息包
 * 
 * @author dragon
 *
 */

public class UserBasicInfoHandler {
	private static final Logger logger2 = LoggerFactory.getLogger("UserBasicInfo");
	public static void getUserBasicInfo(Packet packet, IoSession session){
		OPUserBasicInfo params = null;
		OPUserBasicInfoRet.Builder builder = OPUserBasicInfoRet.newBuilder();
		
		try {
			params = OPUserBasicInfo.parseFrom(packet.data.array());
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

		logger2.info("user basic info handler received: {}, {}, {}", new Object[]{playerId , version});
		
		Util.calcPowerAndVality(player);
		
		long current_time = Util.getServerMillTime();
		builder.setLevel(player.getLevel());
		builder.setExp(player.getExp());
		builder.setPower(player.getPowerbytime() + player.getPowerbychicken());
		builder.setPoweraddtime(player.getPoweraddtime());
		builder.setMaxpower(player.getMaxpower());
		builder.setVitality(player.getVitalitybypellet() + player.getVitalitybytime());
		builder.setVitalityaddtime(player.getVitalityaddtime());
		builder.setMaxvitality(player.getMaxvitality());
		builder.setViplevel(player.getViplevel());
		builder.setRechargenum(player.getRechargenum());
		builder.setSilvercoins(player.getSilvercoins());
		builder.setGoldcoins(player.getSysgoldcoins() + player.getRechargegoldcoins());
		builder.setTutorialstep(player.getTutorialstep());
		builder.setName(player.getName());
		String setting = player.getSetting();
		Map<String, Integer> settingMap = Util.jsonToMap(setting);
		builder.setMusic(settingMap.get("music"));
		builder.setSound(settingMap.get("sound"));
		builder.setChat(settingMap.get("chat"));
        builder.setServertime(Util.getServerTime());
        builder.setHasGotContinueLogin(player.getUserStats().getGetreward());
        builder.setTitleTotalNum(player.getMomentum());
        builder.setTodayAddedPowerNum(player.getTodaychickennum());
        builder.setTodayBuyPowerNum(player.getTodaybuychickennum());
        builder.setTodayAddedVitalityNum(player.getTodaypelletnum());
        builder.setTodayBuyVitalityNum(player.getTodaybuypelletnum());
        builder.setDropStatus(player.getFirstgain());
        builder.setUnreadMsgCount(SysMsgInfoServices.getUnreadMsgCount(player));
        //活动版本检测
        JSONObject _obj=ActivateVersionService.getActivateVersion();
//        JSONObject _obj=new JSONObject();
//        _obj.put("m",1);
//        _obj.put("t",1);
//        _obj.put("ad",1);
        builder.setActStatus(_obj.toString());
		player.sendPacket(OP.code.OPCODE_GET_USER_BASIC_INFORET_S_VALUE, builder);

		long current_time_2 = Util.getServerMillTime();
		player.setLastVerifyTime(Util.getServerTime());
		long taken_time=current_time_2-current_time;
		logger2.info("user basic info handler ret send: {}, {}, {}", new Object[]{playerId ,taken_time});
		
	}
}

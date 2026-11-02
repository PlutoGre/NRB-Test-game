package com.oldmountain.dzm.packethandler;

import java.util.Calendar;
import java.util.HashMap;
import java.util.Map;

import com.oldmountain.dzm.entity.ToolsXmlTemplate;
import com.oldmountain.dzm.message.AddPowerForCareer.*;
import com.oldmountain.dzm.message.ErrorReporter;
import com.oldmountain.dzm.util.UserToolConst;
import com.oldmountain.dzm.util.XMLTemplateService;
import org.apache.mina.common.IoSession;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.google.protobuf.InvalidProtocolBufferException;
import com.oldmountain.dzm.entity.Bag;
import com.oldmountain.dzm.entity.Career;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.message.OP;
import com.oldmountain.dzm.message.AddPower.OPAddPower;
import com.oldmountain.dzm.message.AddPower.OPAddPowerRet;
import com.oldmountain.dzm.message.ErrorReporter.OPErrorReporter;
import com.oldmountain.dzm.message.GetAdventurePowerInfo.OPGetAdventurePower;
import com.oldmountain.dzm.message.GetAdventurePowerInfo.OPGetAdventurePowerRet;
import com.oldmountain.dzm.net.ObjectAccessor;
import com.oldmountain.dzm.net.packet.Packet;
import com.oldmountain.dzm.util.Util;

/**
 * 用户增加体力包，type=1为时间段免费增加体力，type=2 为吃叫花鸡增加体力
 * 
 * @author dragon
 *
 */
public class AddPowerHandler {
	private static final Logger logger2 = LoggerFactory.getLogger("UserBasicInfo");
	private static final Logger logger_goldCost = LoggerFactory.getLogger("GoldCost");
	public static void getAdventurePowerStats(Packet packet,IoSession session){
		OPGetAdventurePower params = null;
		OPGetAdventurePowerRet.Builder builder = OPGetAdventurePowerRet.newBuilder();
		
		try {
			params = OPGetAdventurePower.parseFrom(packet.data.array());
		} catch (InvalidProtocolBufferException e) {
			e.printStackTrace();
			return;
		}
		
		int version = params.getVersion();
		
		if (version != 1) {
			
		}
		
		int playerId = (Integer)session.getAttribute("playerid");
		Player player = ObjectAccessor.getOnlineUser(playerId);
		int id=player.getId();
		logger2.info("add power handler received: getAdventurePower: {}, {}, {}", new Object[]{id , version});
		long current_time = Util.getServerMillTime();
		Calendar calendar = Calendar.getInstance();
		
		int hour = calendar.get(Calendar.HOUR_OF_DAY);
		if (hour < 12) {
			builder.setAmStatus(0);
			builder.setPmStatus(0);
		}
		else if(hour < 18) {
			if (player.getUserStats().getAmeatfood() == 1) {
				builder.setAmStatus(2);
			}
			else {
				builder.setAmStatus(0);
			}
			builder.setPmStatus(0);
		}
		else {
			if (player.getUserStats().getAmeatfood() == 1) {
				builder.setAmStatus(2);
			}
			else {
				builder.setAmStatus(0);
			}
			if (player.getUserStats().getPmeatfood() == 1) {
				builder.setPmStatus(2);
			}
			else {
				builder.setPmStatus(0);
			}
		}
		
		builder.setServertime(Util.getServerTime());
		builder.setStatus(1);
		
		player.setLastVerifyTime(Util.getServerTime());
		
		player.sendPacket(OP.code.OPCODE_GET_ADVENTURE_POWERINFORET_S_VALUE, builder);
		long current_time_2 = Util.getServerMillTime();
		long taken_time=current_time_2-current_time;
		logger2.info("add power handler ret send: getAdventurePower: {}, {}, {}", new Object[]{id ,taken_time});
	}
	
	public static void addPower(Packet packet, IoSession session){
		OPAddPower params = null;
		OPAddPowerRet.Builder builder = OPAddPowerRet.newBuilder();
		try {
			params = OPAddPower.parseFrom(packet.data.array());
		} catch (InvalidProtocolBufferException e) {
			e.printStackTrace();
			return;
		}
		
		//补充体力的类型，1为时间段更新，2为吃叫花鸡补充
		int type = params.getTypeid();
		int uuid = (Integer)session.getAttribute("playerid");
		Player player = ObjectAccessor.getOnlineUser(uuid);
		int id=player.getId();
		logger2.info("add power handler received: {}, {}, {}", new Object[]{id , type});
		long current_time = Util.getServerMillTime();
		int time = Util.getServerTime();
		switch (type) {
			case 1://按时间吃叫花鸡，只能在12-13点以及18-19点之间使用
				Calendar calendar = Calendar.getInstance();
				int current_hour = calendar.get(Calendar.HOUR_OF_DAY);
				if (current_hour != 12 && current_hour != 18) {//不是那两个时间段的话
					builder.setStatus(2);
					builder.setServertime(time);
					builder.setPower(player.getPowerbytime() + player.getPowerbychicken());
				}
				else {
					//如果已经吃过美食
					if (current_hour == 12 && player.getUserStats().getAmeatfood() != 1) {
						player.setPowerbytime(player.getPowerbytime() + 10);
						player.getUserStats().setAmeatfood(1);
						builder.setStatus(0);
					}
					else if (current_hour == 18 && player.getUserStats().getPmeatfood() != 1) {
						player.setPowerbytime(player.getPowerbytime() + 10);
						player.getUserStats().setPmeatfood(1);
						builder.setStatus(0);
					}
					else {
						builder.setStatus(2);
					}
					builder.setServertime(time);
					builder.setPower(player.getPowerbytime() + player.getPowerbychicken());
				}
				break;
            case 3:
            	int dayLimit = UserToolConst.vipLvToolLimit.get(player.getViplevel());
            	if (player.getTodaychickennum() >= dayLimit) {
                    // 判断是否今日已不能使用海鲜饭
                    OPErrorReporter.Builder eBuilder = OPErrorReporter.newBuilder();
                    eBuilder.setErrorid(OP.code.OPCODE_ADDPOWER_CAREERRET_S_VALUE);
                    eBuilder.setErrormsg("CANT EAT ANY MORE");
                    player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, eBuilder);
                    logger2.info("Error Reporter send: use tool handler : {}, {}, {}", new Object[]{id});
                    return ;
                }
                ToolsXmlTemplate tm= XMLTemplateService.getToolTemplate(UserToolConst.HAIXIANFAN);
                int cost = tm.getPrice() * (player.getTodaybuychickennum() + 1);
                if(player.getGoldCoins() >= cost)
                {
                	int userGold=player.getGoldCoins();
                    player.consumeGold(cost);
                    player.setPowerbychicken(player.getPowerbychicken() + 8);
                    player.setTodaychickennum(player.getTodaychickennum() + 1);
                    player.setTodaybuychickennum(player.getTodaybuychickennum() + 1);
                    player.setEatchickentime(time);
                    builder.setStatus(301);
                    builder.setServertime(time);
                    builder.setPower(player.getPowerbychicken() + player.getPowerbytime());
                    logger_goldCost.info("Gold Cost Info: {} , {} , {} , {} , {}, {} , {}",new Object[]{ Util.getServerTime() , player.getId() , 1005 , 1 , userGold , player.getGoldCoins() , cost});
                }
                else
                {
                    // 元宝数量不够
                    ErrorReporter.OPErrorReporter.Builder eBuilder = ErrorReporter.OPErrorReporter.newBuilder();
                    eBuilder.setErrorid(OP.code.OPCODE_ADD_POWERRET_S_VALUE);
                    eBuilder.setErrormsg("GOLD NOT ENOUGH!");
                    player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, eBuilder);
                    logger2.info("Add ArenaNum Gold Not Enough {}, {}, {}", new Object[]{player.getId()});
                }

                break;
		}
		player.sendPacket(OP.code.OPCODE_ADD_POWERRET_S_VALUE, builder);// 发送包内容
		long current_time_2 = Util.getServerMillTime();
		player.setLastVerifyTime(time);
		long taken_time=current_time_2-current_time;
		logger2.info("add power handler ret send: {}, {}, {}", new Object[]{id ,taken_time});
	}

    /**
     * 江湖吃鸡
     * @param packet
     * @param session
     */
    public static void addPowerCareer(Packet packet, IoSession session)
    {
        OPAddPowerCareer params = null;
        OPAddPowerCareerRet.Builder builder = OPAddPowerCareerRet.newBuilder();
        try
        {
            params = OPAddPowerCareer.parseFrom(packet.data.array());
        }
        catch (InvalidProtocolBufferException e)
        {
            e.printStackTrace();
            return;
        }

        int version=params.getVersion();
        //江湖id
        int chapterId = params.getChapterId();
        int uuid = (Integer)session.getAttribute("playerid");
        Player player = ObjectAccessor.getOnlineUser(uuid);
        int id=player.getId();
        logger2.info("addPowerCareer handler received: {}, {}, {}", new Object[]{id , version,chapterId});
        long current_time = Util.getServerMillTime();
        int time = Util.getServerTime();

        Career career = player.getCareer();
        int currentChapter = career.getCurrentchapter();
        if (chapterId >= currentChapter) {
			OPErrorReporter.Builder errBuilder = OPErrorReporter.newBuilder();
			errBuilder.setErrorid(OP.code.OPCODE_ADDPOWER_CAREERRET_S_VALUE);
			errBuilder.setErrormsg("cannot eat chicken for this chapter");
			player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errBuilder);
			return;
		}
        
        String chapterPower = career.getChapteraddpower();
        Map<String,Integer> statusMap = Util.jsonToMap(chapterPower);
        if (statusMap.containsKey(String.valueOf(chapterId))) {
        	OPErrorReporter.Builder errBuilder = OPErrorReporter.newBuilder();
			errBuilder.setErrorid(OP.code.OPCODE_ADDPOWER_CAREERRET_S_VALUE);
			errBuilder.setErrormsg("already eat!");
			player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errBuilder);
			return;
		}
        statusMap.put(String.valueOf(chapterId), 1);
        
        career.setChapteraddpower(Util.mapToString(statusMap));
        player.setCareer(career);

        player.setPowerbychicken(player.getPowerbychicken()+10);
        
        player.setLastVerifyTime(time);
        
        builder.setStatus(0);
        builder.setServertime(time);
        builder.setPower(player.getPowerbychicken() + player.getPowerbytime());
        player.sendPacket(OP.code.OPCODE_ADDPOWER_CAREERRET_S_VALUE, builder);// 发送包内容
        long current_time_2 = Util.getServerMillTime();
        long taken_time=current_time_2-current_time;
        logger2.info("addPowerCareer handler ret send: {}, {}, {}", new Object[]{id ,taken_time});
    }
}

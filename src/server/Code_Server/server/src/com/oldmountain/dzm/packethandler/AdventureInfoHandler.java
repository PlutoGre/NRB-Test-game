package com.oldmountain.dzm.packethandler;

import com.oldmountain.dzm.entity.SkillBook;
import org.apache.mina.common.IoSession;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.google.protobuf.InvalidProtocolBufferException;
import com.oldmountain.dzm.entity.ActivityTimeConfig;
import com.oldmountain.dzm.entity.Adventures;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.message.OP;
import com.oldmountain.dzm.message.AdventureInfo.OPAdventureInfo;
import com.oldmountain.dzm.message.AdventureInfo.OPAdventureInfoRet;
import com.oldmountain.dzm.message.ChargeRebate.OPChargeRebateRet;
import com.oldmountain.dzm.net.ObjectAccessor;
import com.oldmountain.dzm.net.packet.Packet;
import com.oldmountain.dzm.services.ActivityServices;
import com.oldmountain.dzm.util.Util;
import com.oldmountain.dzm.util.XMLTemplateService;

import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.Iterator;
import java.util.List;
import java.util.ListIterator;
import java.util.Map;

/**
 * 获取奇遇所有能显示的活动
 * 
 * @author dragon
 *
 */
public class AdventureInfoHandler {
	private static final Logger log = LoggerFactory.getLogger(AdventureInfoHandler.class);

	public static void getAdventureInfo(Packet packet, IoSession session)
    {
        final int ALWAYS_EXIST = 1;
        final int BY_REG_TIME_EXIST = 2;
        final int USER_DEFINE_EXIST = 3;
        final int CONDITIONAL_EXIST = 4;
        
        OPAdventureInfo opAdventureInfo = null;
		try
        {
			opAdventureInfo = OPAdventureInfo.parseFrom(packet.data.array());
		}
        catch (InvalidProtocolBufferException e)
        {
			e.printStackTrace();
			return;
		}
		
		int version = opAdventureInfo.getVersion();//version default = 1;

        int uuid = (Integer)session.getAttribute("playerid");
        Player player = ObjectAccessor.getOnlineUser(uuid);
        String puid=player.getPuid();
        log.info("adventure info handler received: {}, {}, {}", new Object[]{ version});
        long current_time = Util.getServerMillTime();
        int time = Util.getServerTime();
        OPAdventureInfoRet.Builder builder = OPAdventureInfoRet.newBuilder();
        SimpleDateFormat df = new SimpleDateFormat("yyyy-MM-dd_HH:mm:ss");
        List<Adventures> adventures =  XMLTemplateService.getAdventureTemplates();
        int index = 0;
        for ( ListIterator<Adventures> it = adventures.listIterator(); it.hasNext(); )
        {

            Adventures adventure =  it.next();
            if(adventure.getTimeType()==CONDITIONAL_EXIST)
            {
                switch (adventure.getId())
                {
                	//赠送vip
                    case 9:
                        if(player.getUserStats().getLevelupreward().split(",").length!= XMLTemplateService.getLevelUpTemplateSize())
                        {
                            OPAdventureInfoRet.AdventureItem.Builder adventureItem=OPAdventureInfoRet.AdventureItem.newBuilder();
                            adventureItem.setAdventureId(adventure.getId());
                            adventureItem.setId(index);
                            adventureItem.setLevelLimit(adventure.getLevelLimit());
                            adventureItem.setBeginTime(time-10000);
                            adventureItem.setEndTime(time + 9999999);
                            adventureItem.setVersion(1);
                            builder.addAdventureItem(adventureItem);
                            index++;
                        }
                        break;
                    //残章    
                    case 8:
                        for (Map.Entry<Integer, SkillBook> integerSkillBookEntry : player.getSkillBooks().entrySet())
                        {
                            Map.Entry entry = (Map.Entry) integerSkillBookEntry;
                            SkillBook _item = (SkillBook) entry.getValue();
                            OPAdventureInfoRet.AdventureItem.Builder adventureItem = OPAdventureInfoRet.AdventureItem.newBuilder();
                            adventureItem.setAdventureId(adventure.getId());
                            adventureItem.setId(index);
                            adventureItem.setLevelLimit(adventure.getLevelLimit());
                            adventureItem.setBeginTime(time-10000);
                            adventureItem.setEndTime(time + 9999999);
                            adventureItem.setItemId(_item.getSkillId());
                            adventureItem.setItemCount(_item.getPieceChance());
                            adventureItem.setVersion(1);
                            builder.addAdventureItem(adventureItem);
                            index++;
                        }
                        break;
                    //点拨
                    case 4:
                        for(int k=0;k<10;k++)
                        {
                            OPAdventureInfoRet.AdventureItem.Builder adventureItem=OPAdventureInfoRet.AdventureItem.newBuilder();
                            adventureItem.setAdventureId(adventure.getId());
                            adventureItem.setId(index);
                            adventureItem.setLevelLimit(adventure.getLevelLimit());
                            adventureItem.setBeginTime(time-10000);
                            adventureItem.setEndTime(time + 9999999);
                            adventureItem.setItemId(k);
                            builder.addAdventureItem(adventureItem);
                            adventureItem.setVersion(1);
                            index++;
                        }
                        break;
                    //充值返利活动
                    case 12:
                    	int chargeDurationTimes=Util.inChargeDuration(player, time);
                    	int rebateDurationTimes=Util.inRebateDuration(player, time);
                    	int termId=0;
                    	if(chargeDurationTimes!=0 || rebateDurationTimes!=0){
                    		if(chargeDurationTimes!=0){
                    			termId=chargeDurationTimes;
                    		}
                    		else if(rebateDurationTimes!=0){
                    			termId=rebateDurationTimes;
                    		}
                    		int[] beginEndTime=Util.getChargeRebateBeginEndTime(player, termId);
                    		OPAdventureInfoRet.AdventureItem.Builder adventureItem=OPAdventureInfoRet.AdventureItem.newBuilder();
                    		adventureItem.setAdventureId(adventure.getId());
                    		adventureItem.setId(index);
                    		adventureItem.setLevelLimit(adventure.getLevelLimit());
                    		adventureItem.setBeginTime(beginEndTime[0]);
                    		adventureItem.setEndTime(beginEndTime[1]);
                    		builder.addAdventureItem(adventureItem);
                            adventureItem.setVersion(termId);
                    		index++;
                    	}
                    	break;
                    //财神到活动
                    case 13:
                    	int instructionFinishTime=player.getInstructionFinishTime();
                    	if(instructionFinishTime==0){
                    		instructionFinishTime=Util.getServerTime();
                    		player.setInstructionFinishTime(instructionFinishTime);
                    	}
                    	int totalTimes=XMLTemplateService.getFortuneComeXmlTemplatesSize();
                    	int fortuneComeTimes=player.getUserStats().getFortuneComeTimes();
                    	if(fortuneComeTimes < totalTimes && time>=instructionFinishTime && time<(instructionFinishTime+48*60*60)){
                    		OPAdventureInfoRet.AdventureItem.Builder adventureItem=OPAdventureInfoRet.AdventureItem.newBuilder();
                    		adventureItem.setAdventureId(adventure.getId());
                    		adventureItem.setId(index);
                    		adventureItem.setLevelLimit(adventure.getLevelLimit());
                    		adventureItem.setBeginTime(instructionFinishTime);
                    		adventureItem.setEndTime(instructionFinishTime+48*60*60);
                    		builder.addAdventureItem(adventureItem);
                            adventureItem.setVersion(1);
                    		index++;
                    	}
                    	break;
                    //深海探宝(嫏嬛福地)
                    case 14:
                    	int termid=Util.getActivityTermId(current_time, XMLTemplateService.getDeepWaterTimeConfigs());
                    	if(termid!=0){
                    		ActivityTimeConfig activityTimeConfig=XMLTemplateService.getDeepWaterTimeConfig(termid);
                    		long endTime = 0;
                            long startTime = 0;
                            try
                            {
                            	startTime =  df.parse(activityTimeConfig.getBeginTime()).getTime();
                                endTime = df.parse(activityTimeConfig.getEndTime()).getTime();
                            }catch (ParseException e)
                            {
                                e.printStackTrace();
                            }
                    		OPAdventureInfoRet.AdventureItem.Builder adventureItem=OPAdventureInfoRet.AdventureItem.newBuilder();
                    		adventureItem.setAdventureId(adventure.getId());
                    		adventureItem.setId(index);
                    		adventureItem.setLevelLimit(adventure.getLevelLimit());
                    		adventureItem.setBeginTime((int)(startTime/1000));
                    		adventureItem.setEndTime((int)(endTime/1000));
                            adventureItem.setVersion(termid);
                    		builder.addAdventureItem(adventureItem);
                    		index++;
                    	}
                    	break;
                    //女神回馈(七夕活动)
                    case 15:
                    	int termid15=ActivityServices.getTermIdById(ActivityServices.GODDESS_REBATE, current_time);
                    	if(termid15!=0){
                    		int[]times=ActivityServices.getActivityTimeByTermid(ActivityServices.GODDESS_REBATE, termid15);
                    		OPAdventureInfoRet.AdventureItem.Builder adventureItem=OPAdventureInfoRet.AdventureItem.newBuilder();
                    		adventureItem.setAdventureId(adventure.getId());
                    		adventureItem.setId(index);
                    		adventureItem.setLevelLimit(adventure.getLevelLimit());
                    		adventureItem.setBeginTime(times[0]);
                    		adventureItem.setEndTime(times[1]);
                            adventureItem.setVersion(termid15);
                    		builder.addAdventureItem(adventureItem);
                    		index++;
                    	}
                    	break;
                    //邀请奖励
                    case 11:
                    	OPAdventureInfoRet.AdventureItem.Builder adventureItem=OPAdventureInfoRet.AdventureItem.newBuilder();
                        adventureItem.setAdventureId(adventure.getId());
                        adventureItem.setId(index);
                        adventureItem.setLevelLimit(adventure.getLevelLimit());
                        adventureItem.setBeginTime(time-10000);
                        adventureItem.setEndTime(time + 9999999);
                        adventureItem.setVersion(1);
                        builder.addAdventureItem(adventureItem);
                        index++;
                    	break;
                    default:
                        break;
                }
            }
            else
            {

                switch (adventure.getTimeType())
                {
                    case ALWAYS_EXIST:
                        {
                            OPAdventureInfoRet.AdventureItem.Builder adventureItem=OPAdventureInfoRet.AdventureItem.newBuilder();
                            adventureItem.setAdventureId(adventure.getId());
                            adventureItem.setId(index);
                            adventureItem.setLevelLimit(adventure.getLevelLimit());
                            adventureItem.setBeginTime(time-10000);
                            adventureItem.setEndTime(time + 9999999);
                            adventureItem.setItemId(adventure.getId());
                            adventureItem.setVersion(1);
                            builder.addAdventureItem(adventureItem);
                            index++;
                        }
                        break;
                    case BY_REG_TIME_EXIST:
                        int beginTime = player.getRegistertime()+Integer.parseInt(adventure.getBeginTime());
                        if(beginTime+adventure.getLastTime()>current_time)
                        {
                            OPAdventureInfoRet.AdventureItem.Builder adventureItem=OPAdventureInfoRet.AdventureItem.newBuilder();
                            adventureItem.setAdventureId(adventure.getId());
                            adventureItem.setId(index);
                            adventureItem.setLevelLimit(adventure.getLevelLimit());
                            adventureItem.setBeginTime(beginTime);
                            adventureItem.setEndTime(beginTime+adventure.getLastTime());
                            adventureItem.setItemId(adventure.getId());
                            adventureItem.setVersion(1);
                            builder.addAdventureItem(adventureItem);
                            index++;
                        }
                        break;
                    case USER_DEFINE_EXIST:
                        long endTime = 0;
                        long startTime = 0;
                        try
                        {
                            endTime =  df.parse(adventure.getEndTime()).getTime();
                            startTime = df.parse(adventure.getBeginTime()).getTime();
                        }catch (ParseException e)
                        {
                            e.printStackTrace();
                        }
                        if(endTime>current_time)
                        {
                            if(adventure.getId()==10)
                            {
                                if(AdventureContinueLoginHandler.checkShow(player))
                                {
                                    break;
                                }
                            }
                            OPAdventureInfoRet.AdventureItem.Builder adventureItem=OPAdventureInfoRet.AdventureItem.newBuilder();
                            adventureItem.setAdventureId(adventure.getId());
                            adventureItem.setId(index);
                            adventureItem.setLevelLimit(adventure.getLevelLimit());
                            adventureItem.setBeginTime((int)(startTime/1000));
                            adventureItem.setEndTime((int)(endTime/1000));
                            adventureItem.setItemId(adventure.getId());
                            adventureItem.setVersion(1);
                            builder.addAdventureItem(adventureItem);
                            index++;
                        }
                        break;
                    default:
                        break;
                }
            }

        }
        player.setLastVerifyTime(time);
        builder.setServertime(time);
		player.sendPacket(OP.code.OPCODE_GET_ADVENTURE_INFORET_S_VALUE, builder);// 发送返回包
		long current_time_2 = Util.getServerMillTime();
		long taken_time=current_time_2-current_time;
		log.info("adventure info handler ret send: {}, {}, {}", new Object[]{puid, taken_time});
	}

}

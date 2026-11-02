package com.oldmountain.dzm.packethandler;

import com.google.protobuf.InvalidProtocolBufferException;
import com.oldmountain.dzm.entity.*;
import com.oldmountain.dzm.message.AddBoundlessHole.*;
import com.oldmountain.dzm.message.ErrorReporter;
import com.oldmountain.dzm.message.OP;
import com.oldmountain.dzm.net.ObjectAccessor;
import com.oldmountain.dzm.net.packet.Packet;
import com.oldmountain.dzm.services.EquipServices;
import com.oldmountain.dzm.services.SkillServices;
import com.oldmountain.dzm.util.UserToolConst;
import com.oldmountain.dzm.util.Util;
import com.oldmountain.dzm.util.XMLTemplateService;
import org.apache.mina.common.IoSession;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.util.ArrayList;
import java.util.Calendar;
import java.util.List;

/**
 *
 * 
 * @author lvjc
 */
public class AddBoundlessHoleHandler
{
	private static final Logger log = LoggerFactory.getLogger(AddBoundlessHoleHandler.class);
	private static final Logger logger2 = LoggerFactory.getLogger("AdventureInfo");
	public static void addBoundlessHole(Packet packet, IoSession session)
    {
        OPAddBoundlessHole opAddBoundlessHole = null;
		try
        {
            opAddBoundlessHole = OPAddBoundlessHole.parseFrom(packet.data.array());
		}
        catch (InvalidProtocolBufferException e)
        {
			e.printStackTrace();
			return;
		}
		
		int version = opAddBoundlessHole.getVersion();//version default = 1;
        int uuid = (Integer)session.getAttribute("playerid");
        Player player = ObjectAccessor.getOnlineUser(uuid);
        int id=player.getId();
        log.info("add boundless hole handler received: {}, {}, {}", new Object[]{id , version});
        logger2.info("add boundless hole handler received: {}, {}, {}", new Object[]{id , version});
        long current_time = Util.getServerMillTime();
        int time = Util.getServerTime();
        UserStats userStats = player.getUserStats();

        OPAddBoundlessHoleRet.Builder builder = OPAddBoundlessHoleRet.newBuilder();
        if(userStats.getHasVisited()==1)
        {
           ErrorReporter.OPErrorReporter.Builder eBuilder = ErrorReporter.OPErrorReporter.newBuilder();
            eBuilder.setErrorid(OP.code.OPCODE_ADD_ADVENTURE_BOUNDLESSHOLEINFORET_S_VALUE);
            eBuilder.setErrormsg("Already Kissed Godnees!");
            player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, eBuilder);
            log.info("Error Reporter send: add boundless hole handler: {}, {}, {}", new Object[]{id});
            logger2.info("Error Reporter send: add boundless hole handler: {}, {}, {}", new Object[]{id});
            return;
        }
        else
        {
            userStats.setHasVisited(1);
            VisitRewardSelecter visitRewardSelecter = XMLTemplateService.getVisitRewardSelecter(userStats.getVisitRewardCount());
            int continueDay = visitRewardSelecter.getContinueDay();
            boolean hasMidStop = false;
            if (!Util.isNextDay(time, userStats.getLastVisitTime()) && userStats.getVisitTimes()>0) {
            	hasMidStop = true;
			}

            if(!hasMidStop)
            {
                builder.setTodaystatus(1);
                if(continueDay<=userStats.getVisitTimes()+1)
                {
                    randomReward(builder,visitRewardSelecter,player);
                    int newRewardCount = userStats.getVisitRewardCount()+1;
                    userStats.setVisitRewardCount(newRewardCount);
                    userStats.setVisitTimes(0);
                    builder.setCurrcount(0);
                    int newContinueDay = XMLTemplateService.getVisitRewardSelecter(newRewardCount).getContinueDay();
                    builder.setAccumulatecount(newContinueDay);
                }else
                {
                    int visitTimes = userStats.getVisitTimes()+1;
                    userStats.setVisitTimes(visitTimes);
                    builder.setCurrcount(visitTimes);
                    builder.setAccumulatecount(continueDay);
                }
            }else
            {
                builder.setTodaystatus(2);
                userStats.setVisitTimes(1);
                builder.setCurrcount(1);
                builder.setAccumulatecount(continueDay);
            }
            userStats.setLastVisitTime(time);
        }
        
        player.setLastVerifyTime(time);
        
		builder.setServertime(time);
		player.sendPacket(OP.code.OPCODE_ADD_ADVENTURE_BOUNDLESSHOLEINFORET_S_VALUE, builder);// 发送返回包
		long current_time_2 = Util.getServerMillTime();
		long taken_time=current_time_2-current_time;
		log.info("add boundless hole handler ret send: {}, {}, {}", new Object[]{id ,taken_time});
		logger2.info("add boundless hole handler ret send: {}, {}, {}", new Object[]{id ,taken_time});
	}
	
    public static void randomReward(OPAddBoundlessHoleRet.Builder builder,VisitRewardSelecter visitRewardSelecter,Player player)
    {
        double randomNum = Math.random();
        List<Float> rateList = visitRewardSelecter.getRates();
        int listLen = rateList.size();
        int selectedNo=0;
        for(int i=0;i<listLen;i++)
        {
            if(randomNum<rateList.get(i))
            {
                selectedNo = i;
                break;
            }
        }
        int itemType = visitRewardSelecter.getRewardTypes().get(selectedNo);
        int type = UserToolConst.getType(itemType);
        int itemId = 0;
        switch (type)
        {
            case UserToolConst.EQUIP_TYPE:
                EquipSelecter equipSelecter = XMLTemplateService.getEquitSelecter(itemType);
                List<Integer> equipRateList = equipSelecter.getVisitRate();
                double equipRandom = Math.random()*(equipRateList.get(equipRateList.size()-1));
                int equipLen = equipRateList.size();
                int equipSelectedNo=0;
                for(int i=0;i<equipLen;i++)
                {
                    if(equipRandom<equipRateList.get(i))
                    {
                        equipSelectedNo = i;
                        break;
                    }
                }
                itemId = equipSelecter.getId().get(equipSelectedNo);
                if (itemType == UserToolConst.A_ARMOR || itemType == UserToolConst.A_WEAPON || itemType == UserToolConst.A_ACCESSORIES) {
                	ArrayList<Integer> params = new ArrayList<Integer>(2);
	            	params.add(1);
	            	params.add(itemId);
	            	BoardCastInfoHandler.addBoardCase(BoardCastInfoHandler.CANBAI_TYPE, player.getName(), params);
	            }
                builder.setEquipInfo(EquipServices.createEquitAlone(player, itemId));
                break;
            case UserToolConst.SKILL_TYPE:
                int quality = 0;
                switch (itemType)
                {
                    case UserToolConst.A_SKILL:
                        quality = UserToolConst.A_QUALITY;
                        break;
                    case UserToolConst.B_SKILL:
                        quality = UserToolConst.B_QUALITY;
                        break;
                    case UserToolConst.C_SKILL:
                        quality = UserToolConst.C_QUALITY;
                        break;
                    case UserToolConst.D_SKILL:
                        quality = UserToolConst.D_QUALITY;
                        break;
                    default:
                        break;
                }
                SkillSelecter skillSelecter = XMLTemplateService.getSkillSelecter(quality);
                List<Integer> skillRateList = skillSelecter.getVisitRate();
                double skillRandom = Math.random()*(skillRateList.get(skillRateList.size()-1));
                int skillLen = skillRateList.size();
                int skillSelectedNo=0;
                for(int i=0;i<skillLen;i++)
                {
                    if(skillRandom<skillRateList.get(i))
                    {
                        skillSelectedNo = i;
                        break;
                    }
                }
                itemId = skillSelecter.getId().get(skillSelectedNo);
                if (itemType == UserToolConst.A_SKILL) {//添加系统广播
	            	   ArrayList<Integer> params = new ArrayList<Integer>(2);
	            	   params.add(2);
	            	   params.add(itemId);
	            	   BoardCastInfoHandler.addBoardCase(BoardCastInfoHandler.CANBAI_TYPE, player.getName(), params);
	            }
                builder.setSkillInfo(SkillServices.createSkillAlone(player, itemId));
                break;
            default:
                break;
        }
        builder.setReward(Util.getRewardInfoBuilder(itemType, itemId, 1));
    }

}

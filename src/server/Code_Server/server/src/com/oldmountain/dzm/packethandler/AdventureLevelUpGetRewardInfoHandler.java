package com.oldmountain.dzm.packethandler;

import com.oldmountain.dzm.message.ErrorReporter;
import org.apache.mina.common.IoSession;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.google.protobuf.InvalidProtocolBufferException;
import com.oldmountain.dzm.entity.LevelUpXmlTemplate;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.message.OP;
import com.oldmountain.dzm.message.AdventureLevelUpGetReward.OPAdventureLevelUpGetReward;
import com.oldmountain.dzm.message.AdventureLevelUpGetReward.OPAdventureLevelUpGetRewardRet;
import com.oldmountain.dzm.net.ObjectAccessor;
import com.oldmountain.dzm.net.packet.Packet;
import com.oldmountain.dzm.util.TitleUtil;
import com.oldmountain.dzm.util.Util;
import com.oldmountain.dzm.util.XMLTemplateService;

/*
 * Created with IntelliJ IDEA.
 * User: Administrator
 * Date: 13-3-26
 * Time: 下午3:01
 * To change this template use File | Settings | File Templates.
 */
public class AdventureLevelUpGetRewardInfoHandler {
    private static final Logger log = LoggerFactory.getLogger(AddAdventureTeachExpHandler.class);
    private static final Logger logger2 = LoggerFactory.getLogger("AdventureInfo");
    public static void getInfo(Packet packet, IoSession session){
    	OPAdventureLevelUpGetReward params = null;
    	OPAdventureLevelUpGetRewardRet.Builder builder = OPAdventureLevelUpGetRewardRet.newBuilder();
    	try{
    		params = OPAdventureLevelUpGetReward.parseFrom(packet.data.array());
    		
    	} catch(InvalidProtocolBufferException e){
    		e.printStackTrace();
    		return;
    	}
    	
    	int version = params.getVersion();
    	int rewardId = params.getRewardId();
    	
    	if(version != 1){
    		
    	}
    	int uuid = (Integer)session.getAttribute("playerid");
    	Player player = ObjectAccessor.getOnlineUser(uuid);
    	int id=player.getId();
    	log.info("adventure level up get reward info handler received: {}, {}, {}", new Object[]{id , version , rewardId});
    	logger2.info("adventure level up get reward info handler received: {}, {}, {}", new Object[]{id , version , rewardId});
    	long current_time = Util.getServerMillTime();
    	builder.setServertime((int) System.currentTimeMillis());
    	LevelUpXmlTemplate levelUpInfo = XMLTemplateService.getLevelUpTemplate(rewardId);
    	

        //modify 活动奖励元宝只送vip，不计充值额度，20130418 4人pk讨论结果
    	if(player.getLevel() >= levelUpInfo.getNeedLevel())
        {
            String key=levelUpInfo.getId()+",";
            if(player.getUserStats().getLevelupreward().length()==0|| !player.getUserStats().getLevelupreward().contains(key))
            {
                player.getUserStats().rewardLevelUpReward(levelUpInfo.getId());
                player.setSysgoldcoins(player.getSysgoldcoins()+levelUpInfo.getGoldcoins());
                if(levelUpInfo.getUpVipLevel()!=0&&levelUpInfo.getUpVipLevel()>player.getViplevel())
                {
                    player.setViplevel(levelUpInfo.getUpVipLevel());
                    TitleUtil.checkNewTitle(player, 2, levelUpInfo.getUpVipLevel());
                }
                builder.setGoldcoins(player.getSysgoldcoins()+player.getRechargegoldcoins());
                builder.setRechargenum(player.getRechargenum());
                builder.setViplevel(player.getViplevel());
            }
            else
            {
                ErrorReporter.OPErrorReporter.Builder eBuilder = ErrorReporter.OPErrorReporter.newBuilder();
                eBuilder.setErrorid(OP.code.OPCODE_ADVENTURE_LEVEL_UP_GET_REWARDRET_S_VALUE);
                eBuilder.setErrormsg("Can't Jump Get Reward Or This Reward Already Get!");
                player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, eBuilder);
                return;
            }
    	}
        else
        {
    		builder.setGoldcoins(0);
    		builder.setRechargenum(0);
    	}
    	
    	player.setLastVerifyTime(Util.getServerTime());
    	
    	player.sendPacket(OP.code.OPCODE_ADVENTURE_LEVEL_UP_GET_REWARDRET_S_VALUE, builder);
    	long current_time_2 = Util.getServerMillTime();
    	long taken_time=current_time_2-current_time;
    	log.info("adventure level up get reward info handler ret send: {}, {}, {}", new Object[]{id ,taken_time});
    	logger2.info("adventure level up get reward info handler ret send: {}, {}, {}", new Object[]{id ,taken_time});
    	
    }
}
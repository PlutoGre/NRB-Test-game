package com.oldmountain.dzm.packethandler;

import org.apache.mina.common.IoSession;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.google.protobuf.InvalidProtocolBufferException;
import com.oldmountain.dzm.entity.LevelUpXmlTemplate;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.message.OP;
import com.oldmountain.dzm.message.AdventureLevelUpReward.OPAdventureLevelUpReward;
import com.oldmountain.dzm.message.AdventureLevelUpReward.OPAdventureLevelUpRewardRet;
import com.oldmountain.dzm.net.ObjectAccessor;
import com.oldmountain.dzm.net.packet.Packet;
import com.oldmountain.dzm.util.Util;
import com.oldmountain.dzm.util.XMLTemplateService;

/*
 * Created with IntelliJ IDEA.
 * User: Administrator
 * Date: 13-3-26
 * Time: 下午3:01
 * To change this template use File | Settings | File Templates.
 */
public class AdventureLevelUpRewardInfoHandler {
    private static final Logger log = LoggerFactory.getLogger(AddAdventureTeachExpHandler.class);
    private static final Logger logger2 = LoggerFactory.getLogger("AdventureInfo");
    public static void getInfo(Packet packet, IoSession session){
    	OPAdventureLevelUpReward params = null;
    	OPAdventureLevelUpRewardRet.Builder builder = OPAdventureLevelUpRewardRet.newBuilder();
    	try{
    		params = OPAdventureLevelUpReward.parseFrom(packet.data.array());
    		
    	} catch(InvalidProtocolBufferException e){
    		e.printStackTrace();
    		return;
    	}
    	
    	int version = params.getVersion();
    	
    	if(version != 1){
    		
    	}
    	int uuid = (Integer)session.getAttribute("playerid");
    	Player player = ObjectAccessor.getOnlineUser(uuid);
    	int id=player.getId();
    	log.info("adventure level up reward info handler received: {}, {}, {}", new Object[]{id , version});
    	logger2.info("adventure level up reward info handler received: {}, {}, {}", new Object[]{id , version});
    	long current_time = Util.getServerMillTime();
    	OPAdventureLevelUpRewardRet.AdventureLevelUpRewardInfo.Builder levelUpBuilder = OPAdventureLevelUpRewardRet.AdventureLevelUpRewardInfo.newBuilder();
    	builder.setServertime((int) System.currentTimeMillis());

    	LevelUpXmlTemplate levelUpInfo;
    	for(int i=1;i<=XMLTemplateService.getLevelUpTemplateSize();i++)
        {
    		levelUpInfo = XMLTemplateService.getLevelUpTemplate(i);
    		levelUpBuilder.setId(i);
    		levelUpBuilder.setNeedLevel(levelUpInfo.getNeedLevel());
    		levelUpBuilder.setRechargenum(levelUpInfo.getRechargenum());
    		levelUpBuilder.setGoldcoins(levelUpInfo.getGoldcoins());
    		levelUpBuilder.setUpVipLevel(levelUpInfo.getUpVipLevel());
            String key=i+",";
    		levelUpBuilder.setRewardStatus((player.getLevel() >= levelUpInfo.getNeedLevel()?1:0)+(player.getUserStats().getLevelupreward().contains(key)?1:0));

    		builder.addLevelUpReward(levelUpBuilder);
    	}
    	
    	player.setLastVerifyTime(Util.getServerTime());
    	
    	player.sendPacket(OP.code.OPCODE_ADVENTURE_LEVEL_UP_REWARDRET_S_VALUE, builder);
    	long current_time_2 = Util.getServerMillTime();
    	long taken_time=current_time_2-current_time;
    	log.info("adventure level up reward info handler ret send: {}, {}, {}", new Object[]{id ,taken_time});
    	logger2.info("adventure level up reward info handler ret send: {}, {}, {}", new Object[]{id ,taken_time});
    }
}
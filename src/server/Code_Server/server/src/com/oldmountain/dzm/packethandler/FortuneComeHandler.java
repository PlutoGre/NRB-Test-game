package com.oldmountain.dzm.packethandler;

import java.util.Random;

import org.apache.mina.common.IoSession;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.google.protobuf.InvalidProtocolBufferException;
import com.oldmountain.dzm.DZM;
import com.oldmountain.dzm.entity.FortuneComeXmlTemplate;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.entity.UserStats;
import com.oldmountain.dzm.message.ErrorReporter;
import com.oldmountain.dzm.message.OP;
import com.oldmountain.dzm.message.FortuneCome.OPFortuneCome;
import com.oldmountain.dzm.message.FortuneCome.OPFortuneComeRet;
import com.oldmountain.dzm.net.ObjectAccessor;
import com.oldmountain.dzm.net.packet.Packet;
import com.oldmountain.dzm.util.Util;
import com.oldmountain.dzm.util.XMLTemplateService;

public class FortuneComeHandler {
	private static final Logger logger = LoggerFactory.getLogger(FortuneComeHandler.class);
/**
 * rewardStatus: 0--只获取界面信息  1--成功    2--异常状态   
 * */
	public static void getFortuneComeInfo(Packet packet, IoSession session) {
		OPFortuneCome params = null;
		OPFortuneComeRet.Builder builder = OPFortuneComeRet.newBuilder();
		
		try {
			params = OPFortuneCome.parseFrom(packet.data.array());
		} catch (InvalidProtocolBufferException e) {
			e.printStackTrace();
			return;
		}
		boolean getReward=params.hasErnie();
		int version=params.getVersion();
		int playerId = (Integer)session.getAttribute("playerid");
		Player player = ObjectAccessor.getOnlineUser(playerId);
		logger.info("fortune come handler received: {}, {}, {}", new Object[]{playerId , version});
		long current_time = Util.getServerMillTime();
		int time=Util.getServerTime();
		UserStats userStats=player.getUserStats();
		int instructionFinishTime=player.getInstructionFinishTime();
		int totalTimes=XMLTemplateService.getFortuneComeXmlTemplatesSize();
		int fortuneComeTimes=userStats.getFortuneComeTimes();
		/************************************************************************/
		if(time<instructionFinishTime || time>(instructionFinishTime+48*60*60)){
			ErrorReporter.OPErrorReporter.Builder eBuilder = ErrorReporter.OPErrorReporter.newBuilder();
	        eBuilder.setErrorid(OP.code.OPCODE_FORTUNE_COMERET_S_VALUE);
	        eBuilder.setErrormsg("PlayerId:"+player.getId()+" this adventure is not open!");
	        player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, eBuilder);
	        return;
		}
		else if(fortuneComeTimes>=totalTimes){
			ErrorReporter.OPErrorReporter.Builder eBuilder = ErrorReporter.OPErrorReporter.newBuilder();
	        eBuilder.setErrorid(OP.code.OPCODE_FORTUNE_COMERET_S_VALUE);
	        eBuilder.setErrormsg("PlayerId:"+player.getId()+" get reward times out of bound!");
	        player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, eBuilder);
	        return;
		}
		int chanceNum=0;//剩余摇奖次数
		int needGold=0;
		int reeardStatus=0;
		int rewardGold=0;
		FortuneComeXmlTemplate template=XMLTemplateService.getFortuneComeXmlTemplate(fortuneComeTimes+1);
		int minReward=template.getMinReward();
		int maxReward=template.getMaxReward();
		needGold=template.getNeedGold();
		if(getReward){
			if(player.getGoldCoins()<needGold){
				reeardStatus=2;
			}
			else{
				//摇奖
				rewardGold=getFortune(minReward,maxReward);
				player.increaseSysGoldCoins(rewardGold);
				//元宝扣除
				player.consumeGold(needGold);
				builder.setCostGold(needGold);
				fortuneComeTimes=fortuneComeTimes+1;
				userStats.setFortuneComeTimes(fortuneComeTimes);
				player.setUserStats(userStats);
				DZM.getEntityManager().updateSync(userStats);
				reeardStatus=1;
				//获取下次的信息
				if(fortuneComeTimes<totalTimes){
					//再次获取配置信息
					template=XMLTemplateService.getFortuneComeXmlTemplate(fortuneComeTimes+1);
				}
			}
		}
		chanceNum=totalTimes-fortuneComeTimes;
		needGold=template.getNeedGold();
		int recurittime=instructionFinishTime+48*60*60-time;
		
		
		builder.setServertime(time);
		builder.setLastDays(2);
		builder.setChanceNum(chanceNum);
		builder.setRecurittime(recurittime);
		builder.setNeedGold(needGold);
		builder.setUserGold(player.getGoldCoins());
		builder.setRewardGold(rewardGold);
		builder.setRewardStat(reeardStatus);
		
		
		
		
		/************************************************************************/
		player.sendPacket(OP.code.OPCODE_FORTUNE_COMERET_S_VALUE, builder);// 发送返回包
		long current_time_2 = Util.getServerMillTime();
		long taken_time = current_time_2 - current_time;
		logger.info("fortune come handler ret send: {}, {}, {}", new Object[] {playerId, taken_time });
		
	}
	private static int getFortune(int minReward, int maxReward){
		Random random=new Random();
		int reward=random.nextInt(maxReward-minReward+1)+minReward;
		return reward;
	}
}

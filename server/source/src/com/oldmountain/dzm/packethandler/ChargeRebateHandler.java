package com.oldmountain.dzm.packethandler;


import org.apache.mina.common.IoSession;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.google.protobuf.InvalidProtocolBufferException;
import com.oldmountain.dzm.DZM;
import com.oldmountain.dzm.entity.ChargeRebateXmlTemplate;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.entity.UserStats;
import com.oldmountain.dzm.message.ErrorReporter;
import com.oldmountain.dzm.message.OP;
import com.oldmountain.dzm.message.ChargeRebate.OPChargeRebate;
import com.oldmountain.dzm.message.ChargeRebate.OPChargeRebateRet;
import com.oldmountain.dzm.net.ObjectAccessor;
import com.oldmountain.dzm.net.packet.Packet;
import com.oldmountain.dzm.util.Util;
import com.oldmountain.dzm.util.XMLTemplateService;

/**
 * getRewardStat:领取返利状态:0-没领取  ; 1-正常领取 ; 2-已领取(重复领取)
 * */
public class ChargeRebateHandler {
	private static final Logger logger = LoggerFactory.getLogger(ChargeRebateHandler.class);

	public static void getChargeRebateInfo(Packet packet, IoSession session) {
		
		OPChargeRebate params = null;
		OPChargeRebateRet.Builder builder = OPChargeRebateRet.newBuilder();
		
		try {
			params = OPChargeRebate.parseFrom(packet.data.array());
		} catch (InvalidProtocolBufferException e) {
			e.printStackTrace();
			return;
		}
		boolean getRebate=params.hasGetRebate();
		int version=params.getVersion();
		int playerId = (Integer)session.getAttribute("playerid");
		Player player = ObjectAccessor.getOnlineUser(playerId);
		UserStats userStats=player.getUserStats();
		logger.info("charge rebate info handler received: {}, {}, {}", new Object[]{playerId , version});
		long current_time = Util.getServerMillTime();
		int time=Util.getServerTime();//操作时间
		int termId=0;
		int chargeDurationTimes=Util.inChargeDuration(player, time);
    	int rebateDurationTimes=Util.inRebateDuration(player, time);
    	int charge_type=0;
    	if((chargeDurationTimes==0 && rebateDurationTimes==0) || (chargeDurationTimes!=0 && rebateDurationTimes!=0)){
    		ErrorReporter.OPErrorReporter.Builder eBuilder = ErrorReporter.OPErrorReporter.newBuilder();
	        eBuilder.setErrorid(OP.code.OPCODE_CHARGE_REBATERET_S_VALUE);
	        eBuilder.setErrormsg("PlayerId:"+player.getId()+" this adventure is not open!");
	        player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, eBuilder);
	        return;
    	}
		if(chargeDurationTimes!=0){
			termId=chargeDurationTimes;
			charge_type=1;
		}
		else if(rebateDurationTimes!=0){
			termId=rebateDurationTimes;
			charge_type=2;
		}
		int chargeTerm=userStats.getChargeTerm();
		if(chargeTerm!=termId){
			userStats.setChargeHeap(0);
			userStats.setChargeTerm(termId);
		}
		ChargeRebateXmlTemplate chargeRebateXmlTemplate=XMLTemplateService.getChargeRebateXmlTemplate(termId);
		int activeTimeInterval=chargeRebateXmlTemplate.getActiveTimeInterval();	//活动间隔时间(天)
		int chargeDuration=chargeRebateXmlTemplate.getChargeDuration();	//储值累积时间(天)
		int rebateDuration=chargeRebateXmlTemplate.getRebateDuration();	//返利时间(天)
		int minRecharge=chargeRebateXmlTemplate.getMinRecharge();	//最小储值额度
		int maxRecharge=chargeRebateXmlTemplate.getMaxRecharge();	//最大储值额度
		float rebateRate=chargeRebateXmlTemplate.getRefundRate();	//返还比率
		
		
		int instructionFinishTime=player.getInstructionFinishTime();	//玩家完成新手指引的时间+48小时(即财神活动的结束时间)
		int primaryTime=Util.getNextDayMarshal((long)instructionFinishTime); //新手引导完成当天晚上24:00:00的时间
		
		int chargeHeap=userStats.getChargeHeap();	//实充累计金额
		int getRewardStat=0;	//领取返利状态	
		int exchangeRate=10; //钻石:人名币额度 固定为10
		int refundGoldCoins=0;
		if(chargeHeap>=minRecharge){
			int baseMoney=(chargeHeap>maxRecharge)?maxRecharge:chargeHeap;
			refundGoldCoins=Math.round(rebateRate*baseMoney*exchangeRate);		//返利钻石数,四舍五入
		}
		
		builder.setRefund(rebateRate*rebateDuration);
		builder.setRechargeInfact(chargeHeap);
		builder.setServertime(time);
		builder.setMaxRecharge(maxRecharge);
		builder.setMinRecharge(minRecharge);
		builder.setExchangeRate(exchangeRate);
		builder.setRefundGoldCoins(refundGoldCoins);
		builder.setRebateDuringDays(rebateDuration);
		
		int rebateBeginTime=0;	//当前返利开始时间
		int rebateEndTime=0;	//当前返利结束时间
		switch (charge_type) {
		case 1:
			if(termId==1){
				//新手充值返利
				if(activeTimeInterval==0){
					rebateBeginTime=instructionFinishTime;
					rebateEndTime=primaryTime+(activeTimeInterval+chargeDuration)*24*60*60;
				}
				else{
					rebateBeginTime=primaryTime+activeTimeInterval*24*60*60;
					rebateEndTime=rebateBeginTime+chargeDuration*24*60*60;
				}
			}else{
				rebateBeginTime=primaryTime+activeTimeInterval*24*60*60;
				rebateEndTime=rebateBeginTime+chargeDuration*24*60*60;
			}
			builder.setChargeType(OPChargeRebateRet.Charge_Type.CHARGE);
			break;
		case 2:
			rebateBeginTime=primaryTime+(activeTimeInterval+chargeDuration)*24*60*60;
			rebateEndTime=rebateBeginTime+rebateDuration*24*60*60;
			builder.setChargeType(OPChargeRebateRet.Charge_Type.REBATE);
			break;
		default:
			ErrorReporter.OPErrorReporter.Builder eBuilder = ErrorReporter.OPErrorReporter.newBuilder();
	        eBuilder.setErrorid(OP.code.OPCODE_CHARGE_REBATERET_S_VALUE);
	        eBuilder.setErrormsg("PlayerId:"+player.getId()+" activity is not open!");
	        player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, eBuilder);
	        return;
		}
		int recurittime=rebateEndTime-time;	//储值累积倒计时(s)
		builder.setRecurittime(recurittime);
		builder.setRebateBeginTime(rebateBeginTime);
		builder.setRebateEndTime(rebateEndTime);
		if(getRebate){
			if(rebateDurationTimes==0){
				//如果玩家不处于返利阶段
				getRewardStat=3;
			}	
			else if(userStats.getGetRebate()==0){
				//正常领取
				player.increaseSysGoldCoins(refundGoldCoins);
				userStats.setGetRebate(1);
				getRewardStat=1;
			}
			else{
				//不能重复领取
				getRewardStat=2;
			}
			builder.setGetRewardStat(getRewardStat);
			builder.setUserGoldCoins(player.getGoldCoins());
		}
		player.setUserStats(userStats);
		DZM.getEntityManager().updateSync(userStats);
		builder.setGetRewardToday(userStats.getGetRebate());
		player.sendPacket(OP.code.OPCODE_CHARGE_REBATERET_S_VALUE, builder);// 发送返回包
		long current_time_2 = Util.getServerMillTime();
		long taken_time = current_time_2 - current_time;
		logger.info("charge rebate info handler ret send: {}, {}, {}", new Object[] {playerId, taken_time });
	}
	
	public static void addChargeHeap(Packet packet, IoSession session ,int charge ,Player player ,int time) {
		int termId=Util.inChargeDuration(player, time);
		if(termId==0){
			return;
		}else{
			OPChargeRebateRet.Builder builder = OPChargeRebateRet.newBuilder();
			UserStats userStats=player.getUserStats();
			int chargeTerm=userStats.getChargeTerm();
			if(chargeTerm!=termId){
				userStats.setChargeHeap(0);
				userStats.setChargeTerm(termId);
			}
			int chargeHeap=userStats.getChargeHeap();
			chargeHeap+=charge;
			userStats.setChargeHeap(chargeHeap);
			player.setUserStats(userStats);
			DZM.getEntityManager().updateSync(userStats);
			ChargeRebateXmlTemplate chargeRebateXmlTemplate=XMLTemplateService.getChargeRebateXmlTemplate(termId);
			int activeTimeInterval=chargeRebateXmlTemplate.getActiveTimeInterval();	//活动间隔时间(天)
			int chargeDuration=chargeRebateXmlTemplate.getChargeDuration();	//储值累积时间(天)
			int rebateDuration=chargeRebateXmlTemplate.getRebateDuration();	//返利时间(天)
			int minRecharge=chargeRebateXmlTemplate.getMinRecharge();	//最小储值额度
			int maxRecharge=chargeRebateXmlTemplate.getMaxRecharge();	//最大储值额度
			float rebateRate=chargeRebateXmlTemplate.getRefundRate();	//返还比率
			int refundGoldCoins=0;
			int exchangeRate=10; //钻石:人名币额度 固定为10
			if(chargeHeap>=minRecharge){
				int baseMoney=(chargeHeap>maxRecharge)?maxRecharge:chargeHeap;
				refundGoldCoins=Math.round(rebateRate*baseMoney*exchangeRate);		//返利钻石数,四舍五入
			}
			
			
			int instructionFinishTime=player.getInstructionFinishTime();	//玩家完成新手指引的时间+48小时(即财神活动的结束时间)
			int primaryTime=Util.getNextDayMarshal((long)instructionFinishTime); //新手引导完成当天晚上24:00:00的时间
			int rebateBeginTime=0;
			int rebateEndTime=0;
			if(termId==1){
				//新手充值返利
				if(activeTimeInterval==0){
					rebateBeginTime=instructionFinishTime;
					rebateEndTime=primaryTime+(activeTimeInterval+chargeDuration)*24*60*60;
				}
				else{
					rebateBeginTime=primaryTime+activeTimeInterval*24*60*60;
					rebateEndTime=rebateBeginTime+chargeDuration*24*60*60;
				}
			}else{
				rebateBeginTime=primaryTime+activeTimeInterval*24*60*60;
				rebateEndTime=rebateBeginTime+chargeDuration*24*60*60;
			}
			int recurittime=rebateEndTime-time;	//储值累积倒计时(s)
			builder.setServertime(Util.getServerTime());
			builder.setChargeType(OPChargeRebateRet.Charge_Type.CHARGE);
			builder.setRecurittime(recurittime);
			builder.setMinRecharge(minRecharge);
			builder.setMaxRecharge(maxRecharge);
			builder.setRefund(rebateRate*rebateDuration);
			builder.setRechargeInfact(chargeHeap);
			builder.setRebateBeginTime(rebateBeginTime);
			builder.setRebateEndTime(rebateEndTime);
			builder.setExchangeRate(exchangeRate);
			builder.setRefundGoldCoins(refundGoldCoins);
			builder.setRebateDuringDays(rebateDuration);
			player.sendPacket(OP.code.OPCODE_CHARGE_REBATERET_S_VALUE, builder);// 发送返回包
		}
	}
}

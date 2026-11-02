package com.oldmountain.dzm.util;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Random;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.oldmountain.dzm.entity.GiveSoulSelecter;
import com.oldmountain.dzm.entity.GiveSoulStage;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.entity.Soul;
import com.oldmountain.dzm.entity.SoulSelecter;
import com.oldmountain.dzm.entity.UserStats;
import com.oldmountain.dzm.message.LeagueStructExt.OPLeaguaShopBuyRet;
import com.oldmountain.dzm.message.MarketRecruitDisciple.OPMarketRecruitDiscipleRet;
import com.oldmountain.dzm.message.UserSoulInfo.OPUserSoulInfoRet;

public class SoulUtil {
	private static final Logger logger2 = LoggerFactory.getLogger("MarketInfo");
	/**
	 * 根据魂魄的品质和奖池来随机一个魂魄ID
	 * @param int type(1为甲级，2为乙级，3为丙级，4为丁级)
	 * @param int pool(1为连续登陆奖励，2为江湖闯关奖励)
	 * @return int soulId
	 */
	public static int randSoulId(int type,int pool){
		SoulSelecter selecter = XMLTemplateService.getSoulSelecter(type);
		ArrayList<Integer>ids = selecter.getSoulIds();
		ArrayList<Integer>rates = new ArrayList<Integer>();
		
		switch (pool) {
			case 1://连续登陆池
				rates = selecter.getLoginRates();
				break;
			case 2://关卡奖励池
				rates = selecter.getCareerRates();
				break;
		}
		
		Random random = new Random();
		int a = random.nextInt(rates.get(rates.size() - 1)) + 1;
		int index = rates.size() - 1;
		for (int i = 0; i < rates.size(); i++) {
			if (a <= rates.get(i)) {
				index = i;
				break;
			}
		}
		
		return ids.get(index);
	}
	
	public static OPUserSoulInfoRet.SoulInfo.Builder buildSoulBuilder(Soul soul){
		OPUserSoulInfoRet.SoulInfo.Builder builder = OPUserSoulInfoRet.SoulInfo.newBuilder();
		builder.setId(soul.getId());
		builder.setItemid(soul.getItemid());
		builder.setCount(soul.getCount());
		return builder;
	}
	
	public static OPLeaguaShopBuyRet.SoulInfo.Builder buildSoulBuilderForLeagua(Soul soul){
		OPLeaguaShopBuyRet.SoulInfo.Builder builder = OPLeaguaShopBuyRet.SoulInfo.newBuilder();
		builder.setId(soul.getId());
		builder.setItemid(soul.getItemid());
		builder.setCount(soul.getCount());
		return builder;
	}
	
	public static void giveSoul(Player player , OPMarketRecruitDiscipleRet.Builder builder , int kind , int itermId){
		
		GiveSoulSelecter selecter=XMLTemplateService.getGiveSoulSelecter(kind);
		GiveSoulStage soulStage=XMLTemplateService.getGiveSoulStage(kind);
		UserStats userStats=player.getUserStats();
		
		int differTimes=0;
		int realcount=0;
		int additionalSoulCount=0;
		int additionalSoulCount_give=0;
		int additionalSoulItem=0;
		//玩家第一次进入该期活动时初始化相关数据
		if(userStats.getGiveSoulTerm()!=itermId){
			userStats.setGiveSoulTerm(itermId);
			userStats.setGiveSoulDifferTimes2(0);
			userStats.setGiveSoulDifferTimes3(0);
		}
		List<Integer> condition_count=soulStage.getCondition_count();
		List<Integer> condition_soulId=soulStage.getCondition_soulId();
		if(condition_soulId.size()<2){
			return;
		}else{
			realcount=getSoulCount(player, condition_soulId.get(1));
		}
		
		if(kind==2)
		{
			differTimes = userStats.getGiveSoulDifferTimes2();
		}
		else if(kind==3)
		{
			differTimes = userStats.getGiveSoulDifferTimes3();
		}
		//所在阶段
		int stage=0;
		int stage_size=condition_count.size();
		
		if(realcount>condition_count.get(stage_size-1)){
			stage=stage_size-1;
		}
		else{
			for(int i=1;i<stage_size-1;i++){
				if(realcount>=condition_count.get(i)&&realcount<=condition_count.get(i+1)){
					stage=i;
				}
			}
		}
		
		//该类型下所有赠送魂魄的id
		List<Integer> selecter_soulItemid=selecter.getSoulItemid();
		
		//该类型该阶段下的保底抽取次数
		int balanceTime = soulStage.getBalanceTimes().get(stage);
		int balanceId = soulStage.getBalanceId().get(stage);
		int balanceSoulId = soulStage.getBalanceSoulId().get(stage);
		
		//该类型下可抽中的列表ids
		List<Integer> selecter_ids=selecter.getId();
		List<Integer> selecter_count1=selecter.getCount1();
		List<Integer> selecter_count2=selecter.getCount2();
		int selecter_index=-1;
		Soul soul=new Soul();
		HashMap<Integer, Soul> userSouls=player.getSouls();
		//达到保底条件
		if(differTimes>=(balanceTime-1)){
			for(int i=0;i<selecter_ids.size();i++){
				if(selecter_ids.get(i)==balanceId){
					selecter_index=i;
				}
			}
			if(kind==2)
			{
				userStats.setGiveSoulDifferTimes2(0);
			}
			else if(kind==3)
			{
				userStats.setGiveSoulDifferTimes3(0);
			}
		}
		else{
			
			//该类型该阶段下的累加权值
			List<Integer> rates=new ArrayList<Integer>();
			switch (stage) {
			case 1:
				rates=selecter.getRate1();
				break;
			case 2:
				rates=selecter.getRate2();
				break;
			case 3:
				rates=selecter.getRate3();
				break;
			case 4:
				rates=selecter.getRate4();
				break;
			case 5:
				rates=selecter.getRate5();
				break;
			}
			int rates_size=rates.size();
			Random random=new Random();
			
			
			int random_count=0;
			if(rates_size>0){
				random_count=random.nextInt(rates.get(rates_size-1))+1;
				if(random_count>0 && random_count<=rates.get(0)){
					selecter_index=0;
				}
				else{
					for(int i=1;i<rates_size;i++){
						if(random_count>rates.get(i-1) && random_count<= rates.get(i)){
							selecter_index=i;
						}
					}
				}
			}
		}
		
		if(kind==2){
			if(selecter_soulItemid.get(selecter_index)!=balanceSoulId){
				userStats.setGiveSoulDifferTimes2(userStats.getGiveSoulDifferTimes2()+1);
			}
			else{
				userStats.setGiveSoulDifferTimes2(0);
			}
		}
		else if(kind==3){
			if(selecter_soulItemid.get(selecter_index)!=balanceSoulId){
				userStats.setGiveSoulDifferTimes3(userStats.getGiveSoulDifferTimes3()+1);
			}
			else{
				userStats.setGiveSoulDifferTimes3(0);
			}
		}
		
		
		additionalSoulItem=selecter_soulItemid.get(selecter_index);
		additionalSoulCount=selecter_count1.get(selecter_index);
		additionalSoulCount_give=selecter_count2.get(selecter_index);
		player.setUserStats(userStats);
		if(additionalSoulItem>0){
			if(userSouls.containsKey(additionalSoulItem)){
				soul=userSouls.get(additionalSoulItem);
				soul.setCount(soul.getCount()+additionalSoulCount+additionalSoulCount_give);
				player.updateOneSoul(soul);
			}
			else{
				soul.setItemid(additionalSoulItem);
				soul.setCount(additionalSoulCount+additionalSoulCount_give);
				soul.setPlayerid(player.getId());
				player.addOneSoul(soul);
			}
			for(int i=0; i<selecter_soulItemid.size();i++){
				builder.addAdditionalSoulList(selecter_soulItemid.get(i));
			}
			OPUserSoulInfoRet.SoulInfo.Builder soulBuilder=buildSoulBuilder(soul);
			builder.setAdditionalSoul(soulBuilder);
			builder.setAdditionalSoulCount(additionalSoulCount);
			builder.setAdditionalSoulCountGive(additionalSoulCount_give);
		}
		else {
			logger2.info("Give Soul Error cant get additionalSoul: {}, {}, {}", new Object[]{player.getId() , kind , itermId});
		}
		logger2.info("Market get additionalSoul: {}, {}, {}, {}", new Object[]{player.getId() , additionalSoulItem , additionalSoulCount , additionalSoulCount_give});
	}
	public static int getSoulCount(Player player , int itemId){
		int count=0;
		HashMap<Integer, Soul> souls=player.getSouls();
		if(souls.containsKey(itemId)){
			Soul soul=souls.get(itemId);
			count=soul.getCount();
		}
		return count;
	}
}

package com.oldmountain.dzm.packethandler;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map.Entry;

import com.oldmountain.dzm.util.BagUtil;

import org.apache.mina.common.IoSession;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.google.protobuf.InvalidProtocolBufferException;
import com.oldmountain.dzm.entity.Bag;
import com.oldmountain.dzm.entity.Battle;
import com.oldmountain.dzm.entity.BattleArrayObject;
import com.oldmountain.dzm.entity.CharacterResult;
import com.oldmountain.dzm.entity.Disciple;
import com.oldmountain.dzm.entity.DiscipleXmlTemplate;
import com.oldmountain.dzm.entity.EquipSelecter;
import com.oldmountain.dzm.entity.LuckyStarXmlTemplate;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.entity.SkillSelecter;
import com.oldmountain.dzm.entity.ToolContainReward;
import com.oldmountain.dzm.entity.TreasureSelecter;
import com.oldmountain.dzm.entity.UserStats;
import com.oldmountain.dzm.message.OP;
import com.oldmountain.dzm.message.ErrorReporter.OPErrorReporter;
import com.oldmountain.dzm.message.UseTool.OPUseTool;
import com.oldmountain.dzm.message.UseTool.OPUseToolRet;
import com.oldmountain.dzm.message.UserBattleArray.OPUserBattleArrayRet;
import com.oldmountain.dzm.message.UserDiscipleInfo.OPUserDiscipleInfoRet;
import com.oldmountain.dzm.message.UserRewardInfo.OPRewardInfoRet;
import com.oldmountain.dzm.net.ObjectAccessor;
import com.oldmountain.dzm.net.packet.Packet;
import com.oldmountain.dzm.services.BagToolServices;
import com.oldmountain.dzm.services.EquipServices;
import com.oldmountain.dzm.services.SkillServices;
import com.oldmountain.dzm.services.SoulServices;
import com.oldmountain.dzm.services.UserPropertyServices;
import com.oldmountain.dzm.util.BattleUtil;
import com.oldmountain.dzm.util.DiscipleUtil;
import com.oldmountain.dzm.util.UserToolConst;
import com.oldmountain.dzm.util.Util;
import com.oldmountain.dzm.util.XMLTemplateService;

public class UseToolHandler {
	private static final Logger logger2 = LoggerFactory.getLogger("ToolInfo");
	public static void useTool(Packet packet,IoSession session) {
        final int OPTYPE_ADD_POWER = 1;
        final int OPTYPE_ADD_VATALITY = 2;
        final int OPTYPE_ADD_CHALLENGE_TIMRES = 3;
        final int OPTYPE_OPEN_TREASURE = 4;
        final int OPTYPE_OPEN_GIFT_BOX = 5;
        final int OPTYPE_ADD_DISCIPLE_EXP = 6;
        final int OPTYPE_ADD_DISCIPLE_EXP_FAIL =7 ;
        
		OPUseTool params = null;
		OPUseToolRet.Builder builder = OPUseToolRet.newBuilder();
		
		try {
			params = OPUseTool.parseFrom(packet.data.array());
		} catch (InvalidProtocolBufferException e) {
			e.printStackTrace();
			return;
		}
		
		int bagItemId = params.getId();
		int num = params.getNum();
		
		int playerId = (Integer)session.getAttribute("playerid");
		Player player = ObjectAccessor.getOnlineUser(playerId);
		
		if (num <= 0) {
			OPErrorReporter.Builder eBuilder = OPErrorReporter.newBuilder();
			eBuilder.setErrorid(OP.code.OPCODE_USE_TOOLRET_S_VALUE);
			eBuilder.setErrormsg("count wrong");
			player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, eBuilder);
			return ;
		}
		logger2.info("use tool handler received: {}, {}, {}", new Object[]{playerId , bagItemId});
		long current_time = Util.getServerMillTime();
		int time = Util.getServerTime();
		HashMap<Integer, Bag> userTools = player.getBags();
		boolean use_flag=true;
		if (userTools == null || !userTools.containsKey(bagItemId)) {
			OPErrorReporter.Builder eBuilder = OPErrorReporter.newBuilder();
			eBuilder.setErrorid(OP.code.OPCODE_USE_TOOLRET_S_VALUE);
			eBuilder.setErrormsg("NO TOOL EXIST");
			player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, eBuilder);
			logger2.info("Error Reporter send: use tool handler: {}, {}, {}", new Object[]{playerId});
			return ;
		}
		
		Bag bag = userTools.get(bagItemId);
		
		if (bag.getCount() < num) {
			// 数量不够
            OPErrorReporter.Builder eBuilder = OPErrorReporter.newBuilder();
            eBuilder.setErrorid(OP.code.OPCODE_USE_TOOLRET_S_VALUE);
            eBuilder.setErrormsg("NUMBER OF TOOL IS NOT ENOUGH");
            player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, eBuilder);
    		logger2.info("Error Reporter send: use tool handler: {}, {}, {}", new Object[]{playerId});
            return ;
		}

        int toolItemType =UserToolConst.getType(bagItemId);

        switch (toolItemType)
        {
            case UserToolConst.GIFT_BOX_TOOL:
                ToolContainReward[] containTools = XMLTemplateService.getToolTemplate(bagItemId).getContainTools();
                int len = containTools.length;
                for(int i=0;i<len;i++)
                {
                    int type =UserToolConst.getType(containTools[i].getType());
                    int giftItemId = containTools[i].getItemId();
                    switch (type)
                    {
                        case UserToolConst.USER_PROPERTY_TYPE:
                            UserPropertyServices.changeProperty(player,giftItemId,containTools[i].getQuantity());
                            switch (giftItemId)
                            {
                                case UserToolConst.USER_PROPERTY_GOLD_COINS:
                                    builder.setGoldcoins(player.getSysgoldcoins()+player.getRechargegoldcoins());
                                    break;
                                case UserToolConst.USER_PROPERTY_SILVER_COINS:
                                    builder.setSilvercoins(player.getSilvercoins());
                                    break;
                                case UserToolConst.USER_PROPERTY_EXP:
                                    builder.setExp(player.getExp());
                                    break;
                                default:
                                    break;
                            }
                            break;
                        case UserToolConst.SOUL_TYPE:
                            builder.addSoulInfo(SoulServices.addSoul(player, giftItemId, containTools[i].getQuantity()));
                            break;
                        case UserToolConst.SKILL_TYPE:
                            builder.addSkillInfo(SkillServices.createSkillAlone(player,giftItemId));
                            break;
                        case UserToolConst.EQUIP_TYPE:
                            builder.addEquipInfo(EquipServices.createEquitAlone(player,giftItemId));
                            break;
                        case UserToolConst.TOOLS_TYPE:
                            builder.addToolInfo(BagToolServices.addBagTool(player,giftItemId,containTools[i].getQuantity()));
                            //添加宝箱运气池的运气值
                            if(UserToolConst.getType(giftItemId)==UserToolConst.TREASURE_TOOL)
                            {
                                BagUtil.getInstance().addLuckPool(player,giftItemId,containTools[i].getQuantity());
                            }
                            break;
                        default:
                            break;
                    }
                }
                builder.setOptype(OPTYPE_OPEN_GIFT_BOX);
                break;
            case UserToolConst.TREASURE_TOOL:
            	//初始化玩家幸运星活动的相关数据
            	boolean needClear=Util.initLuckyStarActivit(current_time, player);
            	if(needClear){
            		 LuckyStarXmlTemplate luckyStarXmlTemplate1=XMLTemplateService.getLuckyStarXmlTemplate(1);
            	     LuckyStarXmlTemplate luckyStarXmlTemplate2=XMLTemplateService.getLuckyStarXmlTemplate(2);
            	     Bag bag1=userTools.get(luckyStarXmlTemplate1.getItemId());
            	     Bag bag2=userTools.get(luckyStarXmlTemplate2.getItemId());
            	     if(bag1!=null){
            	    	 builder.addToolInfo(BagToolServices.buildToolBuilder(bag1));
            	     }
            	     if(bag2!=null){
            	    	 builder.addToolInfo(BagToolServices.buildToolBuilder(bag2));
            	     }
            	}
                int relativeToolId = UserToolConst.treasureMap.get(bagItemId);
                if(num>1 && player.getViplevel()<5){
                	OPErrorReporter.Builder eBuilder = OPErrorReporter.newBuilder();
                	eBuilder.setErrorid(OP.code.OPCODE_USE_TOOLRET_S_VALUE);
                	eBuilder.setErrormsg("viplevel is not enough");
                	player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, eBuilder);
                	logger2.info("Error Reporter send: use tool handler : {}, {}, {}", new Object[]{playerId});
                	return ;
                }
                //如果接收到的开宝箱数量大于100
                if(num>100){
                	OPErrorReporter.Builder eBuilder = OPErrorReporter.newBuilder();
                	eBuilder.setErrorid(OP.code.OPCODE_USE_TOOLRET_S_VALUE);
                	eBuilder.setErrormsg("opentimes out of bound");
                	player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, eBuilder);
                	logger2.info("Error Reporter send: use tool handler : {}, {}, {}", new Object[]{playerId});
                	return ;
                }
                for(int i=1 ; i<=num ; i++){
                	if(!userTools.containsKey(relativeToolId))
                	{
                		if(i==1){
                			// 没有相对应的宝箱或钥匙
                			OPErrorReporter.Builder eBuilder = OPErrorReporter.newBuilder();
                			eBuilder.setErrorid(OP.code.OPCODE_USE_TOOLRET_S_VALUE);
                			eBuilder.setErrormsg("RELATIVE TOOL NOT EXIST");
                			player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, eBuilder);
                			logger2.info("Error Reporter send: use tool handler : {}, {}, {}", new Object[]{playerId});
                			return ;
                		}
                		else{
                			num=i-1;
                			break;
                		}
                			
                	}else if(userTools.get(relativeToolId).getCount()<1 || userTools.get(bagItemId).getCount()<1)
                	{
                		if( i==1 ){
                			// 对应的宝箱或钥匙数量不够
                			OPErrorReporter.Builder eBuilder = OPErrorReporter.newBuilder();
                			eBuilder.setErrorid(OP.code.OPCODE_USE_TOOLRET_S_VALUE);
                			eBuilder.setErrormsg("NUMBER OF RELATIVE TOOL IS NOT ENOUGH");
                			player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, eBuilder);
                			logger2.info("Error Reporter send: use tool handler : {}, {}, {}", new Object[]{playerId});
                			return ;
                		}
                		else{
                			num=i-1;
                			break;
                		}
                	}
                	openTreasure(player,bagItemId,builder);//开宝箱根据概率得奖励
                	Util.decreaseBagTool(player,relativeToolId,1);
                	Util.decreaseBagTool(player,bagItemId,1);
                	use_flag=false;
                }
                builder.setOptype(OPTYPE_OPEN_TREASURE);
                break;
            case UserToolConst.RECOVERY_TOOL:
                int dayLimit = UserToolConst.vipLvToolLimit.get(player.getViplevel());
                switch (bagItemId)
                {
                    case UserToolConst.HAIXIANFAN:
                        if (player.getTodaychickennum() >= dayLimit) {
                            // 判断是否今日已不能使用海鲜饭
                            OPErrorReporter.Builder eBuilder = OPErrorReporter.newBuilder();
                            eBuilder.setErrorid(OP.code.OPCODE_USE_TOOLRET_S_VALUE);
                            eBuilder.setErrormsg("CANT EAT ANY MORE");
                            player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, eBuilder);
                            logger2.info("Error Reporter send: use tool handler : {}, {}, {}", new Object[]{playerId});
                            return ;
                        }
                        else {
                            player.setPowerbychicken(player.getPowerbychicken() + 12);
                            player.setTodaychickennum(player.getTodaychickennum() + 1);
                            player.setEatchickentime(time);

                            // builder里放回包内容
                            builder.setPower(player.getPowerbychicken()+player.getPowerbytime());
                            builder.setOptype(OPTYPE_ADD_POWER);
                        }
                        break;
                    case UserToolConst.JISUZHEN:
                        if (player.getTodaypelletnum() >= dayLimit) {
                            // 判断是否今日已不能使用海鲜饭
                            OPErrorReporter.Builder eBuilder = OPErrorReporter.newBuilder();
                            eBuilder.setErrorid(OP.code.OPCODE_USE_TOOLRET_S_VALUE);
                            eBuilder.setErrormsg("CANT EAT ANY MORE");
                            player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, eBuilder);
                            logger2.info("Error Reporter send: use tool handler : {}, {}, {}", new Object[]{playerId});
                            return ;
                        }
                        else {
                            player.setVitalitybypellet(player.getVitalitybypellet() + 12);
                            player.setTodaypelletnum(player.getTodaypelletnum() + 1);
                            player.setEatpellettime(time);
                            builder.setVitality(player.getVitalitybypellet()+player.getVitalitybytime());
                        }
                        builder.setOptype(OPTYPE_ADD_VATALITY);
                        break;
                    case UserToolConst.TIAOZHANSHU:
                    	int currentCount = player.getUserStats().getChallengetimes();
                    	int countLimit = Util.getChallengeCountLimitByVipLevel(player.getViplevel());
                    	int newCount = currentCount + 3;
                    	newCount = newCount > countLimit?countLimit:newCount;
                    	player.getUserStats().setChallengetimes(newCount);
                        builder.setOptype(OPTYPE_ADD_CHALLENGE_TIMRES);
                        builder.setChallengetimes(newCount);
                        break;

                }
                break;
            case UserToolConst.UPGRADE_EQUIPMENT_TOOL:break;
            case UserToolConst.UPGRADE_DISCIPLE_TOOL:break;
            case UserToolConst.OTHER_TOOL:break;
            case UserToolConst.DISCIPLE_EXP:
            	switch(bagItemId)
            	{
            		case UserToolConst.PUTAOJIU:
            			use_flag =addAllDiscipleExp(player, 2000, builder);
            			if(use_flag){
            				builder.setOptype(OPTYPE_ADD_DISCIPLE_EXP);
            			}
            			else{
            				builder.setOptype(OPTYPE_ADD_DISCIPLE_EXP_FAIL);
            			}
            			break;
            		case UserToolConst.LANGMUJIU:
            			use_flag=addLowestDiscipleExp(player, 2000, builder);
            			if(use_flag){
            				builder.setOptype(OPTYPE_ADD_DISCIPLE_EXP);
            			}
            			else{
            				builder.setOptype(OPTYPE_ADD_DISCIPLE_EXP_FAIL);
            			}
            			break;
            		
            	}
            	break;
            default:
                OPErrorReporter.Builder eBuilder = OPErrorReporter.newBuilder();
                eBuilder.setErrorid(OP.code.OPCODE_USE_TOOLRET_S_VALUE);
                eBuilder.setErrormsg("WRONG TYPE OF TOOL IS USED!");
                player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, eBuilder);
                logger2.info("Error Reporter send: use tool handler : {}, {}, {}", new Object[]{playerId});
                break;

        }
        player.setLastVerifyTime(time);
        if(use_flag){
        	Util.decreaseBagTool(player,bagItemId,num);
        }
		player.sendPacket(OP.code.OPCODE_USE_TOOLRET_S_VALUE, builder);
		long current_time_2 = Util.getServerMillTime();
		long taken_time=current_time_2-current_time;
		logger2.info("use tool handler ret send: {}, {}, {}", new Object[]{playerId ,taken_time});
	}
	
	
    private static void openTreasure(Player player,int toolItemId,OPUseToolRet.Builder builder)
    {
        int treasureType = getTreasuretype(toolItemId);
        UserStats userStats = player.getUserStats();
        int addLuck = 0;
        switch (treasureType)
        {
            case UserToolConst.D_QUALITY:  //木
                addLuck = 5;
                break;
            case UserToolConst.C_QUALITY:  //铜
                addLuck = 10;
                break;
            case UserToolConst.B_QUALITY:  //银
                addLuck = 20;
                break;
            case UserToolConst.A_QUALITY: //金
                addLuck = 30;
                break;
            default:
                break;
        }
        
        /*************************************进行幸运星的相关判断******************************************/
        int termId=Util.getActivityTermId(Util.getServerMillTime(), XMLTemplateService.getluckyStarTimeConfigs());
        int luckyStar_type=0;
        LuckyStarXmlTemplate luckyStarXmlTemplate1=XMLTemplateService.getLuckyStarXmlTemplate(1);
        LuckyStarXmlTemplate luckyStarXmlTemplate2=XMLTemplateService.getLuckyStarXmlTemplate(2);
        if(termId!=0){
			//如果玩家开的金宝箱,而且背包中有武道神兵幸运星且数量大于0时,进入相应的抽取类型,并对heap1进行一次累积
			if(treasureType==UserToolConst.A_QUALITY && BagUtil.getToolCountByToolId(player, luckyStarXmlTemplate1.getItemId())>0 )
			{
				luckyStar_type=luckyStarXmlTemplate1.getId();
				treasureType=luckyStarXmlTemplate1.getTreasureType();
				userStats.setLuckyStayHeap1(userStats.getLuckyStayHeap1()+1);
				
			}
			//如果玩家开的金宝箱,而且背包中有黄金神兵幸运星且数量大于0时,进入相应的抽取类型
			if(treasureType==UserToolConst.B_QUALITY && BagUtil.getToolCountByToolId(player, luckyStarXmlTemplate2.getItemId())>0)
			{
				luckyStar_type=luckyStarXmlTemplate2.getId();
				treasureType=luckyStarXmlTemplate2.getTreasureType();
				userStats.setLuckyStayHeap2(userStats.getLuckyStayHeap2()+1);
			}
			
        }
        logger2.info("lucky star type: {}, {}, {}, {}, {}, {}", new Object[]{ treasureType , userStats.getLuckyStarTermid() , userStats.getLuckyStayHeap1() , userStats.getLuckyStayHeap2(),userStats.getLuckyStarDropTimes1(),userStats.getLuckyStarDropTimes2()});
        /***************************************进行幸运星的相关判断****************************************/
        TreasureSelecter selecter = XMLTemplateService.getTreasureSelecter(treasureType);
        List<Float> rateList;
        double randomNum = Math.random();
        int rewardsPoolNo = 1;
        logger2.info("-------------------->TreasureInfo    ReadyGo     :LuckyOne ---{}---,LuckyTwo ---{}---, treasureWorth---{}---,playerLevel---{}---,LuckPool---{}----", new Object[]{userStats.getLuckOne(),userStats.getLuckTwo(),userStats.getTreasureWorth(),player.getLevel(),userStats.getLuckPool()});
        if(userStats.getLuckPool()>0)
        {
            float remainLuck = userStats.getLuckPool()-addLuck;
            if(remainLuck<0)
            {
            	addLuck = (int)StrictMath.ceil(userStats.getLuckPool());
                userStats.setLuckPool(0.0f);
            }else
            {
                userStats.setLuckPool(remainLuck);
            }
        }
        else
        {
            addLuck =  (int)StrictMath.ceil(addLuck*XMLTemplateService.getSysBasicConfig().getEmptyLuckPoolProportion());
        }
        if(userStats.getLuckTwo()>=XMLTemplateService.getSysBasicConfig().getTreasurePoolNeed().get(4)){
              //奖池4抽奖
            rateList = selecter.getRate4();
            userStats.setLuckTwo(0);
            rewardsPoolNo = 4;
        }
        else{
        	if (player.getLevel()>=20){
        		if(userStats.getLuckOne()>=XMLTemplateService.getSysBasicConfig().getTreasurePoolNeed().get(3)){
        			//奖池3抽奖
        			rateList = selecter.getRate3();
        			rewardsPoolNo = 3;
        		}else{
                //奖池1抽奖
                rateList = selecter.getRate1();
                rewardsPoolNo = 1;
        		}
        	}
        	else{
        		if(userStats.getLuckOne()>=XMLTemplateService.getSysBasicConfig().getTreasurePoolNeed().get(2)){
        			//奖池2抽奖
        			rateList = selecter.getRate2();
        			rewardsPoolNo = 2;
        		}else{
        			//奖池1抽奖
        			rateList = selecter.getRate1();
        			rewardsPoolNo = 1;
        		}
        	}
        }
        logger2.info("---------------------->RewardPoolNo:----{}----", new Object[]{rewardsPoolNo});
        int listLen = rateList.size();
        int selectedNo=0;
        for(int i=0;i<listLen;i++){
            if(randomNum<rateList.get(i)){
                selectedNo = i;
                break;
            }
        }
        // 增加或者清空幸运值及累计额度
        if(rewardsPoolNo==4){
            userStats.setTreasureWorth(0);
            if(player.getLevel()>=20){
                if(userStats.getLuckOne()<XMLTemplateService.getSysBasicConfig().getTreasurePoolNeed().get(3)){
                    userStats.setLuckOne(userStats.getLuckOne()+addLuck);
                }
            }else{
            	if(userStats.getLuckOne()<XMLTemplateService.getSysBasicConfig().getTreasurePoolNeed().get(2)){
            		userStats.setLuckOne(userStats.getLuckOne()+addLuck);
            	}
            }
        } else {
            if(rewardsPoolNo>1){
                int totalWorth = userStats.getTreasureWorth()+selecter.getWorth().get(selectedNo);
                if(player.getLevel()>=20){
                	if(totalWorth>=XMLTemplateService.getSysBasicConfig().getTreasureClearCount().get(1)){
                		totalWorth = 0;
                		userStats.setLuckOne(0);
                	}
                }else{
            		if(totalWorth>=XMLTemplateService.getSysBasicConfig().getTreasureClearCount().get(0)){
            			totalWorth = 0;
            			userStats.setLuckOne(0);
            		}
            	}
            	userStats.setTreasureWorth(totalWorth);
        	}else{
        		userStats.setLuckOne(userStats.getLuckOne()+addLuck);
        	}
        	userStats.setLuckTwo(userStats.getLuckTwo()+addLuck);
    	}
        
        player.setUserStats(userStats);
        logger2.info("-------------------->TreasureInfo     After    :LuckyOne ---{}---,LuckyTwo ---{}---, treasureWorth---{}---,playerLevel---{}---,luckPool---{}---", new Object[]{userStats.getLuckOne(),userStats.getLuckTwo(),userStats.getTreasureWorth(),player.getLevel(),userStats.getLuckPool()});

    	int propId = selecter.getPropId().get(selectedNo);
    	int itemType = selecter.getItemType().get(selectedNo);
    	int count = selecter.getCount().get(selectedNo);
    	int balanceId=0;
    	//如果活动开启,进行保底判定
    	if(termId!=0){
    		//如果达到武道幸运星的保底次数,则固定获得指定的物品
    		if(luckyStar_type==luckyStarXmlTemplate1.getId() && userStats.getLuckyStayHeap1()>=luckyStarXmlTemplate1.getBalanceTimes()){
    			propId=0;
    			itemType=luckyStarXmlTemplate1.getBalanceType();
    			count=1;
    			balanceId=luckyStarXmlTemplate1.getBalanceId();
    		}
    		//如果达到黄金幸运星的保底次数,则固定获得指定的物品
    		if(luckyStar_type==luckyStarXmlTemplate2.getId() && userStats.getLuckyStayHeap2()>=luckyStarXmlTemplate2.getBalanceTimes()){
    			propId=0;
    			itemType=luckyStarXmlTemplate2.getBalanceType();
    			count=1;
    			balanceId=luckyStarXmlTemplate2.getBalanceId();
    		}
    	}
    	if(propId!=0){ //直接给予道具和用户属性奖励
    		switch (itemType){
            	case UserToolConst.USER_PROPERTY:
                    if(propId==UserToolConst.USER_PROPERTY_GOLD_COINS)
                    {
                        UserPropertyServices.changeProperty(player,propId,count);
                        builder.setGoldcoins(player.getSysgoldcoins()+player.getRechargegoldcoins());
                    }
                    break;
                case UserToolConst.USER_TOOLS:
                    builder.addToolInfo(BagToolServices.addBagTool(player,propId,count));
                    //添加宝箱运气池的运气值
                    if(UserToolConst.getType(propId)==UserToolConst.TREASURE_TOOL)
                    {
                        BagUtil.getInstance().addLuckPool(player,propId,count);
                    }
                    break;
                case UserToolConst.USER_LEVEL_PROPERTY:
                    if(propId==UserToolConst.USER_LEVEL_PROPERTY_SILVER_COINS){
                        // 根据等级获取银两数
                        int lvSilvers = XMLTemplateService.getUserLevelConfig(player.getLevel()).getCoins1();
                        count=lvSilvers*selecter.getCount().get(selectedNo);
                        UserPropertyServices.changeProperty(player,propId,count);
                        builder.setSilvercoins(player.getSilvercoins());
                    }
                    break;
                default:
                    break;
    		}
    	} else {//从技能奖池或者装备奖池给予奖励
    		int type=UserToolConst.getType(itemType);
    		if(type==UserToolConst.SKILL_TYPE){
    			int quality = 0;
    			switch (itemType){
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
    			List<Integer> skillRateList;
    			switch (rewardsPoolNo){
                   case 1:
                       skillRateList = skillSelecter.getRate1();
                       break;
                   case 2:
                       skillRateList = skillSelecter.getRate2();
                       break;
                   case 3:
                       skillRateList = skillSelecter.getRate3();
                       break;
                   case 4:
                       skillRateList = skillSelecter.getRate4();
                       break;
                   default:
                       skillRateList =  skillSelecter.getRate1();
                       break;
               }
               double skillRandom = Math.random()*(skillRateList.get(skillRateList.size()-1));
               int skillLen = skillRateList.size();
               int skillSelectedNo=0;
               for(int i=0;i<skillLen;i++){
                   if(skillRandom<skillRateList.get(i)){
                       skillSelectedNo = i;
                       break;
                   }
               }
               int addSkillId = skillSelecter.getId().get(skillSelectedNo);
               //如果在在幸运星活动期间
               if(termId!=0){
            	   //达到保底条件,获得指定物品
            	   if(balanceId!=0){
            		   addSkillId=balanceId;
            	   }
            	   //进行武道神兵幸运星加成的抽取,且抽中指定类型指定id的物品,则清除相应的抽取次数积累
            	   if(luckyStar_type==luckyStarXmlTemplate1.getId() && addSkillId==luckyStarXmlTemplate1.getBalanceId()&& itemType==luckyStarXmlTemplate1.getBalanceType()){
            		   if(addSkillId==luckyStarXmlTemplate1.getBalanceId()&& itemType==luckyStarXmlTemplate1.getBalanceType()){
            			   userStats.setLuckyStayHeap1(0);
            			   
            		   }
            	   }
            	   //进行黄金神兵幸运星加成的抽取,且抽中指定类型指定id的物品,则清除相应的抽取次数积累
            	   if(luckyStar_type==luckyStarXmlTemplate2.getId() && addSkillId==luckyStarXmlTemplate2.getBalanceId()&& itemType==luckyStarXmlTemplate2.getBalanceType()){
            		   if(addSkillId==luckyStarXmlTemplate2.getBalanceId()&& itemType==luckyStarXmlTemplate2.getBalanceType()){
            			   userStats.setLuckyStayHeap2(0);
            		   }
            	   }
               }
               propId = addSkillId;
               if (itemType == UserToolConst.A_SKILL) {//添加系统广播
            	   ArrayList<Integer> params = new ArrayList<Integer>(2);
            	   params.add(2);
            	   params.add(addSkillId);
            	   BoardCastInfoHandler.addBoardCase(BoardCastInfoHandler.OPENCASE_type, player.getName(), params);
               }
               builder.addSkillInfo(SkillServices.createSkillAlone(player,addSkillId));
    		}
    		else if(type==UserToolConst.EQUIP_TYPE){
    			EquipSelecter equipSelecter = XMLTemplateService.getEquitSelecter(itemType);
    			List<Integer> equipRateList;
                switch (rewardsPoolNo){
                	case 1:
                		equipRateList = equipSelecter.getRate1();
	                    break;
	                case 2:
	                    equipRateList = equipSelecter.getRate2();
	                    break;
	                case 3:
	                    equipRateList = equipSelecter.getRate3();
	                    break;
	                case 4:
	                    equipRateList = equipSelecter.getRate4();
	                    break;
	                default:
                        equipRateList = equipSelecter.getRate1();
                        break;
               }
               double equipRandom = Math.random()*(equipRateList.get(equipRateList.size()-1));
               int equipLen = equipRateList.size();
               int equipSelectedNo=0;
               for(int i=0;i<equipLen;i++){
                   if(equipRandom<equipRateList.get(i))
                   {
                       equipSelectedNo = i;
                       break;
                   }
               }
               int equipId = equipSelecter.getId().get(equipSelectedNo);
             //如果在在幸运星活动期间
               if(termId!=0){
            	   //达到保底条件,获得指定物品
            	   if(balanceId!=0){
            		   equipId=balanceId;
            	   }
            	   HashMap<Integer, Bag> bags=player.getBags();
            	   //进行武道神兵幸运星加成的抽取,且抽中指定类型指定id的物品,则清除相应的抽取次数积累
            	   if(luckyStar_type==luckyStarXmlTemplate1.getId() && equipId==luckyStarXmlTemplate1.getBalanceId()&& itemType==luckyStarXmlTemplate1.getBalanceType()){
            		   if(equipId==luckyStarXmlTemplate1.getBalanceId()&& itemType==luckyStarXmlTemplate1.getBalanceType()){
            			   userStats.setLuckyStayHeap1(0);
            			   Util.decreaseBagTool(player,luckyStarXmlTemplate1.getItemId(),1);
            			   builder.addToolInfo(BagToolServices.buildToolBuilder(bags.get(luckyStarXmlTemplate1.getItemId())));
            		   }
            	   }
            	   //进行黄金神兵幸运星加成的抽取,且抽中指定类型指定id的物品,则清除相应的抽取次数积累
            	   if(luckyStar_type==luckyStarXmlTemplate2.getId() && equipId==luckyStarXmlTemplate2.getBalanceId()&& itemType==luckyStarXmlTemplate2.getBalanceType()){
            		   if(equipId==luckyStarXmlTemplate2.getBalanceId()&& itemType==luckyStarXmlTemplate2.getBalanceType()){
            			   userStats.setLuckyStayHeap2(0);
            			   Util.decreaseBagTool(player,luckyStarXmlTemplate2.getItemId(),1);
            			   builder.addToolInfo(BagToolServices.buildToolBuilder(bags.get(luckyStarXmlTemplate2.getItemId())));
            		   }
            	   }
               }
               propId = equipId;
               builder.addEquipInfo(EquipServices.createEquitAlone(player,equipId));
              //添加广播信息
               if (itemType == UserToolConst.A_ARMOR || itemType == UserToolConst.A_WEAPON || itemType == UserToolConst.A_ACCESSORIES) {
            	   ArrayList<Integer> params = new ArrayList<Integer>(2);
            	   params.add(1);
            	   params.add(equipId);
            	   BoardCastInfoHandler.addBoardCase(BoardCastInfoHandler.OPENCASE_type, player.getName(), params);
               }
    		}
    	}
    	player.setUserStats(userStats);
    	//连开宝箱奖励回包
        List<OPRewardInfoRet.RewardInfo.Builder> rewardBuilderList=builder.getRewardBuilderList();
        boolean has_reward=false;
        if(rewardBuilderList==null && rewardBuilderList.size()==0){
        	has_reward=false;
        }
        else{
	        for(int i=0;i<rewardBuilderList.size();i++){
	        	OPRewardInfoRet.RewardInfo.Builder reward_builder=rewardBuilderList.get(i);
	        	int item_type=reward_builder.getItemType();
	        	int item_id=reward_builder.getItemId();
	        	int item_count=reward_builder.getCount();
	        	if(itemType==item_type && propId==item_id){
	        		has_reward=true;
	        		count+=item_count;
	        		builder.removeReward(i);
	        		builder.addReward(Util.getRewardInfoBuilder(itemType,propId,count));
	        		break;
	        	}
	        }
        }
        if(!has_reward){
        	builder.addReward(Util.getRewardInfoBuilder(itemType,propId,count));
        	logger2.info("lucky star rewardInfo: {}, {}, {}", new Object[]{ itemType,propId,count });
        }
    }
    
    private static int getTreasuretype(int toolId)
    {
         if(toolId==UserToolConst.WOOD_TREASURE||toolId==UserToolConst.WOOD_KEY)
         {
             return UserToolConst.D_QUALITY;
         }else if(toolId==UserToolConst.COPPER_TREASURE||toolId==UserToolConst.COPPER_KEY)
         {
             return UserToolConst.C_QUALITY;
         }else if(toolId==UserToolConst.SILVER_TREASURE||toolId==UserToolConst.SILVER_KEY)
         {
             return UserToolConst.B_QUALITY;
         }else if(toolId==UserToolConst.GOLD_TREASURE||toolId==UserToolConst.GOLD_KEY)
         {
             return UserToolConst.A_QUALITY;
         }
        return 0;
    }
	//增加所有上阵弟子经验
	private static boolean addAllDiscipleExp(Player player, int amount , OPUseToolRet.Builder builder) {
		int sum_amount=0;
		int level_max = player.getLevel() * 3;//船员等级不能超过船长等级3倍
		BattleArrayObject userBattleInfo = BattleUtil.getUserBattleArrayObject(player);
		ArrayList<Battle> userBattles = userBattleInfo.getBattlearrays();
		for (int i = 0; i < userBattles.size(); i++) {
			Battle battle = userBattles.get(i);
			if (battle.getDiscipleid() == 0) {
				continue;
			}
			Disciple disciple = player.getDisciples().get(battle.getDiscipleid());
			int itemID = disciple.getItemid();
			DiscipleXmlTemplate discipleInfo = XMLTemplateService.getDiscipleTemplate(itemID);
			int exp_limited=(int) (discipleInfo.getDefaultExp()*XMLTemplateService.getDiscipleLevelConfig(level_max - 1));
			amount=((disciple.getExp()+amount)>exp_limited)?(exp_limited-disciple.getExp()):amount;
			if(amount>0){
				int addLevel=Util.getlevelByExp(disciple, amount);
				if (addLevel > 0) {//弟子升级
					BattleUtil.resetBattleProperty(battle, disciple,player,null,0);
					OPUserBattleArrayRet.BattleArrayInfo.Builder battleBuilder = OPUserBattleArrayRet.BattleArrayInfo.newBuilder();
					BattleUtil.buildBattleBuilder(battleBuilder, battle, i);
					builder.addBattleArrayInfo(battleBuilder);
					player.updateOneBattle(battle);
				}
				OPUserDiscipleInfoRet.DiscipleInfo.Builder discipleBuilder = DiscipleUtil.buildDiscipleBuilder(disciple);
				builder.addDiscipleInfo(discipleBuilder);
			}
			sum_amount+=amount;
		}
		if(sum_amount>0){
			return true;
		}else {
			return false;
		}
	}
	private static boolean addLowestDiscipleExp(Player player, int amount , OPUseToolRet.Builder builder){
		/*************************************************************************/
		BattleArrayObject userBattleInfo = BattleUtil.getUserBattleArrayObject(player);
		ArrayList<Battle> userBattles = userBattleInfo.getBattlearrays();
		int level_max = player.getLevel() * 3;//船员等级不能超过船长等级3倍
		int lowest_level = level_max;
		int position = -1;
		Battle lowest_battel=null;
		Disciple lowest_disciple=null;
		for (int i = 0; i < userBattles.size(); i++) {
			Battle battle = userBattles.get(i);
			if (battle.getDiscipleid() == 0) {
				continue;
			}
			Disciple disciple = player.getDisciples().get(battle.getDiscipleid());
			if(disciple.getLevel()<lowest_level){
				lowest_battel=battle;
				lowest_disciple=disciple;
				lowest_level=disciple.getLevel();
				position = i;
			}
			
		}
		if ( position == - 1 ) {
			return false;
		}
		//if(lowest_disciple.getLevel()==XMLTemplateService.getDiscipleLevelCount()){
			//int level_limited=XMLTemplateService.getDiscipleLevelCount();
			int itemID = lowest_disciple.getItemid();
			DiscipleXmlTemplate discipleInfo = XMLTemplateService.getDiscipleTemplate(itemID);
			int exp_limited=(int) (discipleInfo.getDefaultExp()*XMLTemplateService.getDiscipleLevelConfig(level_max - 1));
			amount=((lowest_disciple.getExp()+amount)>exp_limited)?(exp_limited-lowest_disciple.getExp()):amount;
		//}
		if(amount>0)
		{
			int addLevel=Util.getlevelByExp(lowest_disciple, amount);
			if (addLevel > 0) {//弟子升级
				BattleUtil.resetBattleProperty(lowest_battel, lowest_disciple,player,null,0);
				OPUserBattleArrayRet.BattleArrayInfo.Builder battleBuilder = OPUserBattleArrayRet.BattleArrayInfo.newBuilder();
				BattleUtil.buildBattleBuilder(battleBuilder, lowest_battel, position);
				builder.addBattleArrayInfo(battleBuilder);
				player.updateOneBattle(lowest_battel);
			}
			OPUserDiscipleInfoRet.DiscipleInfo.Builder discipleBuilder = DiscipleUtil.buildDiscipleBuilder(lowest_disciple);
			builder.addDiscipleInfo(discipleBuilder);
			return true;
		}
		else{
			return false;
		}
		/*************************************************************************/
	}
}

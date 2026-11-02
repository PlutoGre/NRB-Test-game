package com.oldmountain.dzm.packethandler;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import org.apache.mina.common.IoSession;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import com.google.protobuf.InvalidProtocolBufferException;
import com.oldmountain.dzm.entity.Disciple;
import com.oldmountain.dzm.entity.DiscipleXmlTemplate;
import com.oldmountain.dzm.entity.KindSelecter;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.entity.Skill;
import com.oldmountain.dzm.entity.Soul;
import com.oldmountain.dzm.entity.UserStats;
import com.oldmountain.dzm.message.OP;
import com.oldmountain.dzm.message.ErrorReporter.OPErrorReporter;
import com.oldmountain.dzm.message.MarketRecruitDisciple.OPMarketRecruitDisciple;
import com.oldmountain.dzm.message.MarketRecruitDisciple.OPMarketRecruitDiscipleRet;
import com.oldmountain.dzm.message.UserDiscipleInfo.OPUserDiscipleInfoRet;
import com.oldmountain.dzm.message.UserMarketInfo.OPUserMarketInfoRet;
import com.oldmountain.dzm.message.UserSkillInfo.OPUserSkillInfoRet;
import com.oldmountain.dzm.message.UserSoulInfo.OPUserSoulInfoRet;
import com.oldmountain.dzm.net.ObjectAccessor;
import com.oldmountain.dzm.net.packet.Packet;
import com.oldmountain.dzm.util.DiscipleUtil;
import com.oldmountain.dzm.util.SkillUtil;
import com.oldmountain.dzm.util.SoulUtil;
import com.oldmountain.dzm.util.Util;
import com.oldmountain.dzm.util.XMLTemplateService;

/**
 * 集市中招募弟子操作
 * @author dragon
 *
 */
public class MarketRecruitDiscipleHandler {
	private static final Logger logger2 = LoggerFactory.getLogger("MarketInfo");
	private static final Logger logger_goldCost = LoggerFactory.getLogger("GoldCost");
	public static void recruitDisciple(Packet packet,IoSession session){
		OPMarketRecruitDisciple params = null;
		OPMarketRecruitDiscipleRet.Builder builder = OPMarketRecruitDiscipleRet.newBuilder();
		
		try {
			params = OPMarketRecruitDisciple.parseFrom(packet.data.array());
		} catch (InvalidProtocolBufferException e) {
			e.printStackTrace();
			return;
		}
		//type标示为普通招募或者花金元宝立即招募,type=1为普通招募,type=2为元宝招募
		int type = params.getType();
		//kind标示为十里（1），百里（2），还是万里（3）
		int kind = params.getKind();
		int playerId = (Integer)session.getAttribute("playerid");
		Player player = ObjectAccessor.getOnlineUser(playerId);
		int id=player.getId();
		logger2.info("market recruit disciple handler received: {}, {}, {}", new Object[]{id , type , kind});
		long start_time = Util.getServerMillTime();
		int current_time = Util.getServerTime();
		//获取送魂魄活动开启情况
		int itermID=Util.getGiveSoulTermId(start_time);
		
		
		UserStats userStats = player.getUserStats();
		int poolId = 2;
		int addLucky = 0;
		switch (type) {
			case 1://普通招募
				int lastTime = 0;
				int coolTime = 0;
				switch (kind){
					case 1://十里
						lastTime = (player.getTodayleavetimes())>0?player.getTentime():current_time;
						coolTime = XMLTemplateService.getSysBasicConfig().getTenCoolTime();
						if (player.getTodayleavetimes() <= 0) {
							break;
						}
						break;
					case 2://百里
						lastTime = player.getHundredtime();
						addLucky = 100;
						coolTime = XMLTemplateService.getSysBasicConfig().getHundredCoolTime();
						break;
					case 3://万里
						lastTime = player.getWanlitime();
						addLucky = 300;
						coolTime = XMLTemplateService.getSysBasicConfig().getWanliCoolTime();
						break;
				}
				if (lastTime + coolTime > current_time) {//冷却时间未到，还不可以招募弟子
					OPErrorReporter.Builder errorBuilder = OPErrorReporter.newBuilder();
					errorBuilder.setErrorid(OP.code.OPCODE_MARKET_RECRUIT_DISCIPLERET_S_VALUE);
					errorBuilder.setErrormsg("Still in cooling");
					player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errorBuilder);
					logger2.info("Error Reporter: market recruit disciple handler send: {}, {}, {}", new Object[]{id});
					return;
				}
				switch (kind) {
				case 1:
					player.setTentime(current_time);
					player.setTodayleavetimes(player.getTodayleavetimes() - 1);
					break;
				case 2:
					player.setHundredtime(current_time);
					break;
				case 3:
					player.setWanlitime(current_time);
					break;
				}
				break;
			case 2://元宝招募
				int userGold = player.getSysgoldcoins() + player.getRechargegoldcoins();
				int cost = 0;
				int costId=0;
				switch (kind) {
					case 1:
						cost = XMLTemplateService.getSysBasicConfig().getTenCostGold();
						costId=1007;
						break;
					case 2:
						addLucky = 100;
						if (userStats.getFirstbaitime() == 0) {
							poolId = 1;
							addLucky = 0;
						}
						cost = XMLTemplateService.getSysBasicConfig().getHundredCostGold();
						costId=1008;
						break;
					case 3:
						addLucky = 300;
						if (userStats.getFirstwantime() == 0) {
							addLucky = 0;
							poolId = 1;
						}
						cost = XMLTemplateService.getSysBasicConfig().getWanliCostGold();
						costId=1009;
						break;
				}
				if (userGold < cost) {//用户的元宝数不够
					OPErrorReporter.Builder errorBuilder = OPErrorReporter.newBuilder();
					errorBuilder.setErrorid(OP.code.OPCODE_MARKET_RECRUIT_DISCIPLERET_S_VALUE);
					errorBuilder.setErrormsg("Not enough gold");
					player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errorBuilder);
					logger2.info("Error Reporter: market recruit disciple handler send: {}, {}, {}", new Object[]{id});
					return ;
				}
				else {
					switch (kind) {
					case 2:
						if (userStats.getFirstbaitime() == 0) {
							userStats.setFirstbaitime(current_time);
							player.setUserStats(userStats);
						}
						break;
					case 3:
						if (userStats.getFirstwantime() == 0) {
							userStats.setFirstwantime(current_time);
							player.setUserStats(userStats);
						}
						break;
					}
					
					player.consumeGold(cost);
					
//					if (player.getSysgoldcoins() >= cost) {
//						player.setSysgoldcoins(player.getSysgoldcoins() - cost);
//					}
//					else {
//						player.setRechargegoldcoins(player.getRechargegoldcoins() - cost + player.getSysgoldcoins());
//						player.setSysgoldcoins(0);
//					}
					logger_goldCost.info("Gold Cost Info: {} , {} , {} , {} , {}, {} , {}",new Object[]{ Util.getServerTime() , player.getId() , costId , 1 , userGold , player.getGoldCoins() , cost});
				}
				break;
		}
		
		int itemid = 0;
		int hunpoid = 0;
		KindSelecter kindSelecter = XMLTemplateService.getKindSelecter(kind);
		
		int poolLimit = XMLTemplateService.getSysBasicConfig().getPoolLimits().get(userStats.getNextDisciple());
		if (userStats.getDiscipleLucky() + addLucky >= poolLimit && poolId != 1) {
			poolId = userStats.getNextDisciple();
		}
		
		List<Integer> randKinds = kindSelecter.getKinds();
		List<Integer> randRates = getRandRates(kindSelecter, poolId);
		
		
		int randIndex = Util.getRandResult(randRates);
		
		int randResult = randKinds.get(randIndex);
		int isHunpo = randResult/1000%100;//31是弟子，32是魂魄
		int discipleType = randResult%10;//1甲，2乙，3丙，4丁
		
		KindSelecter discipleSelecter = new KindSelecter();
		discipleSelecter = XMLTemplateService.getDiscipleSelecter(discipleType);
		
		List<Integer> discipleIds = discipleSelecter.getKinds();
		List<Integer> discipleRates = getRandRates(discipleSelecter, poolId);
		List<Integer> userHasDisciple = player.getDiscipleitems();
		
		if (isHunpo == 31) {//抽到的是弟子
			int index = Util.getRandResult(discipleRates);
			int discipleId = discipleIds.get(index);
			if (userHasDisciple.contains(discipleId)) {
				hunpoid = discipleId;
			}
			else {
				itemid = discipleId;
			}
		}
		else {//抽到的是魂魄
			ArrayList<Integer> hasAQuality = new ArrayList<Integer>();
			ArrayList<Integer> hasBQuality = new ArrayList<Integer>();
			ArrayList<Integer> hasCQuality = new ArrayList<Integer>();
			ArrayList<Integer> hasDQuality = new ArrayList<Integer>();
			for (int i = 0; i < userHasDisciple.size(); i++) {
				DiscipleXmlTemplate discipleInfo = XMLTemplateService.getDiscipleTemplate(userHasDisciple.get(i));
				switch (discipleInfo.getQuality()) {
					case 1:
						hasAQuality.add(userHasDisciple.get(i));
						break;
					case 2:
						hasBQuality.add(userHasDisciple.get(i));
						break;
					case 3:
						hasCQuality.add(userHasDisciple.get(i));
						break;
					case 4:
						hasDQuality.add(userHasDisciple.get(i));
						break;
				}
			}
			
			int secondSelectorId = 0;
			
			switch (discipleType) {
				case 1:
					if (hasAQuality.size() != 0) {
						int index = (int)(Math.random() * hasAQuality.size());
						hunpoid = hasAQuality.get(index);
					}
					else {
						secondSelectorId = 2;
					}
					break;
				case 2:
					if (hasBQuality.size() != 0) {
						int index = (int)(Math.random() * hasBQuality.size());
						hunpoid = hasBQuality.get(index);
					}
					else {
						secondSelectorId = 2;
					}
					break;
				case 3:
					if (hasCQuality.size() != 0) {
						int index = (int)(Math.random() * hasCQuality.size());
						hunpoid = hasCQuality.get(index);
					}
					else {
						secondSelectorId = 3;
					}
					break;
				case 4:
					if (hasDQuality.size() != 0) {
						int index = (int)(Math.random() * hasDQuality.size());
						hunpoid = hasDQuality.get(index);
					}
					else {
						secondSelectorId = 4;
					}
					break;
			}

			if (hunpoid == 0 && secondSelectorId != 0) {//此情况为没有该类型的弟子，所以生成不了对应的魂魄，需要转化成下级弟子
				KindSelecter discipleSelecter2 = XMLTemplateService.getDiscipleSelecter(secondSelectorId);
				List<Integer> discipleIds2 = discipleSelecter2.getKinds();
				List<Integer> discipleRates2 = getRandRates(discipleSelecter2, poolId);
				
				ArrayList<Integer> hasQualityDisciple = new ArrayList<Integer>();
				
				switch (secondSelectorId) {
					case 1:
						hasQualityDisciple = hasAQuality;
						break;
					case 2:
						hasQualityDisciple = hasBQuality;
						break;
					case 3:
						hasQualityDisciple = hasCQuality;
						break;
					case 4:
						hasQualityDisciple = hasDQuality;
						break;
				}
				List<Integer> diffIds = new ArrayList<Integer>();
				List<Integer> diffRates = new ArrayList<Integer>();
				
				if (hasQualityDisciple.size() == 0) {
					diffIds = discipleIds2;
					diffRates = discipleRates2;
				}
				else {
					for (int i = 0; i < discipleIds2.size(); i++) {
						if (!hasQualityDisciple.contains(discipleIds2.get(i))) {
							diffIds.add(discipleIds2.get(i));
							diffRates.add(discipleRates2.get(i));
						}
					}
				}
				
				if (diffIds.size() != 0) {
					int index = Util.getRandResult(diffRates);
					itemid = diffIds.get(index);
				}
				else {
					int index = Util.getRandResult(discipleRates2);
					hunpoid = discipleIds2.get(index);
					
				}
			}
		}
		
		DiscipleXmlTemplate discipleInfo = new DiscipleXmlTemplate();
		if (itemid != 0 && hunpoid == 0) {
			discipleInfo = XMLTemplateService.getDiscipleTemplate(itemid);
		}
		else if (hunpoid != 0 && itemid == 0) {
			discipleInfo = XMLTemplateService.getDiscipleTemplate(hunpoid);
		} 
		
		if (itemid != 0) {
			Disciple disciple = DiscipleUtil.newDisciple(player, itemid, 1, 0);
			
			Skill skill = player.getSkills().get(disciple.getSkill1());
			OPUserSkillInfoRet.SkillInfo.Builder skillBuilder = SkillUtil.buildSkillBuilder(skill);
			builder.setSkill(skillBuilder);
			
			OPUserDiscipleInfoRet.DiscipleInfo.Builder discipleBuilder = DiscipleUtil.buildDiscipleBuilder(disciple);
			
			builder.setDisciple(discipleBuilder);
			
			if (discipleInfo.getQuality() == 1) {
				ArrayList<Integer> broParams = new ArrayList<Integer>(1);
				broParams.add(itemid);
				BoardCastInfoHandler.addBoardCase(BoardCastInfoHandler.RECRUITDISCIPLE_TYPE, player.getName(), broParams);
			}
		}
		else {
			HashMap<Integer, Soul> userSouls = player.getSouls();
			Soul soul = new Soul();
			int count = XMLTemplateService.getSysBasicConfig().getSoulCounts().get(discipleInfo.getQuality());
			if (userSouls.containsKey(hunpoid)) {
				soul = userSouls.get(hunpoid);
				soul.setCount(soul.getCount() + count);
				player.updateOneSoul(soul);
			}
			else {
				soul.setItemid(hunpoid);
				soul.setPlayerid(playerId);
				soul.setCount(count);
				player.addOneSoul(soul);
			}
			
			OPUserSoulInfoRet.SoulInfo.Builder soulBuilder= OPUserSoulInfoRet.SoulInfo.newBuilder();
			soulBuilder.setId(soul.getId());
			soulBuilder.setItemid(soul.getItemid());
			soulBuilder.setCount(soul.getCount());
			builder.setSoul(soulBuilder);
			builder.setSoulCount(count);
		}
		
		builder.setServertime(Util.getServerTime());
		if (discipleType == 1 && isHunpo == 31 && userStats.getDiscipleLucky() + addLucky >= poolLimit) {
			userStats.setDiscipleLucky(0);
			int nextId = userStats.getNextDisciple() + 1;
			if (nextId >= XMLTemplateService.getSysBasicConfig().getPoolLimits().size()) {
				nextId = XMLTemplateService.getSysBasicConfig().getPoolLimits().size() - 1;
			}
			userStats.setNextDisciple(nextId);
		}
		else if (userStats.getDiscipleLucky() + addLucky < poolLimit) {
			userStats.setDiscipleLucky(userStats.getDiscipleLucky() + addLucky);
		} 
		player.setUserStats(userStats);
		
		OPUserMarketInfoRet.Builder infoBuilder = OPUserMarketInfoRet.newBuilder();
		infoBuilder.setTenleavetime(player.getTodayleavetimes());
        
		int time = player.getHundredtime() + 24 * 3600 - current_time;
		time = time < 0?0:time;
		infoBuilder.setHundredtime(time);
		time = player.getWanlitime() + 3 * 24 * 3600 - current_time;
		time = time < 0?0:time;
		infoBuilder.setWanlitime(time);
		time = player.getTentime() + 10 * 60 - current_time;
		time = time < 0?0:time;
		infoBuilder.setTentime(time);
		infoBuilder.setTencost(10);
		infoBuilder.setHundredcost(100);
		infoBuilder.setWanlicost(300);
		infoBuilder.setServertime(current_time);
		infoBuilder.setTenleavetime(player.getTodayleavetimes());
		
		String buyGifts = player.getUserStats().getBuygifts();
		if( !buyGifts.equals("") && buyGifts != null){
			String[] gifts = buyGifts.split(",");
			for (int i = 0; i < gifts.length; i++) {
				infoBuilder.addGotGiftID(Integer.parseInt(gifts[i]));
			}
		}
		
		if (player.getRechargenum() == 0) {
			infoBuilder.setFirstPay(true);
		}
		else {
			infoBuilder.setFirstPay(false);
		}
		
		if (player.getUserStats().getFirstbaitime() == 0) {
			infoBuilder.setFirstBaili(true);
		}
		else {
			infoBuilder.setFirstBaili(false);
		}
		
		if (player.getUserStats().getFirstwantime() == 0) {
			infoBuilder.setFirstWanli(true);
		}
		else {
			infoBuilder.setFirstWanli(false);
		}
		//送魂魄活动是否开启
		if(itermID!=0){
			infoBuilder.setGivesoulstatus(itermID!=0);
			if(kind==2 || kind==3){
				SoulUtil.giveSoul(player, builder, kind, itermID);
			}
		}
		
		
		player.setLastVerifyTime(current_time);
		
		builder.setUserMarketInfoRet(infoBuilder);
		builder.setGoldcoins(player.getGoldCoins());
		
		
		player.sendPacket(OP.code.OPCODE_MARKET_RECRUIT_DISCIPLERET_S_VALUE, builder);
		long current_time_2 = Util.getServerMillTime();
		long taken_time=current_time_2-start_time;
		logger2.info("market recruit disciple handler ret send: {}, {}, {}", new Object[]{id ,taken_time});
	}
	
	public static List<Integer> getRandRates(KindSelecter selecter,int poolId){
		List<Integer> randRates = new ArrayList<Integer>();
		switch (poolId) {
			case 1:
				randRates = selecter.getRate1();
				break;
			case 2:
				randRates = selecter.getRate2();
				break;
			case 3:
				randRates = selecter.getRate3();
				break;
			case 4:
				randRates = selecter.getRate4();
				break;
			case 5:
				randRates = selecter.getRate5();
				break;
			case 6:
				randRates = selecter.getRate6();
				break;
		}
		return randRates;
	}
}

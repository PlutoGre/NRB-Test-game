package com.oldmountain.dzm.packethandler;

import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.Collections;
import java.util.Date;
import java.util.HashMap;
import java.util.List;
import java.util.Random;

import net.sf.json.JSONArray;
import net.sf.json.JSONObject;

import org.apache.commons.collections.functors.IfClosure;
import org.apache.mina.common.IoSession;
import org.hibernate.mapping.Array;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.google.protobuf.InvalidProtocolBufferException;
import com.oldmountain.dzm.DZM;
import com.oldmountain.dzm.configEntity.WorldBossConfig;
import com.oldmountain.dzm.entity.*;
import com.oldmountain.dzm.message.OP;
import com.oldmountain.dzm.message.ErrorReporter.OPErrorReporter;
import com.oldmountain.dzm.message.UserBattle.OPUserBattleRet;
import com.oldmountain.dzm.message.WorldBossFight.OPWorldBossFight;
import com.oldmountain.dzm.message.WorldBossFight.OPWorldBossFightRet;
import com.oldmountain.dzm.message.WorldBossInfo.OPGetWorldBossInfo;
import com.oldmountain.dzm.message.WorldBossInfo.OPGetWorldBossInfoRet;
import com.oldmountain.dzm.message.WorldBossRefresh.OPWorldBossRefresh;
import com.oldmountain.dzm.message.WorldBossRefresh.OPWorldBossRefreshRet;
import com.oldmountain.dzm.net.ObjectAccessor;
import com.oldmountain.dzm.net.packet.Packet;
import com.oldmountain.dzm.services.DaoManager;
import com.oldmountain.dzm.services.FightEndRankingServices;
import com.oldmountain.dzm.services.RewardServices;
import com.oldmountain.dzm.services.SysMsgInfoServices;
import com.oldmountain.dzm.util.Fight;
import com.oldmountain.dzm.util.Util;
import com.oldmountain.dzm.util.XMLTemplateService;

class WorldBossDamageInfo{
	public String name;
	public int attackNum;
	public int damageBlood;
	public int discipleId;
}

public class WorldBossHandler {
	private static final Logger log = LoggerFactory.getLogger(WorldBossHandler.class);
	private static WorldBoss worldBoss = new WorldBoss();
	private static ArrayList<WorldBossRank> lastRank = new ArrayList<WorldBossRank>();
	private static ArrayList<WorldBossRank> currentRank = new ArrayList<WorldBossRank>();
	private static List<WorldBossDamageInfo> damageList = new ArrayList<WorldBossDamageInfo>();
	private static int lastKillerId = 0;
	private static String lastKillerName = "";
	private static int lastKillerLevel = 0;
	private static int lastKillerDamage = 0;
	private static int lastKillTime = 0;
	private static int lastSortTime = 0;
	
	public static final int RANK_LIMIT = 10;
	public static int minRankDamage = 0;
	
	public static void getWorldBossInfo(Packet packet,IoSession session){
		OPGetWorldBossInfo params = null;
		OPGetWorldBossInfoRet.Builder builder = OPGetWorldBossInfoRet.newBuilder();
		OPGetWorldBossInfoRet.WorldBossBattleArray.Builder rankBuilder = null;
		
		try {
			params = OPGetWorldBossInfo.parseFrom(packet.data.array());
		} catch (InvalidProtocolBufferException e) {
			e.printStackTrace();
			return;
		}
		
		int version = params.getVersion();
		if (version != 1) {
			
		}
		int playerId = (Integer)session.getAttribute("playerid");
		Player player = ObjectAccessor.getOnlineUser(playerId);
		
		WorldBossConfig bossConfig = XMLTemplateService.getWorldBossConfig();
		if (player.getLevel() < bossConfig.getStartLevel()) {
			OPErrorReporter.Builder errBuilder = OPErrorReporter.newBuilder();
			errBuilder.setErrorid(OP.code.OPCODE_GET_WORLD_BOSS_INFORET_S_VALUE);
			errBuilder.setErrormsg("level not enough");
			player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errBuilder);
		}
		builder.setServertime(Util.getServerTime());
		Calendar calendar = Calendar.getInstance();
		int current_hour = calendar.get(Calendar.HOUR_OF_DAY);
		int current_minute = calendar.get(Calendar.MINUTE);
		int current_time = current_hour*100 + current_minute;
		checkWorldBoss();
		builder.setBossLevel(worldBoss.getLevel());
		
		if (current_time < bossConfig.getStartTime() || current_time >= bossConfig.getEndTime() || worldBoss.getleftHealth() <= 0) {
			builder.setFinished(1);
			for (int i = 0; i < lastRank.size(); i++) {
				rankBuilder = OPGetWorldBossInfoRet.WorldBossBattleArray.newBuilder();
				WorldBossRank rank = lastRank.get(i);
				rankBuilder.setPlayerid(rank.getPlayerid());
				rankBuilder.setRank(rank.getRank());
				rankBuilder.setName(rank.getName());
				rankBuilder.setLevel(rank.getLevel());
				rankBuilder.setDamageBlood(rank.getDamage());
				rankBuilder.setIsFinal(false);
				builder.addBattleArray(rankBuilder);
			}
			if (lastKillerId != 0) {
				rankBuilder = OPGetWorldBossInfoRet.WorldBossBattleArray.newBuilder();
				rankBuilder.setPlayerid(lastKillerId);
				rankBuilder.setRank(-1);
				rankBuilder.setName(lastKillerName);
				rankBuilder.setLevel(lastKillerLevel);
				rankBuilder.setDamageBlood(lastKillerDamage);
				rankBuilder.setIsFinal(true);
				builder.addBattleArray(rankBuilder);
			}
		}
		else {
			builder.setFinished(0);
			int rank_count = Math.min(RANK_LIMIT, currentRank.size());
			for (int i = 0; i < rank_count; i++) {
				rankBuilder = OPGetWorldBossInfoRet.WorldBossBattleArray.newBuilder();
				WorldBossRank rank = currentRank.get(i);
				rankBuilder.setPlayerid(rank.getPlayerid());
				rankBuilder.setRank(i+1);
				rankBuilder.setName(rank.getName());
				rankBuilder.setLevel(rank.getLevel());
				rankBuilder.setDamageBlood(rank.getDamage());
				rankBuilder.setIsFinal(false);
				builder.addBattleArray(rankBuilder);
			}
		}
		
		
		player.sendPacket(OP.code.OPCODE_GET_WORLD_BOSS_INFORET_S_VALUE, builder);
	}
	
	public static void refreshWorldBoss(Packet packet,IoSession session){
		OPWorldBossRefresh params = null;
		OPWorldBossRefreshRet.Builder builder = OPWorldBossRefreshRet.newBuilder();
		OPGetWorldBossInfoRet.WorldBossBattleArray.Builder rankBuilder = null;
		
		try {
			params = OPWorldBossRefresh.parseFrom(packet.data.array());
		} catch (InvalidProtocolBufferException e) {
			e.printStackTrace();
			return;
		}
		
		int version = params.getVersion();
		if (version != 1) {
			
		}
		
		int playerId = (Integer)session.getAttribute("playerid");
		Player player = ObjectAccessor.getOnlineUser(playerId);
		WorldBossConfig bossConfig = XMLTemplateService.getWorldBossConfig();
		if (player.getLevel() < bossConfig.getStartLevel()) {
			OPErrorReporter.Builder errBuilder = OPErrorReporter.newBuilder();
			errBuilder.setErrorid(OP.code.OPCODE_GET_WORLD_BOSS_INFORET_S_VALUE);
			errBuilder.setErrormsg("level not enough");
			player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errBuilder);
		}
		
		DareWorldBoss userWorldBoss = getUserWorldBoss(player);
		
//		DareWorldBoss userWorldBoss = player.getWorldBoss();
//		if (userWorldBoss == null) {
//			userWorldBoss = new DareWorldBoss();
//			userWorldBoss.setPlayerid(playerId);
//			userWorldBoss.setLastverifytime(Util.getServerTime());
//			player.setWorldBoss(userWorldBoss);
//		}
		checkWorldBoss();
		builder.setServertime(Util.getServerTime());
		builder.setBossTotalBlood(worldBoss.getTotalHealth());
		builder.setBossLeftBlood(worldBoss.getleftHealth());
		builder.setAttackNum(userWorldBoss.getAttackcount());
		builder.setDamageBlood(userWorldBoss.getAttackamount());
		builder.setUseFireTime(userWorldBoss.getAshestimes());
		int waitTime = bossConfig.getTimeLimitMax() - (Util.getServerTime() - userWorldBoss.getLastverifytime());
		if (waitTime < 0) {
			waitTime = 0;
		}
		builder.setWaitForFightTime(waitTime);
		
		Calendar calendar = Calendar.getInstance();
		int current_hour = calendar.get(Calendar.HOUR_OF_DAY);
		int current_minute = calendar.get(Calendar.MINUTE);
		int current_time = current_hour*100 + current_minute;
		if (current_time < bossConfig.getStartTime() || current_time >= bossConfig.getEndTime() || worldBoss.getleftHealth() <= 0) {
			builder.setFinished(1);
			for (int i = 0; i < lastRank.size(); i++) {
				rankBuilder = OPGetWorldBossInfoRet.WorldBossBattleArray.newBuilder();
				WorldBossRank rank = lastRank.get(i);
				rankBuilder.setPlayerid(rank.getPlayerid());
				rankBuilder.setRank(rank.getRank());
				rankBuilder.setName(rank.getName());
				rankBuilder.setLevel(rank.getLevel());
				rankBuilder.setDamageBlood(rank.getDamage());
				rankBuilder.setIsFinal(false);
				builder.addBattleArray(rankBuilder);
			}
			if (lastKillerId != 0) {
				rankBuilder = OPGetWorldBossInfoRet.WorldBossBattleArray.newBuilder();
				rankBuilder.setPlayerid(lastKillerId);
				rankBuilder.setRank(-1);
				rankBuilder.setName(lastKillerName);
				rankBuilder.setLevel(lastKillerLevel);
				rankBuilder.setDamageBlood(lastKillerDamage);
				rankBuilder.setIsFinal(true);
				builder.addBattleArray(rankBuilder);
			}
		}
		else {
			builder.setFinished(0);
			int rank_count = Math.min(RANK_LIMIT, currentRank.size());
			for (int i = 0; i < rank_count; i++) {
				rankBuilder = OPGetWorldBossInfoRet.WorldBossBattleArray.newBuilder();
				WorldBossRank rank = currentRank.get(i);
				rankBuilder.setPlayerid(rank.getPlayerid());
				rankBuilder.setRank(i+1);
				rankBuilder.setName(rank.getName());
				rankBuilder.setLevel(rank.getLevel());
				rankBuilder.setDamageBlood(rank.getDamage());
				rankBuilder.setIsFinal(false);
				builder.addBattleArray(rankBuilder);
			}
		}
		
		int count = 0;
		for (int i = damageList.size() - 1; i >= 0; i--) {
			if (count >= 30) {
				break;
			}
			WorldBossDamageInfo damageInfo = damageList.get(i);
			OPWorldBossRefreshRet.WorldBossDamageArray.Builder damageBuilder = OPWorldBossRefreshRet.WorldBossDamageArray.newBuilder();
			damageBuilder.setName(damageInfo.name);
			damageBuilder.setAttackNum(damageInfo.attackNum);
			damageBuilder.setDamageBlood(damageInfo.damageBlood);
			damageBuilder.setDiscipleId(damageInfo.discipleId);
			builder.addDamageArray(damageBuilder);
			count ++;
		}
		
		player.sendPacket(OP.code.OPCODE_WORLDBOSS_REFRESHRET_S_VALUE, builder);
	}
	
	public static void WorldBossFight(Packet packet,IoSession session){
		OPWorldBossFight params = null;
		OPWorldBossFightRet.Builder builder = OPWorldBossFightRet.newBuilder();
		OPGetWorldBossInfoRet.WorldBossBattleArray.Builder rankBuilder = null;
		OPErrorReporter.Builder errBuilder = OPErrorReporter.newBuilder();
		
		try {
			params = OPWorldBossFight.parseFrom(packet.data.array());
		} catch (InvalidProtocolBufferException e) {
			e.printStackTrace();
			return;
		}
		
		int version = params.getVersion();
		if (version != 1) {
			
		}
		
		int current_time = Util.getServerTime();
		WorldBossConfig worldBossConfig = XMLTemplateService.getWorldBossConfig();
		
		int playerId = (Integer)session.getAttribute("playerid");
		Player player = ObjectAccessor.getOnlineUser(playerId);
		
		if (player.getLevel() < worldBossConfig.getStartLevel()) {
			errBuilder.setErrorid(OP.code.OPCODE_GET_WORLD_BOSS_INFORET_S_VALUE);
			errBuilder.setErrormsg("level not enough");
			player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errBuilder);
			return;
		}
		
//		DareWorldBoss userBossInfo = player.getWorldBoss();
		DareWorldBoss userBossInfo = getUserWorldBoss(player);
		
		Calendar calendar = Calendar.getInstance();
		int current_hour = calendar.get(Calendar.HOUR_OF_DAY);
		int current_minute = calendar.get(Calendar.MINUTE);
		int currentDate = current_hour*100 + current_minute;
		WorldBossConfig bossConfig = XMLTemplateService.getWorldBossConfig();
		builder.setServertime(current_time);
		builder.setBossTotalBlood(worldBoss.getTotalHealth());
		
		if (currentDate < bossConfig.getStartTime() || currentDate >= bossConfig.getEndTime() || worldBoss.getleftHealth() <= 0) {
			builder.setFinished(1);
			for (int i = 0; i < lastRank.size(); i++) {
				rankBuilder = OPGetWorldBossInfoRet.WorldBossBattleArray.newBuilder();
				WorldBossRank rank = lastRank.get(i);
				rankBuilder.setPlayerid(rank.getPlayerid());
				rankBuilder.setRank(rank.getRank());
				rankBuilder.setName(rank.getName());
				rankBuilder.setLevel(rank.getLevel());
				rankBuilder.setDamageBlood(rank.getDamage());
				rankBuilder.setIsFinal(false);
				builder.addBattleArray(rankBuilder);
			}
			if (lastKillerId != 0) {
				rankBuilder = OPGetWorldBossInfoRet.WorldBossBattleArray.newBuilder();
				rankBuilder.setPlayerid(lastKillerId);
				rankBuilder.setRank(-1);
				rankBuilder.setName(lastKillerName);
				rankBuilder.setLevel(lastKillerLevel);
				rankBuilder.setDamageBlood(lastKillerDamage);
				rankBuilder.setIsFinal(true);
				builder.addBattleArray(rankBuilder);
			}
		}
		else {
			int type = params.getFighttype();
			int costGold = 0;
			int coolingTime = 0;
			float addQuantity = 0;
			switch (type) {
				case 1://普通进攻
					coolingTime = worldBossConfig.getTimeLimitMax();
					break;
				case 2://立即复活进攻
					coolingTime = worldBossConfig.getTimeLimitMin();
					costGold = worldBossConfig.getReliveCost();
					break;
				case 3://浴火重生进攻
					coolingTime = worldBossConfig.getTimeLimitMin();
					costGold = worldBossConfig.getAshesCost() + worldBossConfig.getAshesIncrease() * (userBossInfo.getAshestimes());
					addQuantity = worldBossConfig.getAshesAddQuantity();
					break;
			}
			//判断时间是否冷却
			if (current_time - userBossInfo.getLastverifytime() < coolingTime) {
				errBuilder.setErrorid(OP.code.OPCODE_WORLDBOSS_FIGHTRET_S_VALUE);
				errBuilder.setErrormsg("still cooling");
				player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errBuilder);
				return ;
			}
			//判断是否够元宝消耗
			if (player.getGoldCoins() < costGold) {
				errBuilder.setErrorid(OP.code.OPCODE_WORLDBOSS_FIGHTRET_S_VALUE);
				errBuilder.setErrormsg("gold not enough");
				player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errBuilder);
				return ;
			}
			
			//战斗
			OPUserBattleRet.Builder battleBuilder = OPUserBattleRet.newBuilder();
			Result result = new Result();
			result.setWinlose(5);
			
			battleBuilder.setVersion(1);
			battleBuilder.setServertime(current_time);
			
			BattleArrayObject userBattleArrayObject = ObjectAccessor.getBattleArray(playerId);
			Fight fight = new Fight();
			int damage = fight.worldBossFight(userBattleArrayObject,addQuantity);
			
			battleBuilder.setRound1(UserBattleHandler.getRoundBuilder(fight.getRound1()));
			battleBuilder.setRound2(UserBattleHandler.getRoundBuilder(fight.getRound2()));
			battleBuilder.setRound3(UserBattleHandler.getFinalRound(fight.getFinalRound()));
			
			//更新用户攻击boss信息
			userBossInfo.addAttackCount(1);
			userBossInfo.addAttackAmount(damage);
			if (type == 3) {
				userBossInfo.addAshestimes(1);
			}
			userBossInfo.setLastverifytime(current_time);
			//更新排行榜信息
			addRanking(player, userBossInfo.getAttackamount());
			
			int rank_count = Math.min(RANK_LIMIT, currentRank.size());
			for (int i = 0; i < rank_count; i++) {
				rankBuilder = OPGetWorldBossInfoRet.WorldBossBattleArray.newBuilder();
				WorldBossRank rank = currentRank.get(i);
				rankBuilder.setPlayerid(rank.getId());
				rankBuilder.setRank(i+1);
				rankBuilder.setName(rank.getName());
				rankBuilder.setLevel(rank.getLevel());
				rankBuilder.setDamageBlood(rank.getDamage());
				rankBuilder.setIsFinal(false);
				builder.addBattleArray(rankBuilder);
			}
			builder.setFinished(0);
			int gainCoins = Math.round(damage * worldBossConfig.getSilverCoinsBase());
			//更新世界boss剩余血量
			long leftHealth = worldBoss.getleftHealth() - damage;
			if (leftHealth < 0) {
				leftHealth = 0;
				worldBoss.setKillerId(playerId);
				worldBoss.setleftHealth(leftHealth);
				worldBoss.setKillerName(player.getName());
				worldBoss.setKillerLevel(player.getLevel());
				worldBoss.setKillerDamage(userBossInfo.getAttackamount());
				worldBoss.setKilledTime(Util.getServerTime());
				bossDone();
				rankBuilder = OPGetWorldBossInfoRet.WorldBossBattleArray.newBuilder();
				rankBuilder.setPlayerid(playerId);
				rankBuilder.setRank(-1);
				rankBuilder.setName(player.getName());
				rankBuilder.setLevel(player.getLevel());
				rankBuilder.setDamageBlood(userBossInfo.getAttackamount());
				rankBuilder.setIsFinal(true);
				builder.addBattleArray(rankBuilder);
				builder.setFinished(1);
				builder.setIsFinalFight(true);
				gainCoins += Math.round(worldBoss.getTotalHealth() * bossConfig.getBossKillRatio());
			}
			
			player.setSilvercoins(player.getSilvercoins() + gainCoins);
			result.setGotCoin(gainCoins);
			//奖励经验药剂暂时不用，后期需要修改proto再增加
//			UserLevelConfig levelConfig = XMLTemplateService.getUserLevelConfig(player.getLevel());
//			Random random= new Random();
//			int rand = random.nextInt(1000) + 1;
//			if (rand <= levelConfig.getWorldBossRumProbability()) {
//				int amount = levelConfig.getWorldBossRumCount();
//				Util.rewardBagTool(player, 1501001, amount);
//				
//				result.setGotItemType(0);
//				result.setGotItemId(1501001);
//				result.setGotItemCount(amount);
//			}
			
			Random random= new Random();
			int rand = random.nextInt(100) + 1;
			if (rand <= worldBossConfig.getPerAttackRewardProbability() && userBossInfo.getHasGotRewardCount() < worldBossConfig.getTotalRewardMax()) {
				int amount = worldBossConfig.getPerAttackRewardNum();
				Util.rewardBagTool(player, 1502001, amount);
				result.setGotItemType(0);
				result.setGotItemId(1502001);
				result.setGotItemCount(amount);
				userBossInfo.setHasGotRewardCount(userBossInfo.getHasGotRewardCount() + amount);
			}
			
			battleBuilder.setResult(UserBattleHandler.getResult(player, result));
			OPUserBattleRet.Team.Builder teamBuilder = fight.getTeamBuilder();
			teamBuilder.setName1("黑胡子");
			teamBuilder.setName2(player.getName());
			teamBuilder.setTeam1Archievements(userBattleArrayObject.getMomentum());
			teamBuilder.setTeam2Archievements(0);
			battleBuilder.setTeamArchievements(teamBuilder);
			
			builder.setFightInfo(battleBuilder);
			
			
			worldBoss.setleftHealth(leftHealth);
			worldBoss.setLastRefreshTime(current_time);
			
			addDamageInfo(player.getName(), userBossInfo.getAttackcount(), damage,userBattleArrayObject.getBattlearrays().get(0).getItemid());
			
			int count = 0;
			for (int i = damageList.size() - 1; i >= 0; i--) {
				if (count >= 30) {
					break;
				}
				WorldBossDamageInfo damageInfo = damageList.get(i);
				OPWorldBossRefreshRet.WorldBossDamageArray.Builder damageBuilder = OPWorldBossRefreshRet.WorldBossDamageArray.newBuilder();
				damageBuilder.setName(damageInfo.name);
				damageBuilder.setAttackNum(damageInfo.attackNum);
				damageBuilder.setDamageBlood(damageInfo.damageBlood);
				damageBuilder.setDiscipleId(damageInfo.discipleId);
				builder.addDamageArray(damageBuilder);
				count ++;
			}
			player.consumeGold(costGold);
		}
		
		builder.setBossLeftBlood(worldBoss.getleftHealth());
		builder.setAttackNum(userBossInfo.getAttackcount());
		builder.setDamageBlood(userBossInfo.getAttackamount());
		builder.setUseFireTime(userBossInfo.getAshestimes());
		builder.setWaitForFightTime(worldBossConfig.getTimeLimitMax());
		builder.setUserGoldNum(player.getGoldCoins());
		
		player.sendPacket(OP.code.OPCODE_WORLDBOSS_FIGHTRET_S_VALUE, builder);
	}
	
	private static void addRanking(Player player,int damageNum){
		int current_time = Util.getServerTime();
		if (damageNum > minRankDamage || worldBoss.getCurrentRank().size() <= RANK_LIMIT) {
			WorldBossRank newRank = null;
			if (worldBoss.getCurrentRank().containsKey(player.getId())) {
				newRank = worldBoss.getCurrentRank().get(player.getId());
				newRank.setDamage(damageNum);
			}
			else {
				newRank = new WorldBossRank();
				newRank.setPlayerid(player.getId());
				newRank.setName(player.getName());
				newRank.setLevel(player.getLevel());
				newRank.setDamage(damageNum);
			}
			worldBoss.getCurrentRank().put(player.getId(), newRank);
		}
		if (worldBoss.getCurrentRank().size() >= 100 || worldBoss.getCurrentRank().size() <= RANK_LIMIT || current_time - lastSortTime>=60) {
			sortCurrentWorldBossRank();
		}
	}
	
	private synchronized static void addDamageInfo(String name,int attackNum,int damageBlood,int discipleItem){
		WorldBossDamageInfo info = new WorldBossDamageInfo();
		info.name = name;
		info.attackNum = attackNum;
		info.damageBlood = damageBlood;
		info.discipleId = discipleItem;
		damageList.add(info);
		
		if (damageList.size() >= 300) {
			List<WorldBossDamageInfo> subList = damageList.subList(0, 30);
			damageList = subList;
		}
	}
	
	private synchronized static void initWorldBossInfo(){
		if (worldBoss.getCurrentRank().size() != 0) {
			exportToDb();
		}
		worldBoss = new WorldBoss();
		WorldBossConfig bossConfig = XMLTemplateService.getWorldBossConfig();
		SimpleDateFormat sdf = new SimpleDateFormat("yyyyMMdd");
		try {
			Date serverOpenDate = sdf.parse(DZM.serverStartDate);
			Date today = new Date();
			int passDay = (int)((today.getTime() - serverOpenDate.getTime())/(3600*24*1000));
			int level = (1+passDay)>bossConfig.getMaxLevel()?bossConfig.getMaxLevel():1+passDay;
			long totalHealth =XMLTemplateService.getWorldBossLvHp(level).getHp();
			worldBoss.setDate(Integer.valueOf(sdf.format(today)));
			worldBoss.setLevel(level);
			worldBoss.setTotalHealth(totalHealth);
			worldBoss.setleftHealth(totalHealth);
			worldBoss.setKilledTime(0);
			worldBoss.setLastRefreshTime(Util.getServerTime());
			ArrayList<WorldBossRank> rank = new ArrayList<WorldBossRank>();
			worldBoss.setCurrentRank(new HashMap<Integer, WorldBossRank>());
			currentRank = rank;
			minRankDamage = 0;
			lastSortTime = 0;
//			DZM.getEntityManager().createSync(worldBoss);
		} catch (ParseException e) {
			e.printStackTrace();
		}
		
	}
	
	private synchronized static void bossDone(){
		sortCurrentWorldBossRank();
		exportToDb();
		lastRank = currentRank;
		currentRank = new ArrayList<WorldBossRank>();
		worldBoss.setCurrentRank(new HashMap<Integer, WorldBossRank>());
		lastKillerId = worldBoss.getKillerId();
		lastKillerName = worldBoss.getKillerName();
		lastKillerLevel = worldBoss.getKillerLevel();
		lastKillerDamage = worldBoss.getKillerDamage();
		lastKillTime = worldBoss.getKilledTime();
	}
	
	private static void exportToDb(){
		DZM.getEntityManager().createSync(worldBoss);
		int currentTime = Util.getServerTime();
		if (currentRank.size() != 0) {
			for (int i = 0; i < currentRank.size(); i++) {
				if (i >= RANK_LIMIT) {
					break;
				}
				else {
					WorldBossRank rank = currentRank.get(i);
					
					DZM.getEntityManager().createSync(rank);
					Reward reward = addRankReward(rank,currentTime);
					try
			        {
			            if(ObjectAccessor.players.containsKey(rank.getPlayerid()))
			            {
			            	Player player = ObjectAccessor.getOnlineUser(rank.getPlayerid());
			            	player.getRewards().put(reward.getId(),reward);
//			                ObjectAccessor.getOnlineUser(rank.getPlayerid()).setNeedSyncRewards(true);
			            	SysMsgInfoServices.commitWorldBossMsg(player, reward);
			            }
			        }
			        catch (Exception e)
			        {
			            log.info("exportRanking update needSyncReward fail playerId:"+rank.getPlayerid()+","+e.getMessage());
			        }
				}
			}
		}
	}
	
	private static Reward addRankReward(WorldBossRank rank,int time){
		Reward reward=new Reward();
        reward.setPlayerId(rank.getPlayerid());
        reward.setAddTime(time);
        reward.setRewardTime(time);
        reward.setStatus(0);
        reward.setRewardType(RewardServices.WORLDBOSSRANK_REWARD);
        reward.getRewardInfoJson().put("t", rank.getRanktime());
        reward.getRewardInfoJson().put("l", rank.getRank());
        reward.setRewardContentFromJson(getRewardByRank(rank));
        DaoManager.getRewardDao().addReward(reward);
        return reward;
	}
	
	private static JSONArray getRewardByRank(WorldBossRank rank){
		JSONArray array = new JSONArray();
		JSONObject toolObject = new JSONObject();
		JSONObject silverObject = new JSONObject();
		int toolCount = (11 - rank.getRank()) * 5;
		int silverCount = Math.round(rank.getDamage()/2);
		toolObject.put("t",61001);
		toolObject.put("i",1502001);
		toolObject.put("q", toolCount);
		
		silverObject.put("t", 11001);
		silverObject.put("i", 1002);
		silverObject.put("q",silverCount);
		array.add(toolObject);
		array.add(silverObject);
		return array;
	}
	
	private synchronized static void checkWorldBoss(){
		if (worldBoss.getLastRefreshTime() < Util.getTodayZeroTimeStamp()) {
			initWorldBossInfo();
		}
		if (currentRank.size() != 0 ) {
			Calendar cal = Calendar.getInstance();
			int current_hour = cal.get(Calendar.HOUR_OF_DAY);
			int current_minute = cal.get(Calendar.MINUTE);
			int currentDate = current_hour*100 + current_minute;
			WorldBossConfig bossConfig = XMLTemplateService.getWorldBossConfig();
			if (currentDate >= bossConfig.getEndTime()) {
				bossDone();
			}
		}
	}
	
	public static void loadYesBossInfoToRam(){
		Calendar calendar = Calendar.getInstance();
		int date = Integer.valueOf(new SimpleDateFormat("yyyyMMdd").format(calendar.getTime()));
		int current_hour = calendar.get(Calendar.HOUR_OF_DAY);
		int current_minute = calendar.get(Calendar.MINUTE);
		int currentDate = current_hour*100 + current_minute;
		WorldBossConfig bossConfig = XMLTemplateService.getWorldBossConfig();
		if (currentDate < bossConfig.getStartTime()) {
			calendar.add(Calendar.DATE, -1);
			String yestedayDate = new SimpleDateFormat("yyyyMMdd").format(calendar.getTime());
			date = Integer.valueOf(yestedayDate);
		}
		
		List<Object> bossList = DZM.getEntityManager().limitQuery("from WorldBoss where date = ?", 0, 1, date);
		if (bossList.size() == 1) {
			worldBoss = (WorldBoss)bossList.get(0);
			
			ArrayList<WorldBossRank>ranks = new ArrayList<WorldBossRank>();
			List<Object> rankList = DZM.getEntityManager().limitQuery("from WorldBossRank where ranktime = ?", 0, 10, date);
			for (int i = 0; i < rankList.size(); i++) {
				WorldBossRank rank = (WorldBossRank)rankList.get(i);
				ranks.add(rank);
			}
			lastRank = ranks;
			
			lastKillerId = worldBoss.getKillerId();
			lastKillerName = worldBoss.getKillerName();
			lastKillerLevel = worldBoss.getKillerLevel();
			lastKillerDamage = worldBoss.getKillerDamage();
			lastKillTime = worldBoss.getKilledTime();
		}
		
//		if (currentDate > bossConfig.getStartTime() && bossList.size() <= 0) {
//			initWorldBossInfo();
//		}
	}
	
	private static DareWorldBoss getUserWorldBoss(Player player){
		DareWorldBoss userWorldBoss = player.getWorldBoss();
		if (userWorldBoss.getLastverifytime() < Util.getTodayZeroTimeStamp()) {
			userWorldBoss = new DareWorldBoss();
			userWorldBoss.setPlayerid(player.getId());
			userWorldBoss.setLastverifytime(Util.getServerTime() - 60);
			player.setWorldBoss(userWorldBoss);
		}
		return userWorldBoss;
	}
	
	private static void sortCurrentWorldBossRank(){
		ArrayList<WorldBossRank> rankList = new ArrayList<WorldBossRank>();
		rankList.addAll(worldBoss.getCurrentRank().values());
		Collections.sort(rankList,Collections.reverseOrder());
		ArrayList<WorldBossRank> finalRank = new ArrayList<WorldBossRank>();
		for (int i = 0; i < rankList.size(); i++) {
			if (i < RANK_LIMIT) {
				rankList.get(i).setRank(i+1);
				finalRank.add(rankList.get(i));
				minRankDamage = rankList.get(i).getDamage();
			}
			else {
				worldBoss.getCurrentRank().remove(rankList.get(i).getPlayerid());
			}
		}
		lastSortTime = Util.getServerTime();
		currentRank = finalRank;
	}
}

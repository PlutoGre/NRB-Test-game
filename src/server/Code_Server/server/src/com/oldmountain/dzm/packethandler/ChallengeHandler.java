package com.oldmountain.dzm.packethandler;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import com.oldmountain.dzm.services.SysMsgInfoServices;

import org.apache.mina.common.IoSession;
import org.hibernate.HibernateException;
import org.hibernate.SQLQuery;
import org.hibernate.Session;
import org.hibernate.Transaction;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.google.protobuf.InvalidProtocolBufferException;
import com.oldmountain.dzm.DZM;
import com.oldmountain.dzm.entity.Bag;
import com.oldmountain.dzm.entity.Battle;
import com.oldmountain.dzm.entity.BattleArrayObject;
import com.oldmountain.dzm.entity.CareerXmlTemplate;
import com.oldmountain.dzm.entity.Challenge;
import com.oldmountain.dzm.entity.CharacterResult;
import com.oldmountain.dzm.entity.Disciple;
import com.oldmountain.dzm.entity.FinalRound;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.entity.Result;
import com.oldmountain.dzm.entity.Round;
import com.oldmountain.dzm.entity.TiaozhanPoolConfig;
import com.oldmountain.dzm.entity.UserLevelConfig;
import com.oldmountain.dzm.entity.UserStats;
import com.oldmountain.dzm.message.ErrorReporter;
import com.oldmountain.dzm.message.OP;
import com.oldmountain.dzm.message.ChallengeOpponent.OPChallengeOpponent;
import com.oldmountain.dzm.message.ChallengeOpponent.OPChallengeOpponentRet;
import com.oldmountain.dzm.message.ErrorReporter.OPErrorReporter;
import com.oldmountain.dzm.message.GetArenaOpponentList.OPGetArenaOpponentList;
import com.oldmountain.dzm.message.GetArenaOpponentList.OPGetArenaOpponentListRet;
import com.oldmountain.dzm.message.GetArenaReward.OPGetArenaReward;
import com.oldmountain.dzm.message.GetArenaReward.OPGetArenaRewardRet;
import com.oldmountain.dzm.message.RefreshArenaScore.OPRefreshArenaScore;
import com.oldmountain.dzm.message.RefreshArenaScore.OPRefreshArenaScoreRet;
import com.oldmountain.dzm.message.UserArenaInfo.OPUserArenaInfo;
import com.oldmountain.dzm.message.UserArenaInfo.OPUserArenaInfoRet;
import com.oldmountain.dzm.message.UserBattle.OPUserBattleRet;
import com.oldmountain.dzm.message.UserBattleArray.OPUserBattleArrayRet;
import com.oldmountain.dzm.message.UserDiscipleInfo.OPUserDiscipleInfoRet;
import com.oldmountain.dzm.message.UserToolInfo.OPUserToolInfoRet;
import com.oldmountain.dzm.net.ObjectAccessor;
import com.oldmountain.dzm.net.packet.Packet;
import com.oldmountain.dzm.util.BattleUtil;
import com.oldmountain.dzm.util.DiscipleUtil;
import com.oldmountain.dzm.util.Fight;
import com.oldmountain.dzm.util.TitleUtil;
import com.oldmountain.dzm.util.Util;
import com.oldmountain.dzm.util.XMLTemplateService;

public class ChallengeHandler {
	private static final Logger activate_log = LoggerFactory.getLogger("ChallengeActivate");
	public static final ArrayList<Integer> rankAwards = new ArrayList<Integer>(Arrays.asList(3,4,5,6,7,8,9));
	private static int lastChangeTime=0; //活动期间上次第一名变更时间
	private static int activityEndTime=0; //活动终止时间
	public static void getUserChallengeInfo(Packet packet,IoSession session){
		OPUserArenaInfo params = null;
		OPUserArenaInfoRet.Builder builder = OPUserArenaInfoRet.newBuilder();
		
		try {
			params = OPUserArenaInfo.parseFrom(packet.data.array());
		} catch (InvalidProtocolBufferException e) {
			e.printStackTrace();
		}
		
		int version = params.getVersion();
		if (version != 1) {
			//TODO 版本控制
		}
		
		int playerId = (Integer)session.getAttribute("playerid");
		Player player = ObjectAccessor.getOnlineUser(playerId);
		
		if (player.getTutorialstep() < 1 || player.getLevel() < 5) {
			builder.setArenaNum(0);
			builder.setArenaRank(0);
			builder.setArenaScore(0);
			builder.setRewardScore(0);
			builder.setRewardRefreshTime(0);
			builder.setServertime(Util.getServerTime());
			player.sendPacket(OP.code.OPCODE_USER_ARENA_INFORET_S_VALUE, builder);
			return;
		}
		
		Challenge challenge = player.getChallenge();
		if (challenge == null) {
			challenge = getChallenge(playerId);
			if (challenge == null) {
				challenge = newChallengeInfo(playerId, player.getLevel(), player.getName());
			}
			player.setChallenge(challenge);
		}
		/**************************************/
		UserStats userStats=player.getUserStats();
		//相关统计及奖励活动开启判定
		long current_time=Util.getServerMillTime();
		int termId=Util.getActivityTermId(current_time, XMLTemplateService.getTiamzhanTimeConfigs());
		TiaozhanPoolConfig poolConfig=XMLTemplateService.getTiaozhanPoolConfig();
		//如果不再活动期间或者玩家的活动id与本次活动的不符,则进行相关初始化
		if(termId==0 ){
			userStats.setHighestRankInAct(0);
			userStats.setChallengeActTermId(0);
		}
		else if(termId!=userStats.getChallengeActTermId()){
			userStats.setChallengeActTermId(termId);
			//活动期间玩家首次登陆时将当前决斗名次设为活动期间最高达到过的名次
			userStats.setHighestRankInAct(player.getChallenge().getRank());
			int addIndex= poolConfig.getArenaRewardIndex();
			
			//初始化时将追加奖励的id从玩家已获取奖励中移除
			ArrayList<Integer> hasGotsArray = new ArrayList<Integer>();
			if (challenge.getHasgotreward() != null && !challenge.getHasgotreward().equals("")) {
				String[] hasGots = challenge.getHasgotreward().split(",");
				int index=-1;
				for (int i = 0; i < hasGots.length; i++) {
					hasGotsArray.add(Integer.parseInt(hasGots[i]));
					if(hasGotsArray.contains(addIndex)){
						index=i;
					}
				}
				if (hasGotsArray.contains(addIndex)) {
					hasGotsArray.remove(index);
				}
			}
			String newGot = "";
			if (hasGotsArray.size() > 0) {
				newGot = newGot + hasGotsArray.get(0);
				for (int i = 1; i < hasGotsArray.size(); i++) {
					newGot = newGot + "," + hasGotsArray.get(i);
				}
			}
			challenge.setHasgotreward(newGot);
			
		}
		/**************************************/
		int rankAddScore = calcScore(challenge);
		
		builder.setArenaRank(challenge.getRank());
		builder.setArenaScore(challenge.getScore());
		int leftTime = userStats.getChallengetimes();
		if (leftTime < 0) {
			leftTime = 0;
			userStats.setChallengetimes(0);
		}
		player.setUserStats(userStats);
		builder.setArenaNum(leftTime);
		builder.setRewardScore(rankAddScore);
		builder.setRewardRefreshTime(challenge.getLastrefreshtime());
		ArrayList<Integer> hasGot = new ArrayList<Integer>();
		if (challenge.getHasgotreward() != null && !challenge.getHasgotreward().equals("")) {
			String[] rewards = challenge.getHasgotreward().split(",");
			for (int i = 0; i < rewards.length; i++) {
				hasGot.add(Integer.parseInt(rewards[i]));
				builder.addGotRewardIDs(Integer.parseInt(rewards[i]));
			}
		}
		for (int i = 0; i < rankAwards.size(); i++) {
			if (!hasGot.contains(rankAwards.get(i)) && challenge.getHighestrank() <= getRewardConfigById(rankAwards.get(i))[1]) {
				builder.addRewardIDs(rankAwards.get(i));
			}
		}
		//如果活动开启,满足领取条件时,添加领取id
		if(termId!=0){
			if(!hasGot.contains(poolConfig.getArenaRewardIndex()) && userStats.getHighestRankInAct()!=0 &&userStats.getHighestRankInAct()<=poolConfig.getRankLimit() &&challenge.getScore()>=poolConfig.getArenaCost()){
				builder.addRewardIDs(poolConfig.getArenaRewardIndex());
			}
		}
		builder.setServertime(Util.getServerTime());
		
		player.setLastVerifyTime(Util.getServerTime());
		
		player.sendPacket(OP.code.OPCODE_USER_ARENA_INFORET_S_VALUE, builder);
	}
	
	public static void getEnemyList(Packet packet,IoSession session){
		OPGetArenaOpponentList params = null;
		OPGetArenaOpponentListRet.Builder builder = OPGetArenaOpponentListRet.newBuilder();
		OPGetArenaOpponentListRet.ArenaOpponentInfo.Builder enemyBuilder = OPGetArenaOpponentListRet.ArenaOpponentInfo.newBuilder();
		
		try {
			params = OPGetArenaOpponentList.parseFrom(packet.data.array());
		} catch (InvalidProtocolBufferException e) {
			e.printStackTrace();
		}
		
		int version = params.getVersion();
		if (version != 1) {
			//TODO 版本控制
		}
		
		int playerId = (Integer)session.getAttribute("playerid");
		Player player = ObjectAccessor.getOnlineUser(playerId);
		
		if (player.getLevel() < 5) {
			ErrorReporter.OPErrorReporter.Builder eBuilder = ErrorReporter.OPErrorReporter.newBuilder();
            eBuilder.setErrorid(OP.code.OPCODE_GET_ARENA_OPPONENT_LISTRET_S_VALUE);
            eBuilder.setErrormsg("level less than 5");
            player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, eBuilder);
            return;
		}
		
		Challenge userChallenge = getChallenge(playerId);
		if (userChallenge == null) {
			userChallenge = newChallengeInfo(playerId, player.getLevel(), player.getName());
		}
		
		int rank = userChallenge.getRank();
		
		ArrayList<Integer> ranksIntegers = new ArrayList<Integer>();
		for (int i = 1; i <= 10; i++) {
			if (i == rank) {
				continue;
			}
			ranksIntegers.add(i);
		}
		
		int gap = getListGap(rank);
		int frontMaxCount = 5;
		int behindCount = 3;
		int frontCount = 0;
		
		for (int i = 1; i <= frontMaxCount; i++) {
			int trank = rank - i * gap;
			if (trank < 10 && rank > 10) {
				trank = 10;
			}
			if (trank < 1) {
				trank = 1;
			}
			if (trank > 10) {
				ranksIntegers.add(trank);
			}
			frontCount ++;
			if (trank == 10 || trank == 1) {
				break;
			}
		}
		
		for (int i = 1; i <= behindCount; i++) {
			int trank = rank + i * gap;
			if (trank > 10) {
				ranksIntegers.add(trank);
			}
		}
		
		ArrayList<Challenge> challenges = getChallenges(ranksIntegers);
		
		for (int i = 0; i < challenges.size(); i++) {
			Challenge challenge = challenges.get(i);
			if(challenge.getLevel()<5 && challenge.getIsnpc()==0){
				continue;
			}
			enemyBuilder = OPGetArenaOpponentListRet.ArenaOpponentInfo.newBuilder();
			BattleArrayObject battleArrayObject = new BattleArrayObject();
			if (challenge.getIsnpc() == 0) {
				Player enemy = ObjectAccessor.getOnlineUser(challenge.getPlayerid());
				if (enemy == null) {
					enemy = new Player();
					enemy.setId(challenge.getPlayerid());
				}
				battleArrayObject = BattleUtil.getUserBattleArrayObject(enemy);
				if (battleArrayObject == null) {
					continue;
				}
				
			}
			else {
				int enemyId = challenge.getPlayerid();
				int careerId = enemyId / 100000;
				CareerXmlTemplate careerXmlTemplate = XMLTemplateService.getCareerTemplate(careerId);
				battleArrayObject = careerXmlTemplate.getBattleInfo();
				if (battleArrayObject == null) {
					continue;
				}
			}
			
			ArrayList<Battle> battles = battleArrayObject.getBattlearrays();
			int length = battles.size();
			int max = Math.min(length, 3);
			for (int j = 0; j < max; j++) {
				if (battles.get(j).getItemid() != 0) {
					enemyBuilder.addDicipleItemID(battles.get(j).getItemid());
				}
			}
			enemyBuilder.setId(challenge.getPlayerid());
			enemyBuilder.setName(challenge.getName());
			enemyBuilder.setLevel(challenge.getLevel());
			enemyBuilder.setArenaRank(challenge.getRank());
			enemyBuilder.setRewardScore(getAddScoreByRank(challenge.getRank()));
			if (challenge.getRank() > rank) {
				enemyBuilder.setArenaType(4);
			}
			else if (challenge.getRank() >= rank - gap * frontCount) {
				enemyBuilder.setArenaType(3);
			}
			else{
				enemyBuilder.setArenaType(1);
			} 
			
			builder.addArenaOpponents(enemyBuilder);
		}
		
		player.setLastVerifyTime(Util.getServerTime());
		
		builder.setServertime(Util.getServerTime());
		
		player.sendPacket(OP.code.OPCODE_GET_ARENA_OPPONENT_LISTRET_S_VALUE, builder);
	}
	
	public static void refreshScore(Packet packet,IoSession session){
		OPRefreshArenaScore params = null;
		OPRefreshArenaScoreRet.Builder builder = OPRefreshArenaScoreRet.newBuilder();
		
		try {
			params = OPRefreshArenaScore.parseFrom(packet.data.array());
		} catch (InvalidProtocolBufferException e) {
			e.printStackTrace();
		}
		
		int version = params.getVersion();
		
		if (version != 1) {
			//TODO 版本控制
		}
		
		int playerId = (Integer)session.getAttribute("playerid");
		Player player = ObjectAccessor.getOnlineUser(playerId);
		
		if (player.getLevel() < 5) {
			ErrorReporter.OPErrorReporter.Builder eBuilder = ErrorReporter.OPErrorReporter.newBuilder();
            eBuilder.setErrorid(OP.code.OPCODE_REFRESH_ARENARET_S_VALUE);
            eBuilder.setErrormsg("level less than 5");
            player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, eBuilder);
            return;
		}
		
		Challenge challenge = player.getChallenge();
		if (challenge == null) {
			challenge = getChallenge(playerId);
			player.setChallenge(challenge);
		}
		
		challenge.setLevel(player.getLevel());
		challenge.setName(player.getName());
		
		calcScore(challenge);
		
		player.setLastVerifyTime(Util.getServerTime());
		
		builder.setArenaScore(challenge.getScore());
		builder.setServertime(Util.getServerTime());
		
		player.sendPacket(OP.code.OPCODE_REFRESH_ARENARET_S_VALUE, builder);
	}
	
	public static void getReward(Packet packet,IoSession session){
		OPGetArenaReward params = null;
		OPGetArenaRewardRet.Builder builder = OPGetArenaRewardRet.newBuilder();
		
		try {
			params = OPGetArenaReward.parseFrom(packet.data.array());
		} catch (InvalidProtocolBufferException e) {
			e.printStackTrace();
			return;
		}
		
		int id = params.getRewardID();
		
		int playerId = (Integer)session.getAttribute("playerid");
		Player player = ObjectAccessor.getOnlineUser(playerId);
		
		if (player.getLevel() < 5) {
			ErrorReporter.OPErrorReporter.Builder eBuilder = ErrorReporter.OPErrorReporter.newBuilder();
            eBuilder.setErrorid(OP.code.OPCODE_GET_ARENA_REWARDRET_S_VALUE);
            eBuilder.setErrormsg("level less than 5");
            player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, eBuilder);
            return;
		}
		
		/******************************************************/
		boolean isGetAddReward=false;
		UserStats userStats=player.getUserStats();
		//相关统计及奖励活动开启判定
		long current_time=Util.getServerMillTime();
		int termId=Util.getActivityTermId(current_time, XMLTemplateService.getTiamzhanTimeConfigs());
		TiaozhanPoolConfig poolConfig=XMLTemplateService.getTiaozhanPoolConfig();
		/******************************************************/
		Challenge challenge = player.getChallenge();
		if (challenge == null) {
			challenge = getChallenge(playerId);
			if (challenge == null) {
				challenge = newChallengeInfo(playerId, player.getLevel(), player.getName());
			}
			player.setChallenge(challenge);
		}
		
		challenge.setLevel(player.getLevel());
		challenge.setName(player.getName());
		
		calcScore(challenge);
		
		int[] rewardConfig = getRewardConfigById(id);
		
		if (rewardConfig[0] == 1) {//重复领奖类型
			if (challenge.getScore() < rewardConfig[1]) {//积分不够
				OPErrorReporter.Builder errBuilder = OPErrorReporter.newBuilder();
				errBuilder.setErrorid(OP.code.OPCODE_CHALLENGE_OPPONENTRET_S_VALUE);
				errBuilder.setErrormsg("not enough score");
				player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errBuilder);
				return;
			}
			challenge.setScore(challenge.getScore() - rewardConfig[1]);
			Util.rewardBagTool(player, 1502001, rewardConfig[2]);
		}
		else if (rewardConfig[0] == 2) {//最大排名领奖类型
			if (challenge.getHighestrank() > rewardConfig[1]) {//未达到名次要求
				OPErrorReporter.Builder errBuilder = OPErrorReporter.newBuilder();
				errBuilder.setErrorid(OP.code.OPCODE_CHALLENGE_OPPONENTRET_S_VALUE);
				errBuilder.setErrormsg("rank not enough");
				player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errBuilder);
				return;
			}
			ArrayList<Integer> hasGotsArray = new ArrayList<Integer>();
			if (challenge.getHasgotreward() != null && !challenge.getHasgotreward().equals("")) {
				String[] hasGots = challenge.getHasgotreward().split(",");
				
				for (int i = 0; i < hasGots.length; i++) {
					hasGotsArray.add(Integer.parseInt(hasGots[i]));
				}
				if (hasGotsArray.contains(id)) {
					OPErrorReporter.Builder errBuilder = OPErrorReporter.newBuilder();
					errBuilder.setErrorid(OP.code.OPCODE_CHALLENGE_OPPONENTRET_S_VALUE);
					errBuilder.setErrormsg("already got");
					player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errBuilder);
					return;
				}
			}
			hasGotsArray.add(id);
			String newGot = "";
			if (hasGotsArray.size() > 0) {
				newGot = newGot + hasGotsArray.get(0);
				for (int i = 1; i < hasGotsArray.size(); i++) {
					newGot = newGot + "," + hasGotsArray.get(i);
				}
			}
			challenge.setHasgotreward(newGot);
			Util.rewardBagTool(player, 1502001, rewardConfig[2]);
		}
		else if(rewardConfig[0]==0) {
			if(termId!=0){
				int addRewardIndex=poolConfig.getArenaRewardIndex();
				int rankLimit=poolConfig.getRankLimit();
				int rewardType=poolConfig.getArenaRewardType();
				int rewardId=poolConfig.getArenaRewardId();
				int type=poolConfig.getType();
				int costCount=poolConfig.getArenaCost();
				int rewardCount=poolConfig.getRewardCount();
				if (userStats.getHighestRankInAct() > rankLimit) {//未达到名次要求
					OPErrorReporter.Builder errBuilder = OPErrorReporter.newBuilder();
					errBuilder.setErrorid(OP.code.OPCODE_CHALLENGE_OPPONENTRET_S_VALUE);
					errBuilder.setErrormsg("HighestRankInAct not enough");
					player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errBuilder);
					return;
				}
				else if(challenge.getScore()<costCount){//积分不够
					OPErrorReporter.Builder errBuilder = OPErrorReporter.newBuilder();
					errBuilder.setErrorid(OP.code.OPCODE_CHALLENGE_OPPONENTRET_S_VALUE);
					errBuilder.setErrormsg("not enough score");
					player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errBuilder);
					return;
				}
				ArrayList<Integer> hasGotsArray = new ArrayList<Integer>();
				if (challenge.getHasgotreward() != null && !challenge.getHasgotreward().equals("")) {
					String[] hasGots = challenge.getHasgotreward().split(",");
					
					for (int i = 0; i < hasGots.length; i++) {
						hasGotsArray.add(Integer.parseInt(hasGots[i]));
					}
					if (hasGotsArray.contains(addRewardIndex)) {
						OPErrorReporter.Builder errBuilder = OPErrorReporter.newBuilder();
						errBuilder.setErrorid(OP.code.OPCODE_CHALLENGE_OPPONENTRET_S_VALUE);
						errBuilder.setErrormsg("already got");
						player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errBuilder);
						return;
					}
				}
				hasGotsArray.add(addRewardIndex);
				String newGot = "";
				if (hasGotsArray.size() > 0) {
					newGot = newGot + hasGotsArray.get(0);
					for (int i = 1; i < hasGotsArray.size(); i++) {
						newGot = newGot + "," + hasGotsArray.get(i);
					}
				}
				challenge.setHasgotreward(newGot);
				challenge.setScore(challenge.getScore() - costCount);
				Util.rewardBagTool(player, rewardId, rewardCount);
				isGetAddReward=true;
			}
		}
		
		DZM.getEntityManager().updateSync(challenge);
		player.setChallenge(challenge);
		
		player.setLastVerifyTime(Util.getServerTime());
		
		builder.setArenaScore(challenge.getScore());
		
		ArrayList<Integer> hasGot = new ArrayList<Integer>();
		if (challenge.getHasgotreward() != null && !challenge.getHasgotreward().equals("")) {
			String[] rewards = challenge.getHasgotreward().split(",");
			for (int i = 0; i < rewards.length; i++) {
				hasGot.add(Integer.parseInt(rewards[i]));
				builder.addGotRewardIDs(Integer.parseInt(rewards[i]));
			}
		}
		for (int i = 0; i < rankAwards.size(); i++) {
			if (!hasGot.contains(rankAwards.get(i)) && challenge.getHighestrank() <= getRewardConfigById(rankAwards.get(i))[1]) {
				builder.addRewardIDs(rankAwards.get(i));
			}
		}
		
		OPUserToolInfoRet.ToolInfo.Builder toolBuilder = OPUserToolInfoRet.ToolInfo.newBuilder();
		if(isGetAddReward){
			//如果是领取增加的奖励,则添加该道具的回包信息
			Bag tool=player.getBags().get(poolConfig.getArenaRewardId());
			toolBuilder.setId(tool.getId());
			toolBuilder.setCount(tool.getCount());
			toolBuilder.setItemid(tool.getItemid());
		}else{
			Bag tool = player.getBags().get(1502001);
			toolBuilder.setId(tool.getId());
			toolBuilder.setItemid(tool.getItemid());
			toolBuilder.setCount(tool.getCount());
		}
		builder.setReward(toolBuilder);
		builder.setServertime(Util.getServerTime());
		
		player.sendPacket(OP.code.OPCODE_GET_ARENA_REWARDRET_S_VALUE, builder);
	}
	
	
	public static void doChallenge(Packet packet,IoSession session){
		OPChallengeOpponent params = null;
		OPChallengeOpponentRet.Builder builder = OPChallengeOpponentRet.newBuilder();
		builder.setServertime(Util.getServerTime());
		
		try {
			params = OPChallengeOpponent.parseFrom(packet.data.array());
		} catch (InvalidProtocolBufferException e) {
			e.printStackTrace();
			return;
		}
		
		int enemyId = params.getOpponentID();
		int enemyRank = params.getOpponentRank();
		
		int playerId = (Integer)session.getAttribute("playerid");
		Player player = ObjectAccessor.getOnlineUser(playerId);
		
		if (player.getLevel() < 5) {
			ErrorReporter.OPErrorReporter.Builder eBuilder = ErrorReporter.OPErrorReporter.newBuilder();
            eBuilder.setErrorid(OP.code.OPCODE_CHALLENGE_OPPONENTRET_S_VALUE);
            eBuilder.setErrormsg("level less than 5");
            player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, eBuilder);
            return;
		}
		
		//相关统计及奖励活动开启判定
		long current_time=Util.getServerMillTime();
		UserStats userStats=player.getUserStats();
		int termId=Util.getActivityTermId(current_time, XMLTemplateService.getTiamzhanTimeConfigs());
		
		Challenge userChallenge = player.getChallenge();
		if (userChallenge == null) {
			userChallenge = getChallenge(playerId);
			if (userChallenge == null) {
				userChallenge = newChallengeInfo(playerId, player.getLevel(), player.getName());
			}
		}
		
		int userRank = userChallenge.getRank();
		
		int addScore = calcScore(userChallenge);
		
		Challenge enemyChallenge = getChallenge(enemyId);
		if (enemyChallenge == null) {
			builder.setStatus(false);
			builder.setVitality(player.getVitalitybypellet() + player.getVitalitybytime());
			builder.setArenaRank(userChallenge.getRank());
			builder.setArenaScore(userChallenge.getScore());
			builder.setArenaNum(player.getUserStats().getChallengetimes());
			builder.setRewardScore(addScore);
			builder.setRewardRefreshTime(userChallenge.getLastrefreshtime());
			player.sendPacket(OP.code.OPCODE_CHALLENGE_OPPONENTRET_S_VALUE, builder);
			return ;
			
		}
		int realEnemyRank = enemyChallenge.getRank();
		if (enemyRank != realEnemyRank || enemyRank > userRank) {//论剑排名未同步
			builder.setStatus(false);
			builder.setVitality(player.getVitalitybypellet() + player.getVitalitybytime());
			builder.setArenaRank(userChallenge.getRank());
			builder.setArenaScore(userChallenge.getScore());
			builder.setArenaNum(player.getUserStats().getChallengetimes());
			builder.setRewardScore(addScore);
			builder.setRewardRefreshTime(userChallenge.getLastrefreshtime());
			player.sendPacket(OP.code.OPCODE_CHALLENGE_OPPONENTRET_S_VALUE, builder);
			return ;
		}
		
		int leftChallengeTimes = player.getUserStats().getChallengetimes();
		
		if (leftChallengeTimes <= 0 || player.getVitalitybytime() + player.getVitalitybypellet() < 1) {
			builder.setStatus(false);
			builder.setVitality(player.getVitalitybypellet() + player.getVitalitybytime());
			builder.setArenaRank(userChallenge.getRank());
			builder.setArenaScore(userChallenge.getScore());
			builder.setArenaNum(player.getUserStats().getChallengetimes());
			builder.setRewardScore(addScore);
			builder.setRewardRefreshTime(userChallenge.getLastrefreshtime());
			player.sendPacket(OP.code.OPCODE_CHALLENGE_OPPONENTRET_S_VALUE, builder);
			return ;
		}
		
		Player enemy = null;
		BattleArrayObject userBattleArray = BattleUtil.getUserBattleArrayObject(player);
		BattleArrayObject enemyBattleArray = new BattleArrayObject();
		if (enemyChallenge.getIsnpc() == 0) {
			enemy = ObjectAccessor.getOnlineUser(enemyId);
			if (enemy == null) {
				enemy = new Player();
				enemy.setId(enemyId);
			}
			else {
				enemyChallenge.setName(enemy.getName());
				enemyChallenge.setLevel(enemy.getLevel());
			}
			enemyBattleArray = BattleUtil.getUserBattleArrayObject(enemy);
		}
		else {
			int careerId = enemyChallenge.getPlayerid() / 100000;
			enemyBattleArray = XMLTemplateService.getCareerTemplate(careerId).getBattleInfo();
		}
		
		calcScore(enemyChallenge);
		
		Fight fight = new Fight();
		fight.doFight(userBattleArray, enemyBattleArray, null);
		
		Round round1 = fight.getRound1();
		Round round2 = fight.getRound2();
		FinalRound round3 = fight.getFinalRound();
		int fightResult = fight.getResult();
		ArrayList<CharacterResult> characterResults = new ArrayList<CharacterResult>();
		
		OPUserBattleRet.Builder battleBuilder = OPUserBattleRet.newBuilder();
		battleBuilder.setVersion(1);
		battleBuilder.setServertime(Util.getServerTime());
		battleBuilder.setRound1(UserBattleHandler.getRoundBuilder(round1));
		battleBuilder.setRound2(UserBattleHandler.getRoundBuilder(round2));
		battleBuilder.setRound3(UserBattleHandler.getFinalRound(round3));
		
		Result result = new Result();
		result.setWinlose(fightResult);
		boolean isWin=false;
		if (fightResult <= 2) {//战斗胜利
            isWin=true;
			UserLevelConfig levelConfig = XMLTemplateService.getUserLevelConfig(player.getLevel());
			int gainCoins = levelConfig.getChallengeCoins();
			int gainUserExp = levelConfig.getCareerXp();
			int gainDiscipleExp = levelConfig.getChallengeDiscipleXp();
			
			result.setGotCoin(gainCoins);
			result.setGotExp(gainUserExp);
			
			//弟子属性变化
			ArrayList<Battle> userBattles = userBattleArray.getBattlearrays();
			for (int i = 0; i < userBattles.size(); i++) {
				CharacterResult characterResult = new CharacterResult();
				Battle battle = userBattles.get(i);
				if (battle.getDiscipleid() == 0) {
					continue;
				}
				Disciple disciple = player.getDisciples().get(battle.getDiscipleid());
				
				int addLevel = Util.getlevelByExp(disciple, gainDiscipleExp);
				if (addLevel > 0) {//弟子升级
					OPUserDiscipleInfoRet.DiscipleInfo.Builder discipleBuilder = DiscipleUtil.buildDiscipleBuilder(disciple);
					battleBuilder.addDisciple(discipleBuilder);
					characterResult.setIsUpgrade(true);
					//重新检查阵容属性
					BattleUtil.resetBattleProperty(battle, disciple, player,null,0);
					OPUserBattleArrayRet.BattleArrayInfo.Builder builder2 = OPUserBattleArrayRet.BattleArrayInfo.newBuilder();
					BattleUtil.buildBattleBuilder(builder2, battle, i);
					battleBuilder.addBattleArrayInfo(builder2);
				}
				else {
					characterResult.setIsUpgrade(false);
				}
				characterResult.setItemid(disciple.getItemid());
				characterResult.setGainExp(gainDiscipleExp);
				characterResult.setLevel(disciple.getLevel());
				characterResults.add(characterResult);
			}
			
			player.setSilvercoins(player.getSilvercoins() + gainCoins);
//			player.setExp(player.getExp() + gainUserExp);
			Util.getUserLevelByExp(player, gainUserExp);
			
			userChallenge.setRank(enemyRank);
			if (enemyRank < userChallenge.getHighestrank()) {
				userChallenge.setHighestrank(enemyRank);
			}
			//部分玩家出现达到相应名次领不到称号的问题,改为每次决斗胜利都检查次称号,检测称号的required为玩家最高挑战排行 20130717 by jinxin
			TitleUtil.checkNewTitle(player, 3, userChallenge.getHighestrank());
			enemyChallenge.setRank(userRank);
			//如果活动期间挑战胜利,则更新自己的活动期间最高达到的排名
			if(userStats.getHighestRankInAct()==0 || userStats.getHighestRankInAct()>enemyRank){
				userStats.setHighestRankInAct(enemyRank);
			}
		}
		//活动统计
		//如果被挑战的是排行第一的玩家,则在日志中记录相关信息
		if(termId!=0 && enemyRank==1 && isWin){
			//如果lastChangeTime=0,则将期初始化为活动开启时间
			if(lastChangeTime==0){
				lastChangeTime=Util.getActivityTimeByTermid(XMLTemplateService.getTiamzhanTimeConfigs(), termId)[0];
			}
			//如果activityEndTime=0.则将期初始化为活动结束时间
			if(activityEndTime==0){
				activityEndTime=Util.getActivityTimeByTermid(XMLTemplateService.getTiamzhanTimeConfigs(), termId)[1];
			}
			activate_log.info("record for  challenge activity:------{},{},{},{},{},{},{}", new Object[]{playerId , enemyChallenge.getPlayerid(), Util.getServerTime(), lastChangeTime, activityEndTime,player.getName(),enemyChallenge.getName()});
			//记录第一名变更时间戳
			lastChangeTime=Util.getServerTime();
			
		}
		userChallenge.setLevel(player.getLevel());
		userChallenge.setName(player.getName());
		player.setChallenge(userChallenge);
		
		if (enemy != null) {
			enemy.setChallenge(enemyChallenge);
		}
		
		DZM.getEntityManager().updateSync(userChallenge);
		DZM.getEntityManager().updateSync(enemyChallenge);
		
		UserStats stats = player.getUserStats();
		stats.setChallengetimes(leftChallengeTimes - 1);
		player.setUserStats(stats);
		int leftVitality = player.useVitality(1);
        //add by dylan 论剑传书信息
		if(enemyChallenge.getIsnpc()==0)
        {
            SysMsgInfoServices.cometChallengeMsg(player,enemy,enemyChallenge.getRank(),isWin);
        }
		builder.setVitality(leftVitality);
		builder.setStatus(true);
		builder.setArenaRank(userChallenge.getRank());
		builder.setArenaScore(userChallenge.getScore());
		builder.setArenaNum(leftChallengeTimes - 1);
		builder.setRewardScore(calcScore(userChallenge));
		builder.setRewardRefreshTime(userChallenge.getLastrefreshtime());
		result.setCharacterResults(characterResults);
		battleBuilder.setResult(UserBattleHandler.getResult(player,result));
		battleBuilder.setTeamArchievements(fight.getTeamBuilder());
		builder.setUserBattleRet(battleBuilder);
		builder.setOpponentID(enemyId);
		
		player.sendPacket(OP.code.OPCODE_CHALLENGE_OPPONENTRET_S_VALUE, builder);
	}
	
	private static ArrayList<Challenge> getChallenges(ArrayList<Integer> ranks){
		ArrayList<Challenge> _list=null;
        Session session=DZM.getEntityManager().getSessionFactory();
        Transaction tx = session.beginTransaction( ) ;
		
		try
        {
			String sql = "select playerid,level,name,isnpc,rank,highest_rank,score,hasgotreward,lastsettletime,lastrefreshtime from challenge where rank in (:ranks)";
			SQLQuery query = session.createSQLQuery( sql ) ;
			query.setParameterList("ranks", ranks);
	        query.addEntity(Challenge.class);
	        _list=(ArrayList<Challenge>) query.list();
	        session.clear();
	        tx.commit() ;
        }catch (HibernateException e) {
        	if (tx!=null)
            {
                tx.rollback();
            }
            throw e;
		}
		
		return _list;
	}
	
	private static int getListGap(int rank){
		int gap = 1;
		if (rank > 10) {
			gap = (int)Math.floor(Math.pow(rank, 0.44));
		}
		return gap;
	}
	
	public static Challenge getChallenge(int playerId){
		List<Object> challenges = DZM.getEntityManager().limitQuery("from Challenge where playerid = ?", 0, 1, playerId);
		Challenge challenge = null;
		if (challenges.size() == 1) {
			challenge = (Challenge)challenges.get(0);
		}
		
		return challenge;
	}
	
	private static Challenge newChallengeInfo(int playerid,int level,String name){
		Challenge challenge = new Challenge();
		challenge.setPlayerid(playerid);
		challenge.setLevel(level);
		challenge.setName(name);
		challenge.setRank(DZM.totalChallengeCount + 1);
		challenge.setHighestrank(DZM.totalChallengeCount + 1);
		DZM.getEntityManager().createSync(challenge);
		DZM.addChallengeCount();
		return challenge;
	}
	
	private static int calcScore(Challenge challenge){
		int rankAddScore = getAddScoreByRank(challenge.getRank());
		if (challenge.getLastsettletime() == 0) {
			challenge.setLastsettletime(Util.getServerTime());
			challenge.setScore(0);
			DZM.getEntityManager().updateSync(challenge);
		}
		else if (Util.getServerTime() - challenge.getLastsettletime() >= 600 && challenge.getIsnpc() != 1) {//10分钟重新计算积分
			int period = (Util.getServerTime() - challenge.getLastsettletime())/600;
			int addScore = rankAddScore * period;
			challenge.setScore(challenge.getScore() + addScore);
			challenge.setLastsettletime(challenge.getLastsettletime() + 600 * period);
			DZM.getEntityManager().updateSync(challenge);
		}
		return rankAddScore;
	}
	
	private static int getAddScoreByRank(int rank) {
		int addScore = 0;
		if (rank >= 200) {
			addScore = (int)Math.floor(345 - Math.log(rank) * 30);
			if (addScore < 15) {
				addScore = 15;
			}
		}
		else if (rank < 10) {
			switch (rank) {
				case 1:
					addScore = 300;
					break;
				case 2:
					addScore = 280;
					break;
				case 3:
					addScore = 265;
					break;
				case 4:
					addScore = 255;
					break;
				case 5:
					addScore = 245;
					break;
				case 6:
					addScore = 235;
					break;
				case 7:
					addScore = 230;
					break;
				case 8:
					addScore = 225;
					break;
				case 9:
					addScore = 220;
					break;
			}
		}
		else {
			int rankTen = rank / 10;
			switch (rankTen) {
				case 1:
					addScore = 215;
					break;
				case 2:
					addScore = 214;
					break;
				case 3:
					addScore = 213;
					break;
				case 4:
					addScore = 212;
					break;
				case 5:
					addScore = 211;
					break;
				case 6:
					addScore = 210;
					break;
				case 7:
					addScore = 208;
					break;
				case 8:
					addScore = 206;
					break;
				case 9:
					addScore = 204;
					break;
				case 10:
				case 11:
					addScore = 200;
					break;
				case 12:
				case 13:
				case 14:
					addScore = 195;
					break;
				case 15:
				case 16:
				case 17:
				case 18:
				case 19:
					addScore = 190;
					break;
			}
		}
		return addScore;
	}
	
	private static int[] getRewardConfigById(int id){
		int[] reward = new int[]{0,0,0};
		switch (id) {
			case 1:
				reward[0] = 1;
				reward[1] = 1000;
				reward[2] = 5;
				break;
			case 2:
				reward[0] = 1;
				reward[1] = 10000;
				reward[2] = 50;
				break;
			case 3:
				reward[0] = 2;
				reward[1] = 1000;
				reward[2] = 300;
				break;
			case 4:
				reward[0] = 2;
				reward[1] = 500;
				reward[2] = 300;
				break;
			case 5:
				reward[0] = 2;
				reward[1] = 200;
				reward[2] = 300;
				break;
			case 6:
				reward[0] = 2;
				reward[1] = 100;
				reward[2] = 300;
				break;
			case 7:
				reward[0] = 2;
				reward[1] = 50;
				reward[2] = 300;
				break;
			case 8:
				reward[0] = 2;
				reward[1] = 10;
				reward[2] = 600;
				break;
			case 9:
				reward[0] = 2;
				reward[1] = 1;
				reward[2] = 600;
				break;
		}
		return reward;
	}
}

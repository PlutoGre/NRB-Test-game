package com.oldmountain.dzm.services;

import java.sql.Timestamp;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Calendar;
import java.util.Collections;
import java.util.Date;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Random;

import javax.xml.bind.annotation.XmlAccessOrder;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import sun.misc.CEFormatException;
import net.sf.json.JSONObject;

import com.google.protobuf.Message.Builder;
import com.mchange.v2.c3p0.stmt.GooGooStatementCache;
import com.oldmountain.dzm.DZM;
import com.oldmountain.dzm.configEntity.GuildConfig;
import com.oldmountain.dzm.configEntity.GuildLevelConfig;
import com.oldmountain.dzm.configEntity.GuildNpcConfig;
import com.oldmountain.dzm.configEntity.GuildShopSell;
import com.oldmountain.dzm.entity.Bag;
import com.oldmountain.dzm.entity.Battle;
import com.oldmountain.dzm.entity.BattleArrayObject;
import com.oldmountain.dzm.entity.CharacterResult;
import com.oldmountain.dzm.entity.DiscipleXmlTemplate;
import com.oldmountain.dzm.entity.Equip;
import com.oldmountain.dzm.entity.FinalRound;
import com.oldmountain.dzm.entity.Leagua;
import com.oldmountain.dzm.entity.LeaguaApply;
import com.oldmountain.dzm.entity.LeaguaMedal;
import com.oldmountain.dzm.entity.LeaguaMembers;
import com.oldmountain.dzm.entity.LeaguaObject;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.entity.Result;
import com.oldmountain.dzm.entity.Round;
import com.oldmountain.dzm.entity.Skill;
import com.oldmountain.dzm.entity.Soul;
import com.oldmountain.dzm.message.ErrorReporter.OPErrorReporter;
import com.oldmountain.dzm.message.FightEndInfo.OPFightEndInfoRet;
import com.oldmountain.dzm.message.LeagueStruct.*;
import com.oldmountain.dzm.message.LeagueStructExt.*;
import com.oldmountain.dzm.message.OP;
import com.oldmountain.dzm.message.OPLeague;
import com.oldmountain.dzm.message.UserBattle.OPUserBattleRet;
import com.oldmountain.dzm.net.ObjectAccessor;
import com.oldmountain.dzm.net.packet.Packet;
import com.oldmountain.dzm.packethandler.UserBattleHandler;
import com.oldmountain.dzm.util.BagUtil;
import com.oldmountain.dzm.util.BattleUtil;
import com.oldmountain.dzm.util.EquipUtil;
import com.oldmountain.dzm.util.Fight;
import com.oldmountain.dzm.util.SkillUtil;
import com.oldmountain.dzm.util.SoulUtil;
import com.oldmountain.dzm.util.UserToolConst;
import com.oldmountain.dzm.util.Util;
import com.oldmountain.dzm.util.XMLTemplateService;
import com.sun.org.apache.bcel.internal.generic.NEW;

class LeaguaDefine{
	public static final int BLANK_MEDAL_ID = 1;
	public static final int DEFAULT_MEDAL_ID = 2;
	public static final int SHOP_ID = 1;
	public static final int BANK_ID = 2;
	public static final int BASE_ID = 12;
	public static final int TOTEM_TYPE = 3;
	public static final int BUFFBUILDING_TYPE = 4;
	public static final int GRADE_MEMBER = 1;
	public static final int GRADE_CAPTAIL = 2;
	public static final int GRADE_VICELEADER = 3;
	public static final int GRADE_LEADER = 4;
	public static final int VICELEADER_MAX_COUNT = 1;
	public static final int CAPTAIL_MAX_COUNT = 4;
	public static int getGradeMemberCount(int grade){
		int result = 0;
		switch (grade) {
			case 3:
				result = VICELEADER_MAX_COUNT;
				break;
			case 2:
				result = CAPTAIL_MAX_COUNT;
				break;
		}
		return result;
	}
	public static final ArrayList<Integer> TOTEMIDS = new ArrayList<Integer>(Arrays.asList(3,4,5,6,7));
	public static final ArrayList<Integer> BUFFBUILDINGS = new ArrayList<Integer>(Arrays.asList(8,9,10,11));
}

public class LeaguaServices {
	private static Logger log = LoggerFactory.getLogger("ServerLog");
	public static HashMap<Integer, LeaguaObject> allLeaguaObject = new HashMap<Integer, LeaguaObject>();
	public static ArrayList<String> allLeaguaNames = new ArrayList<String>();
	public static HashMap<Integer,Leagua> allLeagua = new HashMap<Integer,Leagua>();
	public static ArrayList<Leagua>leaguaRank = new ArrayList<Leagua>();
	public static HashMap<Integer, ArrayList<Integer>> levelLeaguaIds = new HashMap<Integer,ArrayList<Integer>>();
	public static HashMap<Integer, LeaguaMedal> allSpecialLeaguaMedals = new HashMap<Integer,LeaguaMedal>();

	
	public static Builder refreshBroadcast(Player player, String broadcast) {
		OPRefreshLeaguaBroadcastRet.Builder builder=OPRefreshLeaguaBroadcastRet.newBuilder();
		int status=2;
		
		int leaguaId = player.getLeaguaid();
		if (leaguaId == 0 || !allLeaguaObject.containsKey(leaguaId)) {
			return builder;
		}
		LeaguaObject leaguaObject = allLeaguaObject.get(leaguaId);
		int serverTime=Util.getServerTime();
		Leagua leagua = leaguaObject.getLeaguaInfo();
		GuildConfig guildConfig=XMLTemplateService.getGuildConfig();
		int refreshCd=guildConfig.getBroadcastRefreshCD();
		if(serverTime-leagua.getBroadcastRefreshTime() < refreshCd || player.getLevel() < guildConfig.getLevelLimit()){
			status=3;
		}
		else{
			leagua.setLeaguaBroadcast(broadcast);
			leagua.setBroadcastRefreshTime(serverTime);
			leaguaObject.updateLeaguaInfo(leagua);
			status=1;
		}
		builder.setNewBroadcast(broadcast);
		builder.setStatus(status);
		return builder;
	}
	
	public static OPReceiveTotemContributionRet.Builder receiveTotemContribution(Player player){
		OPReceiveTotemContributionRet.Builder builder = OPReceiveTotemContributionRet.newBuilder();
		
		builder.setStatus(0);
		builder.setReceiveContribution(0);
		
		int leaguaId = player.getLeaguaid();
		if (leaguaId == 0 || !allLeaguaObject.containsKey(leaguaId)) {
			return builder;
		}
		// 判断时间
		Calendar calendar = Calendar.getInstance();
		int hour = calendar.get(Calendar.HOUR_OF_DAY);
		int minute = calendar.get(Calendar.MINUTE);
		int day = calendar.get(Calendar.DAY_OF_MONTH);
		int month = calendar.get(Calendar.MONTH) + 1;
		int date = month * 100 + day;
		if (hour != 19 || minute >= 30) {
			builder.setStatus(2);
			return builder;
		}
		// 判断是否能领取
		LeaguaObject leaguaObject = allLeaguaObject.get(leaguaId);
		LeaguaMembers memberInfo = leaguaObject.getLeaguaMembersInfo().get(player.getId());
		if (memberInfo.getReccontributiontime() == date) {
			builder.setStatus(3);
			return builder;
		}
		
		
		Leagua leagua = leaguaObject.getLeaguaInfo();
		JSONObject totemsInfo = getLeaguaTotemInfo(leagua);
		Iterator<String> keys = totemsInfo.keys();
		int totalHealth = 0;
		while (keys.hasNext()) {
			String key = keys.next();
			JSONObject totemInfo = checkTotemInfo(totemsInfo.getJSONObject(key));
			totalHealth += totemInfo.getInt("mh");
		}
		
		float rate = getReceiveContributionRate(leagua.getContributionCount(), totalHealth);
		float guildCoinRatio = XMLTemplateService.getGuildConfig().getGetGuildCoinRatio();
		
		int canGet = (int)Math.floor(leagua.getContributionCount() * rate * guildCoinRatio / leagua.getLeaguaMaxMemberCount());
		
		memberInfo.increasePlayerLeftContribution(canGet);
		memberInfo.increasePlayerTotalContribution(canGet);
		
		memberInfo.setReccontributiontime(date);
		leaguaObject.updateOneMember(memberInfo);
		
		builder.setStatus(1);
		builder.setReceiveContribution(canGet);
		
		return builder;
	}
	
	public static OPClearShopBuyCdRet.Builder clearShopBuyCd(Player player){
		OPClearShopBuyCdRet.Builder builder = OPClearShopBuyCdRet.newBuilder();
		builder.setStatus(0);
		int leaguaId = player.getLeaguaid();
		if (leaguaId == 0 || !allLeaguaObject.containsKey(leaguaId)) {
			return builder;
		}
		LeaguaObject leaguaObject = allLeaguaObject.get(leaguaId);
		Leagua leagua = leaguaObject.getLeaguaInfo();
		LeaguaMembers memberInfo = leaguaObject.getLeaguaMembersInfo().get(player.getId());
		
		int lastBuyTime = memberInfo.getLastBuyTime();
		int cdTime = XMLTemplateService.getGuildConfig().getLevelConvigByLevel(leagua.getLeaguaLevel()).getShoppingCD();
		if (lastBuyTime + cdTime <= Util.getServerTime()) {
			return builder;
		}
		int cost = XMLTemplateService.getGuildConfig().getShopWinFreshCost();
		if (player.getGoldCoins() < cost) {
			return builder;
		}
		
		memberInfo.setLastBuyTime(0);
		leaguaObject.updateOneMember(memberInfo);
		player.consumeGold(cost);
		builder.setStatus(1);
		builder.setGoldcoins(player.getGoldCoins());
		
		return builder;
	}
	
	public static OPGetTotemRepairCostRet.Builder getTotemRepairCost(Player player,int totemId){
		OPGetTotemRepairCostRet.Builder builder = OPGetTotemRepairCostRet.newBuilder();
		
		builder.setCost(String.valueOf(0));
		builder.setType(1);
		
		int serverTime = Util.getServerTime();
		
		int leaguaId = player.getLeaguaid();
		if (leaguaId == 0 || !allLeaguaObject.containsKey(leaguaId)) {
			return builder;
		}
		LeaguaObject leaguaObject = allLeaguaObject.get(leaguaId);
		LeaguaMembers memberInfo = leaguaObject.getLeaguaMembersInfo().get(player.getId());
		if (memberInfo.getPlayerGrade() < LeaguaDefine.GRADE_LEADER) {
			return builder;
		}
		Leagua leagua = leaguaObject.getLeaguaInfo();
		JSONObject totemsInfo = getLeaguaTotemInfo(leagua);
		JSONObject totemInfo = checkTotemInfo(totemsInfo.getJSONObject(String.valueOf(totemId)));
		
		if (totemInfo == null) {
			return builder;
		}
		
		if (totemInfo.getInt("lh") >= totemInfo.getInt("mh") || totemInfo.containsKey("rt")) {
			return builder;
		}
		
		int cost = 0;
		
		JSONObject totemRepairCosts = leagua.getTotemRepairCosts();
		if (totemRepairCosts == null || !totemRepairCosts.containsKey(String.valueOf(totemId)) || totemRepairCosts.getJSONObject(String.valueOf(totemId)).getInt("t") < serverTime - 10) {
			String changeRateString = XMLTemplateService.getGuildConfig().getChangeRate();
			String[] rateArr = changeRateString.split(":");
			cost = (totemInfo.getInt("mh") - totemInfo.getInt("lh")) * Integer.parseInt(rateArr[1]);
			JSONObject status = new JSONObject();
			status.put("t", serverTime);
			status.put("c", cost);
			totemRepairCosts.put(String.valueOf(totemId), status);
			leagua.setTotemRepairCosts(totemRepairCosts);
		}
		else {
			cost = totemRepairCosts.getJSONObject(String.valueOf(totemId)).getInt("c");
		}
		
		builder.setType(1);
		builder.setCost(String.valueOf(cost));
		
		return builder;
	}
	
	public static OPRepairTotemRet.Builder repairLeaguaTotem(Player player,int totemId,int type){
		OPRepairTotemRet.Builder builder = OPRepairTotemRet.newBuilder();
		
		int serverTime = Util.getServerTime();
		
		builder.setStatus(0);
		int leaguaId = player.getLeaguaid();
		if (leaguaId == 0 || !allLeaguaObject.containsKey(leaguaId)) {
			return builder;
		}
		LeaguaObject leaguaObject = allLeaguaObject.get(leaguaId);
		LeaguaMembers memberInfo = leaguaObject.getLeaguaMembersInfo().get(player.getId());
		if (memberInfo.getPlayerGrade() < LeaguaDefine.GRADE_LEADER) {
			return builder;
		}
		Leagua leagua = leaguaObject.getLeaguaInfo();
		JSONObject totemsInfo = getLeaguaTotemInfo(leagua);
		Iterator<String> allKeys = totemsInfo.keys();
		while (allKeys.hasNext()) {
			String key = allKeys.next();
			JSONObject totem = totemsInfo.getJSONObject(key);
			if (totem.containsKey("rt")) {
				builder.setStatus(3);
				return builder;
			}
		}
		JSONObject totemInfo = checkTotemInfo(totemsInfo.getJSONObject(String.valueOf(totemId)));
		
		if (totemInfo.getInt("lh") >= totemInfo.getInt("mh") || totemInfo.containsKey("rt")) {
			return builder;
		}
		
		int cost = 0;
		JSONObject totemRepairCosts = leagua.getTotemRepairCosts();
		if (!totemRepairCosts.containsKey(String.valueOf(totemId)) || totemRepairCosts.getJSONObject(String.valueOf(totemId)).getInt("t") < serverTime - 10) {
			return builder;
		}
		else {
			cost = totemRepairCosts.getJSONObject(String.valueOf(totemId)).getInt("c");
		}
		
		
		if (type == 1) {
			if (leagua.getLeaguaFunds() < cost) {
				builder.setStatus(2);
				return builder;
			}
			leagua.setLeaguaFunds(leagua.getLeaguaFunds() - cost);
			builder.setLeaguaFunds(String.valueOf(leagua.getLeaguaFunds()));
		}
		else {
			if (player.getGoldCoins() < cost) {
				builder.setStatus(2);
				return builder;
			}
			
			player.consumeGold(cost);
		}
		
		
		builder.setStatus(1);
		totemInfo.put("rt", Util.getServerTime());
		totemsInfo.put(String.valueOf(totemId), totemInfo);
		
		Iterator<String> keys = totemsInfo.keys();
		while (keys.hasNext()) {
			String key = keys.next();
			JSONObject totembInfo = checkTotemInfo(totemsInfo.getJSONObject(key));
			builder.addTotemInfoList(buildLeaguaTotemBuilder(Integer.parseInt(key), totembInfo));
			totemsInfo.put(key, totembInfo);
		}
		leagua.setLeaguaTotemInfoFromJson(totemsInfo);
		leaguaObject.updateLeaguaInfo(leagua);
		
		return builder;
	}
	
	public static OPLeaguaShopBuyRet.Builder leaguaShopBuy(Player player,int id,int num){
		OPLeaguaShopBuyRet.Builder builder = OPLeaguaShopBuyRet.newBuilder();
		OPErrorReporter.Builder errBuilder = OPErrorReporter.newBuilder();
		
		builder.setStatus(0);
		builder.setCdTime(0);
		builder.setPlayerContribution(0);
		if (num <= 0) {
			return builder;
		}
		
		int leaguaId = player.getLeaguaid();
		if (leaguaId == 0 || !allLeaguaObject.containsKey(leaguaId) || num <= 0) {
			errBuilder.setErrorid(OPLeague.code.OPCODE_GET_LEAGUASHOPINFORET_S_VALUE);
			errBuilder.setErrormsg("data error");
			player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errBuilder);
			return builder;
		}
		
		LeaguaObject leaguaObject = allLeaguaObject.get(leaguaId);
		Leagua leagua = leaguaObject.getLeaguaInfo();
		LeaguaMembers member = leaguaObject.getLeaguaMembersInfo().get(player.getId());
		builder.setPlayerContribution(member.getPlayerLeftContribution());
		int lastBuyTime = member.getLastBuyTime();
		int cdTime = XMLTemplateService.getGuildConfig().getLevelConvigByLevel(leagua.getLeaguaLevel()).getShoppingCD();
		builder.setCdTime(lastBuyTime + cdTime - Util.getServerTime());
		if (lastBuyTime + cdTime > Util.getServerTime()) {
			builder.setStatus(5);
			return builder;
		}
		getMemberPurches(member);
		JSONObject buyInfo = member.getShopPurchaseInfoInJson();
		boolean needUpdateShop = getShopSells(leagua);
		
		JSONObject shopsInfo = leagua.getLeaguaShopInfoInJson();
		
		if (!shopsInfo.containsKey(String.valueOf(id))) {
			builder.setStatus(5);
			return builder;
		}
		
		JSONObject goodInfo = shopsInfo.getJSONObject(String.valueOf(id));
		
		if (goodInfo.getInt("per") > member.getPlayerGrade()) {
			builder.setStatus(3);
			return builder;
		}
		
		int alreadyBuyCount = 0;
		
		if (buyInfo.containsKey(String.valueOf(id))) {
			alreadyBuyCount = buyInfo.getJSONObject(String.valueOf(id)).getInt("c");
		}
		
		if (alreadyBuyCount + num > goodInfo.getInt("cnt")) {
			builder.setStatus(4);
			return builder;
		}
		
		int needContribution = goodInfo.getInt("pri") * num;
		
		if (needContribution <= 0) {
			builder.setStatus(4);
			return builder;
		}
		
		if (member.getPlayerLeftContribution() < needContribution) {
			builder.setStatus(2);
			return builder;
		}
		
		member.decreasePlayerLeftContribution(needContribution);
		
		int[] items = new int[]{goodInfo.getInt("gtype"),goodInfo.getInt("gId"),num};
		ArrayList<Integer> returns = Util.addRewards(player, items);
		
		switch (returns.get(0)) {
			case 0://背包
				Bag bag = player.getBags().get(returns.get(1));
				builder.setToolInfo(BagToolServices.buildToolBuilderForLeaguaBuy(bag));
				break;
			case 2://装备
				Equip equip = player.getEquips().get(returns.get(1));
				builder.setEquipInfo(EquipUtil.buildLeaguaEquipReturn(equip));
				break;
			case 3://技能
				Skill skill = player.getSkills().get(returns.get(1));
				builder.setSkillInfo(SkillUtil.buildSkillBuilderForLeagug(skill));
				break;
			case 4://魂魄
				Soul soul = player.getSouls().get(returns.get(1));
				builder.setSoulInfo(SoulUtil.buildSoulBuilderForLeagua(soul));
				break;
		}
		
		JSONObject object = new JSONObject();
		object.put("c", alreadyBuyCount + num);
		buyInfo.put(String.valueOf(id), object);
		member.setShopPurchaseInfo(buyInfo);
		member.setLastBuyTime(Util.getServerTime());
		leaguaObject.updateOneMember(member);
		if (needUpdateShop) {
			leaguaObject.updateLeaguaInfo(leagua);
		}
		builder.setPlayerContribution(member.getPlayerLeftContribution());
		builder.setCdTime(XMLTemplateService.getGuildConfig().getLevelConvigByLevel(leagua.getLeaguaLevel()).getShoppingCD());
		Iterator<String> keys = shopsInfo.keys();
		while (keys.hasNext()) {
			String key = keys.next();
			JSONObject goodsInfo = shopsInfo.getJSONObject(key);
			builder.addShopGoodInfoList(buildShopGoodInfo(goodsInfo, buyInfo));
		}
		builder.setStatus(1);
		return builder;
	}
	
	public static OPGetLeaguaShopInfoRet.Builder getLeaguaShopInfo(Player player){
		OPGetLeaguaShopInfoRet.Builder builder = OPGetLeaguaShopInfoRet.newBuilder();
		OPErrorReporter.Builder errBuilder = OPErrorReporter.newBuilder();
		builder.setCdTime(0);
		builder.setRefreshTime(0);
		int leaguaId = player.getLeaguaid();
		if (leaguaId == 0 || !allLeaguaObject.containsKey(leaguaId)) {
			errBuilder.setErrorid(OPLeague.code.OPCODE_GET_LEAGUASHOPINFORET_S_VALUE);
			errBuilder.setErrormsg("data error");
			player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errBuilder);
			return builder;
		}
		
		LeaguaObject leaguaObject = allLeaguaObject.get(leaguaId);
		Leagua leagua = leaguaObject.getLeaguaInfo();
		LeaguaMembers member = leaguaObject.getLeaguaMembersInfo().get(player.getId());
		boolean needUpdateMember = getMemberPurches(member);
		JSONObject buyInfo = member.getShopPurchaseInfoInJson();
		boolean needUpdateShop = getShopSells(leagua);
		
		JSONObject shopsInfo = leagua.getLeaguaShopInfoInJson();
		
		Iterator<String> keys = shopsInfo.keys();
		while (keys.hasNext()) {
			String key = keys.next();
			JSONObject goodInfo = shopsInfo.getJSONObject(key);
			builder.addShopGoodInfoList(buildShopGoodInfo(goodInfo, buyInfo));
		}
		if (needUpdateMember) {
			leaguaObject.updateOneMember(member);
		}
		if (needUpdateShop) {
			leaguaObject.updateLeaguaInfo(leagua);
		}
		// 需要进行刷新时间判定
		int cdtime = XMLTemplateService.getGuildConfig().getLevelConvigByLevel(leagua.getLeaguaLevel()).getShoppingCD() - Util.getServerTime() + member.getLastBuyTime();
		
		if (cdtime < 0) {
			cdtime = 0;
		}
		builder.setCdTime(cdtime);
		String shopRefreshCD = XMLTemplateService.getGuildConfig().getShopRefreshCD();
		String[] refreshTimeArr = shopRefreshCD.split(",");
		
		Calendar calendar = Calendar.getInstance();
		calendar.set(Calendar.HOUR_OF_DAY, Integer.parseInt(refreshTimeArr[leagua.getLeaguaShopRefreshTime() % 10 - 1])/100);
		calendar.set(Calendar.MINUTE,Integer.parseInt(refreshTimeArr[leagua.getLeaguaShopRefreshTime() % 10 - 1])%100);
		calendar.set(Calendar.SECOND, 0);
		
		int ctime = Calendar.getInstance().get(Calendar.HOUR_OF_DAY) * 100 + Calendar.getInstance().get(Calendar.MINUTE);
		
		if (ctime >= Integer.parseInt(refreshTimeArr[refreshTimeArr.length - 1])) {
			calendar.add(Calendar.DAY_OF_MONTH, 1);
		}
		
		int refreshTime = (int)(calendar.getTimeInMillis()/1000);
		builder.setRefreshTime(refreshTime - Util.getServerTime());
		
		return builder;
	}
	
	public static OPDonateFoundsRet.Builder donateFounds(Player player,int num){
		OPDonateFoundsRet.Builder builder = OPDonateFoundsRet.newBuilder();
		builder.setStatus(2);
		int leaguaId = player.getLeaguaid();
		if (leaguaId == 0 || !allLeaguaObject.containsKey(leaguaId) || player.getSilvercoins() < num || num <= 0 || num > 300000000) {
			return builder;
		}
		
		LeaguaObject leaguaObject = allLeaguaObject.get(leaguaId);
		
		Leagua leagua = leaguaObject.getLeaguaInfo();
		
		String changeRateString = XMLTemplateService.getGuildConfig().getChangeRate();
		String[] rateArr = changeRateString.split(":");
		
		int contribution = num/Integer.parseInt(rateArr[1]) * Integer.parseInt(rateArr[3]);
		
		if (contribution <= 0) {
			return builder;
		}
		
		LeaguaMembers member = leaguaObject.getLeaguaMembersInfo().get(player.getId());
		
		member.increasePlayerLeftContribution(contribution);
		member.increasePlayerTotalContribution(contribution);
		leaguaObject.updateOneMember(member);
		
		leagua.setLeaguaFunds(leagua.getLeaguaFunds() + num);
		leaguaObject.updateLeaguaInfo(leagua);
		
		player.setSilvercoins(player.getSilvercoins() - num);
		
		builder.setStatus(1);
		builder.setLeaguaFunds(String.valueOf(leagua.getLeaguaFunds()));
		builder.setSilvercoins(String.valueOf(player.getSilvercoins()));
		builder.setPlayerLeftContribution(member.getPlayerLeftContribution());
		builder.setPlayerTotalContribution(member.getPlayerTotalContribution());
		
		return builder;
	}
	
	public static OPDonateBuildingRet.Builder donateBuilding(Player player,int buildId,int donateType){
		OPDonateBuildingRet.Builder builder = OPDonateBuildingRet.newBuilder();
		
		int leaguaId = player.getLeaguaid();
		if (leaguaId == 0 || !allLeaguaObject.containsKey(leaguaId)) {
			return builder;
		}
		
		LeaguaObject leaguaObject = allLeaguaObject.get(leaguaId);
		
		Leagua leagua = leaguaObject.getLeaguaInfo();
		
		JSONObject builds = leagua.getLeaguaBuildingInfoInJson();
		
		JSONObject buildInfo = builds.getJSONObject(String.valueOf(buildId));
		
		if (buildInfo == null) {
			builder.setStatus(2);
		}
		else {
			if (buildInfo.getInt("mxp") == 0 || (buildInfo.getInt("l") >= leagua.getLeaguaLevel() && buildId != LeaguaDefine.BASE_ID)) {
				builder.setStatus(3);
			}
			else {
				JSONObject costObject = getDonateBuildConfigByType(donateType);
				if (!costObject.containsKey("costnum")) {
					builder.setStatus(4);
				}
				else if ((costObject.getInt("costtype") == 1001 && player.getGoldCoins() < costObject.getInt("costnum")) || (costObject.getInt("costtype") == 1002 && player.getSilvercoins() < costObject.getInt("costnum"))) {
					builder.setStatus(4);
				}
				else {
					boolean needSort = false;
					int originLevel = buildInfo.getInt("l");
					buildInfo = calcBuildingLevel(buildId,buildInfo, costObject.getInt("process"));
					builds.put(String.valueOf(buildId), buildInfo);
					leagua.setLeaguaBuildingInfoFromJson(builds);
					if (buildId == LeaguaDefine.BASE_ID && buildInfo.getInt("l") != leagua.getLeaguaLevel()) {//如果是公会基地还需要同步公会等级
						leaguaUpgrade(leagua, buildInfo.getInt("l"));
						builder.setLeaguaMaxMemberCount(leagua.getLeaguaMaxMemberCount());
						needSort = true;
					}
					else if (LeaguaDefine.TOTEMIDS.contains(buildId) && originLevel != buildInfo.getInt("l")) {
						JSONObject totemsInfo = getLeaguaTotemInfo(leagua);
						GuildLevelConfig config = XMLTemplateService.getGuildConfig().getLevelConvigByLevel(buildInfo.getInt("l"));
						int hp = config.getTotemHp();
						JSONObject totemInfo = checkTotemInfo(totemsInfo.getJSONObject(String.valueOf(buildId)));
						
						if (totemInfo.getInt("lh") == totemInfo.getInt("mh") && !totemInfo.containsKey("rt")) {
							totemInfo.put("lh", hp);
						}
						totemInfo.put("mh", hp);
						totemsInfo.put(String.valueOf(buildId), totemInfo);
						leagua.setLeaguaTotemInfoFromJson(totemsInfo);
					}
					else if (buildId == LeaguaDefine.SHOP_ID && originLevel != buildInfo.getInt("l")) {
						shopUpgrade(leagua, originLevel);
					}
					
					LeaguaMembers member = leaguaObject.getLeaguaMembersInfo().get(player.getId());
					member.increasePlayerLeftContribution(costObject.getInt("contribution"));
					member.increasePlayerTotalContribution(costObject.getInt("contribution"));
					
					leaguaObject.updateOneMember(member);
					leaguaObject.updateLeaguaInfo(leagua);
					
					if (needSort) {
						sortRank();
					}
					
					if (costObject.getInt("costtype") == 1001) {
						player.consumeGold(costObject.getInt("costnum"));
					}
					else {
						player.setSilvercoins(player.getSilvercoins() - costObject.getInt("costnum"));
					}
					builder.setBuildingInfo(buildLeaguaBuilding(buildId, buildInfo));
					builder.setGoldcoins(player.getGoldCoins());
					builder.setSilvercoins(String.valueOf(player.getSilvercoins()));
					builder.setPlayerLeftContribution(member.getPlayerLeftContribution());
					builder.setPlayerTotalContribution(member.getPlayerTotalContribution());
					builder.setStatus(1);
				}
			}
		}
		
		return builder;
	}
	
	public static OPGetLeaguaBuildingRet.Builder getLeaguaBuildingInfo(Player player){
		OPGetLeaguaBuildingRet.Builder builder = OPGetLeaguaBuildingRet.newBuilder();
		
		int leaguaId = player.getLeaguaid();
		if (leaguaId == 0 || !allLeaguaObject.containsKey(leaguaId)) {
			return builder;
		}
		
		LeaguaObject leaguaObject = allLeaguaObject.get(leaguaId);
		
		Leagua leagua = leaguaObject.getLeaguaInfo();
		
		JSONObject builds = leagua.getLeaguaBuildingInfoInJson();
		Iterator<String> keys = builds.keys();
		while (keys.hasNext()) {
			String key = keys.next();
			JSONObject buildInfo = builds.getJSONObject(key);
			builder.addBuildingInfoList(buildLeaguaBuilding(Integer.parseInt(key), buildInfo));
		}
		
		return builder;
	}
	
	public static OPUpgradeLeaguaMemberRet.Builder upgradeLeaguaMember(Player player){
		OPUpgradeLeaguaMemberRet.Builder builder = OPUpgradeLeaguaMemberRet.newBuilder();
		
		int leaguaId = player.getLeaguaid();
		if (leaguaId == 0 || !allLeaguaObject.containsKey(leaguaId)) {
			builder.setStatus(2);
			return builder;
		}
		
		LeaguaObject leaguaObject = allLeaguaObject.get(leaguaId);
		
		HashMap<Integer, LeaguaMembers> leaguaMembers = leaguaObject.getLeaguaMembersInfo();
		
		LeaguaMembers member = leaguaMembers.get(player.getId());
		
		if (member.getPlayerGrade() >= LeaguaDefine.GRADE_VICELEADER) {
			builder.setStatus(2);
			return builder;
		}
		
		if (member.getLastUpgradeTime() + XMLTemplateService.getGuildConfig().getMemberUpgradeCD() > Util.getServerTime()) {
			builder.setStatus(3);
			return builder;
		}
		
		ArrayList<LeaguaMembers> memberList = new ArrayList<LeaguaMembers>();
		memberList.addAll(leaguaMembers.values());
		HashMap<Integer, HashMap<String, Integer>> gradeCount = new HashMap<Integer, HashMap<String,Integer>>();
		HashMap<String, Integer> tHashMap = new HashMap<String, Integer>();
		tHashMap.put("count", 0);
		tHashMap.put("min", 0);
		tHashMap.put("minId", 0);
		gradeCount.put(1, (HashMap<String, Integer>)tHashMap.clone());
		gradeCount.put(2, (HashMap<String, Integer>)tHashMap.clone());
		gradeCount.put(3, (HashMap<String, Integer>)tHashMap.clone());
		gradeCount.put(4, (HashMap<String, Integer>)tHashMap.clone());
		for (int i = 0; i < memberList.size(); i++) {
			LeaguaMembers tMember = memberList.get(i);
			if (gradeCount.get(tMember.getPlayerGrade()).get("count") == 0 || tMember.getPlayerTotalContribution() < gradeCount.get(tMember.getPlayerGrade()).get("min")) {
				gradeCount.get(tMember.getPlayerGrade()).put("min",tMember.getPlayerTotalContribution());
				gradeCount.get(tMember.getPlayerGrade()).put("minId",tMember.getPlayerId());
			}
			gradeCount.get(tMember.getPlayerGrade()).put("count",gradeCount.get(tMember.getPlayerGrade()).get("count") + 1);
		}
		
		HashMap<String, Integer> nextGradeInfo = gradeCount.get(member.getPlayerGrade() + 1);
		
		if (nextGradeInfo.get("count") >= LeaguaDefine.getGradeMemberCount(member.getPlayerGrade() + 1) && member.getPlayerTotalContribution() <= nextGradeInfo.get("min")) {
			builder.setStatus(4);
			return builder;
		}
		
		if (nextGradeInfo.get("count") < LeaguaDefine.getGradeMemberCount(member.getPlayerGrade() + 1)) {
			member.setPlayerGrade(member.getPlayerGrade() + 1);
			leaguaObject.updateOneMember(member);
		}
		else {
			LeaguaMembers originMember = leaguaMembers.get(nextGradeInfo.get("minId"));
			originMember.setPlayerGrade(member.getPlayerGrade());
			member.setPlayerGrade(member.getPlayerGrade() + 1);
			leaguaObject.updateOneMember(member);
			leaguaObject.updateOneMember(originMember);
		}
		
		ArrayList<Integer> keys = new ArrayList<Integer>();
		keys.addAll(leaguaObject.getLeaguaMembersInfo().keySet());
		for (int i = 0; i < keys.size(); i++) {
			builder.addMemberInfoList(buildLeagueMember(leaguaObject.getLeaguaMembersInfo().get(keys.get(i))));
		}
		builder.setPlayerGrade(member.getPlayerGrade());
		builder.setStatus(1);
		
		return builder;
	}
	
	public static OPDisbandLeaguaRet.Builder disbandLeagua(Player player){
		OPDisbandLeaguaRet.Builder builder = OPDisbandLeaguaRet.newBuilder();
		
		int leaguaId = player.getLeaguaid();
		if (leaguaId == 0 || !allLeaguaObject.containsKey(leaguaId)) {
			builder.setStatus(2);
			return builder;
		}
		
		LeaguaObject leaguaObject = allLeaguaObject.get(leaguaId);
		
		HashMap<Integer, LeaguaMembers> leaguaMembers = leaguaObject.getLeaguaMembersInfo();
		
		LeaguaMembers member = leaguaMembers.get(player.getId());
		if (member.getPlayerGrade() != LeaguaDefine.GRADE_LEADER) {
			builder.setStatus(3);
			return builder;
		}
		
		ArrayList<Integer> keys = new ArrayList<Integer>();
		keys.addAll(leaguaMembers.keySet());
		OPFireMemberPushMsg.Builder pushBuilder = OPFireMemberPushMsg.newBuilder();
		pushBuilder.setVersion(1);
		Packet pt = new Packet(OPLeague.code.OPCODE_FIREMEMBER_PUSH_VALUE, pushBuilder.build().toByteArray());
		for (int i = 0; i < keys.size(); i++) {
			LeaguaMembers dMember = leaguaMembers.get(keys.get(i));
			int playerId = dMember.getPlayerId();
			Player player2 = ObjectAccessor.getOnlineUser(playerId);
			if (player2 != null) {
				player2.setLeaguaid(0);
				player2.getSession().write(pt);
			}
			else {
				List<Player>list = DZM.getEntityManager().limitQuery("from Player where id = ?", 0, 1, playerId);
				if (list.size() == 1) {
					player2 = list.get(0);
					player2.setLeaguaid(0);
					DZM.getEntityManager().updateSync(player2);
				}
			}
			DZM.getEntityManager().deleteSync(dMember);
		}
		
		Leagua leagua = leaguaObject.getLeaguaInfo();
		
		DZM.getEntityManager().deleteSync(leagua);
		
		HashMap<Integer, LeaguaMedal> leaguaMedals = leaguaObject.getLeaguaMedals();
		ArrayList<Integer> medalKeys = new ArrayList<Integer>();
		medalKeys.addAll(leaguaMedals.keySet());
		for (int i = 0; i < medalKeys.size(); i++) {
			LeaguaMedal medal = leaguaMedals.get(medalKeys.get(i));
			if (medal.getMedalId() == LeaguaDefine.DEFAULT_MEDAL_ID) {
				DZM.getEntityManager().deleteSync(medal);
			}
			else {
				medal.setOwnerLeaguaID(0);
				medal.setOwnerLeaguaName("");
				allSpecialLeaguaMedals.put(medal.getMedalId(), medal);
			}
		}
		allLeaguaNames.remove(allLeaguaNames.indexOf(leagua.getLeaguaName()));
		allLeaguaObject.remove(leaguaId);
		allLeagua.remove(leaguaId);
		sortRank();
		builder.setStatus(1);
		return builder;
	}
	
	public static OPTransferLeaguaOwnerRet.Builder transferLeaguaOwner(Player player,int memberId){
		OPTransferLeaguaOwnerRet.Builder builder = OPTransferLeaguaOwnerRet.newBuilder();
		
		int leaguaId = player.getLeaguaid();
		if (memberId == player.getId() || leaguaId == 0 || !allLeaguaObject.containsKey(leaguaId) || !allLeaguaObject.get(leaguaId).getLeaguaMembersInfo().containsKey(memberId)) {
			builder.setStatus(2);
			return builder;
		}
		
		LeaguaObject leaguaObject = allLeaguaObject.get(leaguaId);
		Leagua leagua = leaguaObject.getLeaguaInfo();
		
		int serverTime = Util.getServerTime();
		if (leagua.getTransLeaderTime() != 0 && serverTime - leagua.getTransLeaderTime() < 3 * 24 * 3600) {
			builder.setCoolTime(3 * 24 * 3600 - serverTime + leagua.getTransLeaderTime());
			builder.setStatus(4);
			return builder;
		}
		
		LeaguaMembers member = leaguaObject.getLeaguaMembersInfo().get(player.getId());
		if (member.getPlayerGrade() != LeaguaDefine.GRADE_LEADER) {
			builder.setStatus(3);
			return builder;
		}
		LeaguaMembers newMember = leaguaObject.getLeaguaMembersInfo().get(memberId);
		member.setPlayerGrade(newMember.getPlayerGrade());
		newMember.setPlayerGrade(LeaguaDefine.GRADE_LEADER);
		leaguaObject.updateOneMember(newMember);
		leaguaObject.updateOneMember(member);
		leagua.setOwnerPlayerId(memberId);
		leagua.setOwnerPlayerName(newMember.getPlayerName());
		leaguaObject.updateLeaguaInfo(leagua);
		
		builder.setPlayerGrade(member.getPlayerGrade());
		ArrayList<Integer> keys = new ArrayList<Integer>();
		keys.addAll(leaguaObject.getLeaguaMembersInfo().keySet());
		for (int i = 0; i < keys.size(); i++) {
			builder.addMemberInfoList(buildLeagueMember(leaguaObject.getLeaguaMembersInfo().get(keys.get(i))));
		}
		builder.setStatus(1);
		leagua.setTransLeaderTime(serverTime);
		builder.setCoolTime(3 * 24 * 3600);
		
		return builder;
	}
	
	public static OPFireLeaguaMemberRet.Builder fireLeaguaMember(Player player,int fireId){
		OPFireLeaguaMemberRet.Builder builder = OPFireLeaguaMemberRet.newBuilder();
		
		int leaguaId = player.getLeaguaid();
		if (fireId == player.getId() || leaguaId == 0 || !allLeaguaObject.containsKey(leaguaId) || !allLeaguaObject.get(leaguaId).getLeaguaMembersInfo().containsKey(fireId)) {
			builder.setStatus(2);
		}
		
		LeaguaMembers member = allLeaguaObject.get(leaguaId).getLeaguaMembersInfo().get(player.getId());
		if (member.getPlayerGrade() < LeaguaDefine.GRADE_LEADER) {
			builder.setStatus(3);
		}
		else {
			builder.setStatus(1);
			Leagua leagua = allLeaguaObject.get(leaguaId).getLeaguaInfo();
			LeaguaMembers fireMember = allLeaguaObject.get(leaguaId).getLeaguaMembersInfo().get(fireId);
			allLeaguaObject.get(leaguaId).getLeaguaMembersInfo().remove(fireId);
			DZM.getEntityManager().deleteSync(fireMember);
			leagua.setLeaguaCurMemberCount(leagua.getLeaguaCurMemberCount() - 1);
			Player firePlayer = ObjectAccessor.getOnlineUser(fireId);
			if (firePlayer!= null) {
				firePlayer.setLeaguaid(0);
				OPFireMemberPushMsg.Builder builder2 = OPFireMemberPushMsg.newBuilder();
				builder2.setVersion(1);
				Packet pt = new Packet(OPLeague.code.OPCODE_FIREMEMBER_PUSH_VALUE, builder2.build().toByteArray());
				player.getSession().write(pt);
			}
			else {
				List<Player> aList = DZM.getEntityManager().limitQuery("from Player where id=?", 0, 1, fireId);
				firePlayer = (Player)aList.get(0);
				firePlayer.setLeaguaid(0);
				DZM.getEntityManager().updateSync(firePlayer);
			}
			allLeaguaObject.get(leaguaId).updateLeaguaInfo(leagua);
		}
		
		HashMap<Integer, LeaguaMembers> members = allLeaguaObject.get(leaguaId).getLeaguaMembersInfo();
		ArrayList<Integer> keys = new ArrayList<Integer>();
		keys.addAll(members.keySet());
		for (int i = 0; i < keys.size(); i++) {
			builder.addMemberInfoList(buildLeagueMember(members.get(keys.get(i))));
		}
		
		return builder;
	}
	
	public static OPQuitLeaguaRet.Builder quitLeagua(Player player){
		OPQuitLeaguaRet.Builder builder = OPQuitLeaguaRet.newBuilder();
		
		int leaguaId = player.getLeaguaid();
		if (leaguaId == 0 || !allLeaguaObject.containsKey(leaguaId)) {
			builder.setStatus(2);
		}
		
		LeaguaMembers member = allLeaguaObject.get(leaguaId).getLeaguaMembersInfo().get(player.getId());
		if (member.getPlayerGrade() >= LeaguaDefine.GRADE_LEADER) {
			builder.setStatus(3);
		}
		else {
			Leagua leagua = allLeaguaObject.get(leaguaId).getLeaguaInfo();
			player.setLeaguaid(0);
			allLeaguaObject.get(leaguaId).getLeaguaMembersInfo().remove(player.getId());
			DZM.getEntityManager().deleteSync(member);
			leagua.setLeaguaCurMemberCount(leagua.getLeaguaCurMemberCount() - 1);
			builder.setStatus(1);
		}
		
		return builder;
	}
	
	public static OPGetLeaguaMemberRet.Builder getLeaguaMemberList(Player player){
		OPGetLeaguaMemberRet.Builder builder = OPGetLeaguaMemberRet.newBuilder();
		OPErrorReporter.Builder errBuilder = OPErrorReporter.newBuilder();
		
		int leaguaId = player.getLeaguaid();
		if (leaguaId == 0 || !allLeaguaObject.containsKey(leaguaId)) {
			errBuilder.setErrorid(OPLeague.code.OPCODE_GET_LEAGUAMEMBER_S_VALUE);
			errBuilder.setErrormsg("no leagua");
			player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errBuilder);
			return builder;
		}
		
		HashMap<Integer, LeaguaMembers> members = allLeaguaObject.get(leaguaId).getLeaguaMembersInfo();
		ArrayList<Integer> keys = new ArrayList<Integer>();
		keys.addAll(members.keySet());
		for (int i = 0; i < keys.size(); i++) {
			builder.addMemberInfoList(buildLeagueMember(members.get(keys.get(i))));
		}
		
		return builder;
	}
	
	public static OPChangeLeaguaActiveMedalRet.Builder changeActiveMedal(Player player,int medalId){
		OPChangeLeaguaActiveMedalRet.Builder builder = OPChangeLeaguaActiveMedalRet.newBuilder();
		
		int leaguaId = player.getLeaguaid();
		if (leaguaId == 0 || !allLeaguaObject.containsKey(leaguaId)) {
			builder.setStatus(false);
			return builder;
		}
		LeaguaObject leaguaObject = allLeaguaObject.get(leaguaId);
		if (!leaguaObject.getLeaguaMedals().containsKey(medalId)) {
			builder.setStatus(false);
			return builder;
		}
		Leagua leagua = leaguaObject.getLeaguaInfo();
		leagua.setLeaguaActiveMedal(medalId);
		leaguaObject.updateLeaguaInfo(leagua);
		builder.setStatus(true);
		
		return builder;
	}
	
	public static OPReceiveLeaguaMedalRet.Builder receiveMedal(Player player,int medalId){
		OPReceiveLeaguaMedalRet.Builder builder = OPReceiveLeaguaMedalRet.newBuilder();
		builder.setStatus(1);
		int leaguaId = player.getLeaguaid();
		if (leaguaId == 0 || !allLeaguaObject.containsKey(leaguaId)) {
			builder.setStatus(2);
			return builder;
		}
		
		LeaguaMedal medal = allSpecialLeaguaMedals.get(medalId);
		if (medal.getOwnerLeaguaID() != 0) {
			builder.setStatus(3);
			return builder;
		}
		
		if (!allLeaguaObject.get(leaguaId).getLeaguaMembersInfo().containsKey(player.getId()) || allLeaguaObject.get(leaguaId).getLeaguaMembersInfo().get(player.getId()).getPlayerGrade() < LeaguaDefine.GRADE_LEADER) {
			builder.setStatus(4);
			return builder;
		}
		
		Leagua leagua = allLeaguaObject.get(leaguaId).getLeaguaInfo();
		if (leagua.getLeaguaLevel() < XMLTemplateService.getGuildConfig().getLeaguaMaxLevel()) {
			builder.setStatus(5);
			return builder;
		}
		
		if (leagua.getReceiveMedalTime() != 0) {
			builder.setStatus(6);
			return builder;
		}
		medal.setOwnerLeaguaID(leaguaId);
		medal.setOwnerLeaguaName(leagua.getLeaguaName());
		allSpecialLeaguaMedals.put(medalId, medal);
		allLeaguaObject.get(leaguaId).updateOneMedal(medal);
		leagua.setReceiveMedalTime(Util.getServerTime());
		allLeaguaObject.get(leaguaId).updateLeaguaInfo(leagua);
		
		ArrayList<Integer> keys = new ArrayList<Integer>();
		keys.addAll(allSpecialLeaguaMedals.keySet());
		for (int i = 0; i < keys.size(); i++) {
			builder.addMedalInfoList(buildLeaguaMedal(allSpecialLeaguaMedals.get(keys.get(i))));
		}
		
		HashMap<Integer, LeaguaMedal> leaguaMedals = allLeaguaObject.get(leaguaId).getLeaguaMedals();
		if (leaguaMedals.containsKey(LeaguaDefine.DEFAULT_MEDAL_ID)) {
			builder.addMedalInfoList(buildLeaguaMedal(leaguaMedals.get(LeaguaDefine.DEFAULT_MEDAL_ID)));
		}
		
		return builder;
	}
	
	public static OPGetLeaguaMedalRet.Builder getMedalInfo(Player player){
		OPGetLeaguaMedalRet.Builder builder = OPGetLeaguaMedalRet.newBuilder();
		builder.setAlreayGotSpecialMedal(true);
		int leaguaId = player.getLeaguaid();
		
		ArrayList<Integer> keys = new ArrayList<Integer>();
		keys.addAll(allSpecialLeaguaMedals.keySet());
		for (int i = 0; i < keys.size(); i++) {
			builder.addMedalInfoList(buildLeaguaMedal(allSpecialLeaguaMedals.get(keys.get(i))));
		}
		
		Leagua leagua = allLeaguaObject.get(leaguaId).getLeaguaInfo();
		
		HashMap<Integer, LeaguaMedal> leaguaMedals = allLeaguaObject.get(leaguaId).getLeaguaMedals();
		if (leaguaMedals.containsKey(LeaguaDefine.DEFAULT_MEDAL_ID)) {
			builder.addMedalInfoList(buildLeaguaMedal(leaguaMedals.get(LeaguaDefine.DEFAULT_MEDAL_ID)));
		}
		
		if (leagua.getReceiveMedalTime() == 0) {
			builder.setAlreayGotSpecialMedal(false);
		}
		
		return builder;
	}
	
	public static OPGetLeaguaRobRet.Builder robLeaguaTotem(Player player,int leaguaId,int totemId){
		OPGetLeaguaRobRet.Builder builder = OPGetLeaguaRobRet.newBuilder();
		builder.setContribution(0);
		builder.setFund(0);
		builder.setIntrestLeftTimes(0);
		builder.setIntrestMaxTimes(0);
		builder.setRobLeftTimes(0);
		builder.setRobMaxTimes(0);
		builder.setCoolDownSeconds(0);
		if (player.getLeaguaid() == 0 || !allLeaguaObject.containsKey(player.getLeaguaid()) || !allLeaguaObject.containsKey(leaguaId)) {
			builder.setStatus(3);
			return builder;
		}
		LeaguaObject userLeaguaObject = allLeaguaObject.get(player.getLeaguaid());
		LeaguaMembers memberInfo = userLeaguaObject.getLeaguaMembersInfo().get(player.getId());
		if (memberInfo.getNextCanAttackTime() > Util.getServerTime() || memberInfo.checkAndGetLeftAttackTimes() <= 0) {
			builder.setStatus(3);
			int coolDown = Util.getServerTime() - memberInfo.getNextCanAttackTime();
			if (coolDown < 0) {
				coolDown = 0;
			}
			builder.setCoolDownSeconds(coolDown);
			return builder;
		}
		LeaguaObject enemyLeaguaObject = allLeaguaObject.get(leaguaId);
		Leagua enemyLeagua = enemyLeaguaObject.getLeaguaInfo();
		JSONObject enemyTotemsInfo = getLeaguaTotemInfo(enemyLeagua);
		JSONObject totemInfo = enemyTotemsInfo.getJSONObject(String.valueOf(totemId));
		if (totemInfo.getInt("lh") <= 0 || totemInfo.containsKey("rt")) {
			builder.setStatus(2);
			return builder;
		}
		
		//TODO npc或者联盟成员判定
		int totemLevel = enemyLeagua.getLeaguaBuildingInfoInJson().getJSONObject(String.valueOf(totemId)).getInt("l");
		GuildNpcConfig npcConfig = XMLTemplateService.getGuildConfig().getNpcConfigByTotemAndLevel(totemId, totemLevel);
		int npcRate = npcConfig.getNpcRate();
		int memberRate = npcConfig.getMemberRate();
		Random random = new Random();
		int rand = random.nextInt(npcRate + enemyLeagua.getLeaguaCurMemberCount() * memberRate) + 1;
		
		BattleArrayObject userBattleObject = BattleUtil.getUserBattleArrayObject(player);
		BattleArrayObject enemyBattleObject = new BattleArrayObject();
		
		ArrayList<Float> skillRate = null;
		boolean isNpc = false;
		if (rand <= npcRate) {
			isNpc = true;
			int npcLevel = npcConfig.getNpcLevel();
			ArrayList<Battle> opponentBattlearrays=new ArrayList<Battle>();
			ArrayList<Integer> npcIds = npcConfig.getNpcIds();
			skillRate=new ArrayList<Float>();
			for (int i = 0; i < npcIds.size(); i++) {
				int npcId=npcIds.get(i);
				DiscipleXmlTemplate disciple=XMLTemplateService.getDiscipleTemplate(npcId);
				if(disciple!=null)
	            {
					Battle _battle=XMLTemplateService.getBattleByDiscipleId(disciple.getId()).clone();
                    _battle.setId(i);
                    _battle.setLevel(npcLevel);
                    int increaseLevel=npcLevel-1;
                    //计算NPC的数据信息
                    _battle.setHealth((int)((_battle.getHealth() + increaseLevel * disciple.getGrowHealth())*npcConfig.getHpBuff()));
                    _battle.setAttack((int)((_battle.getAttack() + increaseLevel * disciple.getGrowAttack())*npcConfig.getAttackBuff()));
                    _battle.setDefence((int)((_battle.getDefence() + increaseLevel * disciple.getGrowDefence())*npcConfig.getDefenceBuff()));
                    _battle.setRp((int)((_battle.getRp() + increaseLevel * disciple.getGrowRp())*npcConfig.getWillBuff()));
                    opponentBattlearrays.add(_battle);
                    skillRate.add((float)(npcConfig.getSkillRatios()));
                }
			}
			enemyBattleObject.setBattlearrays(opponentBattlearrays);
		}
		else {
			HashMap<Integer, LeaguaMembers> enemyMembers = enemyLeaguaObject.getLeaguaMembersInfo();
			ArrayList<LeaguaMembers> memberArrays = new ArrayList(enemyMembers.values());
			int pos = (int)Math.floor(Math.random() * memberArrays.size());
			LeaguaMembers enemyMember = memberArrays.get(pos);
			
			int enemyId = enemyMember.getPlayerId();
			Player enemy = new Player();
			enemy.setId(enemyId);
			
			enemyBattleObject = BattleUtil.getUserBattleArrayObject(enemy);
		}
		
		
		Fight fight = new Fight();
		fight.doFight(userBattleObject, enemyBattleObject, skillRate);
		
		Round round1 = fight.getRound1();
		Round round2 = fight.getRound2();
		FinalRound round3 = fight.getFinalRound();
		int fightResult = fight.getResult();
		Result result = new Result();
		result.setWinlose(fightResult);
		ArrayList<CharacterResult> characterResults = new ArrayList<CharacterResult>();
		ArrayList<Battle> userBattles = userBattleObject.getBattlearrays();
		for (int i = 0; i < userBattles.size(); i++) {
			if (userBattles.get(i).getDiscipleid() == 0) {
				continue;
			}
			CharacterResult characterResult = new CharacterResult();
			characterResult.setItemid(userBattles.get(i).getItemid());
			characterResult.setLevel(userBattles.get(i).getLevel());
			characterResults.add(characterResult);
		}
		result.setCharacterResults(characterResults);
		
		GuildConfig guildBaseConfig = XMLTemplateService.getGuildConfig();
		
		if (fightResult <= 2) {
			Iterator<String> keys = enemyTotemsInfo.keys();
			int totalHealth = 0;
			int leftHealth = 0;
			while (keys.hasNext()) {
				String key = keys.next();
				JSONObject totembInfo = checkTotemInfo(enemyTotemsInfo.getJSONObject(key));
				totalHealth += totembInfo.getInt("mh");
				leftHealth += totembInfo.getInt("lh");
			}
			
			int newLeftHealth = totemInfo.getInt("lh") - guildBaseConfig.getAttckHp();
			leftHealth -= guildBaseConfig.getAttckHp();
			totemInfo.put("lh", newLeftHealth);
			if (newLeftHealth <= 0) {
				totemInfo.put("lh", 0);
				if (leftHealth <= 0) {
					//TODO 夺徽章判定
					int activeMedal = enemyLeagua.getLeaguaActiveMedal();
					HashMap<Integer, LeaguaMedal> medals = (HashMap<Integer, LeaguaMedal>)enemyLeaguaObject.getLeaguaMedals().clone();
					if (activeMedal == LeaguaDefine.DEFAULT_MEDAL_ID || activeMedal == LeaguaDefine.BLANK_MEDAL_ID) {
						LeaguaMedal defaultMedal = null;
						if (medals.containsKey(LeaguaDefine.DEFAULT_MEDAL_ID)) {
							defaultMedal = medals.get(LeaguaDefine.DEFAULT_MEDAL_ID);
							medals.remove(LeaguaDefine.DEFAULT_MEDAL_ID);
						}
						if (medals.size() > 0) {
							ArrayList<Integer> medalKeys = (ArrayList<Integer>)medals.keySet();
							int index = random.nextInt(medalKeys.size());
							int medalId = medalKeys.get(index);
							LeaguaMedal robMedal = allSpecialLeaguaMedals.get(medalId);
							if (robMedal.getOwnerLeaguaID() == leaguaId) {
								robMedal.setOwnerLeaguaID(player.getLeaguaid());
								robMedal.setOwnerLeaguaName(userLeaguaObject.getLeaguaInfo().getLeaguaName());
								allSpecialLeaguaMedals.put(medalId, robMedal);
								enemyLeaguaObject.getLeaguaMedals().remove(medalId);
								userLeaguaObject.getLeaguaMedals().put(medalId, robMedal);
								//TODO添加徽章变动信息
								
								builder.setGotBadgeID(medalId);
							}
						}
						else if (defaultMedal != null && defaultMedal.getCount() > 0) {
							defaultMedal.setCount(defaultMedal.getCount() - 1);
							enemyLeagua.setLeaguaActiveMedal(LeaguaDefine.BLANK_MEDAL_ID);
							enemyLeaguaObject.getLeaguaMedals().put(LeaguaDefine.DEFAULT_MEDAL_ID, defaultMedal);
							
							LeaguaMedal medal = null;
							
							if(userLeaguaObject.getLeaguaMedals().containsKey(LeaguaDefine.DEFAULT_MEDAL_ID)){
								medal = userLeaguaObject.getLeaguaMedals().get(LeaguaDefine.DEFAULT_MEDAL_ID);
								medal.setCount(medal.getCount() + 1);
							}
							else {
								medal = new LeaguaMedal();
								medal.setCount(1);
								medal.setMedalId(LeaguaDefine.DEFAULT_MEDAL_ID);
								medal.setOwnerLeaguaID(player.getLeaguaid());
								medal.setOwnerLeaguaName(userLeaguaObject.getLeaguaInfo().getLeaguaName());
								DZM.getEntityManager().createSync(medal);
							}
							userLeaguaObject.getLeaguaMedals().put(LeaguaDefine.DEFAULT_MEDAL_ID, medal);
							builder.setGotBadgeID(LeaguaDefine.DEFAULT_MEDAL_ID);
						}
					}
					else {
						LeaguaMedal robMedal = allSpecialLeaguaMedals.get(activeMedal);
						if (robMedal.getOwnerLeaguaID() == leaguaId) {
							robMedal.setOwnerLeaguaID(player.getLeaguaid());
							robMedal.setOwnerLeaguaName(userLeaguaObject.getLeaguaInfo().getLeaguaName());
							allSpecialLeaguaMedals.put(activeMedal, robMedal);
							enemyLeaguaObject.getLeaguaMedals().remove(activeMedal);
							enemyLeagua.setLeaguaActiveMedal(LeaguaDefine.BLANK_MEDAL_ID);
							userLeaguaObject.getLeaguaMedals().put(activeMedal, robMedal);
							//TODO添加徽章变动信息
							
							builder.setGotBadgeID(activeMedal);
						}
					}
				}
			}
			enemyTotemsInfo.put(String.valueOf(totemId), totemInfo);
			enemyLeaguaObject.updateLeaguaInfo(enemyLeagua);
			
			if (memberInfo.checkAndGetLeftGetAttackRewardTimes() > 0) {
				int canGet = (int)Math.floor(guildBaseConfig.getAttckHp() * guildBaseConfig.getAttackAdjustmentRatio() * (1 + guildBaseConfig.getAttackRankingRatio1()*Math.pow(guildBaseConfig.getAttackRankingRatio2(), enemyLeagua.getLeaguaRank())) * getRobContribution(leftHealth, totalHealth));
				
				memberInfo.setPlayerTotalContribution(memberInfo.getPlayerTotalContribution() + canGet);
				memberInfo.setPlayerLeftContribution(memberInfo.getPlayerLeftContribution() + canGet);
				builder.setContribution(canGet);
				
				memberInfo.decreseLeftGetAttackRewardTimes();
			}
		}
		memberInfo.setNextCanAttackTime(Util.getServerTime() + guildBaseConfig.getAttackCoolSeconds());
		memberInfo.decreseLeftAttackTimes();
		
		userLeaguaObject.updateOneMember(memberInfo);
		
		OPUserBattleRet.Builder battleBuilder = OPUserBattleRet.newBuilder();
		battleBuilder.setVersion(1);
		battleBuilder.setServertime(Util.getServerTime());
		battleBuilder.setRound1(UserBattleHandler.getRoundBuilder(round1));
		battleBuilder.setRound2(UserBattleHandler.getRoundBuilder(round2));
		battleBuilder.setRound3(UserBattleHandler.getFinalRound(round3));
		battleBuilder.setResult(UserBattleHandler.getResult(player, result));
		player.sendPacket(OP.code.OPCODE_USER_BATTLERET_S_VALUE, battleBuilder);
		
		builder.setIntrestLeftTimes(memberInfo.getLeftGetAttackRewardTimes());
		builder.setIntrestMaxTimes(guildBaseConfig.getDailyMaxRewardTimes());
		builder.setRobLeftTimes(memberInfo.getLeftAttackTimes());
		builder.setRobMaxTimes(guildBaseConfig.getDailyMaxAttackTimes());
		builder.setCoolDownSeconds(guildBaseConfig.getAttackCoolSeconds());
		builder.setStatus(1);
		return builder;
	}
	
	public static OPGetLeaguaRobListRet.Builder getLeaguaRobList(Player player){
		OPGetLeaguaRobListRet.Builder builder = OPGetLeaguaRobListRet.newBuilder();
		
		int leaguaId = player.getLeaguaid();
		if(allLeaguaObject.containsKey(leaguaId)){
			Leagua leagua = allLeaguaObject.get(leaguaId).getLeaguaInfo();
			int selfLeaguaLevel = leagua.getLeaguaLevel();
			ArrayList<Integer> enemyLeaguaIdList = new ArrayList<Integer>();
			
			if (levelLeaguaIds.containsKey(selfLeaguaLevel)) {
				enemyLeaguaIdList = (ArrayList<Integer>)levelLeaguaIds.get(selfLeaguaLevel).clone();
			}
			
			for (int i = -2; i < 3; i++) {
				int newLevel = selfLeaguaLevel + i ;
				if (newLevel <= 0 || !levelLeaguaIds.containsKey(newLevel) || i == 0) {
					continue;
				}
				enemyLeaguaIdList.addAll(levelLeaguaIds.get(newLevel));
			}
			
			for (int i = 0; i < enemyLeaguaIdList.size(); i++) {
				if (!allLeaguaObject.containsKey(enemyLeaguaIdList.get(i)) || enemyLeaguaIdList.get(i) == leaguaId) {
					continue;
				}
				builder.addLeaguaRankInfoList(buildLeaguaRank(allLeaguaObject.get(enemyLeaguaIdList.get(i)).getLeaguaInfo()));
			}
		}
		
		return builder;
	}
	
	public static OPGetTotemInfoRet.Builder getTotemInfo(int leaguaId, Player player){
		OPGetTotemInfoRet.Builder builder = OPGetTotemInfoRet.newBuilder();
		builder.setStatus(2);
		builder.setTotalContribution(0);
		builder.setReceiveContribution(0);
		if (leaguaId == 0 || !allLeaguaObject.containsKey(leaguaId)) {
			return builder;
		}
		
		LeaguaObject leaguaObject = allLeaguaObject.get(leaguaId);
		Leagua leagua = leaguaObject.getLeaguaInfo();
		JSONObject totemsInfo = getLeaguaTotemInfo(leagua);
		Iterator<String> keys = totemsInfo.keys();
		int totalHealth = 0;
		int leftHealth = 0;
		while (keys.hasNext()) {
			String key = keys.next();
			JSONObject totemInfo = checkTotemInfo(totemsInfo.getJSONObject(key));
			builder.addTotemInfoList(buildLeaguaTotemBuilder(Integer.parseInt(key), totemInfo));
			totemsInfo.put(key, totemInfo);
			totalHealth += totemInfo.getInt("mh");
			leftHealth += totemInfo.getInt("lh");
		}
		
		leagua.setLeaguaTotemInfoFromJson(totemsInfo);
		leaguaObject.updateLeaguaInfo(leagua);
		
		float rate = getReceiveContributionRate(leftHealth, totalHealth);
		float guildCoinRatio = XMLTemplateService.getGuildConfig().getGetGuildCoinRatio();
		builder.setReceiveContribution((int)Math.floor(leftHealth * rate * guildCoinRatio/leagua.getLeaguaMaxMemberCount()));
		builder.setTotalContribution((int)Math.floor(totalHealth * guildCoinRatio / leagua.getLeaguaMaxMemberCount()));
		if (leaguaId == player.getLeaguaid()) {
			LeaguaMembers member = leaguaObject.getLeaguaMembersInfo().get(player.getId());
			Calendar calendar = Calendar.getInstance();
			int hour = calendar.get(Calendar.HOUR_OF_DAY);
			int minute = calendar.get(Calendar.MINUTE);
			int day = calendar.get(Calendar.DAY_OF_MONTH);
			int month = calendar.get(Calendar.MONTH) + 1;
			int date = month * 100 + day;
			if (hour == 19 && minute < 30 && member.getReccontributiontime() != date) {
				rate = getReceiveContributionRate(leagua.getContributionCount(), totalHealth);
				builder.setStatus(1);
				builder.setReceiveContribution((int)Math.floor(leagua.getContributionCount() * rate * guildCoinRatio/leagua.getLeaguaMaxMemberCount()));
			}
			else if (member.getReccontributiontime() == date) {
				builder.setStatus(3);
			}
		}
		else {
			builder.setReceiveContribution((int)Math.floor(leftHealth * rate * guildCoinRatio));
			builder.setTotalContribution((int)Math.floor(totalHealth * guildCoinRatio));
		}
		
		return builder;
	}
	
	public static OPGetLeaguaApplyInfoRet.Builder getLeaguaApplyInfo(Player player){
		OPGetLeaguaApplyInfoRet.Builder builder = OPGetLeaguaApplyInfoRet.newBuilder();
		
		int leaguaId = player.getLeaguaid();
		if (leaguaId != 0 && allLeaguaObject.containsKey(leaguaId)) {
			LeaguaMembers member = allLeaguaObject.get(leaguaId).getLeaguaMembersInfo().get(player.getId());
			if (member.getPlayerGrade() >= LeaguaDefine.GRADE_VICELEADER) {//只有会长和副会长能够看到
				HashMap<Integer, LeaguaApply> applies = allLeaguaObject.get(leaguaId).getLeaguaApplys();
				Iterator<Map.Entry<Integer, LeaguaApply>> iterator = applies.entrySet().iterator();
				while (iterator.hasNext()) {
					Map.Entry<Integer, LeaguaApply> entry = iterator.next();
					LeaguaApply apply = entry.getValue();
					builder.addLeaguaApplyInfos(buildLeaguaApplyInfoBuilder(apply));
				}
			}
		}
		return builder;
	}
	
	public static OPDealApplyLeaguaRet.Builder dealApply(ArrayList<Integer> applyIds,ArrayList<Boolean>results,Player player){
		OPDealApplyLeaguaRet.Builder builder = OPDealApplyLeaguaRet.newBuilder();
		builder.setStatus(1);
		builder.setLeaguaCurMemberCount(0);
		int leaguaId = player.getLeaguaid();
		if (leaguaId != 0 && allLeaguaObject.containsKey(leaguaId) && applyIds.size() == results.size()) {
			LeaguaObject leaguaObject = allLeaguaObject.get(leaguaId);
			Leagua leagua = leaguaObject.getLeaguaInfo();
			if (leaguaObject.getLeaguaMembersInfo().containsKey(player.getId())) {
				LeaguaMembers memberInfo = leaguaObject.getLeaguaMembersInfo().get(player.getId());
				if (memberInfo.getPlayerGrade() >= LeaguaDefine.GRADE_VICELEADER) {
					int size = applyIds.size();
					//处理联盟请求，现在先实现，后续优化
					for (int i = 0; i < size; i++) {
						int applyId = applyIds.get(i);
						Boolean op = results.get(i);
						if (!leaguaObject.getLeaguaApplys().containsKey(applyId)) {
							continue;
						}
						LeaguaApply apply = leaguaObject.getLeaguaApplys().get(applyId);
						if (op) {//接受请求
							if (leagua.getLeaguaCurMemberCount() < leagua.getLeaguaMaxMemberCount()) {//人数还未达上限
								int applyPlayerId = apply.getPlayerId();
								Player applyPlayer = ObjectAccessor.getOnlineUser(applyPlayerId);
								if(applyPlayer == null){
									List<Player> aList = DZM.getEntityManager().limitQuery("from Player where id=?", 0, 1, applyPlayerId);
									applyPlayer = (Player)aList.get(0);
									if (applyPlayer != null && applyPlayer.getLeaguaid() == 0) {
										LeaguaMembers newMember = new LeaguaMembers();
										doCreateMember(applyPlayer, leagua, newMember, 1);
										DZM.getEntityManager().updateSync(applyPlayer);
										leaguaObject.getLeaguaMembersInfo().put(applyPlayerId, newMember);
									}
								}
								else {
									if (applyPlayer.getLeaguaid() == 0) {
										LeaguaMembers newMember = new LeaguaMembers();
										doCreateMember(applyPlayer, leagua, newMember, 1);
										leaguaObject.getLeaguaMembersInfo().put(applyPlayerId, newMember);
									}
								}
							}
						}
						leaguaObject.getLeaguaApplys().remove(applyId);
						DZM.getEntityManager().deleteSync(apply);
					}
					if (leagua.getLeaguaCurMemberCount() >= leagua.getLeaguaMaxMemberCount()) 
					{
						builder.setStatus(3);
					}
					leaguaObject.updateLeaguaInfo(leagua);
					builder.setLeaguaCurMemberCount(leagua.getLeaguaCurMemberCount());
				}
				else {
					builder.setStatus(2);
				}
			}
			else {
				player.setLeaguaid(0);
				builder.setStatus(2);
			}
		}
		else {
			player.setLeaguaid(0);
			builder.setStatus(2);
		}
		
		return builder;
	}
	
	public static OPApplyLeaguaRet.Builder applyLeagua(int applyId,Player player){
		OPApplyLeaguaRet.Builder builder = OPApplyLeaguaRet.newBuilder();
		
		if (player.getLeaguaid() != 0) {
			builder.setStatus(2);//玩家已经有公会了
		}
		else if (!allLeaguaObject.containsKey(applyId)) {
			builder.setStatus(3);
		}
		else if (player.getApplyLeaguaNum() >= 20) {
			builder.setStatus(4);
		}
		else {
			LeaguaObject leaguaObject = allLeaguaObject.get(applyId);
			LeaguaApply apply = new LeaguaApply();
			apply.setApplyLeaguaId(applyId);
			apply.setApplyTime(Util.getServerTime());
			apply.setPlayerId(player.getId());
			apply.setPlayerLevel(player.getLevel());
			apply.setPlayerName(player.getName());
			DZM.getEntityManager().createSync(apply);
			leaguaObject.getLeaguaApplys().put(apply.getId(), apply);
			builder.setStatus(1);
			player.setApplyLeaguaNum(player.getApplyLeaguaNum() + 1);
		}
		return builder;
	}
	
	public static OPCreateLeaguaRet.Builder createLeagua(String name,Player player){
		OPCreateLeaguaRet.Builder builder = OPCreateLeaguaRet.newBuilder();
		
		if (allLeaguaNames.contains(name)) {//名字重复
			builder.setStatus(2);
		}
		else if (player.getLeaguaid() != 0) {//玩家已经有公会了
			builder.setStatus(3);
		}
		else {
			GuildConfig config = XMLTemplateService.getGuildConfig();
			if(player.getGoldCoins() < config.getCreateGold() || player.getLevel() < config.getLevelLimit()){//资金不够
				builder.setStatus(4);
			}
			else{
				Leagua leagua = new Leagua();
				LeaguaMembers member = new LeaguaMembers();
				LeaguaMedal medal = new LeaguaMedal();
				doCreateLeagua(name, player, leagua, member,medal);
				builder.setStatus(1);
				builder.setLeaguaBaseInfo(buildLeaguaBaseInfo(leagua, member));
				player.setLeaguaid(leagua.getLeaguaId());
				player.consumeGold(config.getCreateGold());
			}
			builder.setGoldcoins(player.getGoldCoins());
		}
		
		return builder;
	}
	
	public static OPGetUserLeaguaInfoRet.Builder loadUserLeaguaBuilder(Player player){
		OPGetUserLeaguaInfoRet.Builder builder = OPGetUserLeaguaInfoRet.newBuilder();
		for (int i = 0; i < leaguaRank.size(); i++) {
			Leagua leagua = leaguaRank.get(i);
			builder.addLeaguaRankInfoList(buildLeaguaRank(leagua));
		}
		
		int playerGuildId = player.getLeaguaid();
		builder.setHasLeagua(false);
		if (playerGuildId != 0 && allLeaguaObject.containsKey(playerGuildId)) {
			LeaguaObject leaguaObject = allLeaguaObject.get(playerGuildId);
			if (leaguaObject.getLeaguaMembersInfo().containsKey(player.getId())) {
				LeaguaMembers member = leaguaObject.getLeaguaMembersInfo().get(player.getId());
				builder.setLeaguaBaseInfo(buildLeaguaBaseInfo(leaguaObject.getLeaguaInfo(), member));
				builder.setHasLeagua(true);
			}
			else {
				player.setLeaguaid(0);
			}
		}
		return builder;
	}
	
	private static void doCreateLeagua(String name,Player player,Leagua leagua,LeaguaMembers member,LeaguaMedal medal){
		GuildLevelConfig config = getGuildLevelConfig(1);
		leagua.setLeaguaName(name);
		leagua.setLeaguaLevel(1);
		leagua.setLeaguaMaxMemberCount(config.getGuildMaxMember());
		leagua.setLeaguaCurMemberCount(0);
		leagua.setLeaguaFunds(0);
		leagua.setOwnerPlayerId(player.getId());
		leagua.setOwnerPlayerName(player.getName());
		leagua.setLeaguaRank(allLeaguaObject.size() + 1);
		leagua.setLeaguaActiveMedal(getDefaultMedalId());
		leagua.setLeaguaBroadcast(XMLTemplateService.getGuildConfig().getDefaultBrodcast());
		HashMap<Integer, ArrayList<Integer>> allBuilds = XMLTemplateService.getGuildConfig().getAllBuilds();
		JSONObject totemsObject = new JSONObject();
		JSONObject buildsObject = new JSONObject();
		
		Iterator<Map.Entry<Integer, ArrayList<Integer>>> iterator = allBuilds.entrySet().iterator();
		while (iterator.hasNext()) {
			Map.Entry<Integer, ArrayList<Integer>> typeBuilds = iterator.next();
			int type = typeBuilds.getKey();
			ArrayList<Integer> buildIds = typeBuilds.getValue();
			for (int i = 0; i < buildIds.size(); i++) {
				JSONObject buildObject = new JSONObject();
				int buildId = buildIds.get(i);
				buildObject.put("l", 1);
				buildObject.put("xp", 0);
				buildObject.put("mxp", getLevelXpByBuildId(config, buildId));
				buildObject.put("bn", getAltarNumByLevelAndId(config, buildId));
				buildsObject.put(buildId, buildObject);
				if (type == LeaguaDefine.TOTEM_TYPE) {
					JSONObject totem = new JSONObject();
					totem.put("mh",config.getTotemHp());
					totem.put("lh", config.getTotemHp());
					totemsObject.put(buildId, totem);
				}
			}
		}
		leagua.setLeaguaTotemInfoFromJson(totemsObject);
		leagua.setLeaguaBuildingInfoFromJson(buildsObject);
		
		//生成公会商店信息
		leagua.setLeaguaShopInfoFromJson(new JSONObject());
		//设置公会商店刷新时间
		leagua.setLeaguaShopRefreshTime(0);
		
		leagua.setLeaguaWinRate(0);
		leagua.setCreateTime(Util.getServerTime());
		//生成公会信息
		DZM.getEntityManager().createSync(leagua);
		
		/**
		 * 生成默认的徽章
		 */
		medal.setMedalId(LeaguaDefine.DEFAULT_MEDAL_ID);
		medal.setOwnerLeaguaID(leagua.getLeaguaId());
		medal.setOwnerLeaguaName(name);
		DZM.getEntityManager().createSync(medal);
		
		/**
		 * 生成会长的member信息
		 */
		
		doCreateMember(player, leagua, member, LeaguaDefine.GRADE_LEADER);
		
		LeaguaObject leaguaObject = new LeaguaObject();
		leaguaObject.setLeaguaInfo(leagua);
		leaguaObject.getLeaguaMembersInfo().put(member.getPlayerId(), member);
		leaguaObject.getLeaguaMedals().put(medal.getMedalId(), medal);
		
		allLeaguaObject.put(leagua.getLeaguaId(), leaguaObject);
		allLeaguaNames.add(leagua.getLeaguaName());
		if (!levelLeaguaIds.containsKey(leagua.getLeaguaLevel())) {
			levelLeaguaIds.put(leagua.getLeaguaLevel(), new ArrayList<Integer>());
		}
		levelLeaguaIds.get(leagua.getLeaguaLevel()).add(leagua.getLeaguaId());
		allLeagua.put(leagua.getLeaguaId(), leagua);
		sortRank();
	}
	
	private static void doCreateMember(Player player,Leagua leagua,LeaguaMembers member,int grade){
		member.setPlayerId(player.getId());
		member.setPlayerName(player.getName());
		member.setPlayerlevel(player.getLevel());
		member.setPlayerGrade(grade);
		member.setPlayerLeftContribution(0);
		member.setPlayerTotalContribution(0);
		member.setPlayerKillCount(0);
		member.setGuildId(leagua.getLeaguaId());
		member.setShopPurchaseInfo(new JSONObject());
		member.setLastBuyTime(0);
		member.setTime(Util.getServerTime());
		DZM.getEntityManager().createSync(member);
		player.setLeaguaid(leagua.getLeaguaId());
		leagua.setLeaguaCurMemberCount(leagua.getLeaguaCurMemberCount() + 1);
	}
	
	private static int getAltarNumByLevelAndId(GuildLevelConfig config,int id){
		int altar = 0;
		switch (id) {
		case 8:
			altar = config.getHpAltar();
			break;
		case 9:
			altar = config.getAtcAltar();
			break;
		case 10:
			altar = config.getDefAltar();
			break;
		case 11:
			altar = config.getWillAltar();
			break;
		}
		return altar;
	}
	
	private static int getLevelXpByBuildId(GuildLevelConfig config,int buildid){
		int xp = 0;
		switch (buildid) {
			case 1:
				xp = config.getShopXp();
				break;
			case 2:
				xp = config.getBankXp();
				break;
			case 3:
			case 4:
			case 5:
			case 6:
			case 7:
				xp = config.getTotemXp();
				break;
			case 8:
			case 9:
			case 10:
			case 11:
				xp = config.getAltarXp();
				break;
			case 12:
				xp = config.getGuildXp();
				break;
		}
		
		return xp;
	}
	
	private static JSONObject getDonateBuildConfigByType(int type){
		JSONObject object = new JSONObject();
		String changeRateString = XMLTemplateService.getGuildConfig().getChangeRate();
		String[] rateArr = changeRateString.split(":");
		int num = 0;
		int ratetype = 1;
		switch (type) {
			case 1:
				num = 100;
				ratetype = 0;
				object.put("costtype", 1001);
				object.put("costnum", 100);
				break;
			case 2:
				num = 20;
				ratetype = 0;
				object.put("costtype", 1001);
				object.put("costnum", 20);
				break;
			case 3:
				num = 200000;
				ratetype = 1;
				object.put("costtype", 1002);
				object.put("costnum", 200000);
				break;
			case 4:
				num = 2000000;
				ratetype = 1;
				object.put("costtype", 1002);
				object.put("costnum", 2000000);
				break;
		}
		
		int process = num/Integer.parseInt(rateArr[ratetype]) * Integer.parseInt(rateArr[2]);
		int contribution = num/Integer.parseInt(rateArr[ratetype]) * Integer.parseInt(rateArr[3]);
		object.put("process", process);
		object.put("contribution", contribution);
		
		
		return object;
	}
	
	private static int getDefaultMedalId(){
		return XMLTemplateService.getGuildConfig().getAllMedals().get(LeaguaDefine.DEFAULT_MEDAL_ID).get(0);
	}
	
	private static GuildLevelConfig getGuildLevelConfig(int level){
		return XMLTemplateService.getGuildConfig().getLevelConvigByLevel(level);
	}
	
	private static LeaguaTotemInfo.Builder buildLeaguaTotemBuilder(int id,JSONObject totemInfo){
		LeaguaTotemInfo.Builder totemBuilder = LeaguaTotemInfo.newBuilder();
		
		totemBuilder.setTotemID(id);
		totemBuilder.setTotemTotalHP(totemInfo.getInt("mh"));
		totemBuilder.setTotemLeftHP(totemInfo.getInt("lh"));
		if (totemInfo.containsKey("rt")) {
			totemBuilder.setStatus(3);
			totemBuilder.setRepairTime(totemInfo.getInt("rt") + XMLTemplateService.getGuildConfig().getTotemRepairNeedSeconds() - Util.getServerTime());
		}
		else {
			if (totemInfo.getInt("lh") > 0) {
				totemBuilder.setStatus(1);
			}
			else {
				totemBuilder.setStatus(2);
			}
		}
		
		return totemBuilder;
	}
	
	private static LeaguaApplyInfo.Builder buildLeaguaApplyInfoBuilder(LeaguaApply apply){
		LeaguaApplyInfo.Builder applyBuilder = LeaguaApplyInfo.newBuilder();
		
		applyBuilder.setApplyID(apply.getId());
		applyBuilder.setPlayerID(apply.getPlayerId());
		applyBuilder.setPlayerName(apply.getPlayerName());
		applyBuilder.setPlayerLevel(apply.getPlayerLevel());
		
		return applyBuilder;
	}
	
	private static LeaguaBaseInfo.Builder buildLeaguaBaseInfo(Leagua leagua,LeaguaMembers memberInfo){
		LeaguaBaseInfo.Builder baseBuilder = LeaguaBaseInfo.newBuilder();
		
		baseBuilder.setLeaguaID(leagua.getLeaguaId());
		baseBuilder.setLeaguaName(leagua.getLeaguaName());
		baseBuilder.setLeaguaLevel(leagua.getLeaguaLevel());
		baseBuilder.setLeaguaMaxMemberCount(leagua.getLeaguaMaxMemberCount());
		baseBuilder.setLeaguaCurMemberCount(leagua.getLeaguaCurMemberCount());
		baseBuilder.setLeaguaFunds(String.valueOf(leagua.getLeaguaFunds()));
		baseBuilder.setOwnerName(leagua.getOwnerPlayerName());
		baseBuilder.setLeaguaRank(leagua.getLeaguaRank());
		baseBuilder.setLeaguaActiveMedal(leagua.getLeaguaActiveMedal());
		baseBuilder.setPlayerGrade(memberInfo.getPlayerGrade());
		baseBuilder.setPlayerLeftContribution(memberInfo.getPlayerLeftContribution());
		baseBuilder.setPlayerTotalContribution(memberInfo.getPlayerTotalContribution());
		baseBuilder.setPlayerKillCount(memberInfo.getPlayerKillCount());
		baseBuilder.setLeaguaBroadcast(leagua.getLeaguaBroadcast());
		baseBuilder.setLeaguaWinRate(leagua.getLeaguaWinRate());
		
		int coolTime = 3 * 24 * 3600 - (Util.getServerTime() - leagua.getTransLeaderTime());
		if (coolTime < 0) {
			coolTime = 0;
		}
		baseBuilder.setTransLeaderCoolTime(coolTime);
		
		return baseBuilder;
	}
	
	private static LeaguaRankInfo.Builder buildLeaguaRank(Leagua leagua){
		LeaguaRankInfo.Builder rankInfo = LeaguaRankInfo.newBuilder();
		rankInfo.setLeaguaID(leagua.getLeaguaId());
		rankInfo.setLeaguaName(leagua.getLeaguaName());
		rankInfo.setLeaguaLevel(leagua.getLeaguaLevel());
		rankInfo.setLeaguaMaxMemberCount(leagua.getLeaguaMaxMemberCount());
		rankInfo.setLeaguaCurMemberCount(leagua.getLeaguaCurMemberCount());
		rankInfo.setOwnerName(leagua.getOwnerPlayerName());
		rankInfo.setLeaguaRank(leagua.getLeaguaRank());
		rankInfo.setLeaguaWinRate(leagua.getLeaguaWinRate());
		rankInfo.setMedalID(leagua.getLeaguaActiveMedal());
		return rankInfo;
	}
	
	private static LeaguaMedalInfo.Builder buildLeaguaMedal(LeaguaMedal medal){
		LeaguaMedalInfo.Builder medalBuilder = LeaguaMedalInfo.newBuilder();
		
		medalBuilder.setMedalID(medal.getMedalId());
		medalBuilder.setOwnerLeaguaID(medal.getOwnerLeaguaID());
		medalBuilder.setOwnerLeaguaName(medal.getOwnerLeaguaName());
		
		return medalBuilder;
	}
	
	private static LeaguaMemberInfo.Builder buildLeagueMember(LeaguaMembers member){
		LeaguaMemberInfo.Builder memberBuilder = LeaguaMemberInfo.newBuilder();
		
		memberBuilder.setPlayerID(member.getPlayerId());
		memberBuilder.setPlayerName(member.getPlayerName());
		memberBuilder.setPlayerLevel(member.getPlayerlevel());
		memberBuilder.setPlayerGrade(member.getPlayerGrade());
		memberBuilder.setPlayerContribution(member.getPlayerTotalContribution());
		memberBuilder.setPlayerKillCount(member.getPlayerKillCount());
		
		return memberBuilder;
	}
	
	private static LeaguaBuildingInfo.Builder buildLeaguaBuilding(int id,JSONObject buildInfo){
		LeaguaBuildingInfo.Builder buildingBuilder = LeaguaBuildingInfo.newBuilder();
		
		buildingBuilder.setBuildingID(id);
		buildingBuilder.setBuildingLevel(buildInfo.getInt("l"));
		buildingBuilder.setBuildingCurSchedule(buildInfo.getInt("xp"));
		buildingBuilder.setBuildingFinishSchedule(buildInfo.getInt("mxp"));
		
		return buildingBuilder;
	}
	
	private static LeaguaShopGoodInfo.Builder buildShopGoodInfo(JSONObject goodInfo,JSONObject memberBuy){
		LeaguaShopGoodInfo.Builder goodBuilder = LeaguaShopGoodInfo.newBuilder();
		
		goodBuilder.setId(goodInfo.getInt("id"));
		goodBuilder.setGoodID(goodInfo.getInt("gId"));
		goodBuilder.setGoodType(UserToolConst.getType(goodInfo.getInt("gtype")));
		goodBuilder.setGoodPrice(goodInfo.getInt("pri"));
		int count = goodInfo.getInt("cnt");
		if (memberBuy.containsKey(String.valueOf(goodInfo.getInt("id")))) {
			count = count - memberBuy.getJSONObject(String.valueOf(goodInfo.getInt("id"))).getInt("c");
			if (count < 0) {
				count = 0;
			}
		}
		goodBuilder.setGoodCount(count);
		goodBuilder.setGoodBuyPermissions(goodInfo.getInt("per"));
		goodBuilder.setGoodLevel(goodInfo.getInt("lvl"));
		
		return goodBuilder;
	}
	
	private static JSONObject calcBuildingLevel(int id,JSONObject buildInfo,int addExp){
		int currentLevel = buildInfo.getInt("l");
		int xp = buildInfo.getInt("xp");
		int mxp = buildInfo.getInt("mxp");
		int newXp = xp + addExp;
		int bn = buildInfo.getInt("bn");
		
		while (newXp >= mxp && mxp != 0) {
			currentLevel = currentLevel + 1;
			newXp = newXp - mxp;
			GuildLevelConfig config = XMLTemplateService.getGuildConfig().getLevelConvigByLevel(currentLevel);
			mxp = getLevelXpByBuildId(config, id);
			bn = getAltarNumByLevelAndId(config, id);
		}
		
		xp = newXp;
		
		buildInfo.put("l", currentLevel);
		buildInfo.put("xp", xp);
		buildInfo.put("mxp", mxp);
		buildInfo.put("bn", bn);
		
		return buildInfo;
	}
	
	private static boolean getMemberPurches(LeaguaMembers member){
		boolean result = false;
		
		String shopRefreshCD = XMLTemplateService.getGuildConfig().getShopRefreshCD();
		String[] refreshTimeArr = shopRefreshCD.split(",");
		SimpleDateFormat format = new SimpleDateFormat("HHmm");
		int curTime = Integer.parseInt(format.format(new Date()));
		
		int index = 1;
		for (int i = 0; i < refreshTimeArr.length; i++) {
			if (curTime < Integer.parseInt(refreshTimeArr[i])) {
				index = i + 1;
				break;
			}
		}
		
		format = new SimpleDateFormat("dd");
		int curDay = Integer.parseInt(format.format(new Date()));
		
		index = 10 * curDay + index;
		
		if (index != member.getLastShopRefreshTime()) {
			member.setShopPurchaseInfo(new JSONObject());
			member.setLastShopRefreshTime(index);
			result = true;
		}
		
		return result;
	}
	
	private static boolean getShopSells(Leagua leagua){
		boolean result = false;
		String shopRefreshCD = XMLTemplateService.getGuildConfig().getShopRefreshCD();
		String[] refreshTimeArr = shopRefreshCD.split(",");
		SimpleDateFormat format = new SimpleDateFormat("HHmm");
		int curTime = Integer.parseInt(format.format(new Date()));
		int index = 1;
		for (int i = 0; i < refreshTimeArr.length; i++) {
			if (curTime < Integer.parseInt(refreshTimeArr[i])) {
				index = i + 1;
				break;
			}
		}
		
		format = new SimpleDateFormat("dd");
		int curDay = Integer.parseInt(format.format(new Date()));
		
		index = 10 * curDay + index;
		
		if (index != leagua.getLeaguaShopRefreshTime()) {
			leagua.setLeaguaShopInfoFromJson(createShopSellInfo(leagua));
			leagua.setLeaguaShopRefreshTime(index);
			result = true;
		}
		return result;
	}
	
	private static JSONObject createShopSellInfo(Leagua leagua){
		JSONObject object = new JSONObject();
		
		int level = leagua.getLeaguaBuildingInfoInJson().getJSONObject(String.valueOf(LeaguaDefine.SHOP_ID)).getInt("l");
		int levelCanSellCount = XMLTemplateService.getGuildConfig().getLevelConfigs().get(level).getCatalogCount();
		ArrayList<GuildShopSell> levelCanSells = new ArrayList<GuildShopSell>();
		HashMap<Integer, HashMap<Integer, GuildShopSell>> regularShopSells = XMLTemplateService.getGuildConfig().getRegularShopSell();
		
		for (int i = 1; i <= level; i++) {
			if (!regularShopSells.containsKey(i)) {
				continue;
			}
			levelCanSells.addAll(regularShopSells.get(i).values());
		}
		
		ArrayList<GuildShopSell> shopCatalogs = new ArrayList<GuildShopSell>();
		
		ArrayList<GuildShopSell> mustSell = new ArrayList<GuildShopSell>();
		ArrayList<Integer> mustSellRates = new ArrayList<Integer>();
		ArrayList<GuildShopSell> rateSell = new ArrayList<GuildShopSell>();
		ArrayList<Integer> rateSellRates = new ArrayList<Integer>();
		
		for (int i = 0; i < levelCanSells.size(); i++) {
			if (levelCanSells.get(i).getDropPriority() == 1) {
				mustSell.add(levelCanSells.get(i));
				mustSellRates.add(levelCanSells.get(i).getRate());
			}
			else {
				rateSell.add(levelCanSells.get(i));
				rateSellRates.add(levelCanSells.get(i).getRate());
			}
		}
		
		if (mustSell.size() == levelCanSellCount) {
			shopCatalogs = mustSell;
		}
		else if (mustSell.size() > levelCanSellCount) {
			ArrayList<Integer> indexArr = randShopSell(mustSellRates, levelCanSellCount);
			for (int i = 0; i < indexArr.size(); i++) {
				shopCatalogs.add(mustSell.get(indexArr.get(i)));
			}
		}
		else {
			shopCatalogs.addAll(mustSell);
			int needAddCount = levelCanSellCount - mustSell.size();
			if (rateSell.size() <= needAddCount) {
				shopCatalogs.addAll(rateSell);
			}
			else {
				ArrayList<Integer> indexArr = randShopSell(rateSellRates, needAddCount);
				for (int i = 0; i < indexArr.size(); i++) {
					shopCatalogs.add(rateSell.get(indexArr.get(i)));
				}
			}
		}
		
		for (int i = 0; i < shopCatalogs.size(); i++) {
			GuildShopSell sell = shopCatalogs.get(i);
			JSONObject object2 = new JSONObject();
			object2.put("id", sell.getId());
			object2.put("gId", sell.getPropId());
			object2.put("gtype", sell.getItemType());
			object2.put("pri", sell.getPrice());
			object2.put("cnt", sell.getCount());
			object2.put("per", sell.getBuyPermission());
			object2.put("lvl", sell.getShopLvl());
			
			object.put(String.valueOf(sell.getId()), object2);
		}
		
		return object;
	}
	
	private static void leaguaUpgrade(Leagua leagua,int newLevel){
		GuildLevelConfig config = XMLTemplateService.getGuildConfig().getLevelConvigByLevel(newLevel);
		if (levelLeaguaIds.containsKey(leagua.getLeaguaLevel()) && levelLeaguaIds.get(leagua.getLeaguaLevel()).contains(leagua.getLeaguaId())) {
			levelLeaguaIds.get(leagua.getLeaguaLevel()).remove(levelLeaguaIds.get(leagua.getLeaguaLevel()).indexOf(leagua.getLeaguaId()));
		}
		if (!levelLeaguaIds.containsKey(newLevel)) {
			levelLeaguaIds.put(newLevel, new ArrayList<Integer>());
		}
		levelLeaguaIds.get(newLevel).add(leagua.getLeaguaId());
		leagua.setLeaguaMaxMemberCount(config.getGuildMaxMember());
		leagua.setLeaguaLevel(newLevel);
	}
	
	private static void shopUpgrade(Leagua leagua,int oldLevel){
		JSONObject buildsInfo = leagua.getLeaguaBuildingInfoInJson();
		getShopSells(leagua);
		JSONObject shopsInfo = leagua.getLeaguaShopInfoInJson();
		int shopLevel = buildsInfo.getJSONObject(String.valueOf(LeaguaDefine.SHOP_ID)).getInt("l");
		if (shopLevel <= oldLevel) {
			return;
		}
		int levelCanSellCount = XMLTemplateService.getGuildConfig().getLevelConfigs().get(shopLevel).getCatalogCount();
		int addCount = levelCanSellCount - shopsInfo.size();
		if (addCount <= 0) {
			return;
		}
		ArrayList<GuildShopSell> levelCanSells = new ArrayList<GuildShopSell>();
		HashMap<Integer, HashMap<Integer, GuildShopSell>> regularShopSells = XMLTemplateService.getGuildConfig().getRegularShopSell();
		
		for (int i = oldLevel + 1; i <= shopLevel; i++) {
			levelCanSells.addAll(regularShopSells.get(i).values());
		}
		
		ArrayList<GuildShopSell> shopCatalogs = new ArrayList<GuildShopSell>();
		
		ArrayList<GuildShopSell> mustSell = new ArrayList<GuildShopSell>();
		ArrayList<Integer> mustSellRates = new ArrayList<Integer>();
		ArrayList<GuildShopSell> rateSell = new ArrayList<GuildShopSell>();
		ArrayList<Integer> rateSellRates = new ArrayList<Integer>();
		
		for (int i = 0; i < levelCanSells.size(); i++) {
			if (levelCanSells.get(i).getDropPriority() == 1) {
				mustSell.add(levelCanSells.get(i));
				mustSellRates.add(levelCanSells.get(i).getRate());
			}
			else {
				rateSell.add(levelCanSells.get(i));
				rateSellRates.add(levelCanSells.get(i).getRate());
			}
		}
		
		if (mustSell.size() == addCount) {
			shopCatalogs = mustSell;
		}
		else if (mustSell.size() > addCount) {
			ArrayList<Integer> indexArr = randShopSell(mustSellRates, addCount);
			for (int i = 0; i < indexArr.size(); i++) {
				shopCatalogs.add(mustSell.get(indexArr.get(i)));
			}
		}
		else {
			shopCatalogs.addAll(mustSell);
			int needAddCount = addCount - mustSell.size();
			if (rateSell.size() <= needAddCount) {
				shopCatalogs.addAll(rateSell);
			}
			else {
				ArrayList<Integer> indexArr = randShopSell(rateSellRates, needAddCount);
				for (int i = 0; i < indexArr.size(); i++) {
					shopCatalogs.add(rateSell.get(indexArr.get(i)));
				}
			}
		}
		
		for (int i = 0; i < shopCatalogs.size(); i++) {
			GuildShopSell sell = shopCatalogs.get(i);
			JSONObject object2 = new JSONObject();
			object2.put("id", sell.getId());
			object2.put("gId", sell.getPropId());
			object2.put("gtype", sell.getItemType());
			object2.put("pri", sell.getPrice());
			object2.put("cnt", sell.getCount());
			object2.put("per", sell.getBuyPermission());
			object2.put("lvl", sell.getShopLvl());
			
			shopsInfo.put(String.valueOf(sell.getId()), object2);
		}
		
		leagua.setLeaguaShopInfoFromJson(shopsInfo);
	}
	
	private static ArrayList<Integer> randShopSell(ArrayList<Integer> sellRates,int count){
		ArrayList<Integer> indexRates = new ArrayList<Integer>();
		Random random = new Random();
		for (int i = 0; i < count; i++) {
			ArrayList<Integer> rates = new ArrayList<Integer>();
			int temp = 0;
			for (int j = 0; j < sellRates.size(); j++) {
				rates.add(sellRates.get(j) + temp);
				temp += sellRates.get(j);
			}
			
			int rand = random.nextInt(rates.get(rates.size() - 1)) + 1;
			
			for (int k = 0; k < rates.size(); k++) {
				if (rand <= rates.get(k)) {
					indexRates.add(k);
					sellRates.set(k, 0);
					break;
				}
			}
		}
		
		return indexRates;
	}
	
	private static JSONObject checkTotemInfo(JSONObject totemInfo){
		if (totemInfo.containsKey("rt") && totemInfo.getInt("rt") != 0 && Util.getServerTime() - totemInfo.getInt("rt") > XMLTemplateService.getGuildConfig().getTotemRepairNeedSeconds()) {
			totemInfo.put("lh", totemInfo.getInt("mh"));
			totemInfo.remove("rt");
		}
		return totemInfo;
	}
	
	private static float getReceiveContributionRate(int leftHp,int totalHp){
		float rate = 0;
		
		float base = (float)leftHp/totalHp;
		if (base > 0.8) {
			rate = 1.0f;
		}
		else if (base > 0.6) {
			rate = 0.8f;
		}
		else if (base > 0.4) {
			rate = 0.6f;
		}
		else if (base > 0.2) {
			rate = 0.4f;
		}
		else {
			rate = 0.2f;
		}
		
		return rate;
	}
	
	private static float getRobContribution(int leftHp,int totalHp){
		float rate = 0;
		
		float base = leftHp/totalHp;
		if (base > 0.8) {
			rate = 1f;
		}
		else if (base > 0.6) {
			rate = 0.9f;
		}
		else if (base > 0.4) {
			rate = 0.7f;
		}
		else if (base > 0.2) {
			rate = 0.6f;
		}
		else {
			rate = 0.3f;
		}
		
		return rate;
	}
	
	private static JSONObject getLeaguaTotemInfo(Leagua leagua){
		JSONObject totemsInfo = leagua.getLeaguaTotemInfoInJson();
		
		Calendar calendar = Calendar.getInstance();
		int hour = calendar.get(Calendar.HOUR_OF_DAY);
		int day = calendar.get(Calendar.DAY_OF_MONTH);
		int month = calendar.get(Calendar.MONTH) + 1;
		int date = month * 100 + day;
		if (hour >= 19 && leagua.getContributionTime() != date) {
			int totalLeftHp = 0;
			Iterator<String> keys = totemsInfo.keys();
			while (keys.hasNext()) {
				String key = keys.next();
				JSONObject totemInfo = checkTotemInfo(totemsInfo.getJSONObject(key));
				if (!totemInfo.containsKey("rt")) {
					totalLeftHp += totemInfo.getInt("lh");
				}
			}
			
			leagua.setContributionCount(totalLeftHp);
			leagua.setContributionTime(date);
		}
		
		return totemsInfo;
	}
	
	public static void LoadLeaguaDataToRam(){
		log.info("start loading leagua info from db to ram......");
		//服务器所有公会的信息
		List<Leagua> serverLeaguas = DZM.getEntityManager().query(Leagua.class, "from Leagua where 1=?", 1);
		//服务器所有徽章的信息
		List<LeaguaMedal> allMedals = DZM.getEntityManager().query(LeaguaMedal.class, "from LeaguaMedal where 1=?", 1);
		HashMap<Integer, HashMap<Integer,LeaguaMedal>> leaguaMedals = new HashMap<Integer, HashMap<Integer,LeaguaMedal>>();
		ArrayList<Integer> specialMedalIds = XMLTemplateService.getGuildConfig().getMedalsByType(3);
		//徽章按照公会ID配列
		for (int i = 0; i < allMedals.size(); i++) {
			LeaguaMedal medal = allMedals.get(i);
			int ownerLeaguaId = medal.getOwnerLeaguaID();
			if (!leaguaMedals.containsKey(ownerLeaguaId)) {
				leaguaMedals.put(ownerLeaguaId, new HashMap<Integer,LeaguaMedal>());
			}
			leaguaMedals.get(ownerLeaguaId).put(medal.getMedalId(), medal);
			if (medal.getMedalId() != 2) {
				allSpecialLeaguaMedals.put(medal.getMedalId(), medal);
				specialMedalIds.remove(specialMedalIds.indexOf(medal.getMedalId()));
			}
		}
		
		for (int i = 0; i < specialMedalIds.size(); i++) {
			LeaguaMedal medal = new LeaguaMedal();
			medal.setMedalId(specialMedalIds.get(i));
			DZM.getEntityManager().createSync(medal);
			allSpecialLeaguaMedals.put(medal.getMedalId(), medal);
		}
		
		if (serverLeaguas.size() > 0) {
			for (int i = 0; i < serverLeaguas.size(); i++) {
				LeaguaObject leaguaObject = new LeaguaObject();
				
				//设置公会信息
				Leagua leagua = serverLeaguas.get(i);
				allLeaguaNames.add(leagua.getLeaguaName());

				List<Object> list = DZM.getEntityManager().limitQuery("from Player where id = ?", 0, 1, leagua.getOwnerPlayerId());
				if (list.size() != 1) {
					continue;
				}
				Player owner = (Player)list.get(0);
				leagua.setOwnerPlayerName(owner.getName());
				
				
				allLeagua.put(leagua.getLeaguaId(),leagua);
				leaguaObject.setLeaguaInfo(leagua);
				
				
				
				//设置公会徽章
				int leaguaId = leagua.getLeaguaId();
				if (leaguaMedals.containsKey(leaguaId)) {
					leaguaObject.setLeaguaMedals(leaguaMedals.get(leaguaId));
				}
				
				//设置公会成员
				List<LeaguaMembers> leaguaMembers = DZM.getEntityManager().query(LeaguaMembers.class, "from LeaguaMembers where guildId=?", leaguaId);
				HashMap<Integer, LeaguaMembers> members = new HashMap<Integer, LeaguaMembers>();
				for (int j = 0; j < leaguaMembers.size(); j++) {
					LeaguaMembers member = leaguaMembers.get(j);
					members.put(member.getPlayerId(), member);
				}
				leaguaObject.setLeaguaMembersInfo(members);
				
				//设置公会申请
				List<Object> applyList = DZM.getEntityManager().limitQuery("from LeaguaApply where applyLeaguaId=? order by applyTime desc", 0, 50, leaguaId);
				HashMap<Integer, LeaguaApply> applies = new HashMap<Integer, LeaguaApply>();
				for (int k = 0; k < applyList.size(); k++) {
					LeaguaApply apply = (LeaguaApply)applyList.get(k);
					applies.put(apply.getId(), apply);
				}
				leaguaObject.setLeaguaApplys(applies);
				
				//添加到服务器公会信息列表
				allLeaguaObject.put(leaguaId, leaguaObject);
				if (!levelLeaguaIds.containsKey(leagua.getLeaguaLevel())) {
					levelLeaguaIds.put(leagua.getLeaguaLevel(), new ArrayList<Integer>());
				}
				levelLeaguaIds.get(leagua.getLeaguaLevel()).add(leaguaId);
			}
			sortRank();
		}
		log.info("end loading leagua info from db to ram......");
	}
	
	private static void sortRank(){
		leaguaRank = new ArrayList<Leagua>();
		leaguaRank.addAll(allLeagua.values());
		Collections.sort(leaguaRank,Collections.reverseOrder());
		for (int i = 0; i < leaguaRank.size(); i++) {
			leaguaRank.get(i).setLeaguaRank(i+1);
		}
	}
}

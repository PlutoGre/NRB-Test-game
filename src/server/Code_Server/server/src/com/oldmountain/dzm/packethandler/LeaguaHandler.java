package com.oldmountain.dzm.packethandler;

import java.util.ArrayList;

import org.apache.mina.common.IoSession;

import com.google.protobuf.InvalidProtocolBufferException;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.message.LeagueStruct.OPDonateBuilding;
import com.oldmountain.dzm.message.LeagueStruct.OPFireLeaguaMember;
import com.oldmountain.dzm.message.LeagueStruct.OPGetLeaguaBuilding;
import com.oldmountain.dzm.message.LeagueStruct.OPGetLeaguaMedal;
import com.oldmountain.dzm.message.LeagueStruct.OPUpgradeLeaguaMember;
import com.oldmountain.dzm.message.LeagueStructExt.OPChangeLeaguaActiveMedal;
import com.oldmountain.dzm.message.LeagueStructExt.OPClearShopBuyCd;
import com.oldmountain.dzm.message.LeagueStructExt.OPDisbandLeagua;
import com.oldmountain.dzm.message.LeagueStructExt.OPDonateFounds;
import com.oldmountain.dzm.message.LeagueStructExt.OPGetLeaguaRob;
import com.oldmountain.dzm.message.LeagueStructExt.OPGetLeaguaShopInfo;
import com.oldmountain.dzm.message.LeagueStructExt.OPGetTotemRepairCost;
import com.oldmountain.dzm.message.LeagueStructExt.OPLeaguaShopBuy;
import com.oldmountain.dzm.message.LeagueStructExt.OPQuitLeagua;
import com.oldmountain.dzm.message.LeagueStructExt.OPReceiveTotemContribution;
import com.oldmountain.dzm.message.LeagueStructExt.OPRefreshLeaguaBroadcast;
import com.oldmountain.dzm.message.OPLeague;
import com.oldmountain.dzm.message.LeagueStruct.OPCreateLeagua;
import com.oldmountain.dzm.message.LeagueStruct.OPCreateLeaguaRet;
import com.oldmountain.dzm.message.LeagueStruct.OPGetLeaguaApplyInfo;
import com.oldmountain.dzm.message.LeagueStruct.OPGetLeaguaMember;
import com.oldmountain.dzm.message.LeagueStruct.OPGetLeaguaRobList;
import com.oldmountain.dzm.message.LeagueStruct.OPGetTotemInfo;
import com.oldmountain.dzm.message.LeagueStruct.OPGetTotemInfoRet;
import com.oldmountain.dzm.message.LeagueStruct.OPGetUserLeaguaInfo;
import com.oldmountain.dzm.message.LeagueStruct.OPGetUserLeaguaInfoRet;
import com.oldmountain.dzm.message.LeagueStruct.OPReceiveLeaguaMedal;
import com.oldmountain.dzm.message.LeagueStruct.OPRepairTotem;
import com.oldmountain.dzm.message.LeagueStruct.OPTransferLeaguaOwner;
import com.oldmountain.dzm.message.LeagueStructExt.OPApplyLeagua;
import com.oldmountain.dzm.message.LeagueStructExt.OPDealApplyLeagua;
import com.oldmountain.dzm.net.ObjectAccessor;
import com.oldmountain.dzm.net.packet.Packet;
import com.oldmountain.dzm.services.LeaguaServices;
import com.oldmountain.dzm.util.Util;

public class LeaguaHandler {
	public static void getUserLeaguaInfo(Packet packet,IoSession session){
		OPGetUserLeaguaInfo params = null;
		OPGetUserLeaguaInfoRet.Builder builder = OPGetUserLeaguaInfoRet.newBuilder();
		
		try {
			params = OPGetUserLeaguaInfo.parseFrom(packet.data.array());
		} catch (InvalidProtocolBufferException e) {
			e.printStackTrace();
			return;
		}
		
		int version = params.getVersion();
		
		if (version != 1) {
			
		}
		
		int playerId = (Integer)session.getAttribute("playerid");
		Player player = ObjectAccessor.getOnlineUser(playerId);
		
		builder = LeaguaServices.loadUserLeaguaBuilder(player);
		
		player.sendPacket(OPLeague.code.OPCODE_GET_USERLEAGUAINFORET_S_VALUE, builder);
	}
	public static void createLeagua(Packet packet,IoSession session){
		OPCreateLeagua params = null;
		OPCreateLeaguaRet.Builder builder = OPCreateLeaguaRet.newBuilder();
		
		try {
			params = OPCreateLeagua.parseFrom(packet.data.array());
		} catch (InvalidProtocolBufferException e) {
			e.printStackTrace();
			return;
		}
		
		String leaguaName = params.getLeaguaName();
		
		int playerId = (Integer)session.getAttribute("playerid");
		
		Player player = ObjectAccessor.getOnlineUser(playerId);
		
		builder = LeaguaServices.createLeagua(leaguaName, player);
		
		player.sendPacket(OPLeague.code.OPCODE_CREATE_LEAGUARET_S_VALUE, builder);
	}
	
	public static void applyLeagua(Packet packet,IoSession session){
		OPApplyLeagua params = null;
		
		try {
			params = OPApplyLeagua.parseFrom(packet.data.array());
		} catch (InvalidProtocolBufferException e) {
			e.printStackTrace();
			return;
		}
		
		int applyLeaguaId = params.getLeaguaID();
		
		int playerId = (Integer)session.getAttribute("playerid");
		Player player = ObjectAccessor.getOnlineUser(playerId);
		
		player.sendPacket(OPLeague.code.OPCODE_APPLY_LEAGUARET_S_VALUE, LeaguaServices.applyLeagua(applyLeaguaId, player));
	}
	
	public static void getLeaguaApplyInfo(Packet packet,IoSession session){
		OPGetLeaguaApplyInfo params = null;
		
		try {
			params = OPGetLeaguaApplyInfo.parseFrom(packet.data.array());
		} catch (InvalidProtocolBufferException e) {
			e.printStackTrace();
			return;
		}
		
		int version = params.getVersion();
		
		int playerId = (Integer)session.getAttribute("playerid");
		Player player = ObjectAccessor.getOnlineUser(playerId);
		
		player.sendPacket(OPLeague.code.OPCODE_GET_LEAGUA_APPLYINFORET_S_VALUE, LeaguaServices.getLeaguaApplyInfo(player));
		
	}
	
	public static void dealApply(Packet packet,IoSession session){
		OPDealApplyLeagua params = null;
		
		try {
			params = OPDealApplyLeagua.parseFrom(packet.data.array());
		} catch (InvalidProtocolBufferException e) {
			e.printStackTrace();
			return;
		}
		
		int playerId = (Integer)session.getAttribute("playerid");
		Player player = ObjectAccessor.getOnlineUser(playerId);
		
		ArrayList<Integer>applyIds = new ArrayList<Integer>();
		applyIds.addAll(params.getApplyIDList());
		ArrayList<Boolean>results =new ArrayList<Boolean>();
		results.addAll(params.getResultList());
		
		player.sendPacket(OPLeague.code.OPCODE_DEAL_APPLY_LEAGUARET_S_VALUE, LeaguaServices.dealApply(applyIds,results, player));
	}
	
	public static void getTotemInfo(Packet packet,IoSession session) {
		OPGetTotemInfo params = null;
		
		try {
			params = OPGetTotemInfo.parseFrom(packet.data.array());
		} catch (InvalidProtocolBufferException e) {
			e.printStackTrace();
			return;
		}
		
		int version = params.getVersion();
		
		int playerId = (Integer)session.getAttribute("playerid");
		Player player = ObjectAccessor.getOnlineUser(playerId);
		int leaguaId = player.getLeaguaid();
		if (params.hasLeaguaID() && params.getLeaguaID() > 0) {
			leaguaId = params.getLeaguaID();
		}
		
		OPGetTotemInfoRet.Builder builder = LeaguaServices.getTotemInfo(leaguaId,player);
		int coolDown = Util.getServerTime() - LeaguaServices.allLeaguaObject.get(player.getLeaguaid()).getLeaguaMembersInfo().get(player.getId()).getNextCanAttackTime();
		if (coolDown < 0) {
			coolDown = 0;
		}
		builder.setCoolDownSeconds(coolDown);
		
		player.sendPacket(OPLeague.code.OPCODE_GET_TOTEMINFORET_S_VALUE, builder);
	}
	
	public static void getLeaguaRobList(Packet packet,IoSession session){
		OPGetLeaguaRobList params = null;
		
		try {
			params = OPGetLeaguaRobList.parseFrom(packet.data.array());
		} catch (InvalidProtocolBufferException e) {
			e.printStackTrace();
			return;
		}
		
		int playerId = (Integer)session.getAttribute("playerid");
		Player player = ObjectAccessor.getOnlineUser(playerId);
		
		player.sendPacket(OPLeague.code.OPCODE_GET_ROB_LISTRET_VALUE, LeaguaServices.getLeaguaRobList(player));
	}
	
	public static void robLeagua(Packet packet,IoSession session){
		OPGetLeaguaRob params = null;
		
		try {
			params = OPGetLeaguaRob.parseFrom(packet.data.array());
		} catch (InvalidProtocolBufferException e) {
			e.printStackTrace();
			return;
		}
		
		int leaguaId = params.getLeaguaID();
		int totemId = params.getTotemID();
		int playerId = (Integer)session.getAttribute("playerid");
		Player player = ObjectAccessor.getOnlineUser(playerId);
		
		player.sendPacket(OPLeague.code.OPCODE_ROB_LEAGUERET_VALUE, LeaguaServices.robLeaguaTotem(player, leaguaId, totemId));
	}
	
	public static void repairTotem(Packet packet,IoSession session){
		OPRepairTotem params = null;
		
		try {
			params = OPRepairTotem.parseFrom(packet.data.array());
		} catch (InvalidProtocolBufferException e) {
			e.printStackTrace();
			return;
		}
		
		int totemId = params.getTotemID();
		int type = params.getType();
		int playerId = (Integer)session.getAttribute("playerid");
		Player player = ObjectAccessor.getOnlineUser(playerId);
		player.sendPacket(OPLeague.code.OPCODE_REPAIR_TOTEMRET_S_VALUE, LeaguaServices.repairLeaguaTotem(player, totemId,type));
	}
	
	public static void getLeaguaMedalInfo(Packet packet,IoSession session){
		OPGetLeaguaMedal params = null;
		
		try {
			params = OPGetLeaguaMedal.parseFrom(packet.data.array());
		} catch (InvalidProtocolBufferException e) {
			e.printStackTrace();
			return;
		}
		
		int version = params.getVersion();
		int playerId = (Integer)session.getAttribute("playerid");
		Player player = ObjectAccessor.getOnlineUser(playerId);
		
		player.sendPacket(OPLeague.code.OPCODE_GET_LEAGUAMEDALRET_S_VALUE, LeaguaServices.getMedalInfo(player));
	}
	
	public static void receiveMedal(Packet packet,IoSession session){
		OPReceiveLeaguaMedal params = null;
		
		try {
			params = OPReceiveLeaguaMedal.parseFrom(packet.data.array());
		} catch (InvalidProtocolBufferException e) {
			e.printStackTrace();
			return;
		}
		
		int medalId = params.getMedalID();
		
		int playerId = (Integer)session.getAttribute("playerid");
		Player player = ObjectAccessor.getOnlineUser(playerId);
		
		player.sendPacket(OPLeague.code.OPCODE_RECEIVE_LEAGUAMEDALRET_S_VALUE, LeaguaServices.receiveMedal(player, medalId));
	}
	
	public static void changeActiveMedal(Packet packet,IoSession session){
		OPChangeLeaguaActiveMedal params = null;
		
		try {
			params = OPChangeLeaguaActiveMedal.parseFrom(packet.data.array());
		} catch (InvalidProtocolBufferException e) {
			e.printStackTrace();
			return;
		}
		
		int medalId = params.getMedalId();
		
		int playerId = (Integer)session.getAttribute("playerid");
		Player player = ObjectAccessor.getOnlineUser(playerId);
		
		player.sendPacket(OPLeague.code.OPCODE_CHANGE_LEAGUAMEDALRET_VALUE, LeaguaServices.changeActiveMedal(player, medalId));
	}
	
	public static void getLeaguaMembersList(Packet packet,IoSession session){
		OPGetLeaguaMember params = null;
		
		try {
			params = OPGetLeaguaMember.parseFrom(packet.data.array());
		} catch (InvalidProtocolBufferException e) {
			e.printStackTrace();
			return;
		}
		
		int version = params.getVersion();
		
		int playerId = (Integer)session.getAttribute("playerid");
		Player player = ObjectAccessor.getOnlineUser(playerId);
		
		player.sendPacket(OPLeague.code.OPCODE_GET_LEAGUAMEMBER_S_VALUE, LeaguaServices.getLeaguaMemberList(player));
	}
	
	public static void quitLeagua(Packet packet,IoSession session){
		OPQuitLeagua params = null;
		
		try {
			params = OPQuitLeagua.parseFrom(packet.data.array());
		} catch (InvalidProtocolBufferException e) {
			e.printStackTrace();
			return;
		}
		
		int playerId = (Integer)session.getAttribute("playerid");
		Player player = ObjectAccessor.getOnlineUser(playerId);
		
		player.sendPacket(OPLeague.code.OPCODE_QUIT_LEAGUARET_S_VALUE, LeaguaServices.quitLeagua(player));
	}
	
	public static void fireMember(Packet packet,IoSession session){
		OPFireLeaguaMember params = null;
		
		try {
			params = OPFireLeaguaMember.parseFrom(packet.data.array());
		} catch (InvalidProtocolBufferException e) {
			e.printStackTrace();
			return;
		}
		
		int fireId = params.getPlayerID();
		
		int playerId = (Integer)session.getAttribute("playerid");
		Player player = ObjectAccessor.getOnlineUser(playerId);
		
		player.sendPacket(OPLeague.code.OPCODE_FIRE_LEAGUA_MEMBERRET_S_VALUE, LeaguaServices.fireLeaguaMember(player, fireId));
	}
	
	public static void transferLeaguaOwner(Packet packet,IoSession session){
		OPTransferLeaguaOwner params = null;
		
		try {
			params = OPTransferLeaguaOwner.parseFrom(packet.data.array());
		} catch (InvalidProtocolBufferException e) {
			e.printStackTrace();
			return;
		}
		
		int memberId = params.getPlayerID();
		int playerId = (Integer)session.getAttribute("playerid");
		Player player = ObjectAccessor.getOnlineUser(playerId);
		
		player.sendPacket(OPLeague.code.OPCODE_TRANSFER_LEAGUAOWNERRET_S_VALUE, LeaguaServices.transferLeaguaOwner(player, memberId));
	}
	
	public static void disbandLeagua(Packet packet,IoSession session){
		OPDisbandLeagua params = null;
		
		try {
			params = OPDisbandLeagua.parseFrom(packet.data.array());
		} catch (InvalidProtocolBufferException e) {
			e.printStackTrace();
			return;
		}
		
		int version = params.getVersion();
		
		int playerId = (Integer)session.getAttribute("playerid");
		Player player = ObjectAccessor.getOnlineUser(playerId);
		
		player.sendPacket(OPLeague.code.OPCODE_DISBAND_LEAGUARET_S_VALUE, LeaguaServices.disbandLeagua(player));
	}
	
	public static void upgradeMemberGrade(Packet packet,IoSession session){
		OPUpgradeLeaguaMember params = null;
		
		try {
			params = OPUpgradeLeaguaMember.parseFrom(packet.data.array());
		} catch (InvalidProtocolBufferException e) {
			e.printStackTrace();
			return;
		}
		
		int version = params.getVersion();
		
		int playerId = (Integer)session.getAttribute("playerid");
		Player player = ObjectAccessor.getOnlineUser(playerId);
		
		player.sendPacket(OPLeague.code.OPCODE_UPGRADE_LEAGUA_MEMBERRET_S_VALUE, LeaguaServices.upgradeLeaguaMember(player));
	}
	
	public static void getLeaguaBuildingInfo(Packet packet,IoSession session){
		OPGetLeaguaBuilding params = null;
		
		try {
			params = OPGetLeaguaBuilding.parseFrom(packet.data.array());
		} catch (InvalidProtocolBufferException e) {
			e.printStackTrace();
			return;
		}
		
		int version = params.getVersion();
		
		int playerId = (Integer)session.getAttribute("playerid");
		Player player = ObjectAccessor.getOnlineUser(playerId);
		
		player.sendPacket(OPLeague.code.OPCODE_GET_LEAGUABUILDINGRET_S_VALUE, LeaguaServices.getLeaguaBuildingInfo(player));
	}
	
	public static void  donateLeaguaBuilding(Packet packet,IoSession session) {
		OPDonateBuilding params = null;
		
		try {
			params = OPDonateBuilding.parseFrom(packet.data.array());
		} catch (InvalidProtocolBufferException e) {
			e.printStackTrace();
			return;
		}
		
		int buildId = params.getBuildingID();
		int type = params.getDonateType();
		
		int playerId = (Integer)session.getAttribute("playerid");
		Player player = ObjectAccessor.getOnlineUser(playerId);
		
		player.sendPacket(OPLeague.code.OPCODE_DONATE_BUILDINGRET_S_VALUE, LeaguaServices.donateBuilding(player, buildId, type));
	}
	
	public static void donateLeaguaFunds(Packet packet,IoSession session){
		OPDonateFounds params = null;
		
		try {
			params = OPDonateFounds.parseFrom(packet.data.array());
		} catch (InvalidProtocolBufferException e) {
			e.printStackTrace();
			return;
		}
		
		int num = params.getNum();
		
		int playerId = (Integer)session.getAttribute("playerid");
		Player player = ObjectAccessor.getOnlineUser(playerId);
		
		player.sendPacket(OPLeague.code.OPCODE_DONATE_FOUNDSRET_S_VALUE, LeaguaServices.donateFounds(player, num));
	}
	
	public static void getLeaguaShopInfo(Packet packet,IoSession session){
		OPGetLeaguaShopInfo params = null;
		
		try {
			params = OPGetLeaguaShopInfo.parseFrom(packet.data.array());
		} catch (InvalidProtocolBufferException e) {
			e.printStackTrace();
			return;
		}
		
		int version = params.getVersion();
		
		int playerId = (Integer)session.getAttribute("playerid");
		Player player = ObjectAccessor.getOnlineUser(playerId);
		
		player.sendPacket(OPLeague.code.OPCODE_GET_LEAGUASHOPINFORET_S_VALUE, LeaguaServices.getLeaguaShopInfo(player));
	}
	
	public static void leaguaShopBuy(Packet packet,IoSession session){
		OPLeaguaShopBuy params = null;
		
		try {
			params = OPLeaguaShopBuy.parseFrom(packet.data.array());
		} catch (InvalidProtocolBufferException e) {
			e.printStackTrace();
			return;
		}
		
		int playerId = (Integer)session.getAttribute("playerid");
		Player player = ObjectAccessor.getOnlineUser(playerId);
		int id = params.getId();
		int goodNum = params.getGoodNum();
		
		player.sendPacket(OPLeague.code.OPCODE_LEAGUASHOPBUYRET_S_VALUE,LeaguaServices.leaguaShopBuy(player, id, goodNum));
	}
	
	public static void refreshBroadcast(Packet packet, IoSession session) {
		OPRefreshLeaguaBroadcast params=null;
		try {
			params = OPRefreshLeaguaBroadcast.parseFrom(packet.data.array());
		} catch (InvalidProtocolBufferException e) {
			e.printStackTrace();
			return;
		}
		int playerId = (Integer)session.getAttribute("playerid");
		Player player = ObjectAccessor.getOnlineUser(playerId);
		String broadcast=params.getBroadcast();
		player.sendPacket(OPLeague.code.OPCODE_REFRESH_LEAGUA_BROADCASTRET_S_VALUE, LeaguaServices.refreshBroadcast(player,broadcast));
	}
	
	public static void clearShopBuyCd(Packet packet,IoSession session){
		OPClearShopBuyCd params = null;
		
		try {
			params = OPClearShopBuyCd.parseFrom(packet.data.array());
		} catch (InvalidProtocolBufferException e) {
			e.printStackTrace();
			return;
		}
		
		int version = params.getVersion();
		
		int playerId = (Integer)session.getAttribute("playerid");
		Player player = ObjectAccessor.getOnlineUser(playerId);
		player.sendPacket(OPLeague.code.OPCODE_CLEAR_SHOPBUY_CDRET_VALUE, LeaguaServices.clearShopBuyCd(player));
	}
	
	public static void receiveTotemContribution(Packet packet,IoSession session){
		OPReceiveTotemContribution params = null;
		
		try {
			params = OPReceiveTotemContribution.parseFrom(packet.data.array());
		} catch (InvalidProtocolBufferException e) {
			e.printStackTrace();
			return;
		}
		
		int version = params.getVersion();
		
		int playerId = (Integer)session.getAttribute("playerid");
		Player player = ObjectAccessor.getOnlineUser(playerId);
		player.sendPacket(OPLeague.code.OPCODE_RECEIVE_TOTEMCONTRIBUTIONRET_S_VALUE, LeaguaServices.receiveTotemContribution(player));
	}
	
	public static void getTotemRepairCost(Packet packet,IoSession session){
		OPGetTotemRepairCost params = null;
		
		try {
			params = OPGetTotemRepairCost.parseFrom(packet.data.array());
		} catch (InvalidProtocolBufferException e) {
			e.printStackTrace();
			return;
		}
		
		int totemId = params.getTotemID();
		int playerId = (Integer)session.getAttribute("playerid");
		Player player = ObjectAccessor.getOnlineUser(playerId);
		
		player.sendPacket(OPLeague.code.OPCODE_TOTEM_REPAIR_COSTRET_S_VALUE, LeaguaServices.getTotemRepairCost(player, totemId));
	}
}

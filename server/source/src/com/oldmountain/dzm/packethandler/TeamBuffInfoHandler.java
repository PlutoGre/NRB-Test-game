package com.oldmountain.dzm.packethandler;

import java.util.ArrayList;

import org.apache.mina.common.IoSession;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.google.protobuf.InvalidProtocolBufferException;
import com.oldmountain.dzm.entity.Battle;
import com.oldmountain.dzm.entity.Disciple;
import com.oldmountain.dzm.entity.DiscipleXmlTemplate;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.entity.Soul;
import com.oldmountain.dzm.entity.TeamBuffer;
import com.oldmountain.dzm.message.OP;
import com.oldmountain.dzm.message.ErrorReporter.OPErrorReporter;
import com.oldmountain.dzm.message.TeamBuffInfo.OPTeamBuffInfo;
import com.oldmountain.dzm.message.TeamBuffInfo.OPTeamBuffInfoRet;
import com.oldmountain.dzm.message.UpgradeTeamBuffInfo.OPUpgradeTeamBuffInfo;
import com.oldmountain.dzm.message.UpgradeTeamBuffInfo.OPUpgradeTeamBuffInfoRet;
import com.oldmountain.dzm.message.UserBattleArray.OPUserBattleArrayRet;
import com.oldmountain.dzm.net.ObjectAccessor;
import com.oldmountain.dzm.net.packet.Packet;
import com.oldmountain.dzm.util.BattleUtil;
import com.oldmountain.dzm.util.Util;
import com.oldmountain.dzm.util.XMLTemplateService;

/**
 * 留言包
 * 1， 解析数据包
 * 2， 保存留言数据
 * 3， 推送给相关玩家数据信息
 * 4， 下发留言成功
 * @author lvjc
 */
public class TeamBuffInfoHandler {
	private static final Logger logger2 = LoggerFactory.getLogger("TeamBuffInfo");
	public static void getTeamBuffInfo(Packet packet, IoSession session)
    {
        OPTeamBuffInfo params = null;
        OPTeamBuffInfoRet.Builder builder = OPTeamBuffInfoRet.newBuilder();
		try
        {
			params = OPTeamBuffInfo.parseFrom(packet.data.array());
		}
        catch (InvalidProtocolBufferException e)
        {
			e.printStackTrace();
			return;
		}
		
		int version = params.getVersion();//version default = 1;
        int playerId = (Integer)session.getAttribute("playerid");
        Player player = ObjectAccessor.getOnlineUser(playerId);
        int id=player.getId();
        logger2.info("team buff info handler received: {}, {}, {}", new Object[]{id , version});
        long current_time = Util.getServerMillTime();

        int level = player.getLevel();
        
        if (level < 5) {
			OPErrorReporter.Builder errBuilder = OPErrorReporter.newBuilder();
			errBuilder.setErrorid(OP.code.OPCODE_GET_TEAM_BUFF_INFORET_S_VALUE);
			errBuilder.setErrormsg("user level not enough!");
			return ;
		}
        
        TeamBuffer teamBuffer = player.getTeamBuffer();
        
        OPTeamBuffInfoRet.TeamBuff.Builder teamBufferBuilder = OPTeamBuffInfoRet.TeamBuff.newBuilder();
        teamBufferBuilder.setTeamId(1);
        teamBufferBuilder.setTeamLevel(teamBuffer.getHealthbufferlevel());
        teamBufferBuilder.setCurrBoldness(teamBuffer.getHealthbufferexp() - teamBuffer.getHealthbufferlevel() * (teamBuffer.getHealthbufferlevel() + 1) * 2);
        builder.addTeamBuff(teamBufferBuilder);
        if (level >= 10) {
        	teamBufferBuilder = OPTeamBuffInfoRet.TeamBuff.newBuilder();
        	teamBufferBuilder.setTeamId(4);
        	teamBufferBuilder.setTeamLevel(teamBuffer.getRpbufferlevel());
        	teamBufferBuilder.setCurrBoldness(teamBuffer.getRpbufferexp() - teamBuffer.getRpbufferlevel() * (teamBuffer.getRpbufferlevel() + 1) * 2);
        	builder.addTeamBuff(teamBufferBuilder);
		}
        if (level >= 15) {
        	teamBufferBuilder = OPTeamBuffInfoRet.TeamBuff.newBuilder();
        	teamBufferBuilder.setTeamId(3);
        	teamBufferBuilder.setTeamLevel(teamBuffer.getDefencebufferlevel());
        	teamBufferBuilder.setCurrBoldness(teamBuffer.getDefencebufferexp() - teamBuffer.getDefencebufferlevel() * (teamBuffer.getDefencebufferlevel() + 1) * 2);
        	builder.addTeamBuff(teamBufferBuilder);
		}
        if (level >= 20) {
        	teamBufferBuilder = OPTeamBuffInfoRet.TeamBuff.newBuilder();
        	teamBufferBuilder.setTeamId(2);
        	teamBufferBuilder.setTeamLevel(teamBuffer.getAttackbufferlevel());
        	teamBufferBuilder.setCurrBoldness(teamBuffer.getAttackbufferexp() - teamBuffer.getAttackbufferlevel() * (teamBuffer.getAttackbufferlevel() + 1) * 2);
        	builder.addTeamBuff(teamBufferBuilder);
		}
        
        player.setLastVerifyTime(Util.getServerTime());
        
        builder.setServertime((int) System.currentTimeMillis());
		player.sendPacket(OP.code.OPCODE_GET_TEAM_BUFF_INFORET_S_VALUE, builder);// 发送返回包
		long current_time_2 = Util.getServerMillTime();
		long taken_time=current_time_2-current_time;
		logger2.info("team buff info handler ret send: {}, {}, {}", new Object[]{id ,taken_time});
	}
	
	public static void upgradeTeamBuffInfo(Packet packet, IoSession session)
    {
        OPUpgradeTeamBuffInfo upgradeTeamBuffInfo = null;
        OPUpgradeTeamBuffInfoRet.Builder builder = OPUpgradeTeamBuffInfoRet.newBuilder();
        OPErrorReporter.Builder errorBuilder = OPErrorReporter.newBuilder();
		try
        {
			upgradeTeamBuffInfo = OPUpgradeTeamBuffInfo.parseFrom(packet.data.array());
		}
        catch (InvalidProtocolBufferException e)
        {
			e.printStackTrace();
			return;
		}
		
		int version = upgradeTeamBuffInfo.getVersion();//version default = 1;
		int teamId = upgradeTeamBuffInfo.getTeamId();//version default = 1;
		int memberCount = upgradeTeamBuffInfo.getMemberListCount();
		int soulCount = upgradeTeamBuffInfo.getSoulListCount();
		
		int playerId = (Integer)session.getAttribute("playerid");
        Player player = ObjectAccessor.getOnlineUser(playerId);
        String puid=player.getPuid();
        logger2.info("team buff info handler received: upgradeTeamBuffInfo : {}, {}", new Object[]{puid , version , teamId});
        long current_time = Util.getServerMillTime();
        
		if (memberCount + soulCount <= 0) {
			errorBuilder.setErrorid(OP.code.OPCODE_UPGRADE_TEAM_BUFF_INFORET_S_VALUE);
			errorBuilder.setErrormsg("no soul or disciple send!");
			player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errorBuilder);
			logger2.info("Error Reporter send: team buff info handler: upgradeTeamBuffInfo : {}, {}, {}", new Object[]{puid});
			return;
		}
		
		if (!checkLevelCanDo(player.getLevel(), teamId)) {
			errorBuilder.setErrorid(OP.code.OPCODE_UPGRADE_TEAM_BUFF_INFORET_S_VALUE);
			errorBuilder.setErrormsg("level not enough!");
			player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errorBuilder);
			logger2.info("Error Reporter send: team buff info handler: upgradeTeamBuffInfo : {}, {}, {}", new Object[]{puid});
			return;
		}
		
		ArrayList<int[]> souls = new ArrayList<int[]>();
		ArrayList<Integer> disciples = new ArrayList<Integer>();
        
        for(int i=0;i<memberCount;i++)
        {
            OPUpgradeTeamBuffInfo.CousumeItem cousumeItem=upgradeTeamBuffInfo.getMemberList(i);
            if (player.getDisciples().get(cousumeItem.getMemberId()) == null) {
            	errorBuilder.setErrorid(OP.code.OPCODE_UPGRADE_TEAM_BUFF_INFORET_S_VALUE);
    			errorBuilder.setErrormsg("disciple " + cousumeItem.getMemberId() + "not exist!");
    			player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errorBuilder);
    			logger2.info("Error Reporter send: team buff info handler: upgradeTeamBuffInfo : {}, {}, {}", new Object[]{puid});
    			return;
			}
            disciples.add(cousumeItem.getMemberId());
        }

        for(int i=0;i<soulCount;i++)
        {
            OPUpgradeTeamBuffInfo.CousumeItem cousumeItem=upgradeTeamBuffInfo.getSoulList(i);
            if (player.getSouls().get(cousumeItem.getMemberId()) == null || player.getSouls().get(cousumeItem.getMemberId()).getCount() < cousumeItem.getCount()) {
            	errorBuilder.setErrorid(OP.code.OPCODE_UPGRADE_TEAM_BUFF_INFORET_S_VALUE);
    			errorBuilder.setErrormsg("soul " + cousumeItem.getMemberId() + "not exist or not enough for count" + cousumeItem.getCount());
    			player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errorBuilder);
    			logger2.info("Error Reporter send: team buff info handler: upgradeTeamBuffInfo : {}, {}, {}", new Object[]{puid});
    			return;
			}
            int[] soul = new int[]{cousumeItem.getMemberId(),cousumeItem.getCount()};
            souls.add(soul);
        }
        
        int addExp = 0;
        
        for (int i = 0; i < souls.size(); i++) {
        	int[] soul = souls.get(i);
			DiscipleXmlTemplate soulInfo = XMLTemplateService.getDiscipleTemplate(soul[0]);
			addExp += getAddExpByQuality(soulInfo.getQuality(), true, soul[1]);
		}
        
        for (int i = 0; i < disciples.size(); i++) {
			Disciple disciple = player.getDisciples().get(disciples.get(i));
			DiscipleXmlTemplate discipleInfo = XMLTemplateService.getDiscipleTemplate(disciple.getItemid());
			addExp += getAddExpByQuality(discipleInfo.getQuality(), false, 1);
		}
        
        if (addExp <= 0) {
        	errorBuilder.setErrorid(OP.code.OPCODE_UPGRADE_TEAM_BUFF_INFORET_S_VALUE);
			errorBuilder.setErrormsg("addexp is small then zero");
			player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errorBuilder);
			logger2.info("Error Reporter send: team buff info handler: upgradeTeamBuffInfo : {}, {}, {}", new Object[]{puid});
			return;
		}
        
        TeamBuffer teamBuffer = player.getTeamBuffer();
        int newExp = 0;
        int originLevel = 0;
        int addQuantity = 0;
        int newLevel = 0;
        switch (teamId) {
			case 1:
				newExp = teamBuffer.getHealthbufferexp() + addExp;
				newLevel = getLevelByExp(newExp);
				if(newLevel>player.getLevel()){
					errorBuilder.setErrorid(OP.code.OPCODE_UPGRADE_TEAM_BUFF_INFORET_S_VALUE);
	    			errorBuilder.setErrormsg("exp out of bounds");
	    			player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errorBuilder);
	    			logger2.info("Error Reporter send: team buff info handler: upgradeTeamBuffInfo : {}, {}, {}", new Object[]{puid});
	    			return;
				}
				teamBuffer.setHealthbufferexp(newExp);
				originLevel = teamBuffer.getHealthbufferlevel();
				teamBuffer.setHealthbufferlevel(newLevel);
				addQuantity = 6 * (newLevel - originLevel);
				break;
			case 2:
				newExp = teamBuffer.getAttackbufferexp() + addExp;
				newLevel = getLevelByExp(newExp);
				if(newLevel>player.getLevel()){
					errorBuilder.setErrorid(OP.code.OPCODE_UPGRADE_TEAM_BUFF_INFORET_S_VALUE);
	    			errorBuilder.setErrormsg("exp out of bounds");
	    			player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errorBuilder);
	    			logger2.info("Error Reporter send: team buff info handler: upgradeTeamBuffInfo : {}, {}, {}", new Object[]{puid});
	    			return;
				}
				teamBuffer.setAttackbufferexp(newExp);
				originLevel = teamBuffer.getAttackbufferlevel();
				teamBuffer.setAttackbufferlevel(newLevel);
				addQuantity = 3 * (newLevel - originLevel);
				break;
			case 3:
				newExp = teamBuffer.getDefencebufferexp() + addExp;
				newLevel = getLevelByExp(newExp);
				if(newLevel>player.getLevel()){
					errorBuilder.setErrorid(OP.code.OPCODE_UPGRADE_TEAM_BUFF_INFORET_S_VALUE);
	    			errorBuilder.setErrormsg("exp out of bounds");
	    			player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errorBuilder);
	    			logger2.info("Error Reporter send: team buff info handler: upgradeTeamBuffInfo : {}, {}, {}", new Object[]{puid});
	    			return;
				}
				teamBuffer.setDefencebufferexp(newExp);
				originLevel = teamBuffer.getDefencebufferlevel();
				teamBuffer.setDefencebufferlevel(newLevel);
				addQuantity = 2 * (newLevel - originLevel);
				break;
			case 4:
				newExp = teamBuffer.getRpbufferexp() + addExp;
				newLevel = getLevelByExp(newExp);
				if(newLevel>player.getLevel()){
					errorBuilder.setErrorid(OP.code.OPCODE_UPGRADE_TEAM_BUFF_INFORET_S_VALUE);
	    			errorBuilder.setErrormsg("exp out of bounds");
	    			player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errorBuilder);
	    			logger2.info("Error Reporter send: team buff info handler: upgradeTeamBuffInfo : {}, {}, {}", new Object[]{puid});
	    			return;
				}
				teamBuffer.setRpbufferexp(newExp);
				originLevel = teamBuffer.getRpbufferlevel();
				teamBuffer.setRpbufferlevel(newLevel);
				addQuantity = 8 * (newLevel - originLevel);
				break;
		}
        /**将对数据的操作抽出,判定成功后再修改*/
        for (int i = 0; i < souls.size(); i++) {
        	int[] soul = souls.get(i);
			Soul userSoul = player.getSouls().get(soul[0]);
			userSoul.setCount(userSoul.getCount() - soul[1]);
			player.updateOneSoul(userSoul);
		}
        /**将对数据的操作抽出,判定成功后再修改*/
        for (int i = 0; i < disciples.size(); i++) {
			Disciple disciple = player.getDisciples().get(disciples.get(i));
			player.removeOneDisciple(disciple);
		}
        player.setTeamBuffer(teamBuffer);
        
        if (addQuantity > 0) {
			ArrayList<Battle> battles = BattleUtil.getUserBattleArrayObject(player).getBattlearrays();
			for (int i = 0; i < battles.size(); i++) {
				Battle battle = battles.get(i);
				if (battle.getDiscipleid() == 0) {
					continue;
				}
				switch (teamId) {
					case 1:
						battle.setHealth(battle.getHealth() + addQuantity);
						break;
					case 2:
						battle.setAttack(battle.getAttack() + addQuantity);
						break;
					case 3:
						battle.setDefence(battle.getDefence() + addQuantity);
						break;
					case 4:
						battle.setRp(battle.getRp() + addQuantity);
						break;
				}
				player.updateOneBattle(battle);
				OPUserBattleArrayRet.BattleArrayInfo.Builder battleBuilder = OPUserBattleArrayRet.BattleArrayInfo.newBuilder();
				BattleUtil.buildBattleBuilder(battleBuilder, battle, i);
				builder.addBattle(battleBuilder);
			}
		}
        
        player.setLastVerifyTime(Util.getServerTime());
        
        OPUpgradeTeamBuffInfoRet.TeamBuffer.Builder teamBuilder = OPUpgradeTeamBuffInfoRet.TeamBuffer.newBuilder();
        teamBuilder.setId(teamId);
        teamBuilder.setLevel(newLevel);
        teamBuilder.setExp(newExp - newLevel*(newLevel+1)*2);

        builder.setTeamBuffer(teamBuilder);
        
        builder.setStatus((int)(Math.random()%2));
        builder.setServertime((int) System.currentTimeMillis());
		player.sendPacket(OP.code.OPCODE_UPGRADE_TEAM_BUFF_INFORET_S_VALUE, builder);// 发送返回包
		long current_time_2 = Util.getServerMillTime();
		long taken_time=current_time_2-current_time;
		logger2.info("team buff info handler ret send: upgradeTeamBuffInfo : {}, {}, {}", new Object[]{puid ,taken_time});
	}
	
	public static boolean checkLevelCanDo(int level,int teamId){
		Boolean canDo = true;
		switch (teamId) {
			case 1:
				if (level < 5) {
					canDo = false;
				}
				break;
			case 4:
				if (level < 10) {
					canDo = false;
				}
				break;
			case 3:
				if (level < 15) {
					canDo = false;
				}
				break;
			case 2:
				if(level < 20){
					canDo = false;
				}
				break;
		}
		return canDo;
	}
	
	public static int getAddExpByQuality(int quality,Boolean isSoul,int count){
		int addExp = 0;
		switch (quality) {
			case 1:
				if (isSoul) {
					addExp = 4;
				} 
				else {
					addExp = 72;
				}
				break;
			case 2:
				if (isSoul) {
					addExp = 3;
				} 
				else {
					addExp = 27;
				}
				break;
			case 3:
				if (isSoul) {
					addExp = 2;
				} 
				else {
					addExp = 6;
				}
				break;
			case 4:
				if (isSoul) {
					addExp = 1;
				} 
				else {
					addExp = 1;
				}
				break;
		}
		addExp *= count;
		return addExp;
	}
	
	public static int getLevelByExp(int exp){
		int level = 0;
		level = (int)Math.floor((Math.pow((exp * 2 + 1), 0.5) - 1) / 2);
		return level;
	}

}

package com.oldmountain.dzm.packethandler;

import java.util.HashMap;

import org.apache.mina.common.IoSession;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.google.protobuf.InvalidProtocolBufferException;
import com.oldmountain.dzm.entity.Bag;
import com.oldmountain.dzm.entity.Battle;
import com.oldmountain.dzm.entity.Disciple;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.entity.Skill;
import com.oldmountain.dzm.message.OP;
import com.oldmountain.dzm.message.ErrorReporter.OPErrorReporter;
import com.oldmountain.dzm.message.SwallowDisciple.OPSwallowDisciple;
import com.oldmountain.dzm.message.SwallowDisciple.OPSwallowDiscipleRet;
import com.oldmountain.dzm.message.UserBattleArray.OPUserBattleArrayRet;
import com.oldmountain.dzm.net.ObjectAccessor;
import com.oldmountain.dzm.net.packet.Packet;
import com.oldmountain.dzm.util.BattleUtil;
import com.oldmountain.dzm.util.Util;

public class SwallowDiscipleHandler {
	private static final Logger logger2 = LoggerFactory.getLogger("DiscipleInfo");
	public static void swallowDisciple(Packet packet,IoSession session){
		OPSwallowDisciple params = null;
		OPSwallowDiscipleRet.Builder builder = OPSwallowDiscipleRet.newBuilder();
		
		try {
			params = OPSwallowDisciple.parseFrom(packet.data.array());
		} catch (InvalidProtocolBufferException e) {
			e.printStackTrace();
			return;
		}
		
		int id = params.getId();
		int swallowedId = params.getSwallowedid();
		int pelletId = params.getPelletid();
		
		int playerId = (Integer)session.getAttribute("playerid");
		Player player = ObjectAccessor.getOnlineUser(playerId);
		int playerid=player.getId();
		logger2.info("swallow disciple handler received: {}, {}, {}", new Object[]{playerid , id , swallowedId , pelletId});
		long current_time = Util.getServerMillTime();
		HashMap<Integer, Disciple> userDisciples = player.getDisciples();
		HashMap<Integer, Bag> userBags = player.getBags();
		
		if (!userDisciples.containsKey(id) || !userDisciples.containsKey(swallowedId)) {
			OPErrorReporter.Builder errBuilder = OPErrorReporter.newBuilder();
			errBuilder.setErrorid(OP.code.OPCODE_SWALLOW_DISCIPLERET_S_VALUE);
			errBuilder.setErrormsg("disciple not exist!");
			player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errBuilder);
			return ;
		}
		
		if (!userBags.containsKey(pelletId) || userBags.get(pelletId).getCount() < 1) {
			OPErrorReporter.Builder errBuilder = OPErrorReporter.newBuilder();
			errBuilder.setErrorid(OP.code.OPCODE_SWALLOW_DISCIPLERET_S_VALUE);
			errBuilder.setErrormsg("pellet count not enough!");
			player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errBuilder);
			return ;
		}
		
		Bag pellet = userBags.get(pelletId);
		int pelletItem = pellet.getItemid();
		
		double rate = 0;
		switch (pelletItem) {
			case 1503002:
				rate = 1;
				break;
			case 1503001:
				rate = 0.8;
				break;
			default:
				OPErrorReporter.Builder errBuilder = OPErrorReporter.newBuilder();
				errBuilder.setErrorid(OP.code.OPCODE_SWALLOW_DISCIPLERET_S_VALUE);
				errBuilder.setErrormsg("pellet id error!");
				player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errBuilder);
				return ;
		}
		
		Disciple disciple = userDisciples.get(id);
		Disciple sDisciple = userDisciples.get(swallowedId);
		
		if (sDisciple.getBattleid() != 0) {//弟子在阵容中，不能被吞噬
			OPErrorReporter.Builder errBuilder = OPErrorReporter.newBuilder();
			errBuilder.setErrorid(OP.code.OPCODE_SWALLOW_DISCIPLERET_S_VALUE);
			errBuilder.setErrormsg("disciple is in battle!");
			player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errBuilder);
			return ;
		}
		
		if (disciple.getLevel() >= player.getLevel() * 3) {
			OPErrorReporter.Builder errBuilder = OPErrorReporter.newBuilder();
			errBuilder.setErrorid(OP.code.OPCODE_SWALLOW_DISCIPLERET_S_VALUE);
			errBuilder.setErrormsg("over level limit!");
			player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errBuilder);
			return ;
		}

		int addXp = (int)Math.round(sDisciple.getExp() * rate);
		
		Util.getlevelByExp(disciple, addXp);
		
		player.removeOneDisciple(sDisciple);
		
		userDisciples.put(id, disciple);
		if (disciple.getBattleid() != 0) {
			int battleid = disciple.getBattleid();
			String battleids = player.getBattleids();
			String[] battleArray = battleids.split(",");
			for (int i = 0; i < battleArray.length; i++) {
				if (battleid == Integer.parseInt(battleArray[i])) {
					Battle battle = player.getBattlearray().get(battleid);
					BattleUtil.resetBattleProperty(battle, disciple,player,null,0);
					OPUserBattleArrayRet.BattleArrayInfo.Builder battleBuilder= OPUserBattleArrayRet.BattleArrayInfo.newBuilder();
					BattleUtil.buildBattleBuilder(battleBuilder, battle,i);
					builder.setBattle(battleBuilder);
					player.updateOneBattle(battle);
					BattleUtil.getUserBattleArrayObject(player).getBattlearrays().set(i, battle);
					break;
				}
			}
		}
		pellet.setCount(pellet.getCount()-1);
		player.updateOneDisciple(disciple);
		player.updateOneBag(pellet);
		
		player.setLastVerifyTime(Util.getServerTime());
		
		OPSwallowDiscipleRet.DiscipleInfo.Builder builder2 = OPSwallowDiscipleRet.DiscipleInfo.newBuilder();
		builder2.setId(id);
		builder2.setLevel(disciple.getLevel());
		builder2.setExp(disciple.getExp());
		builder2.setAttack(disciple.getAttackToInt());
		builder2.setDefence(disciple.getDefenceToInt());
		builder2.setHealth(disciple.getHealthToInt());
		builder2.setRp(disciple.getRpToInt());
		builder2.setPotentiality(disciple.getPotentiality());
		builder.setDisciple(builder2);
		
		OPSwallowDiscipleRet.ToolInfo.Builder builder3 = OPSwallowDiscipleRet.ToolInfo.newBuilder();
		builder3.setId(pelletId);
		builder3.setCount(pellet.getCount());
		builder.setTool(builder3);
		
		builder.setSwallowedid(swallowedId);
		
		builder.setServertime(Util.getServerTime());
		
		player.sendPacket(OP.code.OPCODE_SWALLOW_DISCIPLERET_S_VALUE, builder);
		
		long current_time_2 = Util.getServerMillTime();
		long taken_time=current_time_2-current_time;
		logger2.info("swallow disciple handler ret send: {}, {}, {}", new Object[]{playerid ,taken_time});
	}
}

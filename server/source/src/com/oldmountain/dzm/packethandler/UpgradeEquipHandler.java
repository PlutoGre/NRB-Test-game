package com.oldmountain.dzm.packethandler;

import java.util.HashMap;
import java.util.Random;

import org.apache.mina.common.IoSession;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.google.protobuf.InvalidProtocolBufferException;
import com.oldmountain.dzm.entity.Battle;
import com.oldmountain.dzm.entity.Disciple;
import com.oldmountain.dzm.entity.Equip;
import com.oldmountain.dzm.entity.EquipXmlTemplate;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.message.OP;
import com.oldmountain.dzm.message.ErrorReporter.OPErrorReporter;
import com.oldmountain.dzm.message.UpgradeEquip.OPUpgradeEquip;
import com.oldmountain.dzm.message.UpgradeEquip.OPUpgradeEquipRet;
import com.oldmountain.dzm.message.UserBattleArray.OPUserBattleArrayRet;
import com.oldmountain.dzm.net.ObjectAccessor;
import com.oldmountain.dzm.net.packet.Packet;
import com.oldmountain.dzm.util.BattleUtil;
import com.oldmountain.dzm.util.EquipUtil;
import com.oldmountain.dzm.util.Util;
import com.oldmountain.dzm.util.XMLTemplateService;

public class UpgradeEquipHandler {
	private static final Logger logger2 = LoggerFactory.getLogger("EquipInfo");
	public static void  doUpgradeEquip(Packet packet,IoSession session){
		OPUpgradeEquip params = null;
		OPUpgradeEquipRet.Builder builder = OPUpgradeEquipRet.newBuilder();
		OPErrorReporter.Builder errBuilder = OPErrorReporter.newBuilder();
		
		try {
			params = OPUpgradeEquip.parseFrom(packet.data.array());
		} catch (InvalidProtocolBufferException e) {
			e.printStackTrace();
		}
		
		int equipId = params.getId();
		
		int playerId = (Integer)session.getAttribute("playerid");
		Player player = ObjectAccessor.getOnlineUser(playerId);
		int id=player.getId();
		logger2.info("upgrade equip handler received: {}, {}, {}", new Object[]{id , equipId});
		long current_time = Util.getServerMillTime();
		HashMap<Integer, Equip> userEquips = player.getEquips();
		
		if (!userEquips.containsKey(equipId)) {
			errBuilder.setErrorid(OP.code.OPCODE_UPGRADE_EQUIPRET_S_VALUE);
			errBuilder.setErrormsg("equip not exist");
			player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errBuilder);
			logger2.info("Error Reporter send: upgrade equip handler: {}, {}, {}", new Object[]{id});
			return;
		}
		else {
			Equip equip = userEquips.get(equipId);
			EquipXmlTemplate equipConfig = XMLTemplateService.getEquipTemplate(equip.getItemid());
			int quality = equipConfig.getQuality();
			Random random = new Random();
			int sum_cost=0;
			int sum_level=0;
			int sum_times=10;
			int max_times=0;
			//如果传入quickUpdate,就进行连续强化,最大强化次数设为10,否则设为1
			if(params.hasQuickUpdate()){
				max_times=10;
				if(player.getViplevel()<3){
					errBuilder.setErrorid(OP.code.OPCODE_UPGRADE_EQUIPRET_S_VALUE);
					errBuilder.setErrormsg("viplevel is not enough");
					player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errBuilder);
					logger2.info("Error Reporter send: upgrade equip handler: {}, {}, {}", new Object[]{id});
					return;
				}
			}
			else{
				max_times=1;
			}
			for(int i=1; i<=max_times ;i++){
				int costCoins = EquipUtil.getEquipUpgradeBase(quality, equip.getType(),equip.getLevel());
				if (player.getSilvercoins() < sum_cost+costCoins) {
					if(i==1){
						errBuilder.setErrorid(OP.code.OPCODE_UPGRADE_EQUIPRET_S_VALUE);
						errBuilder.setErrormsg("coins not enough");
						player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errBuilder);
						logger2.info("Error Reporter send: upgrade equip handler: {}, {}, {}", new Object[]{id});
						return;
					}
					else{
						sum_times=i-1;	
						break;
					}
				}
				if(equip.getLevel()>=3*(player.getLevel())){
					if(i==1){
						errBuilder.setErrorid(OP.code.OPCODE_UPGRADE_EQUIPRET_S_VALUE);
						errBuilder.setErrormsg("arlread arrived the limited lvl");
						player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errBuilder);
						logger2.info("Error Reporter send: upgrade equip handler: {}, {}, {}", new Object[]{id});
						return;
					}
					else{
						sum_times=i-1;
						break;
					}
				}
				int maxCrit = player.getViplevel()+2 > 6?6:player.getViplevel()+2;
				int upgradeLevel = 1 + random.nextInt(maxCrit);
				sum_cost+=costCoins;
				sum_level+=upgradeLevel;
				if(equip.getLevel()+upgradeLevel>XMLTemplateService.getUserLevelCount()*3){
					upgradeLevel=XMLTemplateService.getUserLevelCount()*3-equip.getLevel();
				}
				equip.setLevel(equip.getLevel() + upgradeLevel);
			}
			if(params.hasQuickUpdate()){
				builder.setUpdateTimes(sum_times);
			}
			player.setSilvercoins(player.getSilvercoins() - sum_cost);

            //modify by dylan at 20130710 because 宝石镶嵌改造
			/*switch (equip.getType()) {
				case 1:
					equip.setAttack(equipConfig.getQuantity() + (equip.getLevel()-1) * equipConfig.getGrowth());
					break;
				case 2:
					equip.setDefence(equipConfig.getQuantity() + (equip.getLevel()-1) * equipConfig.getGrowth());
					break;
				case 3:
					equip.setHealth(equipConfig.getQuantity() + (equip.getLevel()-1) * equipConfig.getGrowth());
					break;
			}*/
            equip.calcEquipAttr(equipConfig.getQuantity(), equipConfig.getGrowth(),player.getLevel());
			if (equip.getBattleid() != 0) {
				int battleid = equip.getBattleid();
				String battleids = player.getBattleids();
				String[] battleArray = battleids.split(",");
				for (int i = 0; i < battleArray.length; i++) {
					if (battleid == Integer.parseInt(battleArray[i])) {
						Battle battle = player.getBattlearray().get(battleid);
						Disciple disciple = player.getDisciples().get(battle.getDiscipleid());
						BattleUtil.resetBattleProperty(battle, disciple, player,null,0);
						OPUserBattleArrayRet.BattleArrayInfo.Builder battleBuilder= OPUserBattleArrayRet.BattleArrayInfo.newBuilder();
						BattleUtil.buildBattleBuilder(battleBuilder, battle,i);
						builder.setBattle(battleBuilder);
						player.updateOneBattle(battle);
						BattleUtil.getUserBattleArrayObject(player).getBattlearrays().set(i, battle);
						break;
					}
				}
			}
			
			
			
			OPUpgradeEquipRet.EquipInfo.Builder builder2 = OPUpgradeEquipRet.EquipInfo.newBuilder();
			
			builder2.setId(equipId);
			builder2.setItemid(equip.getItemid());
			builder2.setLevel(equip.getLevel());
			
			OPUpgradeEquipRet.EquipInfo.PropertyInfo.Builder builder3 = OPUpgradeEquipRet.EquipInfo.PropertyInfo.newBuilder();
			
			if (equip.getAttack() != 0) {
				builder3.setType(1);
				builder3.setNum(equip.getAttackToInt());
				builder2.addPInfo(builder3);
			}
			if (equip.getDefence() != 0) {
				builder3.setType(2);
				builder3.setNum(equip.getDefenceToInt());
				builder2.addPInfo(builder3);
			}
			if (equip.getHealth() != 0) {
				builder3.setType(3);
				builder3.setNum(equip.getHealthToInt());
				builder2.addPInfo(builder3);
			}
			builder.setEquip(builder2);
			player.updateOneEquip(equip);
		}
		
		player.setLastVerifyTime(Util.getServerTime());
		
		builder.setServertime(Util.getServerTime());
		builder.setSilvercoins(player.getSilvercoins());
		player.sendPacket(OP.code.OPCODE_UPGRADE_EQUIPRET_S_VALUE, builder);
		long current_time_2 = Util.getServerMillTime();
		long taken_time=current_time_2-current_time;
		logger2.info("upgrade equip handler ret send: {}, {}, {}", new Object[]{id ,taken_time});
	}
}

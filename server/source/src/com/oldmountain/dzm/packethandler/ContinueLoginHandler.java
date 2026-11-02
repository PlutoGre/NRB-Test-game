package com.oldmountain.dzm.packethandler;

import java.util.ArrayList;

import com.google.protobuf.InvalidProtocolBufferException;
import com.oldmountain.dzm.entity.Bag;
import com.oldmountain.dzm.entity.Equip;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.entity.Skill;
import com.oldmountain.dzm.entity.Soul;
import com.oldmountain.dzm.entity.UserLevelConfig;
import com.oldmountain.dzm.entity.UserStats;
import com.oldmountain.dzm.message.ContinueLogin.*;
import com.oldmountain.dzm.message.ErrorReporter.OPErrorReporter;
import com.oldmountain.dzm.message.RewardContinueLogin.OPRewardContinueLogin;
import com.oldmountain.dzm.message.RewardContinueLogin.OPRewardContinueLoginRet;
import com.oldmountain.dzm.message.OP;
import com.oldmountain.dzm.net.ObjectAccessor;
import com.oldmountain.dzm.net.packet.Packet;
import com.oldmountain.dzm.services.BagToolServices;
import com.oldmountain.dzm.util.*;

import org.apache.mina.common.IoSession;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

/**
 * 连续登陆处理包
 * 
 * @author dragon
 *
 */
public class ContinueLoginHandler
{
	private static final Logger logger2 = LoggerFactory.getLogger("UserInfo");
	public static void getContinueLogin(Packet packet, IoSession session)
    {
        OPContinueLogin continueLogin = null;
		try
        {
			continueLogin = OPContinueLogin.parseFrom(packet.data.array());
		}
        catch (InvalidProtocolBufferException e)
        {
			e.printStackTrace();
			return;
		}
		
		int version = continueLogin.getVersion();//version default = 1;
        int playerId = (Integer)session.getAttribute("playerid");
        Player player = ObjectAccessor.getOnlineUser(playerId);
        int id=player.getId();
        logger2.info("continue login handler received: {}, {}, {}", new Object[]{id , version});
        long current_time = Util.getServerMillTime();
        UserStats stats = player.getUserStats();
        
        OPContinueLoginRet.Builder builder = OPContinueLoginRet.newBuilder();
        
        if (!stats.getLoginreward().equals("") && stats.getLoginreward() != null) {
        	 String[] rewards = stats.getLoginreward().split(",");
        	 for (int i = 0; i < rewards.length; i++) {
				String[] reward = rewards[i].split(":");
				OPContinueLoginRet.RewardIdList.Builder rewardBuilder = OPContinueLoginRet.RewardIdList.newBuilder();
				rewardBuilder.setType(Integer.parseInt(reward[0]));
				rewardBuilder.setId(Integer.parseInt(reward[1]));
				if (Integer.parseInt(reward[0]) == 12001) {
					int quantity = 0;
					UserLevelConfig config = XMLTemplateService.getUserLevelConfig(player.getLevel());
					if (Integer.parseInt(reward[1]) == 2002) {
						quantity = Integer.parseInt(reward[2]) * config.getCoins1();
						rewardBuilder.setQuantity(quantity);
					}
				}
				else {
					rewardBuilder.setQuantity(Integer.parseInt(reward[2]));
				}
				builder.addRewardList(rewardBuilder);
			}
		}
        
        player.setLastVerifyTime(Util.getServerTime());
        
        builder.setCount(stats.getLogincount());
        builder.setServertime(1000);
        builder.setServertime((int) System.currentTimeMillis());
		player.sendPacket(OP.code.OPCODE_GET_CONTINUE_LOGINRET_S_VALUE, builder);// 发送返回包
		long current_time_2 = Util.getServerMillTime();
		long taken_time=current_time_2-current_time;
		logger2.info("continue login handler ret send: {}, {}, {}", new Object[]{id ,taken_time});
	}

	public static void rewardContinueLogin(Packet packet, IoSession session)
    {
        OPRewardContinueLogin rewardContinueLogin = null;
        OPErrorReporter.Builder errBuilder = OPErrorReporter.newBuilder();
        OPRewardContinueLoginRet.Builder builder = OPRewardContinueLoginRet.newBuilder();
		try
        {
			rewardContinueLogin = OPRewardContinueLogin.parseFrom(packet.data.array());
		}
        catch (InvalidProtocolBufferException e)
        {
			e.printStackTrace();
			return;
		}
		
		int version = rewardContinueLogin.getVersion();//version default = 1;
        int playerId = (Integer)session.getAttribute("playerid");
        Player player = ObjectAccessor.getOnlineUser(playerId);
        int playerid=player.getId();
        logger2.info("continue login handler received: rewardContinueLogin : {}, {}", new Object[]{playerid , version});
        long current_time = Util.getServerMillTime();
        UserStats stats = player.getUserStats();
        if (stats.getGetreward() == 1) {
			errBuilder.setErrorid(OP.code.OPCODE_REWARD_CONTINUE_LOGINRET_S_VALUE);
			errBuilder.setErrormsg("already get reward");
			player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errBuilder);
			logger2.info("Error Reporter send: continue login handler: rewardContinueLogin : {}, {}, {}", new Object[]{playerid});
			return;
		}
        
        String rewardString = stats.getLoginreward();
        
        if (!rewardString.equals("") && rewardString != null) {
	       	String[] rewards = rewardString.split(",");
	       	for (int i = 0; i < rewards.length; i++) {
				String[] reward = rewards[i].split(":");
				int type = Integer.parseInt(reward[0]);
				int id = Integer.parseInt(reward[1]);
				int count = Integer.parseInt(reward[2]);
				int rewardArray[] = new int[]{type,id,count};
					
				ArrayList<Integer> rewardReturn = Util.addRewards(player, rewardArray);
				switch (rewardReturn.get(0)) {
					case 0:
						Bag bag = player.getBags().get(rewardReturn.get(1));
						builder.addToolInfo(BagToolServices.buildToolBuilder(bag));
                        //添加宝箱运气池的运气值
                        if(UserToolConst.getType(bag.getItemid())==UserToolConst.TREASURE_TOOL)
                        {
                            BagUtil.getInstance().addLuckPool(player,bag.getItemid(),count);
                        }
						break;
					case 1:
						break;
					case 2:
						Equip equip = player.getEquips().get(rewardReturn.get(1));
						builder.addEquipInfo(EquipUtil.buildReturn(equip));
						break;
					case 3:
						Skill skill = player.getSkills().get(rewardReturn.get(1));
						builder.addSkillInfo(SkillUtil.buildSkillBuilder(skill));
						break;
					case 4:
						Soul soul = player.getSouls().get(rewardReturn.get(1));
						builder.addSoulInfo(SoulUtil.buildSoulBuilder(soul));
						break;
					case 5:
						builder.setGoldcoins(player.getGoldCoins());
						break;
					case 6:
						builder.setSilvercoins(player.getSilvercoins());
						break;
				}
			}
		}
        
        stats.setGetreward(1);
        player.setUserStats(stats);
        player.setLastVerifyTime(Util.getServerTime());
        
        builder.setStatus(1);
        builder.setServertime((int) System.currentTimeMillis());
		player.sendPacket(OP.code.OPCODE_REWARD_CONTINUE_LOGINRET_S_VALUE, builder);// 发送返回包
		long current_time_2 = Util.getServerMillTime();
		long taken_time=current_time_2-current_time;
		logger2.info("continue login handler ret send: rewardContinueLogin : {}, {}, {}", new Object[]{playerid ,taken_time});
	}

}

package com.oldmountain.dzm.packethandler;

import java.util.List;

import org.apache.mina.common.IoSession;

import com.google.protobuf.InvalidProtocolBufferException;
import com.oldmountain.dzm.DZM;
import com.oldmountain.dzm.configEntity.InviteConfig;
import com.oldmountain.dzm.entity.Bag;
import com.oldmountain.dzm.entity.InviteStatus;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.entity.ToolContainReward;
import com.oldmountain.dzm.message.OP;
import com.oldmountain.dzm.message.AddInviteKey.OPAddInviteKey;
import com.oldmountain.dzm.message.AddInviteKey.OPAddInviteKeyRet;
import com.oldmountain.dzm.message.GetInviteReward.OPGetInviteReward;
import com.oldmountain.dzm.message.GetInviteReward.OPGetInviteRewardRet;
import com.oldmountain.dzm.message.UserInviteStatus.OPUserInviteStatus;
import com.oldmountain.dzm.message.UserInviteStatus.OPUserInviteStatusRet;
import com.oldmountain.dzm.message.UserRewardInfo.OPRewardInfoRet;
import com.oldmountain.dzm.message.UserToolInfo.OPUserToolInfoRet;
import com.oldmountain.dzm.net.ObjectAccessor;
import com.oldmountain.dzm.net.packet.Packet;
import com.oldmountain.dzm.services.BagToolServices;
import com.oldmountain.dzm.services.EquipServices;
import com.oldmountain.dzm.services.SkillServices;
import com.oldmountain.dzm.services.SoulServices;
import com.oldmountain.dzm.services.UserPropertyServices;
import com.oldmountain.dzm.util.BagUtil;
import com.oldmountain.dzm.util.UserToolConst;
import com.oldmountain.dzm.util.Util;
import com.oldmountain.dzm.util.XMLTemplateService;

public class UserInvitedHandler {
	public static void addInvitedKey(Packet packet,IoSession session){
		OPAddInviteKey params = null;
		OPAddInviteKeyRet.Builder builder = OPAddInviteKeyRet.newBuilder();
		
		try {
			params = OPAddInviteKey.parseFrom(packet.data.array());
		} catch (InvalidProtocolBufferException e) {
			e.printStackTrace();
			return;
		}
		builder.setServertime(Util.getServerTime());
		int playerId = (Integer)session.getAttribute("playerid");
		Player player = ObjectAccessor.getOnlineUser(playerId);
		
		if (player == null) {
			builder.setStatus(0);
			Packet pt = new Packet(OP.code.OPCODE_ADD_INVITEKEYRET_S_VALUE, builder.build().toByteArray());
			session.write(pt);
			return ;
		}
		
		if (player.getInvitedid() != 0) {
			builder.setStatus(0);
		}
		else {
			String inviteKey = params.getKey();
			if (inviteKey.length() != 9) {
				builder.setStatus(0);
			}
			else {
				String platform = inviteKey.substring(0, 2);
				String serverId = inviteKey.substring(2, 4);
				int inviteId = Integer.parseInt(inviteKey.substring(4,9));
				if (!DZM.platForm.equals(platform) || !DZM.serverId.equals(serverId) || inviteId == playerId) {
					builder.setStatus(0);
				}
				else {
					//给用户加上邀请者的id信息
					player.setInvitedid(inviteId);
					//给用户加上邀请礼包
					Util.rewardBagTool(player, 3505001, 1);
					
					Bag tool = player.getBags().get(3505001);
					
					OPUserToolInfoRet.ToolInfo.Builder toolInfo = OPUserToolInfoRet.ToolInfo.newBuilder();
					toolInfo.setId(tool.getId());
					toolInfo.setItemid(tool.getItemid());
					toolInfo.setCount(tool.getCount());
					
					builder.addToolInfo(toolInfo);
					builder.setStatus(1);
				}
			}
		}
		
		player.sendPacket(OP.code.OPCODE_ADD_INVITEKEYRET_S_VALUE, builder);
	}
	
	public static void getUserInviteStatus(Packet packet,IoSession session){
		OPUserInviteStatus params = null;
		OPUserInviteStatusRet.Builder builder = OPUserInviteStatusRet.newBuilder();
		
		try {
			params = OPUserInviteStatus.parseFrom(packet.data.array());
		} catch (InvalidProtocolBufferException e) {
			e.printStackTrace();
			return;
		}
		
		int version = params.getVersion();
		if (version != 1) {
			
		}
		
		int playerId = (Integer)session.getAttribute("playerid");
		Player player = ObjectAccessor.getOnlineUser(playerId);
		
		java.text.DecimalFormat format = new java.text.DecimalFormat("00000");
		
		String inviteKey = "" + DZM.platForm + DZM.serverId + format.format(playerId);
		builder.setServertime(Util.getServerTime());
		builder.setStatus(1);
		builder.setInviteKey(inviteKey);
		
		InviteStatus status = getUserData(player.getId());
		
		builder.setInviteStatus(status.getInvitestatus());
		builder.setRewardStatus(status.getRewardstatus());
		//发送给前端
		player.sendPacket(OP.code.OPCODE_USER_INVITESTATUSRET_S_VALUE, builder);
	}
	
	//兑换邀请奖励
	public static void getInviteReward(Packet packet,IoSession session){
		OPGetInviteReward params = null;
		OPGetInviteRewardRet.Builder builder = OPGetInviteRewardRet.newBuilder();
		
		try {
			params = OPGetInviteReward.parseFrom(packet.data.array());
		} catch (InvalidProtocolBufferException e) {
			e.printStackTrace();
			return;
		}
		
		int kind = params.getType();
		int playerId = (Integer)session.getAttribute("playerid");
		Player player = ObjectAccessor.getOnlineUser(playerId);
		
		builder.setServertime(Util.getServerTime());
		
		InviteConfig config = XMLTemplateService.getInviteConfig(kind);
		InviteStatus status = getUserData(playerId);
		if (config == null) {
			builder.setStatus(0);
		}
		else {
			if (status.getRewardstatusInObject().containsKey(String.valueOf(kind)) || status.getInvitestatusInObject().getInt(String.valueOf(kind)) < config.getRequired() ) {
				builder.setStatus(0);
			}
			else {
				ToolContainReward[] containTools = config.getContainTools();
                int len = containTools.length;
                for(int i=0;i<len;i++)
                {
                	OPRewardInfoRet.RewardInfo.Builder rewardbBuilder = OPRewardInfoRet.RewardInfo.newBuilder();
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
                    }
                    rewardbBuilder.setItemType(containTools[i].getType());
                    rewardbBuilder.setItemId(giftItemId);
                    rewardbBuilder.setCount(containTools[i].getQuantity());
                    builder.addReward(rewardbBuilder);
                }
                DZM.getEntityManager().updateSync(status);
                player.setInviteStatus(status);
                status.setRewardstatusByType(kind, 1);
    			builder.setStatus(1);
			}
		}
		builder.setRewardStatus(status.getRewardstatus());
		
		player.sendPacket(OP.code.OPCODE_GET_INVITE_REWARDRET_S_VALUE, builder);
	}
	
	/**
	 * 更新邀请计数
	 * @param int type(1-次日登陆，2-等级到12级，3-首次充值人数，4-充值总金额)
	 * @param int addnum(增加的数量)
	 */
	public static void updateInviteStatus(int playerId,int type,int addnum){
		InviteStatus status = getUserData(playerId);
		status.addInvitestatusByType(type, addnum);

		DZM.getEntityManager().updateSync(status);
		Player player = ObjectAccessor.getOnlineUser(playerId);
		if (player != null) {
			player.setInviteStatus(status);
		}
	}
	
	public static InviteStatus getUserData(int playerId){
		InviteStatus status = null;
		
		Player player = ObjectAccessor.getOnlineUser(playerId);
		if (player != null && player.getInviteStatus() != null) {
			status = player.getInviteStatus();
		}
		else {
			List<Object> list = DZM.getEntityManager().limitQuery("from InviteStatus where playerid = ?", 0, 1, playerId);
			if (list.size() == 0) {
				status = new InviteStatus();
				status.setPlayerid(playerId);
				DZM.getEntityManager().createSync(status);
			}
			else {
				status = (InviteStatus)list.get(0);
				if (player != null) {
					player.setInviteStatus(status);
				}
			}
		}
		
		return status;
	}
}

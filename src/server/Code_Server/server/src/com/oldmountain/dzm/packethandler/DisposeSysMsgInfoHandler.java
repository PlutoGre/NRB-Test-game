package com.oldmountain.dzm.packethandler;

import com.oldmountain.dzm.DZM;
import com.oldmountain.dzm.entity.RelationShip;
import com.oldmountain.dzm.entity.Reward;
import com.oldmountain.dzm.entity.SysMsgInfo;
import com.oldmountain.dzm.message.ErrorReporter;
import com.oldmountain.dzm.services.*;
import com.oldmountain.dzm.util.UserToolConst;
import net.sf.json.JSONArray;
import net.sf.json.JSONObject;
import org.apache.mina.common.IoSession;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import com.google.protobuf.InvalidProtocolBufferException;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.message.OP;
import com.oldmountain.dzm.message.DisposeSysMsgInfo.OPDisposeSysMsg;
import com.oldmountain.dzm.message.DisposeSysMsgInfo.OPDisposeSysMsgRet;
import com.oldmountain.dzm.message.UserFriendsInfo.OPUserFriendsInfoRet;
import com.oldmountain.dzm.net.ObjectAccessor;
import com.oldmountain.dzm.net.packet.Packet;
import com.oldmountain.dzm.util.Util;
import java.util.Map;
import java.util.HashMap;
import java.util.HashSet;

/**
 * 留言包
 * 1， 解析数据包
 * 2， 保存留言数据
 * 3， 推送给相关玩家数据信息
 * 4， 下发留言成功
 * 
 * TODO 重复登陆的处理， 下发玩家数据信息.
 * 
 * @author lvjc
 */
public class DisposeSysMsgInfoHandler
{
	private static final Logger log = LoggerFactory.getLogger(DisposeSysMsgInfoHandler.class);
	private static final Logger logger2 = LoggerFactory.getLogger("SysMsgInfo");
	public static void DisposeSysMsgInfo(Packet packet, IoSession session)
	{
		OPDisposeSysMsg opDisposeSysMsg = null;
		try
		{
			opDisposeSysMsg = OPDisposeSysMsg.parseFrom(packet.data.array());
		}
		catch (InvalidProtocolBufferException e)
		{
			e.printStackTrace();
			return;
		}
		
		int playerid = opDisposeSysMsg.getId();
		int oprType = opDisposeSysMsg.getOprType();
		int relationId = opDisposeSysMsg.getId();
		int msgType = opDisposeSysMsg.getMsgType();
		int version = opDisposeSysMsg.getVersion();//version default = 1;
		int uuid = (Integer)session.getAttribute("playerid");
		Player player = ObjectAccessor.getOnlineUser(uuid);
		logger2.info("dispose Sysmsginfo handler received: {}, {}, {},{},{}", new Object[]{relationId, version, playerid, oprType,msgType });
		long current_time = Util.getServerMillTime();
		OPDisposeSysMsgRet.Builder builder = OPDisposeSysMsgRet.newBuilder();
		if(msgType== RewardServices.FIGHTEND_REWARD||msgType==RewardServices.COMPENSATE_REWARD||msgType==RewardServices.WORLDBOSSRANK_REWARD)
		{
			Reward _reward=null;
			if (player.getRewards().containsKey(relationId))
			{
				_reward = player.getRewards().get(relationId);
			}
			if (_reward != null)
			{
				if (_reward.getStatus() == 1)
				{
					ErrorReporter.OPErrorReporter.Builder eBuilder = ErrorReporter.OPErrorReporter.newBuilder();
					eBuilder.setErrorid(OP.code.OPCODE_DISPOSE_USER_SYSMSG_INFORET_S_VALUE);
					eBuilder.setErrormsg("Current Reward AlReady Got!");
					player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, eBuilder);
					logger2.info("Error Reporter send: DisposeSysMsgInfo Reward AlReady Got handler: {}, {}, {}",new Object[] { playerid, relationId,msgType });
					return;
				} 
				else
				{
					JSONArray rewards = _reward.getRewardContentJson();
					for (int i = 0; i < rewards.size(); i++)
					{
						JSONObject rewardItem = rewards.getJSONObject(i);
						int type = UserToolConst.getType(rewardItem.getInt("t"));
						int giftItemId = rewardItem.getInt("i");
						int quantity = rewardItem.getInt("q");
						switch (type)
						{
						case UserToolConst.USER_PROPERTY_TYPE:
							UserPropertyServices.changeProperty(player, giftItemId,quantity);
							switch (giftItemId)
							{
							case UserToolConst.USER_PROPERTY_GOLD_COINS:builder.setGoldcoins(player.getSysgoldcoins()+ player.getRechargegoldcoins());
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
							builder.addSoulInfo(SoulServices.addSoul(player,giftItemId, quantity));
							break;
						case UserToolConst.SKILL_TYPE:
							builder.addSkillInfo(SkillServices.createSkillAlone(player, giftItemId));
							break;
						case UserToolConst.EQUIP_TYPE:
							builder.addEquipInfo(EquipServices.createEquitAlone(player, giftItemId));
							break;
						case UserToolConst.TOOLS_TYPE:
							builder.addToolInfo(BagToolServices.addBagTool(player, giftItemId, quantity));
							break;
						default:
							break;
						}
					}
					_reward.setStatus(1);
					_reward.setRewardTime(Util.getServerTime());
					player.updateOneReward(_reward);
				}
			}
		} 
		else if (msgType == SysMsgInfoServices.SKILLBook)
		{
			SysMsgInfo _sysMsgInfo = player.getSysMsgInfos().get(relationId);
			if (_sysMsgInfo != null)
			{
				_sysMsgInfo.setStatus(2);
				if (player.getNeedAdd().containsKey("sysMsgInfo"))
				{
					if (!player.getNeedAdd().get("sysMsgInfo").contains(_sysMsgInfo.getId()))
					{
						player.updateOneSysMsgInfo(_sysMsgInfo);
					}
				}
				// 判断体力是否充足
				if (player.getVitalitybytime() + player.getVitalitybypellet() > 1)
				{

					int _skillBookId = _sysMsgInfo.getMsgInfoJson().getInt("sb");
					int _snatchId = _sysMsgInfo.getMsgInfoJson().getInt("p");
					if (ObjectAccessor.getBattleArray(_snatchId) != null) {
						SnatchSkillBookHandler.snatchSkillBookFromCounterAttack(player,_snatchId, _skillBookId);
					}

				}
			}
		} 
		else if (msgType == SysMsgInfoServices.RANKING) {
			SysMsgInfo _sysMsgInfo = player.getSysMsgInfos().get(relationId);
			if (_sysMsgInfo != null) {
				_sysMsgInfo.setStatus(2);
				if (player.getNeedAdd().containsKey("sysMsgInfo")) {
					if (!player.getNeedAdd().get("sysMsgInfo").contains(_sysMsgInfo.getId())) {
						player.updateOneSysMsgInfo(_sysMsgInfo);
					}
				}
			}
		} 
		else if (msgType == SysMsgInfoServices.ADDFRIEND) {
			// oprType 3:同意 4:拒绝
			RelationShip relationShip = player.getRelationShip();
			Map<String, Integer> friends = Util.jsonToMap(relationShip.getFriendids());
			SysMsgInfo _sysMsgInfo = player.getSysMsgInfos().get(relationId);
			if(_sysMsgInfo!=null){
				if(_sysMsgInfo.getStatus()!=2)
				{
					JSONObject _sysMsg_json =_sysMsgInfo.getMsgInfoJson();
					int friendid = _sysMsg_json.getInt("p");
					String friendname= _sysMsg_json.getString("n");
					int friendlevel=_sysMsg_json.getInt("l");
					Player friend = null;
					friend = ObjectAccessor.getOnlineUser(friendid);
					switch (oprType) 
					{
					case 3:
						// 同意并添加对方到好友列表
						friends = RelationShipService.addRelationShip(friends, friendid);
						relationShip.setFriendids(Util.mapToString(friends));
						player.setRelationShip(relationShip);
						//如果自己已经读取过恩怨信息,则发一个添加好友包用于刷新自己的好友列表
						if(player.getUserStats().getHasGotFriendInfo()==1){
							OPUserFriendsInfoRet.Builder self_friendInfoBuilder = OPUserFriendsInfoRet.newBuilder();
							self_friendInfoBuilder.setStatus(1);
							self_friendInfoBuilder.setType(UserFriendsInfoHandler.TYPE_SHOW_SELF_RELATION);
							self_friendInfoBuilder.setServertime(Util.getServerTime());
							OPUserFriendsInfoRet.FriendsInfo.Builder self_friendInfo=OPUserFriendsInfoRet.FriendsInfo.newBuilder();
							self_friendInfo.setId(friendid);
							self_friendInfo.setName(friendname);
							self_friendInfo.setLevel(friendlevel);
							self_friendInfo.setType(UserFriendsInfoHandler.TYPE_FRIEND);
							self_friendInfoBuilder.addFriendsInfo(self_friendInfo);
							player.sendPacket(OP.code.OPCODE_GET_USER_FRIENDS_INFORET_S_VALUE, self_friendInfoBuilder);
						}
						// 如果邀请人在线,则直接将自己id加入对方好友列表,并向前端发包
						if (friend != null) 
						{
							RelationShip friend_relationShip = friend.getRelationShip();
							Map<String, Integer> friend_friends = Util.jsonToMap(friend_relationShip.getFriendids());
							Map<String, Integer> friend_confirms = Util.jsonToMap(friend_relationShip.getConfirm());
							Map<String, Integer> friend_enemys = Util.jsonToMap(friend_relationShip.getEnemyids());
							//如果自己的id在对方的仇敌列表中则不将自己添加为对方的好友
							if(!friend_enemys.containsKey(""+playerid)){
								friend_friends = RelationShipService.addRelationShip(friend_friends, player.getId());
								friend_confirms = RelationShipService.deleteRelationShip(friend_confirms, player.getId());
								friend_relationShip.setFriendids(Util.mapToString(friend_friends));
								friend_relationShip.setConfirm(Util.mapToString(friend_confirms));
								friend.setRelationShip(friend_relationShip);
								//向申请人发包
								if(friend.getUserStats().getHasGotFriendInfo()==1)
								{
									OPUserFriendsInfoRet.Builder friendInfoBuilder = OPUserFriendsInfoRet.newBuilder();
									friendInfoBuilder.setStatus(1);
									friendInfoBuilder.setServertime(Util.getServerTime());
									friendInfoBuilder.setType(UserFriendsInfoHandler.TYPE_SHOW_SELF_RELATION);
									OPUserFriendsInfoRet.FriendsInfo.Builder friendInfo=OPUserFriendsInfoRet.FriendsInfo.newBuilder();
									friendInfo.setId(player.getId());
									friendInfo.setLevel(player.getLevel());
									friendInfo.setName(player.getName());
									friendInfo.setType(UserFriendsInfoHandler.TYPE_FRIEND);
									friendInfoBuilder.addFriendsInfo(friendInfo);
									friend.sendPacket(OP.code.OPCODE_GET_USER_FRIENDS_INFORET_S_VALUE, friendInfoBuilder);
								}
							}
						}
						break;
					case 4:
						// 拒绝请求,如果邀请人在线,则将自己从对方的待确认列表中删除
						if (friend != null) {
							RelationShip friend_relationShip = friend.getRelationShip();
							Map<String, Integer> friend_confirms = Util.jsonToMap(friend_relationShip.getConfirm());
							friend_confirms = RelationShipService.deleteRelationShip(friend_confirms, player.getId());
							friend_relationShip.setConfirm(Util.mapToString(friend_confirms));
							friend.setRelationShip(friend_relationShip);
						}
						break;
					default:
						break;
					}
					_sysMsgInfo.setStatus(oprType);
					player.getSysMsgInfos().put(relationId, _sysMsgInfo);
					//好友添加信息处理实时添加到数据库
					DZM.getEntityManager().updateSync(_sysMsgInfo);
				}
			}
		}
		builder.setStatus(1);
		builder.setServertime(1000);
		player.setLastVerifyTime(Util.getServerTime());
		player.sendPacket(OP.code.OPCODE_DISPOSE_USER_SYSMSG_INFORET_S_VALUE,builder);// 发送返回包
		long current_time_2 = Util.getServerMillTime();
		long taken_time = current_time_2 - current_time;
		logger2.info("dispose Sysmsginfo handler ret send: {}, {}, {}",new Object[] { relationId, taken_time });
	}

}

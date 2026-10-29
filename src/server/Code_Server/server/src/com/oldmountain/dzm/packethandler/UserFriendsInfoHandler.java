package com.oldmountain.dzm.packethandler;

import java.util.ArrayList;
import java.util.Collections;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.Map.Entry;
import java.util.concurrent.ConcurrentHashMap;

import org.apache.mina.common.IoSession;
import org.hibernate.HibernateException;
import org.hibernate.Query;
import org.hibernate.Session;
import org.hibernate.Transaction;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.google.protobuf.InvalidProtocolBufferException;
import com.oldmountain.dzm.DZM;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.entity.RelationShip;
import com.oldmountain.dzm.entity.UserStats;
import com.oldmountain.dzm.message.OP;
import com.oldmountain.dzm.message.UserFriendsInfo.OPUserFriendsInfo;
import com.oldmountain.dzm.message.UserFriendsInfo.OPUserFriendsInfoRet;
import com.oldmountain.dzm.net.ObjectAccessor;
import com.oldmountain.dzm.net.packet.Packet;
import com.oldmountain.dzm.services.RelationShipService;
import com.oldmountain.dzm.util.Util;

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
public class UserFriendsInfoHandler
{
	private static final Logger logger2 = LoggerFactory.getLogger("FriendInfo");
	private static final ConcurrentHashMap<Integer,ArrayList<OPUserFriendsInfoRet.FriendsInfo.Builder>> OPFriendInfoListByLevel = new ConcurrentHashMap<Integer,ArrayList<OPUserFriendsInfoRet.FriendsInfo.Builder>>(50);
	private static final ConcurrentHashMap<Integer,OPUserFriendsInfoRet.FriendsInfo.Builder> OPFriendInfoMaps = new ConcurrentHashMap<Integer,OPUserFriendsInfoRet.FriendsInfo.Builder>(100);
	public static final int TYPE_FRIEND=1;	//加好友
	public static final int TYPE_ENEMY=2;	//加仇敌
	public static final int TYPE_SHOW_SELF_RELATION=1;	//展现自己的好友仇敌信息
	public static final int TYPE_SEL_FRIEND=2;	//搜索好友或仇敌
	public static int nextClearTime=0;//下一次清理缓存的时间
	public static void getFriendsInfo(Packet packet, IoSession session)
    {
		//判断是否需要清理好友缓存
		checkTimeToClearMap(Util.getServerTime());
        OPUserFriendsInfo params = null;
		try
        {
			params = OPUserFriendsInfo.parseFrom(packet.data.array());
		}
        catch (InvalidProtocolBufferException e)
        {
			e.printStackTrace();
			return;
		}
		
		int version = params.getVersion();//version default = 1;
        int playerId = (Integer)session.getAttribute("playerid");
        Player player = ObjectAccessor.getOnlineUser(playerId);
        long current_time = Util.getServerMillTime();
        RelationShip relationShip=player.getRelationShip();
        Map<String, Integer> friends= Util.jsonToMap(relationShip.getFriendids());
        Map<String, Integer> enemys= Util.jsonToMap(relationShip.getEnemyids());
        Map<String, Integer> confirms= Util.jsonToMap(relationShip.getConfirm());
        //玩家进入恩怨界面时对待确认列表进行确认
        if(confirms!=null && confirms.size()>0){
        	RelationShipService.dealWithConfirm(player,confirms, friends);
        }
        //如果传入condition,则进行相关条件的玩家搜索
        logger2.info("user relationShip: {}, {}, {}, {}, {}", new Object[]{playerId , version,relationShip.getFriendids(),relationShip.getEnemyids() ,relationShip.getConfirm()});
        if(params.hasCondition()){
        	logger2.info("user friends info handler received: {}, {}, {}", new Object[]{playerId , version,params.getCondition()});
        	selFriendsMessage(packet, session);
        	return;
        }
        OPUserFriendsInfoRet.Builder builder = OPUserFriendsInfoRet.newBuilder();
        if(friends!=null && friends.size()>0){
        	RelationShipService.buildFriendInfoBuilder(OPFriendInfoMaps,builder, friends, TYPE_FRIEND);
        }
        if(enemys!=null && enemys.size()>0){
        	RelationShipService.buildFriendInfoBuilder( OPFriendInfoMaps,builder, enemys, TYPE_ENEMY);
        }
        //记录玩家本次登陆是否读取过恩怨信息
        player.getUserStats().setHasGotFriendInfo(1);
        builder.setServertime(Util.getServerTime());
        builder.setStatus(1);
        builder.setType(TYPE_SHOW_SELF_RELATION);
        player.setLastVerifyTime(Util.getServerTime());

		player.sendPacket(OP.code.OPCODE_GET_USER_FRIENDS_INFORET_S_VALUE, builder);// 发送返回包
		long current_time_2 = Util.getServerMillTime();
		long taken_time=current_time_2-current_time;
		logger2.info("user friends info handler ret send: {}, {}, {}", new Object[]{playerId ,taken_time});
	}
	
	
	
	public static void selFriendsMessage(Packet packet, IoSession session)
    {
        OPUserFriendsInfo selFriendsInfoArray = null;
		try
        {
			selFriendsInfoArray = OPUserFriendsInfo.parseFrom(packet.data.array());
		}
        catch (InvalidProtocolBufferException e)
        {
			e.printStackTrace();
			return;
		}
		
		String condition = selFriendsInfoArray.getCondition();
		int version = selFriendsInfoArray.getVersion();//version default = 1;
        int playerid = (Integer)session.getAttribute("playerid");
        Player player = ObjectAccessor.getOnlineUser(playerid);
        int id=player.getId();
        logger2.info("sel friends info array handler received: {}, {}, {}", new Object[]{id , version , condition});
        long current_time = Util.getServerMillTime();
        OPUserFriendsInfoRet.Builder builder = OPUserFriendsInfoRet.newBuilder();

        RelationShip relationShip=player.getRelationShip();
        Map<String, Integer> friends= Util.jsonToMap(relationShip.getFriendids());
        Map<String, Integer> enemys= Util.jsonToMap(relationShip.getEnemyids());
        /*******************************************************************************************************/
        boolean isLevel=true;
        int level=0;
        try {
			level=Integer.parseInt(condition);
		} catch (Exception e) {
			isLevel=false;
		}
		if(level>50){
			isLevel=false;
		}
		List<Player> list=new ArrayList<Player>();
		Player find_player=null;
		int status=1;
		boolean needrefresh=false;
		int count = 0;//显示的个数
		if(isLevel){
			//如果缓存中存在就从缓存中读取该等级的玩家信息
			if(OPFriendInfoListByLevel.containsKey(level))
			{
				List<OPUserFriendsInfoRet.FriendsInfo.Builder> friendInfoBuilders=OPFriendInfoListByLevel.get(level);
				//等级小于30,且内存中当前等级的信息数量小于10,重新导入内存
				if(friendInfoBuilders==null ||(friendInfoBuilders.size()<10 && level<30))
				{
					needrefresh=true;
				}
				else 
				{
					// 洗牌
					Collections.shuffle(friendInfoBuilders);
					OPUserFriendsInfoRet.FriendsInfo.Builder friendinfoBuilder = null;
					// 最多显示10人
					for (int i = 0; i < friendInfoBuilders.size(); i++) 
					{
						if (count >= 10) 
						{
							break;
						}
						friendinfoBuilder = friendInfoBuilders.get(i);
						String relationid=String.valueOf(friendinfoBuilder.getId());
						//如果是玩家或其好友及仇敌,则不显示
						if (friendinfoBuilder.getId() == playerid || friends.containsKey(relationid) || enemys.containsKey(relationid)) 
						{
							continue;
						} 
						else 
						{
							builder.addFriendsInfo(friendinfoBuilder);
						}
						count++;
					}
				}
			}
			else
			{
				needrefresh=true;
			}
			if(needrefresh){
				ArrayList<OPUserFriendsInfoRet.FriendsInfo.Builder> friend_InfoBuilders=new ArrayList<OPUserFriendsInfoRet.FriendsInfo.Builder>();
				list=DZM.getEntityManager().query(Player.class,"from Player where level=?", level);
				if(list!=null && list.size()>0){
					for(int i=0;i<list.size();i++){
						if(count>=10){
							break;
						}
						find_player=list.get(i);
						String relationid=String.valueOf(find_player.getId());
						OPUserFriendsInfoRet.FriendsInfo.Builder friendsInfo=OPUserFriendsInfoRet.FriendsInfo.newBuilder();
						friendsInfo.setId(find_player.getId());
						friendsInfo.setName(find_player.getName());
						friendsInfo.setLevel(find_player.getLevel());
						friend_InfoBuilders.add(friendsInfo);
						OPFriendInfoMaps.put(friendsInfo.getId(), friendsInfo);
						//如果是玩家或其好友及仇敌,则不显示
						if(find_player.getId()!=playerid && count<10 && !friends.containsKey(relationid) && !enemys.containsKey(relationid))
						{
							builder.addFriendsInfo(friendsInfo);
							count++;
						}
					}
					OPFriendInfoListByLevel.put(level, friend_InfoBuilders);
				}
				else{
					status=0;
				}
			}
		}
		else{
			find_player=RelationShipService.getPlayerByName(condition);
			String relationid="";
			if(find_player!=null){
				relationid=String.valueOf(find_player.getId());
			}
			//如果是玩家或其好友及仇敌,则不显示
			if(find_player!=null && find_player.getId()!=playerid && !friends.containsKey(relationid) && !enemys.containsKey(relationid)){
				OPUserFriendsInfoRet.FriendsInfo.Builder friendsInfo=OPUserFriendsInfoRet.FriendsInfo.newBuilder();
		        friendsInfo.setId(find_player.getId());
		        friendsInfo.setName(find_player.getName());
		        friendsInfo.setLevel(find_player.getLevel());
		        builder.addFriendsInfo(friendsInfo);
		        OPFriendInfoMaps.put(find_player.getId(), friendsInfo);
			}
			else{
				status=0;
			}
		}
        /*******************************************************************************************************/

        builder.setServertime(Util.getServerTime());
        builder.setStatus(status);
        builder.setType(TYPE_SEL_FRIEND);
        player.setLastVerifyTime(Util.getServerTime());

		player.sendPacket(OP.code.OPCODE_GET_USER_FRIENDS_INFORET_S_VALUE, builder);// 发送返回包
		long current_time_2 = Util.getServerMillTime();
		long taken_time=current_time_2-current_time;
		logger2.info("sel friends info array handler ret send: {}, {}, {}", new Object[]{id ,taken_time});
	}
	/**
	 * 每隔六小时清理一遍好友信息缓存
	 * */
	public static void checkTimeToClearMap(int current_time){
		boolean needClear=false;
		if (nextClearTime==0) {
			nextClearTime=current_time+6*60*60;
			needClear=true;
		}
		else if(nextClearTime<=current_time){
			needClear=true;
			nextClearTime=current_time+6*60*60;
		}
		if(needClear){
			OPFriendInfoListByLevel.clear();
			OPFriendInfoMaps.clear();
		}
	}
	

}

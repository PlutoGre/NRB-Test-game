package com.oldmountain.dzm.packethandler;

import java.util.Map;

import org.apache.mina.common.IoSession;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.google.protobuf.InvalidProtocolBufferException;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.entity.RelationShip;
import com.oldmountain.dzm.message.OP;
import com.oldmountain.dzm.message.DisposeFriendsInfo.OPDisposeFriendsInfo;
import com.oldmountain.dzm.message.DisposeFriendsInfo.OPDisposeFriendsInfoRet;
import com.oldmountain.dzm.message.UserFriendsInfo.OPUserFriendsInfoRet;
import com.oldmountain.dzm.net.ObjectAccessor;
import com.oldmountain.dzm.net.packet.Packet;
import com.oldmountain.dzm.services.RelationShipService;
import com.oldmountain.dzm.services.SysMsgInfoServices;
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
public class DisposeFriendsInfoHandler
{
	private static final Logger log = LoggerFactory.getLogger(DisposeFriendsInfoHandler.class);
	private static final Logger logger2 = LoggerFactory.getLogger("FriendInfo");
	private static final int TYPE_ADD_FRIEND_CONFIRM=1;
	private static final int TYPE_ADD_ENEMY=2;
	private static final int TYPE_DELETE_FRIEND=3;
	private static final int TYPE_DELETE_ENEMY=4;
	public static void DisposeFriendInfo(Packet packet, IoSession session)
    {
        OPDisposeFriendsInfo opDisposeFriendsInfo = null;
		try
        {
			opDisposeFriendsInfo = OPDisposeFriendsInfo.parseFrom(packet.data.array());
		}
        catch (InvalidProtocolBufferException e)
        {
			e.printStackTrace();
			return;
		}
		
		int relation_id = opDisposeFriendsInfo.getId();
        int oprType = opDisposeFriendsInfo.getOprType();
		int version = opDisposeFriendsInfo.getVersion();//version default = 1;
        int uuid = (Integer)session.getAttribute("playerid");
        Player player = ObjectAccessor.getOnlineUser(uuid);
        int id=player.getId();
        log.info("dispose friends info handler received: {}, {}, {} ,{}", new Object[]{id , version , relation_id , oprType});
        logger2.info("dispose friends info handler received: {}, {}, {} ,{}", new Object[]{id , version , relation_id , oprType});
        long current_time = Util.getServerMillTime();
        OPDisposeFriendsInfoRet.Builder builder = OPDisposeFriendsInfoRet.newBuilder();
        RelationShip relationShip=player.getRelationShip();
        Map<String, Integer> friends= Util.jsonToMap(relationShip.getFriendids());
        Map<String, Integer> enemys= Util.jsonToMap(relationShip.getEnemyids());
        Map<String, Integer> confirms=Util.jsonToMap(relationShip.getConfirm());
        String relationid=String.valueOf(relation_id);
        
        int status=relation_id;
        //1-添加好友确认  2-添加仇敌 ,3删除好友 , 4删除仇敌
        if((oprType==TYPE_ADD_ENEMY || oprType==TYPE_ADD_FRIEND_CONFIRM)&&(friends.containsKey(relationid) || enemys.containsKey(relationid))){
        	status=-1;
        }else{
        	switch (oprType) {
        	case TYPE_ADD_FRIEND_CONFIRM:  
        	
        		confirms=RelationShipService.addRelationShip(confirms, relation_id);
        		relationShip.setConfirm(Util.mapToString(confirms));
        		Player friend=null;
        		friend = ObjectAccessor.getOnlineUser(relation_id);
    			if (friend == null) {
    				friend = new Player();
    				friend.setId(relation_id);
    			}
        		SysMsgInfoServices.commitFriendConfirm(player, friend);
        		status=0;
        		break;
        	case TYPE_ADD_ENEMY:
        		//如果确认列表中存在对该id的好友申请,则将其移除
        		if(confirms.containsKey(relationid)){
        			RelationShipService.deleteRelationShip(confirms, relation_id);
        		}
        		enemys=RelationShipService.addRelationShip(enemys, relation_id);
        		relationShip.setEnemyids(Util.mapToString(enemys));
        		break;
        	case TYPE_DELETE_FRIEND:
        		friends=RelationShipService.deleteRelationShip(friends, relation_id);
        		relationShip.setFriendids(Util.mapToString(friends));
        		break;
        	case TYPE_DELETE_ENEMY:
        		enemys=RelationShipService.deleteRelationShip(enemys, relation_id);
        		relationShip.setEnemyids(Util.mapToString(enemys));
        		break;
        	default:
        		break;
        	}
        }
        player.setRelationShip(relationShip);
		builder.setStatus(status);
        builder.setServertime(Util.getServerTime());
        player.setLastVerifyTime(Util.getServerTime());
        

		player.sendPacket(OP.code.OPCODE_DISPOSE_USER_FRIENDS_INFORET_S_VALUE, builder);// 发送返回包
		long current_time_2 = Util.getServerMillTime();
		long taken_time=current_time_2-current_time;
		log.info("dispose friends info handler ret send: {}, {}, {}", new Object[]{id ,taken_time});
		logger2.info("dispose friends info handler ret send: {}, {}, {}", new Object[]{id ,taken_time});
	}

}

package com.oldmountain.dzm.services;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;
import java.util.concurrent.ConcurrentHashMap;

import org.hibernate.HibernateException;
import org.hibernate.Query;
import org.hibernate.Session;
import org.hibernate.Transaction;

import com.oldmountain.dzm.DZM;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.entity.RelationShip;
import com.oldmountain.dzm.message.UserFriendsInfo.OPUserFriendsInfoRet;
import com.oldmountain.dzm.net.ObjectAccessor;
import com.oldmountain.dzm.util.Util;

public class RelationShipService {
	
	//type:1-朋友  2-仇敌
	public static void buildFriendInfoBuilder(ConcurrentHashMap<Integer,OPUserFriendsInfoRet.FriendsInfo.Builder> OPFriendInfoMaps,OPUserFriendsInfoRet.Builder builder,Map<String, Integer> relationMap, int type ){
		Iterator<Map.Entry<String,Integer>> iterator = relationMap.entrySet().iterator();
		List<Integer> ids_notin_cache=new ArrayList<Integer>();
        while (iterator.hasNext()) {
        	OPUserFriendsInfoRet.FriendsInfo.Builder friendsInfo=OPUserFriendsInfoRet.FriendsInfo.newBuilder();
        	Entry<String, Integer> entry=iterator.next();
			int id=Integer.parseInt(entry.getKey());
			//如果内存中有,就直接从内存中取出friendInfo,并添加type
			if(OPFriendInfoMaps.containsKey(id)){
				friendsInfo=OPFriendInfoMaps.get(id);
				friendsInfo.setType(type);
				builder.addFriendsInfo(friendsInfo);
			}
			else
			{
				ids_notin_cache.add(id);
			}
		}
        if(ids_notin_cache!=null && ids_notin_cache.size()>0){
        	List<Player> playerList=getPlayerListByIds(ids_notin_cache);
        	if(playerList!=null && playerList.size()>0){
        		Player find_player=null;
        		for(int i=0;i<ids_notin_cache.size();i++){
        			OPUserFriendsInfoRet.FriendsInfo.Builder friendsInfo=OPUserFriendsInfoRet.FriendsInfo.newBuilder();
        			find_player=playerList.get(i);
        			friendsInfo.setId(find_player.getId());
        			friendsInfo.setLevel(find_player.getLevel());
        			friendsInfo.setName(find_player.getName());
        			OPFriendInfoMaps.put(find_player.getId(), friendsInfo);
        			friendsInfo.setType(type);
        			builder.addFriendsInfo(friendsInfo);
        		}
        	}
        }
		
	}
	//对待确认列表进行确认操作
	public static void dealWithConfirm(Player player , Map<String, Integer> confirms , Map<String, Integer> friends){
		Iterator<Map.Entry<String,Integer>> iterator = confirms.entrySet().iterator();
		List<Integer> ids_needToRemove=new ArrayList<Integer>();
        while (iterator.hasNext()) 
        {
        	OPUserFriendsInfoRet.FriendsInfo.Builder friendsInfo=OPUserFriendsInfoRet.FriendsInfo.newBuilder();
        	Entry<String, Integer> entry=iterator.next();
			int id=Integer.parseInt(entry.getKey());
			Player friend=null;
			friend=ObjectAccessor.getOnlineUser(id);
			//如果被邀请者在线,则进行确认判定
			if(friend!=null)
			{
				RelationShip firend_relationShip=friend.getRelationShip();
				Map<String, Integer> friend_friends=Util.jsonToMap(firend_relationShip.getFriendids());
				//如果对方好友列表中有己方id,则将对方加入己方好友列表,并将该id加入confirms待删列表
				if(friend_friends.containsKey(String.valueOf(player.getId())))
				{
					addRelationShip(friends, id);
					ids_needToRemove.add(id);
				}
			}
		}
        //将已确认通过的好友从待确认列表中移除
        if(ids_needToRemove!=null && ids_needToRemove.size()>0)
        {
        	for(int i=0;i<ids_needToRemove.size();i++){
        		deleteRelationShip(confirms, ids_needToRemove.get(i));
        	}
        }
		
	}
	//通过姓名查询玩家
	public static Player getPlayerByName(String name){
		ArrayList<Player> _list=null;
        Session session=DZM.getEntityManager().getSessionFactory();
        Transaction tx = session.beginTransaction( ) ;
        Player player=null;
		try
        {
			String hql = "from Player where name='"+name+"'";
			Query query=session.createQuery( hql ) ;
	        _list=(ArrayList<Player>) query.list();
	        session.clear();
	        tx.commit() ;
        }catch (HibernateException e) {
        	if (tx!=null)
            {
                tx.rollback();
            }
            throw e;
		}
        if(_list!=null && _list.size()>=1){
        	player=_list.get(0);
        }
		return player;
	}
	//通过ids查询玩家列表
	public static ArrayList<Player> getPlayerListByIds(List<Integer> ids){
		ArrayList<Player> _list=null;
        Session session=DZM.getEntityManager().getSessionFactory();
        Transaction tx = session.beginTransaction( ) ;
        Player player=null;
		try
        {
			String hql = "from Player where id in (:ids)";
			Query query=session.createQuery( hql ) ;
			query.setParameterList("ids", ids);
	        _list=(ArrayList<Player>) query.list();
	        session.clear();
	        tx.commit() ;
        }catch (HibernateException e) {
        	if (tx!=null)
            {
                tx.rollback();
            }
            throw e;
		}
		return _list;
	}
	//添加恩怨关系
	public static Map<String, Integer> addRelationShip(Map<String, Integer> map, int relation_id){
		String id=String.valueOf(relation_id);
		if(!map.containsKey(id)){
			map.put(id, 1);
		}
		return  map;
	}
	//删除恩怨关系
	public static Map<String, Integer> deleteRelationShip(Map<String, Integer> map, int relation_id){
		String id=String.valueOf(relation_id);
		if(map.containsKey(id)){
			map.remove(id);
		}
		return  map;
	}
	
}

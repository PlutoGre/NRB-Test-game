package com.oldmountain.dzm.net;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.concurrent.ConcurrentHashMap;
import com.oldmountain.dzm.entity.BattleArrayObject;
import com.oldmountain.dzm.entity.BroadCast;
import com.oldmountain.dzm.entity.Mapping;
import com.oldmountain.dzm.entity.Player;

import com.oldmountain.dzm.services.SkillBookServices;
import org.apache.mina.common.IoSession;
import org.slf4j.LoggerFactory;

public class ObjectAccessor
{
    private static final org.slf4j.Logger log = LoggerFactory.getLogger("PlayerLoginInfo");
	public static final ConcurrentHashMap<Integer, Player> players = new ConcurrentHashMap<Integer, Player>(5000);
	public static final ConcurrentHashMap<String, Mapping> mappings = new ConcurrentHashMap<String, Mapping>(5000);
	public static final ConcurrentHashMap<Integer, BattleArrayObject> battleArrays = new ConcurrentHashMap<Integer, BattleArrayObject>(5000);
	public static final ArrayList<BroadCast> systemBro = new ArrayList<BroadCast>(100);
	public static ArrayList<String> existNames = new ArrayList<String>(500);
	public static final HashMap<Integer, IoSession> onlineSessions = new HashMap<Integer, IoSession>(5000);
	
	public static void addOnlineSession(int playerId,IoSession session){
		onlineSessions.put(playerId, session);
		log.info("add online session,playerid:" + playerId);
	}
	
	public static void removeOnlineSession(int playerId){
		onlineSessions.remove(playerId);
		log.info("remove online session,playerid:" + playerId);
	}
	
	public static HashMap<Integer, IoSession> getOnlineSession(){
		return onlineSessions;
	}
	
	public static ArrayList<String> getExistNames(){
		return existNames;
	}
	
	public static void setExistnames(ArrayList<String>names){
		existNames = names;
	}
	
	public static void addToExistName(String name){ 
		existNames.add(name);
	}
	
	public static ArrayList<BroadCast> getSystemBro(){
		return systemBro;
	}
	
	public static void addSystemBro(BroadCast broadCast){
			if (systemBro.size() >= 100) {
				systemBro.clear();
			}
			systemBro.add(broadCast);
	}
	
	public static boolean addOnlineUser(Player player)
    {
		players.put(player.getId(), player);
		return true;
	}

	public static boolean delOnlineUser(int playerId)
    {
        //删除该玩家，先清除该session 初期版本调试打印log   add by dylan
        if(players.containsKey(playerId))
        {
            try
            {
                players.get(playerId).getSession().close();
                log.info("player logout delOnlineUser method|#close current login session sussess playerId:"+playerId);
            }
            catch (Exception ignored)
            {
                log.info("player logout delOnlineUser method|close current login session Exception:"+ignored.getMessage());
            }
        }
        //add by dylan at 20130511 because 玩家退出后移除该玩家对应残章列表数据
        SkillBookServices.clearSkillBookByPlayer(players.get(playerId));
        players.remove(playerId);
		return true;
	}

	public static Player getOnlineUser(int playerId) {
		return players.get(playerId);
	}
	
	public static boolean addMapping(Mapping mapping) {
		mappings.put(mapping.getPuid(), mapping);
		return true;
	}
	
	public static boolean delMapping(String puid){
		mappings.remove(puid);
		return true;
	}
	
	public static Mapping getMapping(String puid){
		return mappings.get(puid);
	}
	
	public static boolean addBattleArray(BattleArrayObject battleArray){
		battleArrays.put(battleArray.getPlayerid(), battleArray);
		return true;
	}
	
	public static boolean delBattleArray(int playerId){
		battleArrays.remove(playerId);
		return true;
	}
	
	public static BattleArrayObject getBattleArray(int playerId){
		return battleArrays.get(playerId);
	}
	
	
}

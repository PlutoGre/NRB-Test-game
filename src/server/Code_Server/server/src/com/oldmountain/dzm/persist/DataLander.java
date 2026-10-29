package com.oldmountain.dzm.persist;

import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.Map;

import com.oldmountain.dzm.entity.*;
import com.oldmountain.dzm.services.LeaguaServices;
import com.oldmountain.dzm.util.OprRetTypeConst;

import org.hibernate.HibernateException;
import org.hibernate.SQLQuery;
import org.hibernate.Session;
import org.hibernate.Transaction;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.oldmountain.dzm.DZM;
import com.oldmountain.dzm.net.ObjectAccessor;
import com.oldmountain.dzm.packethandler.LoginHandler;
import com.oldmountain.dzm.util.Util;

public class DataLander implements Runnable{
	private static final Logger logger2 = LoggerFactory.getLogger("LandDataError");
	private static final Logger log = LoggerFactory.getLogger(LoginHandler.class);
	private static int updateCount = 0;
	public void run(){
		updateCount = 0;
		int current_time = Util.getServerTime();
		
		long startTime = System.currentTimeMillis();
		logger2.info("start landing data to database");
		Iterator<Player> it = ObjectAccessor.players.values().iterator();
		int count = 0;
		while(it.hasNext()){
			Player p = it.next();
			try {
				if(p.getIsDirty()){
                    //modify by dylan 目前只有传书类有非实时插入 不需要同步玩家的数据 
					DZM.getEntityManager().updateSync(p);
					HashMap<String, HashSet<Integer>> needUpdate = p.getNeedUpdate();
					if (needUpdate.size() != 0) {
						Iterator<Map.Entry<String, HashSet<Integer>>> types = needUpdate.entrySet().iterator();
						while (types.hasNext()) {
							Map.Entry<String, HashSet<Integer>> entry = types.next();
							String table = entry.getKey();
							HashSet<Integer> ids = entry.getValue();
							try {
								updateByType(p,table, ids, OprRetTypeConst.Update);
							} catch (Throwable e) {
								logger2.info("Landing data error for playerid:{},type:{}",new Object[]{p.getId(),table});
							}
							
						}
					}
					p.clearNeedUpdate();
                    //add by dylan 残章、论剑传书部分非实时填入数据库
                    HashMap<String, HashSet<Integer>> needAdd = p.getNeedAdd();
                    if(needAdd.size()!=0)
                    {
                        Iterator<Map.Entry<String, HashSet<Integer>>> types = needAdd.entrySet().iterator();
                        while (types.hasNext())
                        {
                            Map.Entry<String, HashSet<Integer>> entry = types.next();
                            String table = entry.getKey();
                            HashSet<Integer> ids = entry.getValue();
                            try {
                            	updateByType(p,table, ids, OprRetTypeConst.ADD);
							} catch (Throwable e) {
								logger2.info("Landing data error for playerid:{},type:{}",new Object[]{p.getId(),table});
							}
                            
                        }
                    }
                    p.clearNeedAdd();
					count ++;
				}
				if (current_time - p.getLastVerifyTime() > 6 * 3600) {
					ObjectAccessor.delOnlineUser(p.getId());
				}
			} catch (Throwable e) {
				logger2.info("Landing data error for playerid:{},message:{}",new Object[]{p.getId(),e.getMessage()});
				e.printStackTrace();
			}
		}
		landLeaugData();
		long endTime = System.currentTimeMillis();
		long costTime = endTime - startTime;
		logger2.info("end landing data to database,total update player count:{},cost Time:{},total update db count:{}",new Object[]{count, costTime, updateCount});
	}
	
	public static void landingImmediately(){
		updateCount = 0;
		int current_time = Util.getServerTime();
		
		long startTime = System.currentTimeMillis();
		logger2.info("start landing data to database");
		Iterator<Player> it = ObjectAccessor.players.values().iterator();
		int count = 0;
		while(it.hasNext()){
			Player p = it.next();
			try {
				if(p.getIsDirty()){
                    //modify by dylan 目前只有传书类有非实时插入 不需要同步玩家的数据 
                    DZM.getEntityManager().updateSync(p);
					HashMap<String, HashSet<Integer>> needUpdate = p.getNeedUpdate();
					if (needUpdate.size() != 0) {
						Iterator<Map.Entry<String, HashSet<Integer>>> types = needUpdate.entrySet().iterator();
						while (types.hasNext()) {
							Map.Entry<String, HashSet<Integer>> entry = types.next();
							String table = entry.getKey();
							HashSet<Integer> ids = entry.getValue();
							try {
								updateByType(p,table, ids, OprRetTypeConst.Update);
							} catch (Throwable e) {
								logger2.info("Landing data error for playerid:{},type:{}",new Object[]{p.getId(),table});
							}
							
						}
					}
					p.clearNeedUpdate();
                    //add by dylan 残章、论剑传书部分非实时填入数据库
                    HashMap<String, HashSet<Integer>> needAdd = p.getNeedAdd();
                    if(needAdd.size()!=0)
                    {
                        Iterator<Map.Entry<String, HashSet<Integer>>> types = needAdd.entrySet().iterator();
                        while (types.hasNext())
                        {
                            Map.Entry<String, HashSet<Integer>> entry = types.next();
                            String table = entry.getKey();
                            HashSet<Integer> ids = entry.getValue();
                            try {
                            	updateByType(p,table, ids, OprRetTypeConst.ADD);
							} catch (Throwable e) {
								logger2.info("Landing data error for playerid:{},type:{}",new Object[]{p.getId(),table});
							}
                            
                        }
                    }
                    p.clearNeedAdd();
					count ++;
				}
				if (current_time - p.getLastVerifyTime() > 6 * 3600) {
					ObjectAccessor.delOnlineUser(p.getId());
				}
			} catch (Throwable e) {
				logger2.info("Landing data error for playerid:{},message:{}",new Object[]{p.getId(),e.getMessage()});
				e.printStackTrace();
			}
		}
		
		landLeaugData();
		long endTime = System.currentTimeMillis();
		long costTime = endTime - startTime;
		logger2.info("end landing data to database,total update player count:{},cost Time:{},total update db count:{}",new Object[]{count, costTime, updateCount});
	}
	
	public static void updateByType(Player player,String type,HashSet<Integer> ids,int oprType){
		int typeId = getTypeByString(type);
		Iterator<Integer> iterator = ids.iterator();
		switch (typeId) {
			case 1:
				while (iterator.hasNext()) {
					int id = iterator.next();
					Bag entity = player.getBags().get(id);
					if (entity == null) {
						continue;
					}
					try {
						DZM.getEntityManager().updateSync(entity);
					} catch (Throwable e) {
						logger2.info("Landing data error for playerid:{},table:{},id:{},message:{}",new Object[]{player.getId(),"bag",id,e.getMessage()});
					}
					
					updateCount ++;
				}
				break;
			case 2:
				while (iterator.hasNext()) {
					int id = iterator.next();
					Battle entity = player.getBattlearray().get(id);
					if (entity == null) {
						continue;
					}
					try {
						DZM.getEntityManager().updateSync(entity);
					} catch (Throwable e) {
						logger2.info("Landing data error for playerid:{},table:{},id:{},message:{}",new Object[]{player.getId(),"battle",id,e.getMessage()});
					}
					
					updateCount ++;
				}
				break;
			case 3:
				while (iterator.hasNext()) {
					iterator.next();
					Career entity = player.getCareer();
					if (entity == null) {
						continue;
					}
					try {
						DZM.getEntityManager().updateSync(entity);
					} catch (Throwable e) {
						logger2.info("Landing data error for playerid:{},table:{},id:{},message:{}",new Object[]{player.getId(),"career",0,e.getMessage()});
					}
					
					updateCount ++;
				}
				break;
			case 4:
				while (iterator.hasNext()) {
					int id = iterator.next();
					Disciple entity = player.getDisciples().get(id);
					if (entity == null) {
						continue;
					}
					try {
						DZM.getEntityManager().updateSync(entity);
					} catch (Throwable e) {
						logger2.info("Landing data error for playerid:{},table:{},id:{},message:{}",new Object[]{player.getId(),"disciple",id,e.getMessage()});
					}
					
					updateCount ++;
				}
				break;
			case 5:
				while (iterator.hasNext()) {
					int id = iterator.next();
					Equip entity = player.getEquips().get(id);
					if (entity == null) {
						continue;
					}
					try {
						DZM.getEntityManager().updateSync(entity);
					} catch (Throwable e) {
						logger2.info("Landing data error for playerid:{},table:{},id:{},message:{}",new Object[]{player.getId(),"equip",id,e.getMessage()});
					}
					
					updateCount ++;
				}
				break;
			case 7:
				while (iterator.hasNext()) {
					int id = iterator.next();
					Skill entity = player.getSkills().get(id);
					if (entity == null) {
						continue;
					}
					try {
						DZM.getEntityManager().updateSync(entity);
					} catch (Throwable e) {
						logger2.info("Landing data error for playerid:{},table:{},id:{},message:{}",new Object[]{player.getId(),"skill",id,e.getMessage()});
					}
					
					updateCount ++;
				}
				break;
			case 8:
				while (iterator.hasNext()) {
					int id = iterator.next();
					Soul entity = player.getSouls().get(id);
					if (entity == null) {
						continue;
					}
					try {
						DZM.getEntityManager().updateSync(entity);
					} catch (Throwable e) {
						logger2.info("Landing data error for playerid:{},table:{},id:{},message:{}",new Object[]{player.getId(),"soul",id,e.getMessage()});
					}
					
					updateCount ++;
				}
				break;
			case 9:
				while (iterator.hasNext()) {
					iterator.next();
					UserStats entity = player.getUserStats();
					if (entity == null) {
						continue;
					}
					try {
						DZM.getEntityManager().updateSync(entity);
					} catch (Throwable e) {
						logger2.info("Landing data error for playerid:{},table:{},id:{},message:{}",new Object[]{player.getId(),"soul",0,e.getMessage()});
					}
					
					updateCount ++;
				}
				break;
			case 10:
				while (iterator.hasNext()) {
					iterator.next();
					TeamBuffer entity = player.getTeamBuffer();
					if (entity == null) {
						continue;
					}
					try {
						DZM.getEntityManager().updateSync(entity);
					} catch (Throwable e) {
						logger2.info("Landing data error for playerid:{},table:{},id:{},message:{}",new Object[]{player.getId(),"teambuffer",0,e.getMessage()});
					}
					
					updateCount ++;
				}
				break;
			case 11:
				while (iterator.hasNext()) {
					iterator.next();
					Title entity = player.getTitle();
					if (entity == null) {
						continue;
					}
					try {
						DZM.getEntityManager().updateSync(entity);
					} catch (Throwable e) {
						logger2.info("Landing data error for playerid:{},table:{},id:{},message:{}",new Object[]{player.getId(),"title",0,e.getMessage()});
					}
					
					updateCount ++;
				}
				break;
            case 12:
                while (iterator.hasNext()) {
                    int id = iterator.next();
                    Reward entity = player.getRewards().get(id);
                    if (entity == null) {
                        continue;
                    }
                    try {
                    	DZM.getEntityManager().updateSync(entity);
					} catch (Throwable e) {
						logger2.info("Landing data error for playerid:{},table:{},id:{},message:{}",new Object[]{player.getId(),"reward",id,e.getMessage()});
					}
                    
                    updateCount ++;
                }
                break;
            case 13:
            	while (iterator.hasNext()) {
            		int id = iterator.next();
            		
            		Challenge entity = player.getChallenge();
            		if (entity == null) {
                        continue;
                    }
	            	Session session=DZM.getEntityManager().getSessionFactory();
	                Transaction tx = session.beginTransaction( ) ;
	        		
	        		try
	                {
	        			String sql = "update challenge set level=?,name=? where playerid=?";
	        			SQLQuery query = session.createSQLQuery( sql ) ;
	        			query.setInteger(0, entity.getLevel());
	        			query.setString(1, entity.getName());
	        	        query.setInteger(2, id);
	        	        query.executeUpdate();
	        	        session.clear();
	        	        tx.commit() ;
	                }catch (HibernateException e) {
	                	if (tx!=null)
	                    {
	                        tx.rollback();
	                    }
	                	logger2.info("Landing data error for playerid:{},table:{},id:{},message:{}",new Object[]{player.getId(),"challenge",id,e.getMessage()});
	        		}
	                updateCount ++;
            	}
            	break;
            case 14:
                while (iterator.hasNext()) {
                    int id = iterator.next();
                    SkillBook entity = player.getSkillBooks().get(id);
                    if (entity == null) {
                        continue;
                    }
                    try {
                    	DZM.getEntityManager().updateSync(entity);
					} catch (Throwable e) {
						logger2.info("Landing data error for playerid:{},table:{},id:{},message:{}",new Object[]{player.getId(),"skillbook",id,e.getMessage()});
					}
                    
                    updateCount ++;
                }
                break;
            case 15:
                while (iterator.hasNext()) {
                    int id = iterator.next();
                    SysMsgInfo entity = player.getSysMsgInfos().get(id);
                    if (entity == null) {
                        continue;
                    }
                    try {
                    	//区分add和update
                        if(oprType==OprRetTypeConst.ADD){
                        	DZM.getEntityManager().createSync(entity);
                        }
                        else if(oprType==OprRetTypeConst.Update){
                        	DZM.getEntityManager().updateSync(entity);
                        }
					} catch (Throwable e) {
						logger2.info("Landing data error for playerid:{},table:{},id:{},message:{}",new Object[]{player.getId(),"sysmsg",id,e.getMessage()});
					}
                    
                    updateCount ++;
                }
                break;
            case 16:
                while (iterator.hasNext())
                {
                    iterator.next();
                    AdventureFightStats entity = player.getFightStats();
                    if (entity == null)
                    {
                        continue;
                    }
                    try {
                    	DZM.getEntityManager().updateSync(entity);
					} catch (Throwable e) {
						logger2.info("Landing data error for playerid:{},table:{},id:{},message:{}",new Object[]{player.getId(),"adventurefightstats",0,e.getMessage()});
					}
                    
                    updateCount ++;
                }
                break;
            case 17:
            	while (iterator.hasNext()) {
					iterator.next();
					RelationShip entity = player.getRelationShip();
					if (entity == null) {
						continue;
					}
					try {
						DZM.getEntityManager().updateSync(entity);
					} catch (Throwable e) {
						logger2.info("Landing data error for playerid:{},table:{},id:{},message:{}",new Object[]{player.getId(),"relationship",0,e.getMessage()});
					}
					
					updateCount ++;
				}
                break;
            case 18:
                while (iterator.hasNext())
                {
					iterator.next();
                    Handbook entity = player.getHandbook();
                    if (entity == null)
                    {
                        continue;
                    }
                    try {
                    	DZM.getEntityManager().updateSync(entity);
					} catch (Throwable e) {
						logger2.info("Landing data error for playerid:{},table:{},id:{},message:{}",new Object[]{player.getId(),"handbook",0,e.getMessage()});
					}
                    
                    updateCount ++;
                }
                break;
            case 19:
            	while (iterator.hasNext())
            	{
            		iterator.next();
            		ActivityInfo entity = player.getActivityInfo();
            		if (entity == null)
            		{
            			continue;
            		}
            		try {
            			DZM.getEntityManager().updateSync(entity);
            		} catch (Throwable e) {
            			logger2.info("Landing data error for playerid:{},table:{},id:{},message:{}",new Object[]{player.getId(),"activityInfo",0,e.getMessage()});
            		}
            		
            		updateCount ++;
            	}
            	break;
		}
	}
	
	private static int getTypeByString(String type){
		int id = 0;
		
		if (type.equals("bag")) {
			id = 1;
		}
		else if (type.equals("battle")) {
			id = 2;
		}
		else if (type.equals("career")) {
			id = 3;
		}
		else if (type.equals("disciple")) {
			id = 4;
		}
		else if (type.equals("equip")) {
			id = 5;
		}
		else if (type.equals("skill")) {
			id = 7;
		}
		else if (type.equals("soul")) {
			id = 8;
		}
		else if (type.equals("stats")) {
			id = 9;
		}
		else if (type.equals("teambuffer")) {
			id = 10;
		}
		else if (type.equals("title")) {
			id = 11;
		}
		else if(type.equals("rewards"))
        {
            id = 12;
        }
		else if (type.equals("challenge")) {
			id = 13;
		}
        else if(type.equals("skillBook"))
        {
            id = 14;
        }
        else if(type.equals("sysMsgInfo"))
        {
            id = 15;
        }
        else if(type.equals("fightEnd"))
        {
            id = 16;
        }
        else if(type.equals("relationship"))
        {
            id = 17;
        }
        else if(type.equals("handbooks"))
        {
            id = 18;
        }
        else if(type.equals("activityInfo"))
        {
        	id = 19;
        }
		return id;
	}
	
	public static void landLeaugData(){
		Iterator<LeaguaObject> it = LeaguaServices.allLeaguaObject.values().iterator();
		while(it.hasNext()){
			LeaguaObject p = it.next();
			try {
				if(p.getIsDirty()){
					DZM.getEntityManager().updateSync(p.getLeaguaInfo());
					HashMap<Integer, LeaguaMedal> leaguaMedals = p.getLeaguaMedals();
					if (leaguaMedals.containsKey(2)) {
						DZM.getEntityManager().updateSync(leaguaMedals.get(2));
					}
					HashMap<String, HashSet<Integer>> needUpdate = p.getNeedUpdate();
					if (needUpdate.size() != 0) {
						Iterator<Map.Entry<String, HashSet<Integer>>> types = needUpdate.entrySet().iterator();
						while (types.hasNext()) {
							Map.Entry<String, HashSet<Integer>> entry = types.next();
							String table = entry.getKey();
							HashSet<Integer> ids = entry.getValue();
							try {
								updateLeaguaByType(p,table, ids, OprRetTypeConst.Update);
							} catch (Exception e) {
								
							}
							
						}
					}
					
					p.clearNeedUpdate();
				}
			} catch (Throwable e) {
				logger2.info("Landing leagua data error for playerid:{},message:{}",new Object[]{p.getLeaguaInfo().getLeaguaId(),e.getMessage()});
				e.printStackTrace();
			}
		}
		
		Iterator<LeaguaMedal> it2 = LeaguaServices.allSpecialLeaguaMedals.values().iterator();
		while(it2.hasNext()){
			LeaguaMedal medal = it2.next();
			DZM.getEntityManager().updateSync(medal);
		}
	}
	
	public static void updateLeaguaByType(LeaguaObject leaguaObject,String type,HashSet<Integer> ids,int oprType){
		Iterator<Integer> iterator = ids.iterator();
		if (type.equals("members")) {
			while (iterator.hasNext()) {
	    		int id = iterator.next();
	    		
	    		LeaguaMembers entity = leaguaObject.getLeaguaMembersInfo().get(id);
	    		if (entity == null) {
	                continue;
	            }
	    		DZM.getEntityManager().updateSync(entity);
	    	}
		}
		else if (type.equals("medals")) {
			while (iterator.hasNext()) {
	    		int id = iterator.next();
	    		
	    		LeaguaMedal entity = leaguaObject.getLeaguaMedals().get(id);
	    		if (entity == null) {
	                continue;
	            }
	    		DZM.getEntityManager().updateSync(entity);
	    	}
		}
	}
	
	public static void landOnePlayer(Player p){
		try {
			if(p.getIsDirty()){
                DZM.getEntityManager().updateSync(p);
				HashMap<String, HashSet<Integer>> needUpdate = p.getNeedUpdate();
				if (needUpdate.size() != 0) {
					Iterator<Map.Entry<String, HashSet<Integer>>> types = needUpdate.entrySet().iterator();
					while (types.hasNext()) {
						Map.Entry<String, HashSet<Integer>> entry = types.next();
						String table = entry.getKey();
						HashSet<Integer> ids = entry.getValue();
						try {
							updateByType(p,table, ids, OprRetTypeConst.Update);
						} catch (Throwable e) {
							logger2.info("Landing data error for playerid:{},type:{}",new Object[]{p.getId(),table});
						}
						
					}
				}
				p.clearNeedUpdate();
                //add by dylan 残章、论剑传书部分非实时填入数据库
                HashMap<String, HashSet<Integer>> needAdd = p.getNeedAdd();
                if(needAdd.size()!=0)
                {
                    Iterator<Map.Entry<String, HashSet<Integer>>> types = needAdd.entrySet().iterator();
                    while (types.hasNext())
                    {
                        Map.Entry<String, HashSet<Integer>> entry = types.next();
                        String table = entry.getKey();
                        HashSet<Integer> ids = entry.getValue();
                        try {
                        	updateByType(p,table, ids, OprRetTypeConst.ADD);
						} catch (Throwable e) {
							logger2.info("Landing data error for playerid:{},type:{}",new Object[]{p.getId(),table});
						}
                        
                    }
                }
                p.clearNeedAdd();
			}
		} catch (Throwable e) {
			logger2.info("Landing data error for playerid:{},message:{}",new Object[]{p.getId(),e.getMessage()});
			e.printStackTrace();
		}
	}
}

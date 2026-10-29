package com.oldmountain.dzm.util;

import java.util.ArrayList;

import com.oldmountain.dzm.services.LeaguaServices;
import com.oldmountain.dzm.services.SysMsgInfoServices;
import org.hibernate.HibernateException;
import org.hibernate.SQLQuery;
import org.hibernate.Session;
import org.hibernate.Transaction;

import com.oldmountain.dzm.DZM;
import com.oldmountain.dzm.net.ObjectAccessor;
import com.oldmountain.dzm.packethandler.WorldBossHandler;
import com.oldmountain.dzm.services.FightEndRankingServices;

/*
 * Created with IntelliJ IDEA.
 * User: dylan
 * Date: 13-4-8
 * Time: 下午8:38
 * To change this template use File | Settings | File Templates.
 */
public class LoadDbToRAMUtil
{
    /**
     * 初始化数据库数据到排行榜内
     */
    public synchronized static void initialize()
    {
       FightEndRankingServices.initRanking();
       loadExistName();
       loadChallengeCount();
       SysMsgInfoServices.loadSysMsgSequenceId();
       WorldBossHandler.loadYesBossInfoToRam();
       LeaguaServices.LoadLeaguaDataToRam();
    }
    
    private synchronized static void loadExistName(){
    	ArrayList<String> _list=null;
        Session session=DZM.getEntityManager().getSessionFactory();
        Transaction tx = session.beginTransaction( ) ;
		
		try
        {
			String sql = "select name from player";
			SQLQuery query = session.createSQLQuery( sql ) ;
	        _list=(ArrayList<String>) query.list();
	        session.clear();
	        tx.commit() ;
        }catch (HibernateException e) {
        	if (tx!=null)
            {
                tx.rollback();
            }
            throw e;
		}
        
        DZM.totalUserCount = _list.size();
        
        ArrayList<String> persistName = XMLTemplateService.getPersistNames();
        
        _list.addAll(persistName);
        
        ObjectAccessor.setExistnames(_list);
    }
    
    private synchronized static void loadChallengeCount(){
    	ArrayList<String> _list=null;
        Session session=DZM.getEntityManager().getSessionFactory();
        Transaction tx = session.beginTransaction( ) ;
		
		try
        {
			String sql = "select rank from challenge";
			SQLQuery query = session.createSQLQuery( sql ) ;
	        _list=(ArrayList<String>) query.list();
	        session.clear();
	        tx.commit() ;
        }catch (HibernateException e) {
        	if (tx!=null)
            {
                tx.rollback();
            }
            throw e;
		}
        
        DZM.totalChallengeCount = _list.size();
    }

    /**
     * 停服时将需要同步的数据进行同步到数据库中后停服
     */
    public synchronized static void export()
    {

    }
}

package com.oldmountain.dzm.services.daoImpl;
import com.oldmountain.dzm.DZM;
import com.oldmountain.dzm.entity.AdventureFightRank;
import com.oldmountain.dzm.services.dao.AdventureFightRankingDao;
import org.hibernate.*;

import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.Date;
import java.util.List;

/*
 * Created with IntelliJ IDEA.
 * User: dylan
 * Date: 13-4-2
 * Time: 上午10:14
 * To change this template use File | Settings | File Templates.
 */
public class AdventureFightRankingImpl implements AdventureFightRankingDao
{

    @Override
    public List<AdventureFightRank> getFightRanking(int time)
    {
        return DZM.getEntityManager().limitQuery("from AdventureFightRank where rankTime=?",0,80,time);
    }

    @Override
    public int addFightRanking(AdventureFightRank rank)
    {
        DZM.getEntityManager().createSync(rank);
        return 0;
    }

    @Override
    public int delFightRanking(int barrierCount,int time)
    {
        //删除重启服务器后今日的排行榜数据
        Session session=DZM.getEntityManager().getSessionFactory();
        Transaction tx = session.beginTransaction( ) ;
        try
        {
            String sql = " delete from adventurefightrank  WHERE barrierCount=? AND rankTime=?" ;
            SQLQuery query = session.createSQLQuery( sql ) ;
            query.setInteger(0,barrierCount);
            query.setInteger(1,time);
            query.executeUpdate();
            session.clear();
            tx.commit() ;
        }
        catch( HibernateException e)
        {
            if (tx!=null)
            {
                tx.rollback();
            }
            throw e;
        }
        return 0;
    }
    @Override
    public boolean syncStatsToRank(int barrierCount,int startTime,int endTime,int rankTime)
    {
        boolean  _ret=true;
        Session session=DZM.getEntityManager().getSessionFactory();
        Transaction tx = session.beginTransaction( ) ;
        
        int lastRankTime = rankTime - 1;
        
        if (lastRankTime % 100 == 0) {
        	Calendar c = Calendar.getInstance(); 
    		Date date=null; 
    		try {
    			date = new SimpleDateFormat("yyyyMMdd").parse(String.valueOf(rankTime)); 
    		} catch (Exception e) {
    			e.printStackTrace();
    		}
    		c.setTime(date);
    		
    		int day = c.get(Calendar.DATE);
    		c.set(Calendar.DATE, day - 1);
    		
    		String dateBefore = new SimpleDateFormat("yyyyMMdd").format(c.getTime());
    		lastRankTime = Integer.valueOf(dateBefore);
		}

        try
        {
            String sql = "INSERT INTO adventurefightrank " +
                    "(playerid,playername,playerlevel,barriercount,passbarrier,obtainstar,continueinrankamount,ranktime)" +
                    " SELECT " +
                    "a.playerid,IFNULL(p.name ,\"\") AS playername ,IFNULL(p.level ,0) AS playerlevel ,a.barriercount AS barriercount,a.maxbarrierLV AS passbarrier," +
                    "a.maxobtainstar AS obtainstar,(IFNULL(ar.continueinrankamount,-1)+1) AS continueinrankamount,? AS ranktime" +
                    " FROM adventurefightstats AS a" +
                    " LEFT JOIN player AS p ON a.playerId=p.id " +
                    " LEFT JOIN adventurefightrank AS ar ON ar.playerid=a.playerid  AND ar.rankTime=? " +
                    "WHERE a.nextResetTime > ?  AND a.nextResetTime<=? AND " +
                    " a.barrierCount=? AND a.maxobtainstar>0 ORDER BY a.maxobtainstar DESC LIMIT 0,20" ;
            SQLQuery query = session.createSQLQuery( sql ) ;
            query.setInteger(0,rankTime);
            query.setInteger(1,lastRankTime);
            query.setInteger(2,startTime);
            query.setInteger(3,endTime);
            query.setInteger(4,barrierCount);
            query.executeUpdate();
            session.clear();
            tx.commit() ;
        }
        catch( HibernateException e)
        {
            if (tx!=null)
            {
                tx.rollback();
            }
            throw e;
        }
        return _ret;
    }

    public List<AdventureFightRank> getFightRankingFromStats(int barrierCount,int startTime,int endTime,int rankTime)
    {
        List<AdventureFightRank> _list=null;
        Session session=DZM.getEntityManager().getSessionFactory();
        Transaction tx = session.beginTransaction( ) ;
        
        int lastRankTime = rankTime - 1;
        
        if (lastRankTime % 100 == 0) {
        	Calendar c = Calendar.getInstance(); 
    		Date date=null; 
    		try {
    			date = new SimpleDateFormat("yyyyMMdd").parse(String.valueOf(rankTime)); 
    		} catch (Exception e) {
    			e.printStackTrace();
    		}
    		c.setTime(date);
    		
    		int day = c.get(Calendar.DATE);
    		c.set(Calendar.DATE, day - 1);
    		
    		String dateBefore = new SimpleDateFormat("yyyyMMdd").format(c.getTime());
    		lastRankTime = Integer.valueOf(dateBefore);
		}
        
        try
        {
            String sql = " SELECT a.playerid AS id," +
                    "a.playerid,IFNULL(p.name ,\"\") AS playername ,IFNULL(p.level ,0) AS playerlevel ,a.barriercount AS barriercount,a.maxbarrierLV AS passbarrier," +
                    "a.maxobtainstar AS obtainstar,(IFNULL(ar.continueinrankamount,-1)+1) AS continueinrankamount,? AS ranktime" +
                    " FROM adventurefightstats AS a" +
                    " LEFT JOIN player AS p ON a.playerId=p.id " +
                    " LEFT JOIN adventurefightrank AS ar ON ar.playerid=a.playerid  AND ar.rankTime=? " +
                    "WHERE a.nextResetTime > ?  AND a.nextResetTime<=? AND" +
                    " a.barrierCount=? AND a.maxobtainstar>0 ORDER BY a.maxobtainstar DESC LIMIT 0,20" ;
            SQLQuery query = session.createSQLQuery( sql ) ;
            query.setInteger(0,rankTime);
            query.setInteger(1,lastRankTime);
            query.setInteger(2,startTime);
            query.setInteger(3,endTime);
            query.setInteger(4,barrierCount);
            query.addEntity(AdventureFightRank.class);
            _list=query.list();
            session.clear();
            tx.commit() ;
        }
        catch( HibernateException e)
        {
            if (tx!=null)
            {
                tx.rollback();
            }
            throw e;
        }
        return _list;
    }
}

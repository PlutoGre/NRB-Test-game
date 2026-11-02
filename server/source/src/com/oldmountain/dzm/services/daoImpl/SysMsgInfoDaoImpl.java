package com.oldmountain.dzm.services.daoImpl;

import com.oldmountain.dzm.DZM;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.entity.Reward;
import com.oldmountain.dzm.entity.SysMsgInfo;
import com.oldmountain.dzm.services.RewardServices;
import com.oldmountain.dzm.services.dao.RewardDao;
import com.oldmountain.dzm.services.dao.SysMsgInfoDao;
import com.oldmountain.dzm.util.Util;
import net.sf.json.JSONArray;
import net.sf.json.JSONObject;
import org.hibernate.HibernateException;
import org.hibernate.SQLQuery;
import org.hibernate.Session;
import org.hibernate.Transaction;

import java.util.List;

/*
 * Created with IntelliJ IDEA.
 * User: Administrator
 * Date: 13-4-4
 * Time: 下午5:08
 * To change this template use File | Settings | File Templates.
 */
public class SysMsgInfoDaoImpl implements SysMsgInfoDao
{
    @Override
    public List<SysMsgInfo> getSysMsgInfo(int id)
    {
        StringBuilder sb=new StringBuilder("from SysMsgInfo where playerId=");
        sb.append(id);
        sb.append(" ORDER BY id desc LIMIT 0,50 ");
        return DZM.getEntityManager().query(SysMsgInfo.class, sb.toString());
    }

    @Override
    public int addSysMsgInfo(SysMsgInfo sysMsgInfo)
    {
        DZM.getEntityManager().createSync(sysMsgInfo);
        return 0;
    }

    @Override
    public int updateSysMsgInfo(SysMsgInfo sysMsgInfo)
    {
        DZM.getEntityManager().updateSync(sysMsgInfo);
        return 0;
    }

    @Override
    public boolean deleteSysMsgInvalid(int playerId,int id)
    {
        Session session=DZM.getEntityManager().getSessionFactory();
        Transaction tx = session.beginTransaction( ) ;
        try
        {
            String sql = " delete from sysmsginfo  WHERE playerId=? and id < ?" ;
            SQLQuery query = session.createSQLQuery( sql ) ;
            query.setInteger(0,playerId);
            query.setInteger(1,id);
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
        return true;
    }

    @Override
    public int getMaxSequence()
    {
        int  _ret=0;
        Session session=DZM.getEntityManager().getSessionFactory();
        Transaction tx = session.beginTransaction() ;
        try
        {
            StringBuilder sb=new StringBuilder();
            sb.append("SELECT IFNULL(MAX(id),0) AS id FROM sysmsginfo  ");
            SQLQuery query = session.createSQLQuery( sb.toString() ) ;
            _ret=Integer.parseInt(query.uniqueResult().toString());
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

    public int getCountSysMsgByReadTime(int playerId,int readTime)
    {
        int  _ret=0;
        Session session=DZM.getEntityManager().getSessionFactory();
        Transaction tx = session.beginTransaction() ;
        try
        {
            StringBuilder sb=new StringBuilder();
            sb.append("SELECT (SELECT COUNT(*) FROM sysmsginfo AS s WHERE s.playerId=? AND s.addTime>?) AS sCount,COUNT(*) AS rCount FROM rewards AS r WHERE r.playerId=? AND r.addTime>?");
            SQLQuery query = session.createSQLQuery( sb.toString() ) ;
            query.setInteger(0,playerId);
            query.setInteger(1,readTime);
            query.setInteger(2,playerId);
            query.setInteger(3,readTime);
            Object[] _r= (Object[])query.list().get(0);
            _ret=Integer.parseInt(_r[0].toString())+Integer.parseInt(_r[1].toString());
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
}
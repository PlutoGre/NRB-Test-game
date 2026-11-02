package com.oldmountain.dzm.services.daoImpl;
import com.oldmountain.dzm.DZM;
import com.oldmountain.dzm.entity.PayInfo;
import com.oldmountain.dzm.services.dao.PayInfoOprDao;
import com.oldmountain.dzm.util.TimeUtil;
import org.hibernate.HibernateException;
import org.hibernate.SQLQuery;
import org.hibernate.Session;
import org.hibernate.Transaction;

import java.util.List;

/*
 * Created with IntelliJ IDEA.
 * User: dylan
 * Date: 13-4-2
 * Time: 上午10:14
 * To change this template use File | Settings | File Templates.
 */
public class PayInfoOprDaoImpl implements PayInfoOprDao
{

    @Override
    public List<PayInfo> getBuyProp(int playerId,int ym) throws HibernateException
    {
        return DZM.getEntityManager().query(PayInfo.class, "from wBuyPropPay"+ym+" where playerid = ?", playerId);
    }

    @Override
    public boolean addBuyPropPay(PayInfo payInfo)  throws HibernateException
    {
        boolean  _ret=true;
        Session session=DZM.getEntityManager().getSessionFactory();
        Transaction tx = session.beginTransaction( ) ;

        try
        {
            String sql = "insert into wpayopr"+ TimeUtil.getSysDate("yyyyMM")+"(playerId, propId,payType,buyNum,originalCash,consumeCash,surplusCash,oprTime) values (?, ?, ?,?, ?, ?,?,?)" ;
            SQLQuery query = session.createSQLQuery( sql ) ;
            query.setInteger(0,payInfo.getPlayerId());
            query.setInteger(1,payInfo.getPropId());
            query.setInteger(2,payInfo.getPayType());
            query.setInteger(3,payInfo.getBuyNum());
            query.setInteger(4,payInfo.getOriginalCash());
            query.setInteger(5,payInfo.getConsumeCash());
            query.setInteger(6,payInfo.getSurplusCash());
            query.setInteger(7,payInfo.getOprTime());
            query.executeUpdate();
            tx.commit() ;
        }
        catch( HibernateException e)
        {
            _ret=false;
            if (tx!=null)
            {
                tx.rollback();
            }
            throw e;
        }
        return _ret;
    }

    @Override
    public boolean updateArmy(PayInfo payInfo)  throws HibernateException
    {
        DZM.getEntityManager().updateSync(payInfo);
        return true;
    }
}

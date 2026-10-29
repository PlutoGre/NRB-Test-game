package com.oldmountain.dzm.services.daoImpl;

import java.util.List;

import net.sf.json.JSONArray;
import net.sf.json.JSONObject;

import org.hibernate.HibernateException;
import org.hibernate.SQLQuery;
import org.hibernate.Session;
import org.hibernate.Transaction;

import com.oldmountain.dzm.DZM;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.entity.Reward;
import com.oldmountain.dzm.services.RewardServices;
import com.oldmountain.dzm.services.dao.RewardDao;
import com.oldmountain.dzm.util.Util;

/*
 * Created with IntelliJ IDEA.
 * User: Administrator
 * Date: 13-4-4
 * Time: 下午5:08
 * To change this template use File | Settings | File Templates.
 */
public class RewardDaoImpl implements RewardDao
{
    @Override
    //搜索玩家最近三天的领取过的奖励传书,按照处理时间排序,取最近的10个
    public List<Reward> getReward(Player player,int currTime)
    {
        StringBuilder sb=new StringBuilder("from Reward where playerId=");
        sb.append(player.getId());
        sb.append(" and rewardTime > ");
        sb.append((currTime-3600*24*3));
        sb.append(" and status=1");
        sb.append(" order by addTime desc");
        return DZM.getEntityManager().limitQuery(sb.toString(),0,10);
    }

    @Override
    //查询玩家未领取的奖励传书
	public List<Reward> getRewardNotDeal(Player player) {
    	StringBuilder sb=new StringBuilder("from Reward where playerId=");
        sb.append(player.getId());
        sb.append(" and status=0 ");
        return DZM.getEntityManager().query(Reward.class, sb.toString());
	}



	@Override
    public int addReward(Reward reward)
    {
        DZM.getEntityManager().createSync(reward);
        return 0;
    }

    @Override
    public boolean deleteRewardInvalid(int currTime)
    {
        Session session=DZM.getEntityManager().getSessionFactory();
        Transaction tx = session.beginTransaction( ) ;
        try
        {
            String sql = " delete from rewards  WHERE rewardTime<? or (addTime<? and status=0)" ;
            SQLQuery query = session.createSQLQuery( sql ) ;
            query.setInteger(0,(currTime-3600*24*3));
            query.setInteger(1,(currTime-3600*24*4));
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
    public boolean createCompensateReward(String rewardMsg, JSONObject rewardInfo, JSONArray rewardContent,int status, int playerId, int lastLogin,int levelStart,int levelEnd,int rankStart,int rankEnd,String channel)
    {
        boolean  _ret=true;
        Session session=DZM.getEntityManager().getSessionFactory();
        Transaction tx = session.beginTransaction( ) ;
        int currTime=Util.getServerTime();
        try
        {
        	StringBuilder sb=new StringBuilder();
        	if (rankStart != 0 && rankEnd != 0) {
        		sb.append("INSERT INTO rewards(playerId,rewardType,rewardMsg,rewardInfo,rewardContent,status,addTime,rewardTime) SELECT playerid,");
	            sb.append(RewardServices.COMPENSATE_REWARD+",");
	            sb.append("?,");
	            sb.append("?,");
	            sb.append("?,");
	            sb.append(status+",");
	            sb.append(currTime).append(",");
	            sb.append(currTime);
	            sb.append(" from challenge");
                sb.append(" where rank >=").append(rankStart);
                sb.append(" and rank <=").append(rankEnd);
			}
        	else {
	            sb.append("INSERT INTO rewards(playerId,rewardType,rewardMsg,rewardInfo,rewardContent,status,addTime,rewardTime) SELECT id,");
	            sb.append(RewardServices.COMPENSATE_REWARD+",");
	            sb.append("?,");
	            sb.append("?,");
	            sb.append("?,");
	            sb.append(status+",");
	            sb.append(currTime).append(",");
	            sb.append(currTime);
	            sb.append(" from player");
	            boolean  isAnd=false;
	            if(playerId!=-1)
	            {
	                sb.append(" where id =").append(playerId);
	                isAnd=true;
	            }
	            if(lastLogin!=-1)
	            {
	                if(isAnd)
	                {
	                    sb.append(" and ");
	                }
	                else
	                {
	                    sb.append(" where ");
	                    isAnd = !isAnd;
	                }
	                sb.append(" lastLogin >=").append(lastLogin);
	            }
	            if (levelStart != 0) {
	            	if(isAnd)
	                {
	                    sb.append(" and ");
	                }
	                else
	                {
	                    sb.append(" where ");
	                    isAnd = !isAnd;
	                }
	                sb.append(" level >=").append(levelStart);
				}
	            if (levelEnd != 0) {
	            	if(isAnd)
	                {
	                    sb.append(" and ");
	                }
	                else
	                {
	                    sb.append(" where ");
	                    isAnd = !isAnd;
	                }
	                sb.append(" level <").append(levelEnd);
				}
	            if (!"".equals(channel)) {
	            	if(isAnd)
	                {
	                    sb.append(" and ");
	                }
	                else
	                {
	                    sb.append(" where ");
	                    isAnd = !isAnd;
	                }
	                sb.append(" puid like \"").append(channel).append("%\" ");
				}
        	}
            SQLQuery query = session.createSQLQuery( sb.toString() ) ;
            query.setString(0,rewardMsg);
            query.setString(1,rewardInfo.toString());
            query.setString(2,rewardContent.toString());
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
            e.printStackTrace();
            throw e;
        }
        return _ret;
    }
}
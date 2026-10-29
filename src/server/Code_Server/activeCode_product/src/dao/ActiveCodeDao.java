package dao;

import java.util.ArrayList;
import java.util.List;

import entity.ActiveCodeEntity;
import org.hibernate.Query;
import org.hibernate.Session;
import org.hibernate.SessionFactory;
import org.hibernate.Transaction;
import org.hibernate.cfg.Configuration;

public class ActiveCodeDao {
	private static Configuration configuration;
	private static SessionFactory sessionFactory;
	static {
		configuration = new Configuration();
		configuration.configure();
		sessionFactory = configuration.buildSessionFactory();
	}

	private Session getSession() {
		return this.sessionFactory.getCurrentSession();
	}

	public void saveList(List<ActiveCodeEntity> cdkeys) throws Exception {
		Session session = this.getSession();
		Transaction tr = session.beginTransaction();
		try {
			for (ActiveCodeEntity cdkey : cdkeys) {
				session.save(cdkey);
			}
			tr.commit();
		} 
		catch (Exception e) {
			tr.rollback();
			throw e;
		}
	}

	public List<ActiveCodeEntity> queryWithCondition(String condition) {
		String hql = "from ActiveCodeEntity ac where 1=1 ";
		hql += condition;
		Session session = getSession();
		Transaction tx = session.beginTransaction();
		List<ActiveCodeEntity> list=new ArrayList<ActiveCodeEntity>();
		try 
		{
			Query query = getSession().createQuery(hql);
			list = query.list();
			tx.commit();
			return list;
		} 
		catch (Exception e) 
		{
			tx.rollback();
		} 
		return list;
	}
}

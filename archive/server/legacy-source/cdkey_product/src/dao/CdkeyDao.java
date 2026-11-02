package dao;

import java.io.Serializable;
import java.util.ArrayList;
import java.util.List;

import org.hibernate.EntityMode;
import org.hibernate.Query;
import org.hibernate.Session;
import org.hibernate.SessionFactory;
import org.hibernate.Transaction;
import org.hibernate.cfg.Configuration;

import entity.Cdkey;

public class CdkeyDao {
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

	public void saveList(List<Cdkey> cdkeys) throws Exception {
		Session session = this.getSession();
		Transaction tr = session.beginTransaction();
		try {
			for (Cdkey cdkey : cdkeys) {
				session.save(cdkey);
			}
			tr.commit();
		} 
		catch (Exception e) {
			tr.rollback();
			throw e;
		}
	}

	public List<Cdkey> queryWithCondition(String condition) {
		String hql = "from Cdkey c where 1=1 ";
		hql += condition;
		Session session = getSession();
		Transaction tx = session.beginTransaction();
		List<Cdkey> list=new ArrayList<Cdkey>();
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

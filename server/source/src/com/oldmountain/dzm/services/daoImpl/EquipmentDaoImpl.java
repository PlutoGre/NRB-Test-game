package com.oldmountain.dzm.services.daoImpl;
import com.oldmountain.dzm.DZM;
import com.oldmountain.dzm.entity.Equip;
import com.oldmountain.dzm.entity.PayInfo;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.services.dao.EquipmentDao;
import com.oldmountain.dzm.services.dao.PayInfoOprDao;
import com.oldmountain.dzm.util.TimeUtil;
import com.oldmountain.dzm.util.Util;
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
public class EquipmentDaoImpl implements EquipmentDao
{

    public List<Equip> getEquipment(Player player)
    {
        return null;
    }
    public int addEquip(Player player,int id)
    {
        return Util.rewardEquip(player,id);
    }
    public boolean updateEquip(Player player,Equip equip)
    {
        return false;
    }
}

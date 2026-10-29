package com.oldmountain.dzm.services.dao;

import com.oldmountain.dzm.entity.Equip;
import com.oldmountain.dzm.entity.PayInfo;
import com.oldmountain.dzm.entity.Player;

import java.util.List;

/*
 * Created with IntelliJ IDEA.
 * User: dylan
 * Date: 13-4-3
 * Time: 下午12:23
 * To change this template use File | Settings | File Templates.
 */
public interface EquipmentDao
{
    public List<Equip> getEquipment(Player player);
    public int addEquip(Player player,int id);
    public boolean updateEquip(Player player,Equip equip);
}

package com.oldmountain.dzm.services.dao;

import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.entity.Soul;

import java.util.List;

/**
 * Created with IntelliJ IDEA.
 * User: Administrator
 * Date: 13-4-4
 * Time: 下午5:01
 * To change this template use File | Settings | File Templates.
 */
public interface SoulDao {
    public List<Soul> getSkill(Player player);
    public int addSoul(Player player, int id,int count);
    public boolean updateSoul(Player player, Soul soul);
}

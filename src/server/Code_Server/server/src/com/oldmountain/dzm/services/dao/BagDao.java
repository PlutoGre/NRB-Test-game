package com.oldmountain.dzm.services.dao;

import com.oldmountain.dzm.entity.Bag;
import com.oldmountain.dzm.entity.Player;

import java.util.List;

/**
 * Created with IntelliJ IDEA.
 * User: Administrator
 * Date: 13-4-4
 * Time: 下午5:39
 * To change this template use File | Settings | File Templates.
 */
public interface BagDao {
    public List<Bag> getBagTools(Player player);
    public int addBagTool(Player player, int itemId,int count);
    public boolean updateBagTool(Player player, Bag bagTool);
}

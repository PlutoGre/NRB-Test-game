package com.oldmountain.dzm.services.daoImpl;

import com.oldmountain.dzm.entity.Bag;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.services.dao.BagDao;
import com.oldmountain.dzm.util.Util;

import java.util.List;

/**
 * Created with IntelliJ IDEA.
 * User: Administrator
 * Date: 13-4-4
 * Time: 下午5:39
 * To change this template use File | Settings | File Templates.
 */
public class BagDaoImpl implements BagDao {
    @Override
    public List<Bag> getBagTools(Player player) {
        return null;  //To change body of implemented methods use File | Settings | File Templates.
    }

    @Override
    public int addBagTool(Player player, int itemId, int count) {
        return Util.rewardBagTool(player, itemId, count);  //To change body of implemented methods use File | Settings | File Templates.
    }

    @Override
    public boolean updateBagTool(Player player, Bag bagTool) {
        return false;  //To change body of implemented methods use File | Settings | File Templates.
    }
}

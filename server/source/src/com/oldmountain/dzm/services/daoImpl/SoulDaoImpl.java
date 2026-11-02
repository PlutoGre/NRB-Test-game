package com.oldmountain.dzm.services.daoImpl;

import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.entity.Soul;
import com.oldmountain.dzm.services.dao.SoulDao;
import com.oldmountain.dzm.util.Util;

import java.util.List;

/*
 * Created with IntelliJ IDEA.
 * User: Administrator
 * Date: 13-4-4
 * Time: 下午5:08
 * To change this template use File | Settings | File Templates.
 */
public class SoulDaoImpl implements SoulDao {
    @Override
    public List<Soul> getSkill(Player player) {
        return null;  //To change body of implemented methods use File | Settings | File Templates.
    }

    @Override
    public int addSoul(Player player, int id,int count) {
        return Util.rewardSoul(player, id, count);  //To change body of implemented methods use File | Settings | File Templates.
    }

    @Override
    public boolean updateSoul(Player player, Soul soul) {
        return false;  //To change body of implemented methods use File | Settings | File Templates.
    }
}

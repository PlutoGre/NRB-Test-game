package com.oldmountain.dzm.services.daoImpl;
import com.oldmountain.dzm.entity.Equip;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.entity.Skill;
import com.oldmountain.dzm.services.dao.EquipmentDao;
import com.oldmountain.dzm.services.dao.SkillDao;
import com.oldmountain.dzm.util.Util;

import java.util.List;

/*
 * Created with IntelliJ IDEA.
 * User: dylan
 * Date: 13-4-2
 * Time: 上午10:14
 * To change this template use File | Settings | File Templates.
 */
public class SkillDaoImpl implements SkillDao
{

    @Override
    public List<Skill> getSkill(Player player)
    {
        return null;
    }

    @Override
    public int addSkill(Player player, int id)
    {
        return Util.rewardSkill(player,id);
    }

    @Override
    public boolean updateSkill(Player player, Equip equip) {
        return false;  //To change body of implemented methods use File | Settings | File Templates.
    }
}

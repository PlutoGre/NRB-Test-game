package com.oldmountain.dzm.services.dao;

import com.oldmountain.dzm.entity.Equip;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.entity.Skill;

import java.util.List;

/*
 * Created with IntelliJ IDEA.
 * User: dylan
 * Date: 13-4-3
 * Time: 下午12:23
 * To change this template use File | Settings | File Templates.
 */
public interface SkillDao
{
    public List<Skill> getSkill(Player player);
    public int addSkill(Player player, int id);
    public boolean updateSkill(Player player, Equip equip);
}

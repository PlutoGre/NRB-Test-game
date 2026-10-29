package com.oldmountain.dzm.services.dao;

import com.oldmountain.dzm.entity.AdventureFightStats;
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
public interface AdventureFightStatsDao
{
    public AdventureFightStats getAdventureFightStats(Player player);
    public int addAdventureFightStats(Player player);
    public int updateAdventureFightStats(Player player);
}

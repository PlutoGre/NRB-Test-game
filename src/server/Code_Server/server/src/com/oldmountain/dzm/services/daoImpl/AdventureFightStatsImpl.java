package com.oldmountain.dzm.services.daoImpl;
import com.oldmountain.dzm.DZM;
import com.oldmountain.dzm.entity.AdventureFightStats;
import com.oldmountain.dzm.entity.Equip;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.entity.Skill;
import com.oldmountain.dzm.services.dao.AdventureFightStatsDao;
import com.oldmountain.dzm.services.dao.SkillDao;
import com.oldmountain.dzm.util.Util;

import java.util.ArrayList;
import java.util.List;

/*
 * Created with IntelliJ IDEA.
 * User: dylan
 * Date: 13-4-2
 * Time: 上午10:14
 * To change this template use File | Settings | File Templates.
 */
public class AdventureFightStatsImpl implements AdventureFightStatsDao
{

    @Override
    public AdventureFightStats getAdventureFightStats(Player player)
    {
        if(player.getFightStats().getPlayerId()==-1)
        {
            List<AdventureFightStats> _list= DZM.getEntityManager().limitQuery("from AdventureFightStats where playerId = ?", 0, 1, player.getId());
            if(_list.size()>0)
            {
                player.setFightStats(_list.get(0));
            }
        }
        return player.getFightStats();
    }

    @Override
    public int addAdventureFightStats(Player player)
    {
        DZM.getEntityManager().createSync(player.getFightStats());
        return  player.getFightStats().getId();
    }

    @Override
    public int updateAdventureFightStats(Player player)
    {
        DZM.getEntityManager().updateSync(player.getFightStats());
        return player.getFightStats().getId();
    }
}

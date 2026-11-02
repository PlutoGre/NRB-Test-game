package com.oldmountain.dzm.services;

import com.oldmountain.dzm.services.dao.*;
import com.oldmountain.dzm.services.daoImpl.*;

/*
 * Created with IntelliJ IDEA.
 * User: dylan
 * Date: 13-4-2
 * Time: 上午11:01
 * To change this template use File | Settings | File Templates.
 */
public class DaoManager
{
    public static PayInfoOprDao getBuyPropPayDao()
    {
        return new PayInfoOprDaoImpl();
    }

    public static EquipmentDao getEquipmentDao()
    {
        return new EquipmentDaoImpl();
    }
    public static SkillDao getSkillDao()
    {
        return new SkillDaoImpl();
    }
    public static SoulDao getSoulDao()
    {
        return new SoulDaoImpl();
    }
    public static BagDao getBagToolDao()
    {
        return new BagDaoImpl();
    }
    public static AdventureFightStatsDao getAdventureFightDao()
    {
        return new AdventureFightStatsImpl();
    }

    public static AdventureFightRankingDao getAdventureFightRankingDao()
    {
        return new AdventureFightRankingImpl();
    }

    public static RewardDao getRewardDao()
    {
        return new RewardDaoImpl();
    }

    public static SkillBookDao getSkillBookDao()
    {
        return new SkillBookDaoImpl();
    }

    public static SysMsgInfoDao getSysMsgInfoDao()
    {
        return new SysMsgInfoDaoImpl();
    }
}

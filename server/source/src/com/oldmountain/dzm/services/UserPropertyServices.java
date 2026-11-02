package com.oldmountain.dzm.services;

import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.util.UserToolConst;

/*
 * Created with IntelliJ IDEA.
 * User: dylan
 * Date: 13-4-3
 * Time: 下午2:19
 * To change this template use File | Settings | File Templates.
 */
public class UserPropertyServices
{
    public static void changeProperty(Player player,int type,int count)
    {
        switch (type)
        {
            case UserToolConst.USER_LEVEL_PROPERTY_GOLD_COINS:
            case UserToolConst.USER_PROPERTY_GOLD_COINS:
                player.increaseSysGoldCoins(count);
                break;
            case UserToolConst.USER_LEVEL_PROPERTY_SILVER_COINS:
            case UserToolConst.USER_PROPERTY_SILVER_COINS:
                player.increaseSilverCoins(count);
                break;
            case UserToolConst.USER_LEVEL_PROPERTY_EXP:
            case UserToolConst.USER_PROPERTY_EXP:
                player.increaseExp(count);
                break;
            default:
                break;
        }
    }
}

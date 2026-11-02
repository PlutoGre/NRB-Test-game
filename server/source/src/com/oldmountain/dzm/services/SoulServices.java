package com.oldmountain.dzm.services;

import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.entity.Soul;
import com.oldmountain.dzm.message.UserSoulInfo;

/*
 * Created with IntelliJ IDEA.
 * User: Administrator
 * Date: 13-4-4
 * Time: 下午5:18
 * To change this template use File | Settings | File Templates.
 */
public class SoulServices {
    public static UserSoulInfo.OPUserSoulInfoRet.SoulInfo.Builder addSoul(Player player,int itemId,int count)
    {
        UserSoulInfo.OPUserSoulInfoRet.SoulInfo.Builder _soulBuilder=UserSoulInfo.OPUserSoulInfoRet.SoulInfo.newBuilder();
        int id=DaoManager.getSoulDao().addSoul(player, itemId, count);
        Soul soul=player.getSouls().get(itemId);
        _soulBuilder.setId(soul.getId());
        _soulBuilder.setCount(soul.getCount());
        _soulBuilder.setItemid(soul.getItemid());
        return _soulBuilder;
    }
}

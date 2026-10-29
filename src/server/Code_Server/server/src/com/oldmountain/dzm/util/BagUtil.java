package com.oldmountain.dzm.util;

import com.oldmountain.dzm.entity.Bag;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.entity.UserStats;

/*
 * Created with IntelliJ IDEA.
 * User: dylan
 * Date: 13-4-2
 * Time: 下午8:02
 * To change this template use File | Settings | File Templates.
 */
public class BagUtil
{
    private static BagUtil _instance = null;

    private BagUtil()
    {
    }

    public synchronized static BagUtil getInstance()
    {
        if (_instance == null)
        {
            _instance = new BagUtil();
        }
        return _instance;
    }

    public static int getToolCountByToolId(Player player,int toolId)
    {
        if(player.getBags().containsKey(toolId))
        {
            return player.getBags().get(toolId).getCount();
        }
        return -1;
    }

    public Bag getToolByToolId(Player player,int toolId)
    {
         return player.getBags().get(toolId);
    }

    public int oprBagByToolId(Player player,int toolId,int count)
    {
        if(player.getBags().containsKey(toolId))
        {
              Bag _bag=player.getBags().get(toolId);
              _bag.setCount(_bag.getCount()+count);
              player.updateOneBag(_bag);
              return OprRetTypeConst.Update;
        }
        else
        {
            if(count>0)
            {
                Bag _bag=new Bag();
                _bag.setCount(count);
                _bag.setPlayerid(player.getId());
                _bag.setCreatetime(Util.getServerTime());
                _bag.setItemid(toolId);
                player.addOneBag(_bag);
                return OprRetTypeConst.ADD;
            }
        }
        return OprRetTypeConst.None;
    }
    public void addLuckPool(Player player,int toolItemId,int count,float proportion)
    {
        UserStats stats = player.getUserStats();
        stats.setLuckPool(stats.getLuckPool()+UserToolConst.treasureWorthMap.get(toolItemId)*proportion*count);
    }
    public void addLuckPool(Player player,int toolItemId,int count)
    {
        addLuckPool(player,toolItemId,count,1.0f);
    }

}

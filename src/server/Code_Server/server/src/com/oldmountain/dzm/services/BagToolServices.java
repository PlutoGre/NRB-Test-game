package com.oldmountain.dzm.services;

import com.oldmountain.dzm.entity.Bag;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.message.LeagueStructExt.OPLeaguaShopBuyRet;
import com.oldmountain.dzm.message.UserToolInfo;
import com.oldmountain.dzm.message.UserToolInfo.OPUserToolInfoRet;

/**
 * Created with IntelliJ IDEA.
 * User: Administrator
 * Date: 13-4-4
 * Time: 下午5:49
 * To change this template use File | Settings | File Templates.
 */
public class BagToolServices {
    public static UserToolInfo.OPUserToolInfoRet.ToolInfo.Builder addBagTool(Player player,int itemId,int count)
    {
        UserToolInfo.OPUserToolInfoRet.ToolInfo.Builder _bagBuilder=UserToolInfo.OPUserToolInfoRet.ToolInfo.newBuilder();
        DaoManager.getBagToolDao().addBagTool(player, itemId, count);
        Bag bagTool=player.getBags().get(itemId);
        _bagBuilder = buildToolBuilder(bagTool);
        return _bagBuilder;
    }
    
    public static OPUserToolInfoRet.ToolInfo.Builder buildToolBuilder(Bag bag){
    	OPUserToolInfoRet.ToolInfo.Builder builder = OPUserToolInfoRet.ToolInfo.newBuilder();
    	
    	builder.setId(bag.getId());
    	builder.setItemid(bag.getItemid());
    	builder.setCount(bag.getCount());
    	
    	return builder;
    }
    
    public static OPLeaguaShopBuyRet.ToolInfo.Builder buildToolBuilderForLeaguaBuy(Bag bag){
    	OPLeaguaShopBuyRet.ToolInfo.Builder builder = OPLeaguaShopBuyRet.ToolInfo.newBuilder();
    	
    	builder.setId(bag.getId());
    	builder.setItemid(bag.getItemid());
    	builder.setCount(bag.getCount());
    	
    	return builder;
    }
}

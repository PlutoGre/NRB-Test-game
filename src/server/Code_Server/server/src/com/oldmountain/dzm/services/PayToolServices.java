package com.oldmountain.dzm.services;

import com.oldmountain.dzm.DZM;
import com.oldmountain.dzm.entity.Bag;
import com.oldmountain.dzm.entity.PayInfo;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.entity.ToolContainReward;
import com.oldmountain.dzm.message.BuyTools.OPBuyToolRet;
import com.oldmountain.dzm.message.UserEquipInfo;
import com.oldmountain.dzm.message.UserSkillInfo;
import com.oldmountain.dzm.message.UserToolInfo;
import com.oldmountain.dzm.util.*;

import java.util.ArrayList;

/*
 * Created with IntelliJ IDEA.
 * User: dylan
 * Date: 13-4-2
 * Time: 下午8:56
 * To change this template use File | Settings | File Templates.
 */
public class PayToolServices
{
    public static OPBuyToolRet.Builder BuyTools(Player player,ToolContainReward[] containTools,int itemId,int count,int needPrice,boolean needRecordStats)
    {
        OPBuyToolRet.Builder builder=OPBuyToolRet.newBuilder();
        ArrayList _arr=new ArrayList();
        //购买消费记录
        PayInfo _pay=new PayInfo();
        _pay.setPlayerId(player.getId());
        _pay.setPropId(itemId);
        _pay.setBuyNum(count);
        _pay.setPayType(PayTypeConst.Tools);
        _pay.setOriginalCash(player.getGoldCoins());
        //更改player cash数据
        player.consumeGold(needPrice);
        _pay.setConsumeCash(needPrice);
        _pay.setSurplusCash(player.getGoldCoins());
        _pay.setOprTime(Util.getServerTime());
        if(needRecordStats)
        {
            player.getUserStats().increaseBuyGifts(String.valueOf(itemId));
            DZM.getEntityManager().updateSync(player.getUserStats());
        }

        //记录购买信息
        DaoManager.getBuyPropPayDao().addBuyPropPay(_pay);
        //添加背包道具内容
        if(containTools!=null&&containTools.length>0)
        {
            for (ToolContainReward _toolContain : containTools)
            {
                switch (UserToolConst.getType(_toolContain.getType()))
                {
                    case UserToolConst.TOOLS_TYPE:
                        builder.addToolInfo(BuyToolsAlone(player, _toolContain.getItemId(), _toolContain.getQuantity()));
                        break;
                    case UserToolConst.SKILL_TYPE:
                        if (_toolContain.getQuantity() > 1)
                        {
                            ArrayList<UserSkillInfo.OPUserSkillInfoRet.SkillInfo.Builder> arrayList=SkillServices.createSkillMuli(player, _toolContain.getItemId(), _toolContain.getQuantity());
                            for(UserSkillInfo.OPUserSkillInfoRet.SkillInfo.Builder _si:arrayList)
                            {
                                builder.addSkillInfo(_si);
                            }
                        }
                        else
                        {
                            builder.addSkillInfo(SkillServices.createSkillAlone(player, _toolContain.getItemId()));
                        }
                        break;

                    case UserToolConst.EQUIP_TYPE:
                        if (_toolContain.getQuantity() > 1)
                        {
                            ArrayList<UserEquipInfo.OPUserEquipInfoRet.EquipInfo.Builder> arrayList=EquipServices.createEquitMuli(player, _toolContain.getItemId(), _toolContain.getQuantity());
                            for(UserEquipInfo.OPUserEquipInfoRet.EquipInfo.Builder _ei:arrayList)
                            {
                                builder.addEquipInfo(_ei);
                            }
                        }
                        else
                        {
                            builder.addEquipInfo(EquipServices.createEquitAlone(player, _toolContain.getItemId()));
                        }
                        break;
                    case UserToolConst.USER_PROPERTY_TYPE:
                        UserPropertyServices.changeProperty(player, _toolContain.getItemId(), _toolContain.getQuantity());
                        break;
                }
            }
        }
        else
        {

            builder.addToolInfo(BuyToolsAlone(player, itemId, count));
        }
        builder.setGoldcoins(player.getGoldCoins());
        builder.setSilvercoins(player.getSilvercoins());
        return builder;
    }

    public static UserToolInfo.OPUserToolInfoRet.ToolInfo.Builder BuyToolsAlone(Player player,int itemId,int count)
    {
        UserToolInfo.OPUserToolInfoRet.ToolInfo.Builder _toolInfo=UserToolInfo.OPUserToolInfoRet.ToolInfo.newBuilder();
        BagUtil.getInstance().oprBagByToolId(player,itemId,count);
        Bag _bag=BagUtil.getInstance().getToolByToolId(player,itemId);

        _toolInfo.setItemid(_bag.getItemid());
        _toolInfo.setCount(_bag.getCount());
        _toolInfo.setId(_bag.getId());
        return _toolInfo;
    }
}

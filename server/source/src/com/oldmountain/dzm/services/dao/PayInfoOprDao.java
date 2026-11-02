package com.oldmountain.dzm.services.dao;

import com.oldmountain.dzm.entity.PayInfo;

import java.util.List;

/*
 * Created with IntelliJ IDEA.
 * User: dylan
 * Date: 13-4-2
 * Time: 上午10:10
 * To change this template use File | Settings | File Templates.
 */
public interface PayInfoOprDao
{
    public List<PayInfo> getBuyProp(int playerId,int ym);
    public boolean addBuyPropPay(PayInfo payInfo);
    public boolean updateArmy(PayInfo payInfo);
}

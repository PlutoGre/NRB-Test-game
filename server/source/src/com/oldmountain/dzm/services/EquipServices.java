package com.oldmountain.dzm.services;

import com.oldmountain.dzm.entity.Equip;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.message.UserEquipInfo;
import com.oldmountain.dzm.util.EquipUtil;
import net.sf.json.JSONArray;

import java.util.ArrayList;

/*
 * Created with IntelliJ IDEA.
 * User: dylan
 * Date: 13-4-2
 * Time: 下午8:56
 * To change this template use File | Settings | File Templates.
 */
public class EquipServices
{
    public static UserEquipInfo.OPUserEquipInfoRet.EquipInfo.Builder createEquitAlone(Player player,int itemId)
    {
        UserEquipInfo.OPUserEquipInfoRet.EquipInfo.Builder _equip=UserEquipInfo.OPUserEquipInfoRet.EquipInfo.newBuilder();
        int id=DaoManager.getEquipmentDao().addEquip(player,itemId);
        Equip equip=player.getEquips().get(id);
        _equip.setId(equip.getId());
        _equip.setItemid(equip.getItemid());
        _equip.setLevel(equip.getLevel());
        _equip.setRefinelevel(equip.getRefinelevel());
        _equip.setRefinexp(equip.getRefinexp());
        UserEquipInfo.OPUserEquipInfoRet.EquipInfo.PropertyInfo.Builder _propBuider = UserEquipInfo.OPUserEquipInfoRet.EquipInfo.PropertyInfo.newBuilder();
        _propBuider.setType(equip.getType());
        switch (equip.getType())
        {
            case 1:
                _propBuider.setNum(equip.getAttackToInt());
                break;
            case 2:
                _propBuider.setNum(equip.getDefenceToInt());
                break;
            case 3:
                _propBuider.setNum(equip.getHealthToInt());
                break;
            default:
                break;
        }
        //_equip.setStoneinlaidinfo(equip.getDiamondInfo().replaceAll("%","_"));
        JSONArray _stoneArr=equip.getStoneJsonArr();
        for(int i=0;i<_stoneArr.size();++i)
        {
            _equip.addStoneInfo(EquipUtil.getStoneInfoByStoneJson(_stoneArr.getJSONObject(i), i + 1));
        }
        _equip.setBuffvalue(equip.getStoneAdditionValue());
        _equip.addPInfo(_propBuider);
        return _equip;
    }

    public static ArrayList<UserEquipInfo.OPUserEquipInfoRet.EquipInfo.Builder> createEquitMuli(Player player, int itemId, int count)
    {
        ArrayList<UserEquipInfo.OPUserEquipInfoRet.EquipInfo.Builder> arrayList=new ArrayList<UserEquipInfo.OPUserEquipInfoRet.EquipInfo.Builder>();
        for(int i=0;i<count;++i)
        {
            arrayList.add(createEquitAlone(player, itemId));
        }
        return arrayList;
    }
}

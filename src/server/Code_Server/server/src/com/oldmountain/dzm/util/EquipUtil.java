package com.oldmountain.dzm.util;

import com.oldmountain.dzm.configEntity.DiamondConfig;
import com.oldmountain.dzm.entity.Equip;
import com.oldmountain.dzm.entity.EquipXmlTemplate;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.message.LeagueStructExt.OPLeaguaShopBuy;
import com.oldmountain.dzm.message.LeagueStructExt.OPLeaguaShopBuyRet;
import com.oldmountain.dzm.message.UserEquipInfo;
import com.oldmountain.dzm.message.UserEquipInfo.OPUserEquipInfoRet;
import net.sf.json.JSONArray;
import net.sf.json.JSONObject;

public class EquipUtil
{
	/**
	 * 根据装备的品质和类型来获取升级所需银两的基数
	 * @param int quality
	 * @param int type
	 * @param int level
	 * @return
	 */

	public static int getEquipUpgradeBase(int quality,int type,int level){
		float levelT = ((float)level)/3.0f;
		int base = XMLTemplateService.getEquipLevelBase((int)Math.ceil(levelT));
		int qualityBase = 1;
		switch (type) {
			case 1:
				switch (quality) {
					case 1:
						qualityBase = 90;
						break;
					case 2:
						qualityBase = 72;
						break;
					case 3:
						qualityBase = 54;
						break;
					case 4:
						qualityBase = 36;
						break;
				}
				break;
			case 2:
				switch (quality) {
					case 1:
						qualityBase = 48;
						break;
					case 2:
						qualityBase = 36;
						break;
					case 3:
						qualityBase = 24;
						break;
					case 4:
						qualityBase = 12;
						break;
				}	
				break;
			case 3:
				switch (quality) {
					case 1:
						qualityBase = 120;
						break;
					case 2:
						qualityBase = 96;
						break;
					case 3:
						qualityBase = 72;
						break;
					case 4:
						qualityBase = 48;
						break;
				}
				break;
		}
		
		return base * qualityBase;
	}
	
	/**
	 * 根据精炼材料的类型来获取使用时增加的经验值
	 * @param int itemId
	 * @return
	 */
	public static int getRefineMeterialXp(int itemId){
		int score = 0;
		//TODO 从xml中读取信息
		return score;
	}
	
	/**
	 * 根据精炼xp来获对应的等级
	 * @param xp
	 * @return
	 */
	public static int getRefineLevelByXp(int xp){
		int level = 1;
		//TODO 根据经验取等级
		return level;
	}
	
	public static OPUserEquipInfoRet.EquipInfo.Builder buildReturn(Equip equip){
		OPUserEquipInfoRet.EquipInfo.Builder builder = OPUserEquipInfoRet.EquipInfo.newBuilder();
		builder.setId(equip.getId());
		builder.setItemid(equip.getItemid());
		builder.setLevel(equip.getLevel());
		builder.setRefinelevel(equip.getRefinelevel());
		builder.setRefinexp(equip.getRefinexp());
		OPUserEquipInfoRet.EquipInfo.PropertyInfo.Builder infoBuilder = OPUserEquipInfoRet.EquipInfo.PropertyInfo.newBuilder();
		if (equip.getAttack() != 0) {
			infoBuilder.setType(1);
			infoBuilder.setNum(equip.getAttackToInt());
			builder.addPInfo(infoBuilder);
		}
		
		if (equip.getDefence() != 0) {
			infoBuilder.setType(2);
			infoBuilder.setNum(equip.getDefenceToInt());
			builder.addPInfo(infoBuilder);
		}
		
		if (equip.getHealth() != 0) {
			infoBuilder.setType(3);
			infoBuilder.setNum(equip.getHealthToInt());
			builder.addPInfo(infoBuilder);
		}
        //builder.setStoneinlaidinfo(equip.getDiamondInfo().replaceAll("%","_"));
        JSONArray _stoneArr=equip.getStoneJsonArr();
        for(int i=0;i<_stoneArr.size();++i)
        {
            builder.addStoneInfo(getStoneInfoByStoneJson(_stoneArr.getJSONObject(i),i+1));
        }
        builder.setBuffvalue(equip.getStoneAdditionValue());
		return builder;
	}
	
	public static OPLeaguaShopBuyRet.EquipInfo.Builder buildLeaguaEquipReturn(Equip equip){
		OPLeaguaShopBuyRet.EquipInfo.Builder builder = OPLeaguaShopBuyRet.EquipInfo.newBuilder();
		builder.setId(equip.getId());
		builder.setItemid(equip.getItemid());
		builder.setLevel(equip.getLevel());
		builder.setRefinelevel(equip.getRefinelevel());
		builder.setRefinexp(equip.getRefinexp());
		OPLeaguaShopBuyRet.EquipInfo.PropertyInfo.Builder infoBuilder = OPLeaguaShopBuyRet.EquipInfo.PropertyInfo.newBuilder();
		if (equip.getAttack() != 0) {
			infoBuilder.setType(1);
			infoBuilder.setNum(equip.getAttackToInt());
			builder.addPInfo(infoBuilder);
		}
		
		if (equip.getDefence() != 0) {
			infoBuilder.setType(2);
			infoBuilder.setNum(equip.getDefenceToInt());
			builder.addPInfo(infoBuilder);
		}
		
		if (equip.getHealth() != 0) {
			infoBuilder.setType(3);
			infoBuilder.setNum(equip.getHealthToInt());
			builder.addPInfo(infoBuilder);
		}
        //builder.setStoneinlaidinfo(equip.getDiamondInfo().replaceAll("%","_"));
        JSONArray _stoneArr=equip.getStoneJsonArr();
        for(int i=0;i<_stoneArr.size();++i)
        {
            builder.addStoneInfo(getStoneInfoByStoneJson(_stoneArr.getJSONObject(i),i+1));
        }
        builder.setBuffvalue(equip.getStoneAdditionValue());
		return builder;
	}
	
	public static Equip newEquip(Player player,int itemId){
		EquipXmlTemplate defaultEquip=XMLTemplateService.getEquipTemplate(itemId);
		Equip equip=new Equip();
		equip.setPlayerid(player.getId());
		equip.setItemid(defaultEquip.getId());
		equip.setType(defaultEquip.getType());
		defaultEquip.getQuality();
		switch (equip.getType()) {
		case 1:
			equip.setAttack(defaultEquip.getQuantity());
			break;
		case 2:
			equip.setDefence(defaultEquip.getQuantity());
			break;
		case 3:
			equip.setHealth(defaultEquip.getQuantity());
			break;

		default:
			break;
		}
        //modify by dylan at 20130710 because 宝石镶嵌改造
        if(DiamondConfig.DiamondOpen())
        {
            equip.generateDiamonInfo();
        }
		player.addOneEquip(equip);
		return equip;
	}

    public static int getStoneInfoByStoneJson(JSONObject _json,int position)
    {
        int stoneInfo=0;
        try
        {
            stoneInfo=(position<<29)+(_json.getInt(Equip.Diamon_Color)<<26)+(_json.getInt(Equip.Diamon_Stats)<<24)+_json.getInt(Equip.Diamon_Id);
        }
        catch (Exception e)
        {
            stoneInfo=0;
        }
        return stoneInfo;
    }
}

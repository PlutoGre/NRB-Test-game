package com.oldmountain.dzm.entity;

import com.oldmountain.dzm.configEntity.Diamond;
import com.oldmountain.dzm.configEntity.DiamondConfig;
import com.oldmountain.dzm.util.Util;
import com.oldmountain.dzm.util.XMLTemplateService;
import net.sf.json.JSONArray;
import net.sf.json.JSONObject;

import java.util.ArrayList;

public class Equip
{
    public static final String Diamon_Color="c";//钻石颜色
    public static final String Diamon_Stats="s";//钻石孔状态
    public static final String Diamon_Id="i";//钻石镶嵌id
    public static final String DiamonInfo="d";//钻石信息
    public static final String Diamon_Addition="a";//钻石附加属性
    public static final String Diamon_Addition_BuffeType="b";//钻石附加buffer类型
    public static final String Diamon_Addition_BuffeValue="v";//钻石附加buffer值
    public static final int Diamon_Hole_Stats_Lock=1;//孔锁定
    public static final int Diamon_Hole_Stats_UnInlay=2;//未镶嵌
    public static final int Diamon_Hole_Stats_Inlay=3;//已镶嵌
    public static final int Diamon_Hole_Color_Blue=1;//蓝色
    public static final int Diamon_Hole_Color_Red=2;//红色
    public static final int Diamon_Hole_Color_Yellow=3;//黄色
    public static final int Diamon_Hole_Color_Gray=4;//无解锁 无色孔
	private int id;
	private int playerid;
	private int itemid;
	private int type;
	private int level;
	private int refinexp;
	private int refinelevel;
	private float attack;
	private float defence;
	private float health;
	private int battleid;
	private int expiretime;
    private JSONObject diamondInfo;
	
	public Equip(){
		level = 1;
		refinelevel = 0;
		refinexp = 0;
		attack = 0;
		defence = 0;
		health = 0;
		battleid = 0;
		expiretime = 0;
        diamondInfo=new JSONObject();
	}
	
	public int getId(){
		return id;
	}
	
	public void setId(int id){
		this.id = id;
	}
	
	public int getPlayerid(){
		return playerid;
	}
	
	public void setPlayerid(int playerid){
		this.playerid = playerid;
	}
	
	public int getItemid(){
		return itemid;
	}
	
	public void setItemid(int itemid){
		this.itemid = itemid;
	}
	
	public int getType(){
		return type;
	}
	
	public void setType(int type){
		this.type = type;
	}
	
	public int getLevel(){
		return level;
	}
	
	public void setLevel(int level){
		this.level = level;
	}
	
	public int getRefinelevel(){
		return refinelevel;
	}
	
	public void setRefinelevel(int refinelevel){
		this.refinelevel = refinelevel;
	}
	
	public float getAttack(){
		return attack;
	}
	
	public int getAttackToInt(){
		return Math.round(attack);
	}
	
	public void setAttack(float attack){
		this.attack = attack;
	}
	
	public float getDefence(){
		return defence;
	}
	
	public int getDefenceToInt(){
		return Math.round(defence);
	}
	
	public void setDefence(float defence){
		this.defence = defence;
	}
	
	public float getHealth(){
		return health;
	}
	
	public int getHealthToInt(){
		return Math.round(health);
	}
	
	public void setHealth(float health){
		this.health = health;
	}
	
	public int getBattleid(){
		return battleid;
	}
	
	public void setBattleid(int id){
		this.battleid = id;
	}
	
	public int getRefinexp(){
		return refinexp;
	}
	
	public void setRefinexp(int xp){
		this.refinexp = xp;
	}
	
	public int getExpiretime(){
		return expiretime;
	}
	
	public void setExpiretime(int time){
		this.expiretime = time;
	}

    public JSONObject getDiamondInfoJson()
    {
        return diamondInfo;
    }

    public String getDiamondInfo()
    {
        return diamondInfo.toString();
    }

    public void setDiamondInfoFromJson(JSONObject diamondInfo)
    {
        this.diamondInfo = diamondInfo;
    }

    private void setDiamondInfo(String diamondInfo)
    {
        this.diamondInfo = JSONObject.fromObject(diamondInfo);
    }

    public JSONArray getStoneJsonArr()
    {
        if(this.diamondInfo.containsKey(DiamonInfo))
        {
            return this.diamondInfo.getJSONArray(DiamonInfo);
        }
        return new JSONArray();
    }

    public String getStoneAdditionValue()
    {
        String _addition="";
        if(this.diamondInfo.containsKey(Diamon_Addition)&&this.diamondInfo.getJSONObject(Diamon_Addition).containsKey(Diamon_Addition_BuffeValue))
        {
            _addition=this.diamondInfo.getJSONObject(Diamon_Addition).getString(Diamon_Addition_BuffeValue);
        }
        return _addition.replaceAll("%","_");
    }

    public int getStoneAdditionType()
    {
        int _type=getType();
        if(this.diamondInfo.containsKey(Diamon_Addition)&&this.diamondInfo.getJSONObject(Diamon_Addition).containsKey(Diamon_Addition_BuffeType))
        {
            _type=this.diamondInfo.getJSONObject(Diamon_Addition).getInt(Diamon_Addition_BuffeType);
        }
        return _type;
    }

    /**
     * 是否有宝石属性
     * @return
     */
    public boolean isHavDiamonInfo()
    {
        return !this.diamondInfo.isEmpty();
    }

    /**
     * 这个只有新装备或者在兼容时候才会被调用
     */
    public void generateDiamonInfo()
    {
        //钻石功能是否开启
        if(!DiamondConfig.DiamondOpen())
        {
            return;
        }
        //generate 钻石信息
        JSONArray _diamonArr=new JSONArray();
        int _diamonSize=getDiamonHoleCount();
        for(int i=0;i<_diamonSize;i++)
        {
            JSONObject _diamon=new JSONObject();
            _diamon.put(Diamon_Color,Diamon_Hole_Color_Gray);
            _diamon.put(Diamon_Stats,Diamon_Hole_Stats_Lock);
            _diamon.put(Diamon_Id,0);
            _diamonArr.add(_diamon);
        }
        this.diamondInfo.put(DiamonInfo,_diamonArr);

        //generate 附加属性
        JSONObject _addition=new JSONObject();
        _addition.put(Diamon_Addition_BuffeType,getType());
        EquipXmlTemplate equipInfo = XMLTemplateService.getEquipTemplate(itemid);
        String key=equipInfo.getQuality()+"_"+ equipInfo.getType();
        ArrayList<Integer > _weightArr= (ArrayList<Integer>) DiamondConfig.getWeightByType(key).clone();
        ArrayList<String > _attrAdditionArr= (ArrayList<String>) DiamondConfig.getAttAdditionByType(key).clone();
        try
        {
            _addition.put(Diamon_Addition_BuffeValue,_attrAdditionArr.get(Util.getRandomResultIndexFromInArray(_weightArr)));
        }
        catch(Exception e)
        {
            _addition.put(Diamon_Addition_BuffeValue,"10");
        }


        this.diamondInfo.put(Diamon_Addition,_addition);
    }

    /**
     * 获取孔的数量信息
     * @return
     */
    private int getDiamonHoleCount()
    {
        EquipXmlTemplate _equipXml= XMLTemplateService.getEquipTemplate(this.itemid);
        int _diamonSize=1;
        if(_equipXml!=null)
        {
            _diamonSize=5-_equipXml.getQuality();
        }
        return _diamonSize;
    }

    /**
     * 宝石信息被改变后计算装备属性值
     */
    public void calcEquipAttr(float equipQuantity, float growth,int level)
    {
        switch (getType())
        {
            case 1:
                setAttack(equipQuantity + (getLevel()-1) * growth);
                break;
            case 2:
                setDefence(equipQuantity + (getLevel()-1) * growth);
                break;
            case 3:
                setHealth(equipQuantity + (getLevel()-1) * growth);
                break;
        }
        if(level>= DiamondConfig.DiamondOpenLevel&&DiamondConfig.DiamondOpen())
        {
            int diamondInfoSize=this.diamondInfo.getJSONArray(DiamonInfo).size();
            boolean isHavAddition=true;
            int _diamonAddition=0;
            if(diamondInfoSize>0)
            {
                for(int i=0;i<diamondInfoSize;++i)
                {
                    JSONObject _diamond=this.diamondInfo.getJSONArray(DiamonInfo).getJSONObject(i);
                    //当前孔是否为已镶嵌
                    if(_diamond.containsKey(Diamon_Stats)&&_diamond.getInt(Diamon_Stats)==Diamon_Hole_Stats_Inlay)
                    {
                        int _holeColor=_diamond.containsKey(Diamon_Color)?_diamond.getInt(Diamon_Color):Diamon_Hole_Color_Gray;
                        //孔的颜色
                        if(_holeColor==Diamon_Hole_Color_Gray)
                        {
                            isHavAddition=false;
                        }
                        else
                        {
                            //镶嵌的宝石编号
                            int _diamondItemId=_diamond.containsKey(Diamon_Id)?_diamond.getInt(Diamon_Id):0;
                            if(_diamondItemId==0)
                            {
                                isHavAddition=false;
                            }
                            else
                            {
                                //宝石信息
                                Diamond _diamondInfo=XMLTemplateService.getDiamondByItemId(_diamondItemId);
                                if(_diamondInfo==null)
                                {
                                    isHavAddition=false;
                                }
                                else
                                {
                                    //宝石与孔颜色不符
                                    if(_holeColor!=_diamondInfo.getColorType())
                                    {
                                        isHavAddition=false;
                                    }
                                    //宝石属性加成
                                    switch (getType())
                                    {
                                        case 1:
                                            _diamonAddition+=_diamondInfo.getWeanponAttr();
                                            break;
                                        case 2:
                                            _diamonAddition+=_diamondInfo.getArmorAttr();
                                            break;
                                        case 3:
                                            _diamonAddition+=_diamondInfo.getJewelryAttr();
                                            break;
                                    }
                                }
                            }
                        }
                    }
                    else
                    {
                        isHavAddition=false;
                    }
                }
            }
            //当前孔全部激活并且激活附加属性
            if(isHavAddition&&diamondInfoSize==getDiamonHoleCount())
            {
                 String _addition="";
                 if(this.diamondInfo.containsKey(Diamon_Addition)&&this.diamondInfo.getJSONObject(Diamon_Addition).containsKey(Diamon_Addition_BuffeValue))
                 {
                     _addition=this.diamondInfo.getJSONObject(Diamon_Addition).getString(Diamon_Addition_BuffeValue);
                 }
                 //附加属性为百分比
                 if(_addition.contains("%"))
                 {
                     _addition=_addition.replaceAll("%","");
                     try
                     {
                         float _additionValue=Float.parseFloat(_addition);
                         switch (getType())
                         {
                             case 1:
                                 setAttack(getAttack()+_diamonAddition*(1+_additionValue/100));
                                 break;
                             case 2:
                                 setDefence(getDefence()+_diamonAddition*(1+_additionValue/100));
                                 break;
                             case 3:
                                 setHealth(getHealth()+_diamonAddition*(1+_additionValue/100));
                                 break;
                         }
                     }
                     catch (Exception e)
                     {
                         switch (getType())
                         {
                             case 1:
                                 setAttack(getAttack()+_diamonAddition);
                                 break;
                             case 2:
                                 setDefence(getDefence()+_diamonAddition);
                                 break;
                             case 3:
                                 setHealth(getHealth()+_diamonAddition);
                                 break;
                         }
                     }
                 }
                else
                {
                    try
                    {
                        float _additionValue=Float.parseFloat(_addition);
                        switch (getType())
                        {
                            case 1:
                                setAttack(getAttack()+_diamonAddition+_additionValue);
                                break;
                            case 2:
                                setDefence(getDefence()+_diamonAddition+_additionValue);
                                break;
                            case 3:
                                setHealth(getHealth()+_diamonAddition+_additionValue);
                                break;
                        }
                    }
                    catch (Exception e)
                    {
                        switch (getType())
                        {
                            case 1:
                                setAttack(getAttack()+_diamonAddition);
                                break;
                            case 2:
                                setDefence(getDefence()+_diamonAddition);
                                break;
                            case 3:
                                setHealth(getHealth()+_diamonAddition);
                                break;
                        }
                    }
                }
            }
            else
            {
                switch (getType())
                {
                    case 1:
                        setAttack(getAttack()+_diamonAddition);
                        break;
                    case 2:
                        setDefence(getDefence()+_diamonAddition);
                        break;
                    case 3:
                        setHealth(getHealth()+_diamonAddition);
                        break;
                }
            }
        }

    }

    /**
     * 更改宝石孔信息或者镶嵌
     * @param index
     * @param stat
     * @param id
     * @return
     */
    public boolean inlayOrRemoveDiamonInfo(int index, int stat, int id)
    {
        boolean  _isChange=false;
        if(index<=0||index>4) return  _isChange;
        if(index<=this.diamondInfo.getJSONArray(DiamonInfo).size())
        {
            --index;//转化为真实的索引
            JSONObject _diamon=this.diamondInfo.getJSONArray(DiamonInfo).getJSONObject(index);
            //开孔
            if(_diamon.containsKey(Diamon_Stats)&&_diamon.getInt(Diamon_Stats)==Diamon_Hole_Stats_Lock)
            {
                _diamon.put(Diamon_Color,(int)(Math.random()*3)+1);
                _isChange=true;
            }
            if(_diamon.getInt(Diamon_Stats)!=stat)
            {
                _diamon.put(Diamon_Stats,stat);
                _isChange=true;
            }
            if(_diamon.getInt(Diamon_Id)!=id)
            {
                _diamon.put(Diamon_Id,id);
                _isChange=true;
            }
            if(_isChange)
            {
                this.diamondInfo.getJSONArray(DiamonInfo).set(index,_diamon);
            }

        }
        else
        {
            int _diamonSize=getDiamonHoleCount();
            if(index<_diamonSize)
            {
                _isChange=true;
                JSONObject _diamon=new JSONObject();
                _diamon.put(Diamon_Color,(int)(Math.random()*3)+1);
                _diamon.put(Diamon_Stats,stat);
                _diamon.put(Diamon_Id,0);
                this.diamondInfo.getJSONArray(DiamonInfo).add(_diamon);
            }
        }
       return _isChange;
    }

    /**
     * 根据孔的位置或移除镶嵌
     * @param index
     * @return
     */
    public boolean openOrRemoveDiamonHoleByIndex(int index)
    {
        return inlayOrRemoveDiamonInfo(index,Diamon_Hole_Stats_UnInlay,0);
    }

    /**
     * 校验当前位置是否可以开孔
     * @param index
     * @return
     */
    public boolean checkOpenDiamondHoleByIndex(int index)
    {
        if(index<=0||index>4) return false;
        if(index<=this.diamondInfo.getJSONArray(DiamonInfo).size())
        {//孔信息已经生成
            //判断当前孔是否锁定
            --index;
            JSONObject _diamon=this.diamondInfo.getJSONArray(DiamonInfo).getJSONObject(index);
            if(_diamon.getInt(Diamon_Stats)!=Diamon_Hole_Stats_Lock)
            {
                return false;
            }
            //判断上一个孔是否锁定
            if(index>0)
            {
                --index;
                _diamon=this.diamondInfo.getJSONArray(DiamonInfo).getJSONObject(index);
                if(_diamon.getInt(Diamon_Stats)==Diamon_Hole_Stats_Lock)
                {
                    return false;
                }
            }

        }
        else
        {//当前孔信息未生成
            if(index>1)
            {//判断上一个孔是否锁定
                if(index!=this.diamondInfo.getJSONArray(DiamonInfo).size())
                {//非第一空并且上一个孔状态不明确时不允许开孔
                    return false;
                }
                --index;
                JSONObject _diamon=this.diamondInfo.getJSONArray(DiamonInfo).getJSONObject(index);
                if(_diamon.getInt(Diamon_Stats)!=Diamon_Hole_Stats_Lock)
                {
                    return false;
                }
                ++index;
            }

            int _diamonSize=getDiamonHoleCount();
            if(index<_diamonSize)
            {
                JSONObject _diamon=new JSONObject();
                _diamon.put(Diamon_Color,Diamon_Hole_Color_Gray);
                _diamon.put(Diamon_Stats,Diamon_Hole_Stats_Lock);
                _diamon.put(Diamon_Id,0);
                this.diamondInfo.getJSONArray(DiamonInfo).add(_diamon);
            }
            else
            {
                return false;
            }
        }
        return true;
    }

    /**
     * 宝石移除校验孔信息和状态
     * @param index
     * @return
     */
    private boolean canRemoveStoneByIndex(int index)
    {
        if(index<=0||index>4) return false;
        if(index<=this.diamondInfo.getJSONArray(DiamonInfo).size())
        {//存在孔信息
            //判断当前孔是否锁定
            --index;
            JSONObject _diamon=this.diamondInfo.getJSONArray(DiamonInfo).getJSONObject(index);
            return _diamon.containsKey(Diamon_Stats) && _diamon.getInt(Diamon_Stats) == Diamon_Hole_Stats_Inlay && _diamon.getInt(Diamon_Id) != 0;
        }
        return false;
    }

    /**
     * 拆除宝石时获取镶嵌宝石编号
     * @param index
     * @return
     */
    public int getInlayDiamonIdByIndex(int index)
    {
       if(!canRemoveStoneByIndex(index))
       {
           return 0;
       }
       JSONObject _diamon=this.diamondInfo.getJSONArray(DiamonInfo).getJSONObject(--index);
       return _diamon.getInt(Diamon_Id);
    }

    public boolean canInlayStoneByIndex(int index)
    {
        if(index<=0||index>4) return false;
        if(index<=this.diamondInfo.getJSONArray(DiamonInfo).size())
        {//存在孔信息
            //判断当前孔是否锁定
            --index;
            JSONObject _diamon=this.diamondInfo.getJSONArray(DiamonInfo).getJSONObject(index);
            return _diamon.containsKey(Diamon_Stats) && _diamon.getInt(Diamon_Stats) == Diamon_Hole_Stats_UnInlay&& _diamon.getInt(Diamon_Id) == 0;
        }
        return false;
    }
}

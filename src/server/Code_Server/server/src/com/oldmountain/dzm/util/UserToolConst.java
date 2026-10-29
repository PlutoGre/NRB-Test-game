package com.oldmountain.dzm.util;

import java.util.HashMap;

import javassist.bytecode.StackMapTable.Walker;

/**
 * Created with IntelliJ IDEA.
 * User: Administrator
 * Date: 13-4-1
 * Time: 上午11:12
 * To change this template use File | Settings | File Templates.
 */
public class UserToolConst {


    //奖励类型对应ID
    public static final int USER_PROPERTY_TYPE = 11;
    public static final int USER_PROPERTY = 11001;
    public static final int USER_PROPERTY_GOLD_COINS = 1001;
    public static final int USER_PROPERTY_SILVER_COINS = 1002;
    public static final int USER_PROPERTY_EXP = 1003;
    public static final int USER_LEVEL_PROPERTY = 12001;
    public static final int USER_LEVEL_PROPERTY_GOLD_COINS = 2001;
    public static final int USER_LEVEL_PROPERTY_SILVER_COINS = 2002;
    public static final int USER_LEVEL_PROPERTY_EXP = 2003;
    public static final int DISCIPLE_PROPERTY=21001;
    public static final int DISCIPLE_TYPE=31;
    public static final int DISCIPLE=31000;
    public static final int A_DISCIPLE=31001;
    public static final int B_DISCIPLE=31002;
    public static final int C_DISCIPLE=31003;
    public static final int D_DISCIPLE=31004;
    public static final int SOUL_TYPE=32;
    public static final int SOUL=32000;
    public static final int A_SOUL=32001;
    public static final int B_SOUL=32002;
    public static final int C_SOUL=32003;
    public static final int D_SOUL=32004;
    public static final int SKILL_TYPE=41;
    public static final int SKILL=41000;
    public static final int A_SKILL=41001;
    public static final int B_SKILL=41002;
    public static final int C_SKILL=41003;
    public static final int D_SKILL=41004;
    public static final int SKILL_PIECE_TYPE=42;
    public static final int SKILL_PIECE=42000;
    public static final int A_SKILL_PIECE=42001;
    public static final int B_SKILL_PIECE=42002;
    public static final int C_SKILL_PIECE=42003;
    public static final int D_SKILL_PIECE=42004;
    public static final int EQUIP_TYPE=50;
    public static final int EQUIP=51000;
    public static final int WEAPON=52000;
    public static final int ARMOR=53000;
    public static final int ACCESSORIES=50000;
    public static final int A_WEAPON=51001;
    public static final int B_WEAPON=51002;
    public static final int C_WEAPON=51003;
    public static final int D_WEAPON=51004;
    public static final int A_ARMOR=52001;
    public static final int B_ARMOR=52002;
    public static final int C_ARMOR=52003;
    public static final int D_ARMOR=52004;
    public static final int A_ACCESSORIES=53001;
    public static final int B_ACCESSORIES=53002;
    public static final int C_ACCESSORIES=53003;
    public static final int D_ACCESSORIES=53004;
    public static final int TOOLS_TYPE=61;
    public static final int USER_TOOLS=61001;
    public static final int USER_INTERFACE=71001;

    public static final int A_QUALITY=1;
    public static final int B_QUALITY=2;
    public static final int C_QUALITY=3;
    public static final int D_QUALITY=4;


    //道具
    public static final int OTHER_TOOL=11;
    public static final int UPGRADE_DISCIPLE_TOOL=15;
    public static final int UPGRADE_EQUIPMENT_TOOL=20;
    public static final int RECOVERY_TOOL=25;
    public static final int TREASURE_TOOL=30;
    public static final int GIFT_BOX_TOOL=35;
    public static final int DISCIPLE_EXP=40;
    public static final int LANGMUJIU=1501001;
    public static final int PUTAOJIU=1501002;
    public static final int HAIXIANFAN=2501001;
    public static final int JISUZHEN= 2502001;
    public static final int TIAOZHANSHU= 2503001;
    public static final int WOOD_TREASURE=3001001;
    public static final int COPPER_TREASURE=3001002;
    public static final int SILVER_TREASURE=3001003;
    public static final int GOLD_TREASURE=3001004;
    public static final int WOOD_KEY=3002001;
    public static final int COPPER_KEY=3002002;
    public static final int SILVER_KEY=3002003;
    public static final int GOLD_KEY=3002004;
    public static final int WOOD_TREASURE_WORTH=0;
    public static final int COPPER_TREASURE_WORTH=0;
    public static final int SILVER_TREASURE_WORTH=5;
    public static final int GOLD_TREASURE_WORTH=10;
    public static final int WOOD_KEY_WORTH=5;
    public static final int COPPER_KEY_WORTH=10;
    public static final int SILVER_KEY_WORTH=15;
    public static final int GOLD_KEY_WORTH=20;
    public static final int GIFT_BOX_1=3501001;
    public static final int GIFT_BOX_2=3501002;
    public static final int GIFT_BOX_3=3501003;
    public static final int GIFT_BOX_4=3501004;
    public static final int GIFT_BOX_5=3501005;
    public static final int GIFT_BOX_6=3501006;
    public static final int GIFT_BOX_7=3501007;
    public static final int GIFT_BOX_8=3501008;
    public static final int GIFT_BOX_9=3501009;
    public static final int GIFT_BOX_10=3501010;
    public static final int GIFT_BOX_11=3501011;
    public static final int GIFT_BOX_12=3501012;
    public static final int GIFT_BOX_13=3501013;
    public static final int GIFT_BOX_PROMOTE=3502001;
    public static final int GIFT_BOX_NEW_PLAYER=3503001;
    public static final int GIFT_BOX_FIRST_PAY=3504001;
    public static final int GIFT_BOX_HANCOCK=3507001;


    public static HashMap<Integer,Integer> treasureMap = new HashMap<Integer,Integer>();
    public static HashMap<Integer,Integer> vipLvToolLimit = new HashMap<Integer,Integer>();
    private static HashMap<Integer,Integer> TypeMap=new HashMap<Integer, Integer>();
    public static HashMap<Integer,Integer> treasureWorthMap=new HashMap<Integer, Integer>();
    static
    {
        treasureMap.put(WOOD_TREASURE,WOOD_KEY);
        treasureMap.put(COPPER_TREASURE,COPPER_KEY);
        treasureMap.put(SILVER_TREASURE,SILVER_KEY);
        treasureMap.put(GOLD_TREASURE,GOLD_KEY);
        treasureMap.put(WOOD_KEY,WOOD_TREASURE);
        treasureMap.put(COPPER_KEY,COPPER_TREASURE);
        treasureMap.put(SILVER_KEY,SILVER_TREASURE);
        treasureMap.put(GOLD_KEY,GOLD_TREASURE);

        treasureWorthMap.put(WOOD_TREASURE,WOOD_TREASURE_WORTH);
        treasureWorthMap.put(COPPER_TREASURE,COPPER_TREASURE_WORTH);
        treasureWorthMap.put(SILVER_TREASURE,SILVER_TREASURE_WORTH);
        treasureWorthMap.put(GOLD_TREASURE,GOLD_TREASURE_WORTH);
        treasureWorthMap.put(WOOD_KEY,WOOD_KEY_WORTH);
        treasureWorthMap.put(COPPER_KEY,COPPER_KEY_WORTH);
        treasureWorthMap.put(SILVER_KEY,SILVER_KEY_WORTH);
        treasureWorthMap.put(GOLD_KEY,GOLD_KEY_WORTH);
        vipLvToolLimit.put(0,1);
        vipLvToolLimit.put(1,1);
        vipLvToolLimit.put(2,2);
        vipLvToolLimit.put(3,3);
        vipLvToolLimit.put(4,3);
        vipLvToolLimit.put(5,5);
        vipLvToolLimit.put(6,5);
        vipLvToolLimit.put(7,15);
        vipLvToolLimit.put(8,15);
        vipLvToolLimit.put(9,25);
        vipLvToolLimit.put(10,25);
        vipLvToolLimit.put(11,25);
        vipLvToolLimit.put(12,25);
        vipLvToolLimit.put(13,25);
        TypeMap.put(USER_PROPERTY_TYPE,USER_PROPERTY_TYPE);
        TypeMap.put(USER_PROPERTY,USER_PROPERTY_TYPE);
        TypeMap.put(USER_PROPERTY_GOLD_COINS,USER_PROPERTY_TYPE);
        TypeMap.put(USER_PROPERTY_SILVER_COINS,USER_PROPERTY_TYPE);
        TypeMap.put(USER_PROPERTY_EXP,USER_PROPERTY_TYPE);
        TypeMap.put(USER_LEVEL_PROPERTY_GOLD_COINS,USER_LEVEL_PROPERTY);
        TypeMap.put(USER_LEVEL_PROPERTY_SILVER_COINS,USER_LEVEL_PROPERTY);
        TypeMap.put(USER_LEVEL_PROPERTY_EXP,USER_LEVEL_PROPERTY);
        TypeMap.put(DISCIPLE_PROPERTY,DISCIPLE_PROPERTY);
        TypeMap.put(DISCIPLE_TYPE,DISCIPLE_TYPE);
        TypeMap.put(A_DISCIPLE,DISCIPLE_TYPE);
        TypeMap.put(B_DISCIPLE,DISCIPLE_TYPE);
        TypeMap.put(C_DISCIPLE,DISCIPLE_TYPE);
        TypeMap.put(D_DISCIPLE,DISCIPLE_TYPE);
        TypeMap.put(SOUL_TYPE,SOUL_TYPE);
        TypeMap.put(A_SOUL,SOUL_TYPE);
        TypeMap.put(B_SOUL,SOUL_TYPE);
        TypeMap.put(C_SOUL,SOUL_TYPE);
        TypeMap.put(D_SOUL,SOUL_TYPE);
        TypeMap.put(SKILL_TYPE,SKILL_TYPE);
        TypeMap.put(A_SKILL,SKILL_TYPE);
        TypeMap.put(B_SKILL,SKILL_TYPE);
        TypeMap.put(C_SKILL,SKILL_TYPE);
        TypeMap.put(D_SKILL,SKILL_TYPE);
        TypeMap.put(SKILL_PIECE_TYPE,SKILL_PIECE_TYPE);
        TypeMap.put(A_SKILL_PIECE,SKILL_PIECE_TYPE);
        TypeMap.put(B_SKILL_PIECE,SKILL_PIECE_TYPE);
        TypeMap.put(C_SKILL_PIECE,SKILL_PIECE_TYPE);
        TypeMap.put(D_SKILL_PIECE,SKILL_PIECE_TYPE);
        TypeMap.put(A_WEAPON,EQUIP_TYPE);
        TypeMap.put(B_WEAPON,EQUIP_TYPE);
        TypeMap.put(C_WEAPON,EQUIP_TYPE);
        TypeMap.put(D_WEAPON,EQUIP_TYPE);
        TypeMap.put(A_ARMOR,EQUIP_TYPE);
        TypeMap.put(B_ARMOR,EQUIP_TYPE);
        TypeMap.put(C_ARMOR,EQUIP_TYPE);
        TypeMap.put(D_ARMOR,EQUIP_TYPE);
        TypeMap.put(A_ACCESSORIES,EQUIP_TYPE);
        TypeMap.put(B_ACCESSORIES,EQUIP_TYPE);
        TypeMap.put(C_ACCESSORIES,EQUIP_TYPE);
        TypeMap.put(D_ACCESSORIES,EQUIP_TYPE);
        TypeMap.put(USER_TOOLS,TOOLS_TYPE);

        TypeMap.put(WOOD_TREASURE,TREASURE_TOOL);
        TypeMap.put(COPPER_TREASURE,TREASURE_TOOL);
        TypeMap.put(SILVER_TREASURE,TREASURE_TOOL);
        TypeMap.put(GOLD_TREASURE,TREASURE_TOOL);
        TypeMap.put(WOOD_KEY,TREASURE_TOOL);
        TypeMap.put(COPPER_KEY,TREASURE_TOOL);
        TypeMap.put(SILVER_KEY,TREASURE_TOOL);
        TypeMap.put(GOLD_KEY,TREASURE_TOOL);

        TypeMap.put(HAIXIANFAN,RECOVERY_TOOL);
        TypeMap.put(JISUZHEN,RECOVERY_TOOL);
        TypeMap.put(TIAOZHANSHU,RECOVERY_TOOL);

        TypeMap.put(GIFT_BOX_1,GIFT_BOX_TOOL);
        TypeMap.put(GIFT_BOX_2,GIFT_BOX_TOOL);
        TypeMap.put(GIFT_BOX_3,GIFT_BOX_TOOL);
        TypeMap.put(GIFT_BOX_4,GIFT_BOX_TOOL);
        TypeMap.put(GIFT_BOX_5,GIFT_BOX_TOOL);
        TypeMap.put(GIFT_BOX_6,GIFT_BOX_TOOL);
        TypeMap.put(GIFT_BOX_7,GIFT_BOX_TOOL);
        TypeMap.put(GIFT_BOX_8,GIFT_BOX_TOOL);
        TypeMap.put(GIFT_BOX_9,GIFT_BOX_TOOL);
        TypeMap.put(GIFT_BOX_10,GIFT_BOX_TOOL);
        TypeMap.put(GIFT_BOX_11,GIFT_BOX_TOOL);
        TypeMap.put(GIFT_BOX_12,GIFT_BOX_TOOL);
        TypeMap.put(GIFT_BOX_13,GIFT_BOX_TOOL);
        TypeMap.put(GIFT_BOX_PROMOTE,GIFT_BOX_TOOL);
        TypeMap.put(GIFT_BOX_NEW_PLAYER,GIFT_BOX_TOOL);
        TypeMap.put(GIFT_BOX_FIRST_PAY,GIFT_BOX_TOOL);
        TypeMap.put(PUTAOJIU, DISCIPLE_EXP);
        TypeMap.put(LANGMUJIU, DISCIPLE_EXP);
		TypeMap.put(DISCIPLE, DISCIPLE_TYPE);
        TypeMap.put(SOUL, SOUL_TYPE);
        TypeMap.put(SKILL, SKILL_TYPE);
        TypeMap.put(EQUIP, EQUIP_TYPE);
        TypeMap.put(WEAPON, EQUIP_TYPE);
        TypeMap.put(ARMOR, EQUIP_TYPE);
        TypeMap.put(ACCESSORIES, EQUIP_TYPE);
        TypeMap.put(SKILL_PIECE, SKILL_PIECE_TYPE);
        TypeMap.put(GIFT_BOX_HANCOCK, GIFT_BOX_TOOL);
    }

    public static int getType(int type)
    {
        int _type=-1;
        try
        {
            _type=TypeMap.get(type);
        }
        catch (Exception e)
        {
             System.out.println("奖励类型 "+type+" 不存在！");
        }
        return _type;
    }
}

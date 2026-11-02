package com.oldmountain.dzm.util;

import com.oldmountain.dzm.configEntity.FightEndNpc;
import com.oldmountain.dzm.message.FightEndInfo.OPFightEndInfoRet;
import net.sf.json.JSONArray;
import net.sf.json.JSONObject;

/*
 * Created with IntelliJ IDEA.
 * User: dylan
 * Date: 13-4-2
 * Time: 下午8:02
 * To change this template use File | Settings | File Templates.
 */
public class AdventureFightEndUtil
{
    //每日可挑战血战的最大次数
    public static final int DAILY_BARRIER_COUNT=3;

    private static AdventureFightEndUtil _instance = null;
    //奖励关卡重置间隔
    public static int resetObatainStarBarrier=5;
    //加成关卡间隔
    public static int barrierAdditionSel=3;
    //
    private JSONObject barrierIndexNpcObj=new JSONObject();
    //关卡生成钥匙比率
    private final int keyRate=25;
    //关卡道具中钥匙id列表
    private final int[] keyIdArr=new int[]{3002001,3002002,3002003,3002004};
    //属性加成数组
    private final int[] additionArr=new int[]{OPFightEndInfoRet.Addition_Type.HP_VALUE,OPFightEndInfoRet.Addition_Type.ATTATCK_VALUE,OPFightEndInfoRet.Addition_Type.DEFENSE_VALUE,OPFightEndInfoRet.Addition_Type.WILL_VALUE};
    //同步时间间隔
    public static int syncTime=120;

    public static final int MIN_BATTLE_COUNT=5;

    private AdventureFightEndUtil()
    {
    }

    public synchronized static AdventureFightEndUtil getInstance()
    {
        if (_instance == null)
        {
            _instance = new AdventureFightEndUtil();
        }
        return _instance;
    }

    /**
     * 获取下次同步数据时间
     * @return
     */
    public int getNextSyncTime()
    {
        return Util.getServerTime()+syncTime;
    }

    /**
     * 下一次重置阶段关卡等级
     * @param currBarrier
     * @return
     */
    public int getNextResetStepBarrier(int currBarrier)
    {
      int next=-1;
      if((currBarrier-1)%resetObatainStarBarrier==0)
      {
          next=(currBarrier-1)+resetObatainStarBarrier;
      }
      else
      {
          next=resetObatainStarBarrier-(currBarrier-1)%resetObatainStarBarrier+(currBarrier-1);
      }
      return next;
    }

    /**
     * 获取下次属性加成关卡等级
     * @param barrier
     * @return
     */
    public int getNextAdditionAddSelBarrier(int barrier)
    {
        if((barrier-1)%barrierAdditionSel==0)
        {
            return (barrier-1)+barrierAdditionSel;
        }
        else
        {
            return barrierAdditionSel-(barrier-1)%barrierAdditionSel+(barrier-1);
        }
    }

    /**
     * 根据当前属性加成索引获得加成比率
     * @param index
     * @return
     */
    public static int getAdditionRateByIndex(int index)
    {
        if(index==1)
        {
            return 3;
        }
        else if(index==2)
        {
            return 15;
        }
        else if(index==3)
        {
            return 30;
        }
        return 0;
    }

    /**
     * 根据属性加成的索引扣除得星数量
     * @param index
     * @return
     */
    public static int getDeducteStarByIndex(int index)
    {
        if(index==1)
        {
            return 3;
        }
        else if(index==2)
        {
            return 15;
        }
        else if(index==3)
        {
            return 30;
        }
        return 0;
    }

    public static int getYestAdditionRateByStar(int _star)
    {
        return (int)Math.floor(_star/4);
    }

    /**
     * npc组编号对应的阵容数量以及血战位置反查索引
     * @param obj
     */
    public void setBarrierIndexNpcObj(JSONObject obj)
    {
        barrierIndexNpcObj=obj;
    }

    /**
     * 通过当前关卡以及阵容获得单星得银币数
     * @param barrier
     * @param battle
     * @return
     */
    public int getSilverByBarrierAndBattle(int barrier,int battle)
    {
        int _silver=0;
        try
        {
            _silver=XMLTemplateService.getFightEndGeneralByBarrier(barrier).getBattlerSilverByBattle(battle);
        }
        catch (Exception e)
        {

        }
        return _silver;
    }

    /**
     * 获取当前关卡超出的评分所得的额外奖励
     * @param barrier
     * @param start
     * @param end
     * @return
     */
    public JSONArray getExtraRewardByBarrierStar(int barrier,int start,int end)
    {
        JSONArray ja=new JSONArray();
        int max=45;
        while (start<end&&start<max)
        {
            if(start>=15)
            {
                start+=15;
            }
            else
            {
                if(start>=5)
                {
                    start+=10;
                }
                else
                {
                    start+=5;
                }
            }
            JSONArray temp=XMLTemplateService.getFightEndGeneralByBarrier(barrier).getExtraBattleReward(start);
            for(int i=0;i<temp.size();i++)
            {
                ja.add(temp.getJSONObject(i));
            }
        }
        return ja;
    }

    /**
     * 获取血战NPC数据信息通过NPC配置表id
     * @param id
     * @return
     */
    public FightEndNpc getFightNpcConfigByNpcId(int id)
    {
        return XMLTemplateService.getFightEndNpcById(id);
    }

    /**
     * 根据阵容和位置，生成NPC编号
     * @param battle
     * @param pos
     * @return
     */
    public int generateNpcIdByBattleAndPos(int battle,int pos)
    {
        JSONArray npcArr=this.barrierIndexNpcObj.getJSONObject(String.valueOf(battle)).getJSONArray(String.valueOf(pos));
        int randomMax=npcArr.size();
        int id=-1;
        try
        {
            id=npcArr.getInt((int)(Math.random()*randomMax));
        }
        catch (Exception e)
        {
            System.out.println("generateNpcIdByBattleAndPos:"+e.getMessage());
        }
        return id;
    }

    /**
     *根据我方阵容数量和位置生成我方、地方弟子数量以及npc编号列表
     * @param battle
     * @param pos
     * @return
     */
    public int[] generateMemberCountByBattleAndPos(int battle,int pos)
    {
        int[] ret=new int[2];
        ret[0]=(int)(Math.random()*battle+1);
        //如果为位置1
        if(pos==1)
        {
             //我方人数为阵容最大数 ，则地方与我方相同
             if(ret[0]==battle)
             {
                 ret[1]=ret[0];
             }
             else
             {//否则敌方人数在我方人数和我方+1中随机
                 ret[1]=(int)(Math.random()*2+ret[0]);
             }
             return ret;
        }
        else if(pos==2)
        {//如果为位置2
            ret[1]=ret[0];
            return ret;
        }
        else
        {//位置三
            if(ret[0]>1)
            {//敌方人数在X-1~X中随机
                ret[1]=(int)(Math.random()*2+ret[0]-1);
            }
            else
            {
                ret[1]=1;
            }
            return ret;
        }
    }

    /**
     * 通过NPC编号和对方数量返回对方npcid数组
     * @param npcId
     * @param num
     * @return
     */
    public String[] getOpponentMemberId(int npcId,int num)
    {
        return getFightNpcConfigByNpcId(npcId).getNpcByLimit(num);
    }

    /**
     *生成关卡钥匙
     * @return
     */
    public int generateToolKeyId()
    {
        int random=(int)(Math.random()*100+1);
        boolean  isHavKey=false;
        int keyId=0;
        if(random<=keyRate)
        {
            isHavKey=true;
        }
        if(isHavKey)
        {
             keyId=keyIdArr[(int)(Math.random()*keyIdArr.length)];
        }
        return keyId;
    }

    /**
     * 从加成列表中生成三项加成信息
     * @return
     */
    public int[] generateAdditionArr()
    {
        int source[]=additionArr.clone();
        int N=source.length;
        int K=3;
        int _ret[]=new int[K];
        for(int i=0;i<K;++i)
        {
            int j=i+(int)(Math.random()*(N-i));
            int temp=source[i];
            source[i]=source[j];
            source[j]=temp;
            _ret[i]=source[i];
        }
        return _ret;
    }

    /**
     * 获得根据当前最多获取额外加成编号
     * @param star
     * @return
     */
    public int getExtraStarIdByStar(int star)
    {
        int _id=5;
        if(star>=30)
        {
            _id=30;
            if(star>=45)
            {
                _id=45;
            }
        }
        else
        {
            if(star>=15)
            {
                _id=15;
            }
            else
            {
                if(star>=5)
                {
                    _id=5;
                }
            }
        }
        return _id;
    }

    /**
     * 根据战斗的轮数和挑战位置得到得星数量
     * @param round
     * @param index
     * @return
     */
    public int getObtainStarByBattleRoundAndIndex(int round,int index)
    {
        return (4-round)*(4-index);
    }

}

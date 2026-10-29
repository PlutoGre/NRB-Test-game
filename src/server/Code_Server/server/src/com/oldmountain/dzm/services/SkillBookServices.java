package com.oldmountain.dzm.services;

import com.oldmountain.dzm.configEntity.SkillBookConfig;
import com.oldmountain.dzm.configEntity.SkillBookNpcConfig;
import com.oldmountain.dzm.configEntity.SkillBookNpcStats;
import com.oldmountain.dzm.configEntity.SkillBookSelecter;
import com.oldmountain.dzm.entity.*;
import com.oldmountain.dzm.message.GetSkillBookLoot;
import com.oldmountain.dzm.net.ObjectAccessor;
import com.oldmountain.dzm.util.BattleUtil;
import com.oldmountain.dzm.util.UserToolConst;
import com.oldmountain.dzm.util.Util;
import com.oldmountain.dzm.util.XMLTemplateService;
import net.sf.json.JSONObject;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.util.*;
import java.util.concurrent.ConcurrentHashMap;

/*
 * Created with IntelliJ IDEA.
 * User: dylan
 * Date: 13-4-2
 * Time: 下午8:56
 * To change this template use File | Settings | File Templates.
 */
public class SkillBookServices
{
    public static final ConcurrentHashMap<Integer, ArrayList<Integer>> SkillBookPlayerMap = new ConcurrentHashMap<Integer, ArrayList<Integer>>(5000);
    private static SkillBookNpcConfig skillBookNpcConfig=new SkillBookNpcConfig();
    private static ArrayList<Integer> NpcIdList=new ArrayList<Integer>();
    private static int SkillBookLootSize=3;
    private static final byte[] lock = new byte[0];
    public static final int activitySelecterId=999;
    private static final Logger logger = LoggerFactory.getLogger("SkillBookInfo");
    /**
     * 玩家加载后初始化残章信息到内存中，并初始化持有该残章的玩家编号
     * @param player
     */
    public static void loadSkillBook(Player player)
    {
        long startTime=Util.getServerMillTime();
        logger.info("start************#playerId:"+player.getId()+",startTime:"+startTime+"*********start");
        try
        {
         ArrayList<SkillBook> list=(ArrayList<SkillBook>) DaoManager.getSkillBookDao().getSkillBook(player);
         for(SkillBook _item:list)
         {
             //将残章id数量大于0的放入持有残章玩家列表
             JSONObject skillBookDetail=_item.getSkillBookDetailForJson();
             if(_item.getPieceChance()>0&&skillBookDetail.size()>0)
             {
                 for (Iterator iter = skillBookDetail.keys();iter.hasNext();)
                 {
                     String key = (String)iter.next();
                     int count=skillBookDetail.getInt(key);
                     if(count>0)
                     {
                         addPlayerIdToMap(player.getId(),Integer.parseInt(key));
                     }
                 }
             }
             player.getSkillBooks().put(_item.getSkillId(),_item);
         }
        }
        catch (Exception e)
        {
            System.out.println("loadSkillBook Exception:"+e.getMessage());
        }
        long endTime=Util.getServerMillTime();
        logger.info("end************playerId:"+player.getId()+",endTime:"+endTime+",cost:"+(endTime-startTime)+"*********end");
    }

    /**
     * 抢夺成功后增加玩家残章的数量,并添加持有残章id到内存
     * @param player
     * @param bookId
     */
    public static void obtainSkillBook(Player player,int bookId)
    {
        int skillId=XMLTemplateService.getSkillIdBySkillBookId(bookId);
        SkillBook _skillBook;
        if(!player.getSkillBooks().containsKey(skillId))
        {
            _skillBook=new SkillBook();
            _skillBook.setSkillId(skillId);
            _skillBook.setPlayerId(player.getId());
            _skillBook.changePartCountByBookId(bookId,1);
            player.getSkillBooks().put(skillId,_skillBook);
            DaoManager.getSkillBookDao().addSkillBook(player,skillId);
        }
        else
        {
            _skillBook=player.getSkillBooks().get(skillId);
            _skillBook.changePartCountByBookId(bookId, 1);
            player.updateOneSkillBook(_skillBook);
        }
        if(_skillBook.getSkillBookCountBySkillBookId(bookId)==1)
        {
            SkillBookServices.addPlayerIdToMap(player.getId(), bookId);
        }
    }

    /**
     * 添加持有残章玩家id
     * @param playerId
     * @param skillBookId
     */
    public static void addPlayerIdToMap(int playerId,int skillBookId)
    {
        if(!SkillBookPlayerMap.get(skillBookId).contains(playerId))
        {
            synchronized (lock)
            {
                SkillBookPlayerMap.get(skillBookId).add(playerId);
            }
        }
    }

    /**
     * 移除持有残章玩家id
     * @param playerId
     * @param skillBookId
     */
    public static void removePlayerIdFromMap(int playerId,int skillBookId)
    {
        if(SkillBookPlayerMap.get(skillBookId).contains(playerId))
        {
            synchronized (lock)
            {
                SkillBookPlayerMap.get(skillBookId).remove(SkillBookPlayerMap.get(skillBookId).indexOf(playerId));
                logger.info("removePlayerIdFromMap#skillBookId:"+skillBookId+",playerId:"+playerId+"containsStatus:"+SkillBookPlayerMap.get(skillBookId).contains(playerId));
            }
        }
    }

    /**
     * 玩家退出后清除该玩家对应在内存中的持有该残章记录
     * @param player
     */
    public static void clearSkillBookByPlayer(Player player)
    {
        long startTime=Util.getServerMillTime();
        logger.info("start************clearSkillBookByPlayer#playerId:"+player.getId()+",startTime:"+startTime+"*********start");
        try
        {
            Iterator<Map.Entry<Integer,SkillBook>> it=player.getSkillBooks().entrySet().iterator();
            while (it.hasNext())
            {
                Map.Entry<Integer, SkillBook> entry= it.next();
                SkillBook _item= entry.getValue();
                //将残章id数量大于0的放入持有残章玩家列表
                JSONObject skillBookDetail=_item.getSkillBookDetailForJson();
                if(_item.getPieceChance()>0&&skillBookDetail.size()>0)
                {
                    for (Iterator iter = skillBookDetail.keys();iter.hasNext();)
                    {
                        String key = (String)iter.next();
                        int count=skillBookDetail.getInt(key);
                        if(count>0)
                        {
                            removePlayerIdFromMap(player.getId(),Integer.parseInt(key));
                        }
                    }
                }
            }
        }
        catch (Exception e)
        {
            System.out.println("clearSkillBookByPlayer Exception:"+e.getMessage());
        }
        long endTime=Util.getServerMillTime();
        logger.info("end************clearSkillBookByPlayer#playerId:"+player.getId()+",endTime:"+endTime+",cost:"+(endTime-startTime)+"*********end");
    }

    /**
     * 初始化残章map内存
     * @param skillId
     */
    public static synchronized void initSkillBookPlayerMap(int skillId)
    {
        if(!SkillBookPlayerMap.containsKey(skillId))
        {
            SkillBookPlayerMap.put(skillId,new ArrayList<Integer>());
        }
    }

    /**
     * 获取持有残章玩家proto
     * @param player
     * @param skillBookId
     * @return
     */
    public static  GetSkillBookLoot.OPSkillBookLootRet.Builder getSkillBookLootBySkillBookId(Player player, int skillBookId)
    {
        GetSkillBookLoot.OPSkillBookLootRet.Builder builder = GetSkillBookLoot.OPSkillBookLootRet.newBuilder();
        if(!SkillBookPlayerMap.containsKey(skillBookId))
        {
            return builder;
        }
        //玩家批量大的时候从该玩家残章中查询上次随机末尾位置开始查找
        int endIndex=SkillBookPlayerMap.get(skillBookId).size();
        int startIndex=player.getSkillBookNpcStats().getLastRandomIndex();
        if(endIndex<(startIndex+SkillBookLootSize))
        {
            startIndex=0;
        }
        int _level=player.getLevel();
        List<Integer > playerIdArr=SkillBookPlayerMap.get(skillBookId).subList(startIndex,endIndex);
        int i=0;
        for(int _playerId:playerIdArr)
        {
            Player _playerItem=ObjectAccessor.getOnlineUser(_playerId);
            //如果当前持有该残章的玩家不在线，踢出持有列表
            if(_playerItem==null)
            {
                 synchronized (lock)
                 {
                     SkillBookPlayerMap.get(skillBookId).remove(SkillBookPlayerMap.get(skillBookId).indexOf(_playerId));
                 }
                continue;
            }
            //当前用户等级上下相差3级以内（含3级）的所有用户
            if((_playerItem.getLevel()>=(_level-3))&&(_playerItem.getLevel()<=(_level+3)))
            {
                GetSkillBookLoot.OPSkillBookLootRet.SkillBookLootPlayer.Builder playerItem= GetSkillBookLoot.OPSkillBookLootRet.SkillBookLootPlayer.newBuilder();
                playerItem.setId(i);
                playerItem.setPlayerId(_playerId);
                playerItem.setPlayerName(_playerItem.getName());
                playerItem.setPlayerLevel(_playerItem.getLevel());
                playerItem.setIsNpc(1);
                ArrayList<Battle> _battle=(ArrayList<Battle>)ObjectAccessor.getBattleArray(_playerId).getBattlearrays().clone();
                int teamCount=_battle.size();
                teamCount=teamCount>3?3:teamCount;
                for(int j=0;j<teamCount;j++)
                {
                    if(_battle.get(j).getItemid()!=0)
                    {
                        GetSkillBookLoot.OPSkillBookLootRet.SkillBookLootPlayer.PlayerTeamItem.Builder teamItem= GetSkillBookLoot.OPSkillBookLootRet.SkillBookLootPlayer.PlayerTeamItem.newBuilder();
                        teamItem.setId(j);
                        teamItem.setMemberId(_battle.get(j).getItemid());
                        playerItem.addTeamItem(teamItem);
                    }
                }
                ++i;
                player.getSkillBookNpcStats().setLastRandomIndex(startIndex+i);
                builder.addSkillBookLootPlayer(playerItem);
            }

            if(i>(SkillBookLootSize-1))
            {
                break;
            }
        }
        //生成NPC列表

        if(i<SkillBookLootSize)
        {
            int generateCount=SkillBookLootSize-i;
            SkillBookNpcStats npcStats=generateNpcByPlayer(player,generateCount);
            for(int j=0;j<generateCount;++j)
            {

                if(j<npcStats.getRandomNpcArr().size())
                {
                    JSONObject npc=(JSONObject)npcStats.getRandomNpcArr().get(j);
                    GetSkillBookLoot.OPSkillBookLootRet.SkillBookLootPlayer.Builder playerItem= GetSkillBookLoot.OPSkillBookLootRet.SkillBookLootPlayer.newBuilder();
                    playerItem.setId(j+generateCount+1);
                    playerItem.setPlayerId(j);
                    playerItem.setPlayerName("");
                    playerItem.setPlayerLevel(npc.getInt("l"));
                    playerItem.setIsNpc(0);
                    int teamCount=npc.getJSONArray("c").size();
                    teamCount=teamCount>3?3:teamCount;
                    for(int m=0;m<teamCount;m++)
                    {
                        GetSkillBookLoot.OPSkillBookLootRet.SkillBookLootPlayer.PlayerTeamItem.Builder teamItem= GetSkillBookLoot.OPSkillBookLootRet.SkillBookLootPlayer.PlayerTeamItem.newBuilder();
                        teamItem.setId(m);
                        teamItem.setMemberId(npc.getJSONArray("c").getInt(m));
                        playerItem.addTeamItem(teamItem);
                    }
                    builder.addSkillBookLootPlayer(playerItem);
                }

            }
        }
        return builder;
    }

    /**
     * 移除玩家残章信息
     * @param player
     * @param skillId
     */
    public static void deleteSkillBook(Player player,int skillId)
    {
        DaoManager.getSkillBookDao().deleteSkillBook(player,skillId);
    }

    public static SkillBookNpcConfig getNpcConfig()
    {
        return skillBookNpcConfig;
    }

    /**
     * 初始化NPC阵容信息
     * @param _skillBookNpcConfig
     */
    public static void initNpcConfig(SkillBookNpcConfig _skillBookNpcConfig)
    {
        skillBookNpcConfig=_skillBookNpcConfig;
        //生成NPC阵容信息
        NpcIdList.clear();
        for(int discipleId=skillBookNpcConfig.getMinDiscipleId();discipleId<=skillBookNpcConfig.getMaxDiscipleId();++discipleId)
        {
            NpcIdList.add(discipleId);
        }
    }

    /**
     * 获取count个不重复的id数组
     * @param count
     * @return
     */
    public static ArrayList<Integer> randomNpcIdByCount(int count)
    {
        ArrayList<Integer> _list=new ArrayList<Integer>();
        ArrayList<Integer> _randomSource=(ArrayList<Integer>)NpcIdList.clone();
        for(int i=0;i<count;i++)
        {
            Collections.swap(_randomSource,i,(int)(i+Math.random()*(_randomSource.size()-i)));
            _list.add(_randomSource.get(i));
        }
        return  _list;
    }

    /**
     * 生成玩家对应等级的npc数据
     * @param player
     * @param count
     * @return
     */
    public static SkillBookNpcStats generateNpcByPlayer(Player player,int count)
    {
        SkillBookNpcStats npcStats=player.getSkillBookNpcStats();
        npcStats.checkLevel(player.getLevel());
        ArrayList<JSONObject> _list=new ArrayList<JSONObject>();
        for(int i=0;i<count;++i)
        {
            JSONObject npc=new JSONObject();
            int npcLevel=(int)(Math.random()*npcStats.getRandomLevel()+1)+npcStats.getFloorLevel();
            npc.put("l",npcLevel);
            npc.put("c",randomNpcIdByCount(BattleUtil.getBattleCountByLevel(npcLevel)));
            _list.add(npc);
        }
        npcStats.setRandomNpcArr(_list);
        return npcStats;
    }

    public static Battle getNpcBattleById(int id)
    {
        return XMLTemplateService.getBattleByDiscipleId(id).clone();
    }

    /**
     * 获取残章掉落掉落的概率
     * @param quality
     * @param count
     * @return
     */
    public static int getRadioByQualityAndCount(int quality,int count)
    {
        JSONObject _config=XMLTemplateService.getSkillBookSnatchByQuality(quality);
        int radio=0;
        if(_config==null)
        {
           return radio;
        }
        if(count>=3)
        {
            count=3;
            if(_config.containsKey(String.valueOf(count)))
            {
                radio=_config.getInt(String.valueOf(count));
            }
        }
        else if(count>=2)
        {
            count=2;
            if(_config.containsKey(String.valueOf(count)))
            {
                radio=_config.getInt(String.valueOf(count));
            }
        }
        else if(count==1)
        {
            if(_config.containsKey(String.valueOf(count)))
            {
                radio=_config.getInt(String.valueOf(count));
            }
        }
        return radio;
    }

    /**
     * 根据章节获取掉落的残章和品质获取掉落的技能残章
     * @param chapter
     * @param quality
     * @return
     */
    public static int getRandomSkillBookByChapter(int chapter,int quality)
    {
        ArrayList<Integer> dropIdArr=new ArrayList<Integer>();
        ArrayList<Integer> weightArr=new ArrayList<Integer>();
        SkillBookSelecter chapterSelecter=XMLTemplateService.getSkillBookSelecterById(chapter);
        if(chapterSelecter!=null)
        {
            dropIdArr.addAll(chapterSelecter.getBookIdArr());
            weightArr.addAll(chapterSelecter.getWeightArr());
        }
        SkillBookSelecter activitySelecter=XMLTemplateService.getSkillBookSelecterById(activitySelecterId);
        if (activitySelecter != null) {
        	 for(int i=0;i<activitySelecter.getBookIdArr().size();++i)
             {
                 if(!dropIdArr.contains(activitySelecter.getBookIdArr().get(i)))
                 {
                     dropIdArr.add(activitySelecter.getBookIdArr().get(i));
                     weightArr.add(activitySelecter.getWeightArr().get(i));
                 }
                 else
                 {
                     //活动掉落与江湖掉重复 权重相加 thiden @20130520
                     int _index=dropIdArr.indexOf(activitySelecter.getBookIdArr().get(i));
                     if(_index!=-1)
                     {
                         weightArr.set(_index,activitySelecter.getWeightArr().get(i)+weightArr.get(_index));
                     }
                 }
             }
		}

        if(quality!=-1)
        {
           for(int i=0;i<dropIdArr.size();++i)
           {
               SkillBookConfig _bookConfig=XMLTemplateService.getSkillBookConfigById(dropIdArr.get(i));
               if(_bookConfig.getSkillQuality()!=quality)
               {
                   dropIdArr.remove(i);
                   weightArr.remove(i);
               }
           }
        }
        if (dropIdArr.size() == 0) {
			return 0;
		}
        return dropIdArr.get(Util.getRandomResultIndexFromInArray(weightArr));
    }
}

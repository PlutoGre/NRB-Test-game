package com.oldmountain.dzm.packethandler;

import com.oldmountain.dzm.configEntity.SkillBookNpcStats;
import com.oldmountain.dzm.entity.*;
import com.oldmountain.dzm.message.*;
import com.oldmountain.dzm.services.SkillBookServices;
import com.oldmountain.dzm.services.SysMsgInfoServices;
import com.oldmountain.dzm.util.*;
import net.sf.json.JSONArray;
import net.sf.json.JSONObject;
import org.apache.mina.common.IoSession;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.google.protobuf.InvalidProtocolBufferException;
import com.oldmountain.dzm.message.SnatchSkillBook.OPSnatchSkillBook;
import com.oldmountain.dzm.message.SnatchSkillBook.OPSnatchSkillBookRet;
import com.oldmountain.dzm.net.ObjectAccessor;
import com.oldmountain.dzm.net.packet.Packet;

import java.util.ArrayList;

/**
 * 留言包
 * 1， 解析数据包
 * 2， 保存留言数据
 * 3， 推送给相关玩家数据信息
 * 4， 下发留言成功
 * 
 *
 * @author lvjc
 */
public class SnatchSkillBookHandler
{
    private static final int version=1;
	private static final Logger logger = LoggerFactory.getLogger("SkillBookInfo");

    /**
     * 通过奇遇残章抢夺残章
     * @param packet
     * @param session
     */
	public static void snatchSkillBook(Packet packet, IoSession session)
    {
        OPSnatchSkillBook skillBookInfo = null;
		try
        {
			skillBookInfo = OPSnatchSkillBook.parseFrom(packet.data.array());
		}
        catch (InvalidProtocolBufferException e)
        {
			e.printStackTrace();
			return;
		}
		
		int version = skillBookInfo.getVersion();//version default = 1;
		int oppentPlayerId = skillBookInfo.getPlayerId();//version default = 1;

		int bookId = skillBookInfo.getBookId();//version default = 1;

        int isNpcParams=skillBookInfo.getIsNpc();
        boolean isNpc=(isNpcParams==0);

        int uuid = (Integer)session.getAttribute("playerid");
        Player player = ObjectAccessor.getOnlineUser(uuid);
        int id=player.getId();
        logger.info("snatchSkillBook handler received: {}, {}, {}", new Object[]{id, version, oppentPlayerId, bookId});
        long startTime = Util.getServerMillTime();
        exeSnatchSkillBook(player,oppentPlayerId,bookId,isNpc);
		long endTime = Util.getServerMillTime();
		long taken_time=endTime-startTime;
		logger.info("snatchSkillBook handler ret send: {},Cost:{}, {}", new Object[]{id, taken_time});
	}

    /**
     * 通过传书抢夺残章
     * @param player
     * @param oppentPlayerId
     * @param bookId
     */
    public static void snatchSkillBookFromCounterAttack(Player player,int oppentPlayerId,int bookId)
    {
        logger.info("snatchSkillBookFromCounterAttack handler received: {}, {}, {}", new Object[]{player.getId(), version, oppentPlayerId, bookId});
        long startTime = Util.getServerMillTime();
        exeSnatchSkillBook(player,oppentPlayerId,bookId,false);
        long endTime = Util.getServerMillTime();
        long taken_time=endTime-startTime;
        logger.info("snatchSkillBookFromCounterAttack handler ret send: {},Cost:{}, {}", new Object[]{player.getId(), taken_time});
    }

    /**
     * 执行残章抢夺
     * @param player
     * @param oppentPlayerId
     * @param bookId
     * @param isNpc
     */
    private static void exeSnatchSkillBook(Player player,int oppentPlayerId,int bookId,boolean isNpc)
    {
        int id=player.getId();
        //判断体力是否充足
        if(player.getVitalitybytime() + player.getVitalitybypellet() < 1)
        {
            ErrorReporter.OPErrorReporter.Builder eBuilder = ErrorReporter.OPErrorReporter.newBuilder();
            eBuilder.setErrorid(OP.code.OPCODE_GET_SKILL_BOOK_INFORET_S_VALUE);
            eBuilder.setErrormsg("PlayerId:"+id+" Current Vitality Not Enough!");
            player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, eBuilder);
            return;
        }
        else
        {
            player.useVitality(1);
        }
        OPSnatchSkillBookRet.Builder builder = OPSnatchSkillBookRet.newBuilder();
        ArrayList<Battle> _selfBattle=(ArrayList<Battle>)ObjectAccessor.getBattleArray(id).getBattlearrays().clone();
        BattleArrayObject selfBattle=new BattleArrayObject();
        selfBattle.setBattlearrays(_selfBattle);
        selfBattle.setPlayerid(player.getId());
        selfBattle.setMomentum(player.getMomentum());
        BattleArrayObject opponentBattlearrays=new BattleArrayObject();
        int npcId=-1;
        if(!isNpc)
        {
        	BattleArrayObject oppentBattleArrayObject=ObjectAccessor.getBattleArray(oppentPlayerId);
            ArrayList<Battle> _oppentBattle=(ArrayList<Battle>)oppentBattleArrayObject.getBattlearrays().clone();
            opponentBattlearrays.setBattlearrays(_oppentBattle);
            opponentBattlearrays.setPlayerid(oppentBattleArrayObject.getPlayerid());
            opponentBattlearrays.setMomentum(oppentBattleArrayObject.getMomentum());
        }
        else
        {
            SkillBookNpcStats _npcStats=player.getSkillBookNpcStats();
            if(_npcStats.getLastNpcBattle()!=null)
            {
                opponentBattlearrays=_npcStats.getLastNpcBattle();
            }
            else
            {
                npcId=oppentPlayerId;
                JSONObject npcConfig=(JSONObject)_npcStats.getRandomNpcArr().get(npcId);
                ArrayList<Battle> battlearrays=new ArrayList<Battle>();
                JSONArray npcArr=npcConfig.getJSONArray("c");
                int captionLevel=npcConfig.getInt("l");
                int ceilLevel=(captionLevel-4)*3;
                ceilLevel=ceilLevel<=0?1:ceilLevel;
                int floorLevel=(captionLevel+2)*3;
                int random=floorLevel-ceilLevel;
                for(int i=0;i<npcArr.size();++i)
                {
                    int increaseLevel=(int)(Math.random()*random+1)+ceilLevel;
                    int _discipleId=npcArr.getInt(i);
                    Battle _battle=SkillBookServices.getNpcBattleById(_discipleId);
                    //获取弟子信息
                    DiscipleXmlTemplate disciple=XMLTemplateService.getDiscipleTemplate(_discipleId);
                    //计算NPC的数据信息
                    _battle.setHealth((int)((_battle.getHealth() + increaseLevel * disciple.getGrowHealth())*SkillBookServices.getNpcConfig().getHpAddition()));
                    _battle.setAttack((int)((_battle.getAttack() + increaseLevel * disciple.getGrowAttack())*SkillBookServices.getNpcConfig().getAttackAddition()));
                    _battle.setDefence((int)((_battle.getDefence() + increaseLevel * disciple.getGrowDefence())*SkillBookServices.getNpcConfig().getDefenseAddition()));
                    _battle.setRp((int)((_battle.getRp() + increaseLevel * disciple.getGrowRp())*SkillBookServices.getNpcConfig().getRpAddition()));
                    _battle.setLevel(increaseLevel);
                    battlearrays.add(_battle);
                }
                opponentBattlearrays.setBattlearrays(battlearrays);
                opponentBattlearrays.setMomentum(0);
                _npcStats.setLastNpcBattle(opponentBattlearrays);
            }


        }
        int snatchType=1;
        //组织玩家信息和NPC的数据信息进行战斗
        Fight _fight=calcFight(player,selfBattle,opponentBattlearrays,npcId);
        int  skillId=XMLTemplateService.getSkillIdBySkillBookId(bookId);
        if(_fight!=null)
        {
            //组织战斗回包信息
            builder.setBattle(getBattleBuilder(player,_fight));
            boolean isWin=false;
            int _quality=XMLTemplateService.getSkillTemplate(skillId).getQuality();
            int _skillBookNeedCount=0;
            //小于2为胜利
            if(_fight.getResult()<=2)
            {
                isWin=true;
                SkillBook _skillBook=player.getSkillBooks().get(skillId);
                //获取技能拼合残章列表
                ArrayList<Integer> _list= XMLTemplateService.getSkillBookListBySkillId(skillId);
                for(int _partId:_list)
                {
                    int count=_skillBook.getSkillBookCountBySkillBookId(_partId);
                    if(count<=0)
                    {
                        _skillBookNeedCount++;
                    }
                }
            }
            //是否胜利
            if(isWin)
            {
                if(!isNpc)
                {
                    //被抢夺玩家不在线，默认其为抢夺不成功
                    Player snatchPlayer=ObjectAccessor.getOnlineUser(oppentPlayerId);
                    if(snatchPlayer==null)
                    {
                        snatchType=0;
                    }
                    else
                    {
                        snatchType=calcSnatchRet(_quality,_skillBookNeedCount)?1:0;
                        if(snatchType==1)
                        {
                            //抢夺成功
                            //处理被抢夺玩家的残章数据减一
                            SkillBook _skillBook=snatchPlayer.getSkillBooks().get(XMLTemplateService.getSkillIdBySkillBookId(bookId));
                            _skillBook.changePartCountByBookId(bookId, -1);
                            snatchPlayer.updateOneSkillBook(_skillBook);
                            //如果残章数量小于1则在内存中清理掉持有该残章的玩家编号
                            if(_skillBook.getSkillBookCountBySkillBookId(bookId)<=0)
                            {
                                SkillBookServices.removePlayerIdFromMap(snatchPlayer.getId(),bookId);
                            }
                            //实时推送被掠夺的玩家残章数据
                            SkillBookInfo.OPGetSkillBookInfoRet.Builder snatchBuilder = SkillBookInfo.OPGetSkillBookInfoRet.newBuilder();
                            SkillBookInfo.OPGetSkillBookInfoRet.SkillBookPartItem.Builder partItem= SkillBookInfo.OPGetSkillBookInfoRet.SkillBookPartItem.newBuilder();
                            partItem.setPartId(bookId);
                            partItem.setCount(_skillBook.getSkillBookCountBySkillBookId(bookId));
                            snatchBuilder.addSkillBookPartItem(partItem);
                            snatchBuilder.setServertime((int) System.currentTimeMillis());
                            snatchBuilder.setStatus(1);
                            snatchBuilder.setSkillId(skillId);
                            snatchPlayer.sendPacket(OP.code.OPCODE_GET_SKILL_BOOK_INFORET_S_VALUE, snatchBuilder);// 发送返回包
                            SysMsgInfoServices.cometSkillBookMsg(player,snatchPlayer,skillId,bookId,false);
                        }
                        else
                        {
                            SysMsgInfoServices.cometSkillBookMsg(player,snatchPlayer,skillId,bookId,true);
                        }
                    }
                }
                else
                {
                    snatchType=calcSnatchRet(_quality,_skillBookNeedCount)?1:0;
                }
            }
            else
            {
                snatchType=2;
                if(!isNpc)
                {
                    //被抢夺玩家不在线，默认其为抢夺不成功
                    Player snatchPlayer=ObjectAccessor.getOnlineUser(oppentPlayerId);
                    if(snatchPlayer==null)
                    {
                        snatchType=0;
                    }
                    else
                    {
                        SysMsgInfoServices.cometSkillBookMsg(player,snatchPlayer,skillId,bookId,true);
                    }
                }
            }
        }

        if(snatchType==1)
        {
            //抢夺成功后增加玩家残章的数量,并添加持有残章id到内存
            SkillBookServices.obtainSkillBook(player,bookId);
        }

        builder.setServertime((int) System.currentTimeMillis());
        builder.setStatus(1);
        builder.setRet(snatchType);
        builder.setVitality(player.getVitalitybypellet() + player.getVitalitybytime());
        player.sendPacket(OP.code.OPCODE_SNATCH_SKILL_BOOK_INFORET_S_VALUE, builder);// 发送返回包
    }

    /**
     * 计算残章抢夺规则和结果
     * @param quality
     * @param needSkillBook
     * @return
     */
    private static boolean calcSnatchRet(int quality,int needSkillBook)
    {
       boolean snatchRet=true;
       //计算残章抢夺规则和结果
       snatchRet=((int) (Math.random()*100+1))<=SkillBookServices.getRadioByQualityAndCount(quality,needSkillBook);
       return snatchRet;
    }

    /**
     * 抢夺残章开始战斗
     * @param player
     * @param selfBattle
     * @param opponentBattle
     * @param npcId
     * @return
     */
    private static Fight calcFight(Player player,BattleArrayObject selfBattle, BattleArrayObject opponentBattle,int npcId)
    {
        Fight _fight=new Fight();
        if(npcId!=-1)
        {

        }

        _fight.doFight(selfBattle,opponentBattle,null);
        return _fight;
    }
    /**
     * 组织玩家战斗数据的返回信息
     * @param player
     * @param fight
     * @return
     */
    private static UserBattle.OPUserBattleRet.Builder getBattleBuilder(Player player,Fight fight)
    {
        UserBattle.OPUserBattleRet.Builder builder = UserBattle.OPUserBattleRet.newBuilder();
        Round round1 = fight.getRound1();
        Round round2 = fight.getRound2();
        FinalRound round3 = fight.getFinalRound();
        int fightResult = fight.getResult();
        ArrayList<CharacterResult> characterResults = new ArrayList<CharacterResult>();

        Result result = new Result();
        result.setWinlose(fightResult);
        if (fightResult <= 2)
        {//战斗胜利
            UserLevelConfig levelConfig = XMLTemplateService.getUserLevelConfig(player.getLevel());
            int gainCoins = levelConfig.getChallengeCoins();
            int gainUserExp = levelConfig.getCareerXp();
            int gainDiscipleExp = levelConfig.getChallengeDiscipleXp();

            result.setGotCoin(gainCoins);
            result.setGotExp(gainUserExp);
            BattleArrayObject userBattleArray = BattleUtil.getUserBattleArrayObject(player);
            //弟子属性变化
            ArrayList<Battle> userBattles = userBattleArray.getBattlearrays();
            for (int i = 0; i < userBattles.size(); i++)
            {
                CharacterResult characterResult = new CharacterResult();
                Battle battle = userBattles.get(i);
                if (battle.getDiscipleid() == 0)
                {
                    continue;
                }
                Disciple disciple = player.getDisciples().get(battle.getDiscipleid());

                int addLevel = Util.getlevelByExp(disciple, gainDiscipleExp);
                if (addLevel > 0)
                {//弟子升级
                    UserDiscipleInfo.OPUserDiscipleInfoRet.DiscipleInfo.Builder discipleBuilder = DiscipleUtil.buildDiscipleBuilder(disciple);
                    builder.addDisciple(discipleBuilder);
                    characterResult.setIsUpgrade(true);
                    //重新检查阵容属性
                    BattleUtil.resetBattleProperty(battle, disciple, player,null,0);
                    UserBattleArray.OPUserBattleArrayRet.BattleArrayInfo.Builder builder2 = UserBattleArray.OPUserBattleArrayRet.BattleArrayInfo.newBuilder();
                    BattleUtil.buildBattleBuilder(builder2, battle, i);
                    builder.addBattleArrayInfo(builder2);
                }
                else
                {
                    characterResult.setIsUpgrade(false);
                }
                characterResult.setItemid(disciple.getItemid());
                characterResult.setGainExp(gainDiscipleExp);
                characterResult.setLevel(disciple.getLevel());
                characterResults.add(characterResult);
            }

            player.setSilvercoins(player.getSilvercoins() + gainCoins);
            Util.getUserLevelByExp(player, gainUserExp);
        }
        result.setCharacterResults(characterResults);
        builder.setRound1(UserBattleHandler.getRoundBuilder(round1));
        builder.setRound2(UserBattleHandler.getRoundBuilder(round2));
        builder.setRound3(UserBattleHandler.getFinalRound(round3));
        builder.setResult(UserBattleHandler.getResult(player,result));
        builder.setTeamArchievements(fight.getTeamBuilder());

        builder.setVersion(version);
        builder.setServertime(Util.getServerTime());
        return  builder;
    }
}

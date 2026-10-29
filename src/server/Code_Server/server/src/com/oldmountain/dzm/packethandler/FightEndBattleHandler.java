package com.oldmountain.dzm.packethandler;

import com.google.protobuf.InvalidProtocolBufferException;
import com.oldmountain.dzm.configEntity.FightEndNpc;
import com.oldmountain.dzm.entity.*;
import com.oldmountain.dzm.message.ErrorReporter;
import com.oldmountain.dzm.message.FightEndBattle.*;
import com.oldmountain.dzm.message.FightEndInfo.OPFightEndInfoRet;
import com.oldmountain.dzm.message.OP;
import com.oldmountain.dzm.message.UserBattle.OPUserBattleRet;
import com.oldmountain.dzm.net.ObjectAccessor;
import com.oldmountain.dzm.net.packet.Packet;
import com.oldmountain.dzm.services.AdventureFightStatsServices;
import com.oldmountain.dzm.services.FightEndRankingServices;
import com.oldmountain.dzm.services.PayToolServices;
import com.oldmountain.dzm.util.*;
import net.sf.json.JSONArray;
import net.sf.json.JSONObject;
import org.apache.mina.common.IoSession;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.util.ArrayList;
import java.util.HashMap;

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
public class FightEndBattleHandler
{
	private static final Logger logger2 = LoggerFactory.getLogger("FightEndBattle");
    private static final int version=1;

    public static void doFightBattleInfo(Packet packet, IoSession session)
    {
        OPFightEndBattle opDisposeFriendsInfo = null;
		try
        {
			opDisposeFriendsInfo = OPFightEndBattle.parseFrom(packet.data.array());
		}
        catch (InvalidProtocolBufferException e)
        {
			e.printStackTrace();
			return;
		}
		int version = opDisposeFriendsInfo.getVersion();//version default = 1;
        int attackIndex = opDisposeFriendsInfo.getId();//选择的挑战编号
        int uuid = (Integer)session.getAttribute("playerid");
        Player player = ObjectAccessor.getOnlineUser(uuid);
        if(player.getBattleCount()< AdventureFightEndUtil.MIN_BATTLE_COUNT)
        {
            ErrorReporter.OPErrorReporter.Builder eBuilder = ErrorReporter.OPErrorReporter.newBuilder();
            eBuilder.setErrorid(OP.code.OPCODE_FIGHTEND_BATTLERET_S_VALUE);
            eBuilder.setErrormsg("player level dissatisfy 5 Exception!");
            player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, eBuilder);
            return;
        }
        int id=player.getId();
		logger2.info("doFightBattleInfo received PlayerId:{},AttackIndex:{},Version:{}", new Object[]{id , attackIndex , version});
		long current_time = Util.getServerMillTime();
        //判断当前传入挑战血战的编号是否合法
        if(attackIndex>3||attackIndex<1)
        {
            ErrorReporter.OPErrorReporter.Builder eBuilder = ErrorReporter.OPErrorReporter.newBuilder();
            eBuilder.setErrorid(OP.code.OPCODE_FIGHTEND_BATTLERET_S_VALUE);
            eBuilder.setErrormsg("PlayerId:"+id+" Current Select Attack Id:" + attackIndex + " Is Wrong!");
            player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, eBuilder);
            logger2.info("Error Reporter send: fight end battle handler: {}, {}, {}", new Object[]{id});
            return;
        }
        OPFightEndBattleRet.Builder fightBattleBuiler=OPFightEndBattleRet.newBuilder();
        if(player.getFightStats().checkIsAcrossDay())
        {//如果跨天，展现面板为主面板
            fightBattleBuiler.setFightEndInfo(AdventureFightStatsServices.getFightEndInfo(player, OPFightEndInfoRet.PanelType.MainPanel_VALUE,false,false));
        }
        else
        {
            if(player.getFightStats().getBattlePanelType()!=OPFightEndInfoRet.PanelType.TeamPanel_VALUE)
            {
                ErrorReporter.OPErrorReporter.Builder eBuilder = ErrorReporter.OPErrorReporter.newBuilder();
                eBuilder.setErrorid(OP.code.OPCODE_FIGHTEND_BATTLERET_S_VALUE);
                eBuilder.setErrormsg("PlayerId:"+id+" Current Panel is not TeamPanel");
                player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, eBuilder);
                logger2.info("Error Reporter send: fight end battle handler: {}, {}, {}", new Object[]{id});
                return;
            }
            else
            {
                OPUserBattleRet.Builder battleBuilder=OPUserBattleRet.newBuilder();
                JSONObject teamJson=player.getFightStats().getTeamConfJson().getJSONObject(String.valueOf(attackIndex));
                if(teamJson.isNullObject())
                {
                    ErrorReporter.OPErrorReporter.Builder eBuilder = ErrorReporter.OPErrorReporter.newBuilder();
                    eBuilder.setErrorid(OP.code.OPCODE_FIGHTEND_BATTLERET_S_VALUE);
                    eBuilder.setErrormsg("PlayerId:"+id+" TeamPanel TeamConfig Exception!");
                    player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, eBuilder);
                    logger2.info("Error Reporter send: fight end battle handler: {}, {}, {}", new Object[]{id});
                    return;
                }
                else
                {
                    int selfTeam=teamJson.getInt("s");
                    int opponentTeam=teamJson.getInt("o");
                    int npcConfigId=teamJson.getInt("ni");
                    JSONArray opponentTeamArr=teamJson.getJSONArray("n");
                    int keyId=teamJson.getInt("k");
                    int additionType=player.getFightStats().getAddAdditionType();
                    if(additionType!=0)
                    {
                        player.getFightStats().calcBattlePropForTemp(-1);
                    }

                    //组织玩家信息和血战NPC的数据信息进行战斗
                    Fight _fight=calcFight(player,selfTeam,opponentTeam,npcConfigId,opponentTeamArr);
                    if(_fight!=null)
                    {
                        //组织战斗回包信息
                        fightBattleBuiler.setBattle(getBattleBuilder(player,_fight));
                        //是否过关
                        boolean isPass=false;
                        //战斗的轮询次数
                        int round=1;
                        //小于2为胜利
                        if(_fight.getResult()<=2)
                        {
                            isPass=true;
                            round=1+_fight.getResult();
                        }
                        //是否过关
                        if(isPass)
                        {
                            if(keyId!=0)
                            {//如果可以 获得钥匙则将添加钥匙到背包
                                fightBattleBuiler.addToolInfo(PayToolServices.BuyToolsAlone(player,keyId,1));
                                //添加宝箱运气池的运气值
                                BagUtil.getInstance().addLuckPool(player,keyId,1);
                            }
                            //过关 ->几轮战斗以及得星评分
                            player.getFightStats().passBarrier(AdventureFightEndUtil.getInstance().getObtainStarByBattleRoundAndIndex(round,attackIndex));
                            //TODO:是否要判断当前关卡后是否还有最大关卡
                            if(player.getFightStats().getCurrObtainStar()>=player.getFightStats().getMaxObtainStar())
//                            if(player.getFightStats().getCurrBarrierLV()>=player.getFightStats().getMaxBarrierLV())
                            {
                                //同步排行榜数据
                                //并预测今日排名
                                player.getFightStats().setCalculateTodayRanking(FightEndRankingServices.addRankingByPlayer(player));
                            }
                            //添加今日的得星数据，以及增加过关信息，检测是否需要展现属性加成面板，是否需要展现的奖励面板
                            player.getFightStats().getNextPanelType(isPass);
                            fightBattleBuiler.setFightEndInfo(AdventureFightStatsServices.getFightEndInfo(player,player.getFightStats().getBattlePanelType(),true,false));
                        }
                        else
                        {
                            //失败->今日闯关次数增加一次
                            fightBattleBuiler.setFightEndInfo(AdventureFightStatsServices.getFightEndInfo(player,OPFightEndInfoRet.PanelType.MainPanel_VALUE,false,false));
                            player.getFightStats().failBarrier();
                        }
                    }

                }

            }
        }
        fightBattleBuiler.setStatus(1);
        fightBattleBuiler.setServertime((int) System.currentTimeMillis());
        player.setLastVerifyTime(Util.getServerTime());
        
		player.sendPacket(OP.code.OPCODE_FIGHTEND_BATTLERET_S_VALUE, fightBattleBuiler);// 发送返回包
		long current_time_2 = Util.getServerMillTime();
		long taken_time=current_time_2-current_time;
		logger2.info("doFightBattleInfo Send PlayerId:{},Cost:{}", new Object[]{id ,taken_time});
	}

    /**
     * 开始打血战
     * @param player
     * @param selfTeam
     * @param opponentTeam
     * @param npcConfigId
     * @param opponentTeamArr
     * @return
     */
    private static Fight calcFight(Player player,int selfTeam,int opponentTeam,int npcConfigId,JSONArray opponentTeamArr)
    {
        int npcLevel= XMLTemplateService.getFightEndNpcLevelByBarrier(player.getFightStats().getCurrBarrierLV()).getBattleLevel(String.valueOf(player.getBattleCount()));
        logger2.info("=====FightEnd Battle 当前玩家：" + player.getId() + "，阵容为：" + player.getBattleCount() + ",攻打：" + player.getFightStats().getCurrBarrierLV() + "关，攻打的NPC组为：" + npcConfigId + ",NPC的等级为：" + npcLevel+"=====");
        Fight _fight=new Fight();
        //组织玩家阵容信息
        BattleArrayObject selfBattle=new BattleArrayObject();
        ArrayList<Battle> selfBattlearrays=new ArrayList<Battle>();
        //我方阵容，从player中拿到玩家阵容信息，目前是每次闯关均从player中拿数据
        HashMap<Integer, Battle> battlearray= (HashMap<Integer, Battle>) player.getBattlearray().clone();
        if(battlearray!=null)
        {
            //logger2.info("=====FightEnd#Battle玩家阵容编号 弟子表编号 弟子编号 原血 攻 防 内 血+ 攻+ 防+ 内+ 现血 攻 防 内=====");
            String battleIds[]=player.getBattleids().split(",");
            if(battleIds.length<selfTeam)
            {
                ErrorReporter.OPErrorReporter.Builder eBuilder = ErrorReporter.OPErrorReporter.newBuilder();
                eBuilder.setErrorid(OP.code.OPCODE_FIGHTEND_BATTLERET_S_VALUE);
                eBuilder.setErrormsg("PlayerId:"+player.getId()+" Current Player Battle length Exception!");
                player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, eBuilder);
                logger2.info("Error Reporter send: fight end battle handler calcFight: player battle less than need selfTeam count {}, {}, {}", new Object[]{player.getId(),battleIds.length,selfTeam});
                return null;
            }
            for (int s=0;s<selfTeam;++s)
            {
                Battle _battle=battlearray.get(Integer.parseInt(battleIds[s])).clone();
                if(_battle!=null)
                {
                    double hp= (player.getFightStats().getAdditionRateByType(OPFightEndInfoRet.Addition_Type.HP_VALUE)*0.01);
                    double attack= (player.getFightStats().getAdditionRateByType(OPFightEndInfoRet.Addition_Type.ATTATCK_VALUE)*0.01);
                    double defence= (player.getFightStats().getAdditionRateByType(OPFightEndInfoRet.Addition_Type.DEFENSE_VALUE)*0.01);
                    double rp= (player.getFightStats().getAdditionRateByType(OPFightEndInfoRet.Addition_Type.DEFENSE_VALUE)*0.01);
                    //logger2.info(_battle.getId()+" "+_battle.getDiscipleid()+" "+_battle.getItemid()+" "+ _battle.getHealth()+" "+_battle.getAttack()+" "+_battle.getDefence()+" "+_battle.getRp()+" "+hp+" "+attack+" "+defence+" "+rp+" "+(int) (_battle.getHealth()*(1+hp))+" "+(int) (_battle.getAttack()*(1+attack))+" "+ (int) (_battle.getDefence()*(1+defence))+" "+(int) (_battle.getRp()*(1+rp)));
                    _battle.setHealth((int) (_battle.getHealth()*(1+hp)));
                    _battle.setAttack((int) (_battle.getAttack()*(1+attack)));
                    _battle.setDefence((int) (_battle.getDefence()*(1+defence)));
                    _battle.setRp((int) (_battle.getRp()*(1+rp)));
                    selfBattlearrays.add(_battle);
                }
                else
                {
                    ErrorReporter.OPErrorReporter.Builder eBuilder = ErrorReporter.OPErrorReporter.newBuilder();
                    eBuilder.setErrorid(OP.code.OPCODE_FIGHTEND_BATTLERET_S_VALUE);
                    eBuilder.setErrormsg("PlayerId:"+player.getId()+" Current Player Battle Exist Exception!");
                    player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, eBuilder);
                    logger2.info("Error Reporter send: fight end battle handler calcFight: player battle not exist {}, {}", new Object[]{player.getId(),battleIds[s]});
                    return null;
                }
            }
        }
        else
        {
            ErrorReporter.OPErrorReporter.Builder eBuilder = ErrorReporter.OPErrorReporter.newBuilder();
            eBuilder.setErrorid(OP.code.OPCODE_FIGHTEND_BATTLERET_S_VALUE);
            eBuilder.setErrormsg("PlayerId:"+player.getId()+" Current Player Battle Exception!");
            player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, eBuilder);
            logger2.info("Error Reporter send: fight end battle handler: {}, {}, {}", new Object[]{player.getId()});
            return null;
        }
        selfBattle.setBattlearrays(selfBattlearrays);
        selfBattle.setPlayerid(player.getId());
        selfBattle.setMomentum(player.getMomentum());

        //组织对手阵容信息
        BattleArrayObject opponentBattle=new BattleArrayObject();
        ArrayList<Battle> opponentBattlearrays=new ArrayList<Battle>();
        //NPC config 编号
        FightEndNpc npcConfig=AdventureFightEndUtil.getInstance().getFightNpcConfigByNpcId(npcConfigId);

        ArrayList<Float> skillRate=new ArrayList<Float>();
        //logger2.info("=====FightEnd#BattleNPC阵容等级 配置编号 npcId 血 攻 防 内 技能 技能概率=====");
        for(int i=0;i<opponentTeam;i++)
        {
            if(i<opponentTeamArr.size())
            {
                int npcId=opponentTeamArr.getInt(i);
                //获取弟子信息
                DiscipleXmlTemplate disciple=XMLTemplateService.getDiscipleTemplate(npcId);
                if(disciple!=null)
                {
                    Battle _battle=XMLTemplateService.getBattleByDiscipleId(disciple.getId()).clone();
                    _battle.setId(i);
                    _battle.setLevel(npcLevel);
                    int increaseLevel=npcLevel-1;
                    //计算NPC的数据信息
                    _battle.setHealth((int)((_battle.getHealth() + increaseLevel * disciple.getGrowHealth())*npcConfig.getAdditionByType(OPFightEndInfoRet.Addition_Type.HP_VALUE)));
                    _battle.setAttack((int)((_battle.getAttack() + increaseLevel * disciple.getGrowAttack())*npcConfig.getAdditionByType(OPFightEndInfoRet.Addition_Type.ATTATCK_VALUE)));
                    _battle.setDefence((int)((_battle.getDefence() + increaseLevel * disciple.getGrowDefence())*npcConfig.getAdditionByType(OPFightEndInfoRet.Addition_Type.DEFENSE_VALUE)));
                    _battle.setRp((int)((_battle.getRp() + increaseLevel * disciple.getGrowRp())*npcConfig.getAdditionByType(OPFightEndInfoRet.Addition_Type.WILL_VALUE)));
                    opponentBattlearrays.add(_battle);
                    skillRate.add((float)(npcConfig.getSkill_rate()));
                    //logger2.info(npcLevel+" "+npcConfig.getId()+" "+ _battle.getHealth()+" "+_battle.getAttack()+" "+_battle.getDefence()+" "+_battle.getRp()+" "+_battle.getDefaultskill()+npcConfig.getSkill_rate());
                }
            }
        }
        opponentBattle.setBattlearrays(opponentBattlearrays);
        _fight.doFight(selfBattle,opponentBattle,skillRate);
        return _fight;
    }

    /**
     * 组织玩家血战战斗数据的返回信息
     * @param player
     * @param fight
     * @return
     */
    private static OPUserBattleRet.Builder getBattleBuilder(Player player,Fight fight)
    {
        OPUserBattleRet.Builder builder = OPUserBattleRet.newBuilder();
        Round round1 = fight.getRound1();
        Round round2 = fight.getRound2();
        FinalRound round3 = fight.getFinalRound();
        int fightResult = fight.getResult();
        ArrayList<CharacterResult> characterResults = new ArrayList<CharacterResult>();

        Result result = new Result();
        result.setWinlose(fightResult);

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

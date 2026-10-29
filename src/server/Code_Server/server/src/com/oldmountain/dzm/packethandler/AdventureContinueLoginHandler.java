package com.oldmountain.dzm.packethandler;

import com.google.protobuf.InvalidProtocolBufferException;
import com.oldmountain.dzm.configEntity.AdventureContinueLogin;
import com.oldmountain.dzm.entity.*;
import com.oldmountain.dzm.message.*;
import com.oldmountain.dzm.message.GetAdventureContinueLogin.*;
import com.oldmountain.dzm.message.RewardAdventureContinueLogin.*;
import com.oldmountain.dzm.net.ObjectAccessor;
import com.oldmountain.dzm.net.packet.Packet;
import com.oldmountain.dzm.services.*;
import com.oldmountain.dzm.util.*;
import net.sf.json.JSONObject;
import org.apache.mina.common.IoSession;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.List;
import java.util.ListIterator;

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
public class AdventureContinueLoginHandler
{
    private static final Logger logger = LoggerFactory.getLogger("AdventureInfo");
    public static final int AdventureContinueLoginId=10;
    public  static Adventures adventures=null;
    /**
     * 获取奇遇连续登陆活动
     * @param packet
     * @param session
     */
    public static void getInfo(Packet packet, IoSession session)
    {
        OPGetAdventureContinueLogin opGetAdventureContinueLogin = null;
        try
        {
            opGetAdventureContinueLogin = OPGetAdventureContinueLogin.parseFrom(packet.data.array());
        }
        catch (InvalidProtocolBufferException e)
        {
            e.printStackTrace();
            return;
        }

        int version = opGetAdventureContinueLogin.getVersion();//version default = 1;
        int uuid = (Integer)session.getAttribute("playerid");
        Player player = ObjectAccessor.getOnlineUser(uuid);
        int id=player.getId();
        logger.info("getInfo received: {}, {}", new Object[]{id , version});

        long current_time = Util.getServerMillTime();
        if(!_checkValid())
        {
            ErrorReporter.OPErrorReporter.Builder eBuilder = ErrorReporter.OPErrorReporter.newBuilder();
            eBuilder.setErrorid(OP.code.OPCODE_GET_FIGHTEND_REWARD_INFORET_S_VALUE);
            eBuilder.setErrormsg("PlayerId:"+player.getId()+" Current Activity Invalid!");
            player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, eBuilder);
           return;
        }
        setAdventureContinueLoginStats(player,false,true);
        UserStats stats = player.getUserStats();
        OPGetAdventureContinueLoginRet.Builder builder = OPGetAdventureContinueLoginRet.newBuilder();
        builder.setServertime(Util.getServerTime());
        builder.setLoginStatus(stats.getAdContinueInfo());
        player.setLastVerifyTime(Util.getServerTime());
        player.sendPacket(OP.code.OPCODE_ADVENTURE_CONTINUELOGIN_GET_INFORET_S_VALUE, builder);// 发送返回包
        long current_time_2 = Util.getServerMillTime();
        long taken_time=current_time_2-current_time;
        logger.info("getInfo Send: PlayerId:{}, Cost:{}", new Object[]{id ,taken_time});
    }


    /**
     * 领取连续登陆奖励
     * 目前没有增加体力、精力、论剑次数，exp奖励
     * @param packet
     * @param session
     */
	public static void getReward(Packet packet, IoSession session)
    {

        OPRewardAdventureContinueLogin opRewardAdventureContinueLogin = null;
		try
        {
			opRewardAdventureContinueLogin = OPRewardAdventureContinueLogin.parseFrom(packet.data.array());
		}
        catch (InvalidProtocolBufferException e)
        {
			e.printStackTrace();
			return;
		}
		
		int version = opRewardAdventureContinueLogin.getVersion();//version default = 1;
        int uuid = (Integer)session.getAttribute("playerid");

        Player player = ObjectAccessor.getOnlineUser(uuid);
        int id=player.getId();
        int rewardId=opRewardAdventureContinueLogin.getId();
        logger.info("getReward received: PlayerId:{}, Version:{},rewardId:{}", new Object[]{id , version,rewardId});
        long current_time = Util.getServerMillTime();
        if(!_checkValid())
        {
            ErrorReporter.OPErrorReporter.Builder eBuilder = ErrorReporter.OPErrorReporter.newBuilder();
            eBuilder.setErrorid(OP.code.OPCODE_GET_FIGHTEND_REWARD_INFORET_S_VALUE);
            eBuilder.setErrormsg("PlayerId:"+player.getId()+" Current Continue Activity Invalid!");
            player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, eBuilder);
            return;
        }


        UserStats stats = player.getUserStats();
        //检测当前奇遇连续登陆活动是否已领取完成
        if(stats.getAdventureContinueInfoJson().size()==(XMLTemplateService.adventureContinueLoginTemplatesSize()+2))
        {
            ErrorReporter.OPErrorReporter.Builder eBuilder = ErrorReporter.OPErrorReporter.newBuilder();
            eBuilder.setErrorid(OP.code.OPCODE_GET_FIGHTEND_REWARD_INFORET_S_VALUE);
            eBuilder.setErrormsg("PlayerId:"+player.getId()+" Current Continue Activity Reward All Get!!");
            player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, eBuilder);
            return;
        }
        //判断是否已经领取
        if(stats.getAdventureContinueInfoJson().containsKey(String.valueOf(rewardId)))
        {
            ErrorReporter.OPErrorReporter.Builder eBuilder = ErrorReporter.OPErrorReporter.newBuilder();
            eBuilder.setErrorid(OP.code.OPCODE_GET_FIGHTEND_REWARD_INFORET_S_VALUE);
            eBuilder.setErrormsg("PlayerId:"+player.getId()+" Current Continue Activity Id:"+rewardId+" Already Get!!");
            player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, eBuilder);
            return;
        }
        //判断活动领取编号是否存在
        AdventureContinueLogin item=XMLTemplateService.getAdventureContinueLoginById(rewardId);
        if(item==null)
        {
            ErrorReporter.OPErrorReporter.Builder eBuilder = ErrorReporter.OPErrorReporter.newBuilder();
            eBuilder.setErrorid(OP.code.OPCODE_GET_FIGHTEND_REWARD_INFORET_S_VALUE);
            eBuilder.setErrormsg("PlayerId:"+player.getId()+" Current Continue Activity ItemId Error!!");
            player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, eBuilder);
            return;
        }

        int currLoginCount=0;
        try
        {
            currLoginCount=stats.getAdventureContinueInfoJson().getInt("c");
        }
        catch (Exception e)
        {
            currLoginCount=0;
        }

        if(currLoginCount<item.getDayCount())
        {
            ErrorReporter.OPErrorReporter.Builder eBuilder = ErrorReporter.OPErrorReporter.newBuilder();
            eBuilder.setErrorid(OP.code.OPCODE_GET_FIGHTEND_REWARD_INFORET_S_VALUE);
            eBuilder.setErrormsg("PlayerId:"+player.getId()+" Current Continue Activity LoginCount Error!!");
            player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, eBuilder);
            return;
        }

        stats.getAdventureContinueInfoJson().put(rewardId,1);
        player.setUserStats(stats);
        OPRewardAdventureContinueLoginRet.Builder builder=OPRewardAdventureContinueLoginRet.newBuilder();
        for (ToolContainReward _toolContain : item.getRewards())
        {
            switch (UserToolConst.getType(_toolContain.getType()))
            {
                case UserToolConst.DISCIPLE_TYPE:
                	if (player.getDiscipleitems().contains(_toolContain.getItemId())) {
                		DiscipleXmlTemplate discipleInfo = XMLTemplateService.getDiscipleTemplate(_toolContain.getItemId());
                		int count = XMLTemplateService.getSysBasicConfig().getSoulCounts().get(discipleInfo.getQuality());
                		builder.addSoulInfo(SoulServices.addSoul(player, _toolContain.getItemId(), count));
					}
                	else {
	                    Disciple disciple = DiscipleUtil.newDisciple(player, _toolContain.getItemId(), 1, 0);
	                    Skill skill = player.getSkills().get(disciple.getSkill1());
	                    builder.addSkillInfo(SkillUtil.buildSkillBuilder(skill));
	                    builder.addDisciple(DiscipleUtil.buildDiscipleBuilder(disciple));
                	}
                    break;
                case UserToolConst.TOOLS_TYPE:
                    builder.addToolInfo(BagToolServices.addBagTool(player, _toolContain.getItemId(), _toolContain.getQuantity()));
                    break;
                case UserToolConst.SOUL_TYPE:
                    builder.addSoulInfo(SoulServices.addSoul(player, _toolContain.getItemId(), _toolContain.getQuantity()));
                    break;
                case UserToolConst.SKILL_TYPE:
                    if (_toolContain.getQuantity() > 1)
                    {
                        ArrayList<UserSkillInfo.OPUserSkillInfoRet.SkillInfo.Builder> arrayList= SkillServices.createSkillMuli(player, _toolContain.getItemId(), _toolContain.getQuantity());
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
                        ArrayList<UserEquipInfo.OPUserEquipInfoRet.EquipInfo.Builder> arrayList= EquipServices.createEquitMuli(player, _toolContain.getItemId(), _toolContain.getQuantity());
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
            builder.addReward(Util.getRewardInfoBuilder(_toolContain.getType(),_toolContain.getItemId(),_toolContain.getQuantity()));
        }

        player.setLastVerifyTime(Util.getServerTime());
        builder.setStatus(1);
        builder.setServertime((int) System.currentTimeMillis());
        builder.setLoginStatus(player.getUserStats().getAdContinueInfo());
        builder.setGoldcoins(player.getGoldCoins());
        builder.setSilvercoins(player.getSilvercoins());
        //builder.setExp(player.getExp());
        //builder.setPower(player.getPowerbychicken()+player.getPowerbytime());
        //builder.setChallengetimes(player.getUserStats().getChallengetimes());
        //builder.setVitality(player.getVitalitybypellet()+player.getVitalitybytime());
		player.sendPacket(OP.code.OPCODE_ADVENTURE_CONTINUELOGIN_REWARDRET_S_VALUE, builder);// 发送返回包
		long current_time_2 = Util.getServerMillTime();
		long taken_time=current_time_2-current_time;
        logger.info("gerReward Send: PlayerId:{},Cost:{}", new Object[]{id ,taken_time});
	}

    public static boolean checkShow(Player player)
    {
        setAdventureContinueLoginStats(player,false,true);
        return (player.getUserStats().getAdventureContinueInfoJson().size()==(XMLTemplateService.adventureContinueLoginTemplatesSize()+2));
    }

    /**
     * 检测活动是否在有效时间之内
     * @return
     */
    private static boolean  _checkValid()
    {
        boolean valid=true;
        Adventures adventure=_getAdventures();
        int current_time=Util.getServerTime();
        if(adventure!=null)
        {
            SimpleDateFormat df = new SimpleDateFormat("yyyy-MM-dd_HH:mm:ss");
            int endTime = 0;
            int startTime = 0;
            try
            {
                endTime =  (int)(df.parse(adventure.getEndTime()).getTime()/1000);
                startTime = (int)(df.parse(adventure.getBeginTime()).getTime()/1000);
            }
            catch (ParseException e)
            {
                endTime=0;
                startTime=0;
            }
            if(endTime<current_time||startTime>current_time)
            {
                valid=false;
            }
        }
        else
        {
            valid=false;
        }
       return valid;
    }

    /**
     * 获取连续登陆活动信息
     * @return
     */
    private static Adventures _getAdventures()
    {
        if(adventures==null)
        {
            List<Adventures> adventuresList =  XMLTemplateService.getAdventureTemplates();
            int index = 0;
            for ( ListIterator<Adventures> it = adventuresList.listIterator(); it.hasNext(); )
            {

                Adventures adventure =  it.next();
                if(adventure.getId()==AdventureContinueLoginId)
                {
                    adventures=adventure;
                    break;
                }
            }
        }

        return adventures;
    }

    /**
     * 设置连续登陆状态
     * @param player
     * @param isIncrease
     */
    public static void setAdventureContinueLoginStats(Player player,boolean isIncrease,boolean check)
    {
          if(_checkValid())
          {
              Adventures adventure=_getAdventures();
              UserStats stats=player.getUserStats();
              JSONObject loginStats=stats.getAdventureContinueInfoJson();
              String dayCountKey="c";
              String version="v";
              if(loginStats.containsKey(version))
              {
                  if(loginStats.getInt(version)!=adventure.getVersion())
                  {
                      loginStats=new JSONObject();
                      loginStats.put(version,adventure.getVersion());
                      loginStats.put(dayCountKey,1);
                  }
                  else
                  {
                      if(!check)
                      {
                          if(loginStats.containsKey(dayCountKey))
                          {
                              if(isIncrease)
                              {
                                  loginStats.put(dayCountKey,(loginStats.getInt(dayCountKey)+1));
                              }
                              else
                              {
                                  loginStats.put(dayCountKey,1);
                              }
                          }
                          else
                          {
                              loginStats.put(dayCountKey,1);
                          }
                      }

                  }
              }
              else
              {
                  loginStats=new JSONObject();
                  loginStats.put(version,adventure.getVersion());
                  loginStats.put(dayCountKey,1);
              }
              stats.setAdContinueInfoFromJson(loginStats);
              player.setUserStats(stats);
          }
    }

}

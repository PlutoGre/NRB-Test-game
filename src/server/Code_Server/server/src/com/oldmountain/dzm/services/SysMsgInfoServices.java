package com.oldmountain.dzm.services;

import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.entity.Reward;
import com.oldmountain.dzm.entity.SysMsgInfo;
import com.oldmountain.dzm.message.OP;
import com.oldmountain.dzm.message.UserRewardInfo;
import com.oldmountain.dzm.message.UserSysMsgInfo.*;
import com.oldmountain.dzm.net.ObjectAccessor;
import com.oldmountain.dzm.persist.EntityManager;
import com.oldmountain.dzm.services.dao.SysMsgInfoDao;
import com.oldmountain.dzm.services.daoImpl.SysMsgInfoDaoImpl;
import com.oldmountain.dzm.util.Util;
import net.sf.json.JSONObject;
import org.apache.mina.common.IoSession;

import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;

/*
 * Created with IntelliJ IDEA.
 * User: dylan
 * Date: 13-5-13
 * Time: 下午9:51
 * To change this template use File | Settings | File Templates.
 */
public class SysMsgInfoServices
{
    public static final int SKILLBook=3;
    public static final int RANKING=2;
    public static final int ADDFRIEND=4;	//加好友
    public static final int CHART_WITH_FRIEND=5; //好友聊天
    private static int SequenceId=0;

    /**
     * 初始化传书最大编号到内存中
     */
    public static void loadSysMsgSequenceId()
    {
        SequenceId=DaoManager.getSysMsgInfoDao().getMaxSequence();
    }

    /**
     * 生成最大序列编号
     * @return
     */
    private synchronized static int generateMaxSequenceId()
    {
        return ++SequenceId;
    }

    /**
     * 初始化玩家传书数据到内存中
     * @param player
     */
    public static void loadUserSysMsgInfoByPlayerId(Player player)
    {
        List<SysMsgInfo> _list=DaoManager.getSysMsgInfoDao().getSysMsgInfo(player.getId());
        if(_list.size()!=0)
        {
        	int min_index=_list.size()-1;
        	if(_list.size()>50){
        		min_index=49;
        	}
            //移除50条以外的数据
            DaoManager.getSysMsgInfoDao().deleteSysMsgInvalid(player.getId(),_list.get(min_index).getId());
            //
            HashMap<Integer,SysMsgInfo> _map=new  HashMap<Integer,SysMsgInfo>();
            for (SysMsgInfo a_list : _list)
            {
                if(!_map.containsKey(a_list.getId()))
                {
                    _map.put(a_list.getId(), a_list);
                }
            }
            player.setSysMsgInfos(_map);
        }

    }

    /**
     *获取玩家奖励以外的传书信息
     * @param player
     * @param builder
     * @return
     */
    public static OPUserSysMsgInfoRet.Builder getUserSysMsgRet(Player player,OPUserSysMsgInfoRet.Builder builder)
    {
        loadUserSysMsgInfoByPlayerId(player);
        Iterator it=player.getSysMsgInfos().entrySet().iterator();
        while (it.hasNext())
        {
            Map.Entry _entry=(Map.Entry)it.next();
            SysMsgInfo _msg=(SysMsgInfo)_entry.getValue();
            builder.addSysMsg(getSysMsgBuilder(player,_msg));
        }
        return builder;
    }

    /**
     * 生成当前序列最大的SysMsgInfo
     * @return
     */
    private static SysMsgInfo generateSysMsgInfo()
    {
        SysMsgInfo _msg=new SysMsgInfo();
        _msg.setId(generateMaxSequenceId());
        return _msg;
    }

    /**
     *生成SysMsgInfo到内存中
     * @param player
     * @return
     */
    private static int generateSysMsgInfoIntoCache(Player player)
    {
        SysMsgInfo _msg=generateSysMsgInfo();
        	player.getSysMsgInfos().put(_msg.getId(), _msg);
        return _msg.getId();
    }

    /**
     *查询传书功能中未读信息条数
     * @param player
     * @return
     */
    public static int getUnreadMsgCount(Player player)
    {
        return DaoManager.getSysMsgInfoDao().getCountSysMsgByReadTime(player.getId(),player.getUserStats().getSysMsgTime());
    }

    /**
     * 抢夺残章
     * @param player
     * @param robbedPlayer
     * @param skillId
     * @param skillBookId
     * @param snatchSucc
     */
    public static void cometSkillBookMsg(Player player,Player robbedPlayer,int skillId,int skillBookId,boolean snatchSucc)
    {
        SysMsgInfo _msg=robbedPlayer.getSysMsgInfos().get(generateSysMsgInfoIntoCache(robbedPlayer));
        if(_msg!=null)
        {
            IoSession _session=null;
            if(ObjectAccessor.getOnlineSession().containsKey(robbedPlayer.getId()))
            {
                _session=ObjectAccessor.getOnlineSession().get(robbedPlayer.getId());
            }
            int time=Util.getServerTime();
            _msg.setMsgType(SKILLBook);
            _msg.setPlayerId(robbedPlayer.getId());
            _msg.setAddTime(time);
            _msg.setStatus(1);
            JSONObject _msgInfo=new JSONObject();
            _msgInfo.put("p",player.getId());
            _msgInfo.put("n",player.getName());
            _msgInfo.put("r",(snatchSucc?1:2));
            _msgInfo.put("sb",skillBookId);
            _msgInfo.put("s",skillId);
            _msg.setMsgInfoFromJson(_msgInfo);
            if(_session!=null)
            {
            	//残障抢夺传书不实时添加到数据库
            	player.addOneSysMsgInfo(_msg);
                OPUserSysMsgInfoRet.Builder ret=OPUserSysMsgInfoRet.newBuilder();
                ret.addSysMsg(getSysMsgBuilder(robbedPlayer,_msg));
                ret.setServertime(time);
                //TODO :推送时判定是否已经获取过传书数据,如果没有获取过,则读取玩家的传书信息
                if(robbedPlayer.getSession()!=null)
                {
                    robbedPlayer.sendPacket(OP.code.OPCODE_GET_USER_SYSMSG_INFORET_S_VALUE, ret);
                }
            }
            //session不存在则直接在数据库中插入该传书
            else{
                 DaoManager.getSysMsgInfoDao().addSysMsgInfo(_msg);
            }

        }
    }
    
    /**
     * 推送世界boss排行奖励的传书
     * @param player
     * @param reward
     */
    public static void commitWorldBossMsg(Player player,Reward reward){
    	OPUserSysMsgInfoRet.Builder builder = OPUserSysMsgInfoRet.newBuilder();
    	builder.setServertime(Util.getServerTime());
    	builder.setReadTime(player.getUserStats().getSysMsgTime());
    	OPUserSysMsgInfoRet.SysMsgInfo.Builder sysMsgInfo = RewardServices.getSysMsgInfo(reward, reward, generateMaxSequenceId());
    	sysMsgInfo.setEventTime(reward.getRewardTime());
    	builder.addSysMsg(sysMsgInfo);
    	player.sendPacket(OP.code.OPCODE_GET_USER_SYSMSG_INFORET_S_VALUE, builder);
    }
    /**
     * 推送好友邀请的传书
     * */
    public static void commitFriendConfirm(Player player, Player friend){
    	SysMsgInfo _msg=friend.getSysMsgInfos().get(generateSysMsgInfoIntoCache(friend));
        if(_msg!=null)
        {
            IoSession _session=null;
            if(ObjectAccessor.getOnlineSession().containsKey(friend.getId()))
            {
                _session=ObjectAccessor.getOnlineSession().get(friend.getId());
            }
            int time=Util.getServerTime();
            _msg.setMsgType(ADDFRIEND);
            _msg.setPlayerId(friend.getId());
            _msg.setAddTime(time);
            _msg.setStatus(1);
            JSONObject _msgInfo=new JSONObject();
            _msgInfo.put("p",player.getId());
            _msgInfo.put("n",player.getName());
            _msgInfo.put("l",player.getLevel());
            _msg.setMsgInfoFromJson(_msgInfo);
            if(_session!=null)
            {
            	//好友邀请传书实时添加到数据库(reason:处理请求需实时update)
            	DaoManager.getSysMsgInfoDao().addSysMsgInfo(_msg);
                OPUserSysMsgInfoRet.Builder ret=OPUserSysMsgInfoRet.newBuilder();
                ret.addSysMsg(getSysMsgBuilder(friend,_msg));
                ret.setServertime(time);
                //TODO :推送时判定是否已经获取过传书数据,如果没有获取过,则读取玩家的传书信息
                if(friend.getSession()!=null)
                {
                	friend.sendPacket(OP.code.OPCODE_GET_USER_SYSMSG_INFORET_S_VALUE, ret);
                }
            }
            //session不存在则直接在数据库中插入该传书
            else{
                 DaoManager.getSysMsgInfoDao().addSysMsgInfo(_msg);
            }
        }

    }
    /**
     * 好友聊天
     * */
    public static void friendChat(Player player, Player friend ,String msg){
    	SysMsgInfo _msg=friend.getSysMsgInfos().get(generateSysMsgInfoIntoCache(friend));
        if(_msg!=null)
        {
            IoSession _session=null;
            if(ObjectAccessor.getOnlineSession().containsKey(friend.getId()))
            {
                _session=ObjectAccessor.getOnlineSession().get(friend.getId());
            }
            int time=Util.getServerTime();
            _msg.setMsgType(CHART_WITH_FRIEND);
            _msg.setPlayerId(friend.getId());
            _msg.setAddTime(time);
            _msg.setStatus(1);
            JSONObject _msgInfo=new JSONObject();
            _msgInfo.put("p",player.getId());
            _msgInfo.put("n",player.getName());
//          _msgInfo.put("l",player.getLevel());
            _msg.setMsgInfoFromJson(_msgInfo);
            _msg.setMsgContent(msg);
            if(_session!=null)
            {
            	//聊天传书不实时添加到数据库
            	player.addOneSysMsgInfo(_msg);
                OPUserSysMsgInfoRet.Builder ret=OPUserSysMsgInfoRet.newBuilder();
                ret.addSysMsg(getSysMsgBuilder(friend,_msg));
                ret.setServertime(time);
                //TODO :推送时判定是否已经获取过传书数据,如果没有获取过,则读取玩家的传书信息
                if(friend.getSession()!=null)
                {
                	friend.sendPacket(OP.code.OPCODE_GET_USER_SYSMSG_INFORET_S_VALUE, ret);
                }
            }
            //session不存在则直接在数据库中插入该传书
            else{
                  DaoManager.getSysMsgInfoDao().addSysMsgInfo(_msg);
            }
        }

    }
    /**
     * 生成并推送论剑数据
     * @param player
     * @param robbedPlayer
     * @param level
     * @param oprRet
     */
    public static void cometChallengeMsg(Player player,Player robbedPlayer,int level,boolean oprRet)
    {
        SysMsgInfo _msg=robbedPlayer.getSysMsgInfos().get(generateSysMsgInfoIntoCache(robbedPlayer));
        if(_msg!=null)
        {
            IoSession _session=null;
            if(ObjectAccessor.getOnlineSession().containsKey(robbedPlayer.getId()))
            {
                _session=ObjectAccessor.getOnlineSession().get(robbedPlayer.getId());
            }
            int time=Util.getServerTime();
            _msg.setMsgType(RANKING);
            _msg.setPlayerId(robbedPlayer.getId());
            _msg.setAddTime(time);
            _msg.setStatus(1);
            JSONObject _msgInfo=new JSONObject();
            _msgInfo.put("p",player.getId());
            _msgInfo.put("n",player.getName());
            _msgInfo.put("r",(oprRet?2:1));
            _msgInfo.put("l",level);
            _msg.setMsgInfoFromJson(_msgInfo);
            if(_session!=null)
            {
            	//论剑传书信息不实时添加到数据库
            	player.addOneSysMsgInfo(_msg);
                OPUserSysMsgInfoRet.Builder ret=OPUserSysMsgInfoRet.newBuilder();
                ret.addSysMsg(getSysMsgBuilder(robbedPlayer,_msg));
                ret.setServertime(time);
                //TODO :推送时判定是否已经获取过传书数据,如果没有获取过,则读取玩家的传书信息
                if(robbedPlayer.getSession()!=null)
                {
                    robbedPlayer.sendPacket(OP.code.OPCODE_GET_USER_SYSMSG_INFORET_S_VALUE, ret);
                }
            }
            //session不存在则直接在数据库中插入该传书
            else{
                DaoManager.getSysMsgInfoDao().addSysMsgInfo(_msg);
            }
        }
    }


    /**
     * 生成传书返回Builder
     * @param player
     * @param _sysMsgInfo
     * @return
     */
    public static OPUserSysMsgInfoRet.SysMsgInfo.Builder getSysMsgBuilder(Player player,SysMsgInfo _sysMsgInfo)
    {
        OPUserSysMsgInfoRet.SysMsgInfo.Builder sysMsgInfoBuilder=OPUserSysMsgInfoRet.SysMsgInfo.newBuilder();
        sysMsgInfoBuilder.setId(player.getRewards().size()+_sysMsgInfo.getId());
        sysMsgInfoBuilder.setRelationId(_sysMsgInfo.getId());
        sysMsgInfoBuilder.setMsgType(_sysMsgInfo.getMsgType());
        sysMsgInfoBuilder.setMsgInfo(_sysMsgInfo.getMsgInfo());
        sysMsgInfoBuilder.setMsgContent(_sysMsgInfo.getMsgContent());
        sysMsgInfoBuilder.setOprType(_sysMsgInfo.getStatus());
        JSONObject _info  =_sysMsgInfo.getMsgInfoJson();
        if (_info.containsKey("r")) {
        	sysMsgInfoBuilder.setFightRet(_info.getInt("r"));
		}
        if(_sysMsgInfo.getMsgType()==CHART_WITH_FRIEND || _sysMsgInfo.getMsgType()==ADDFRIEND){
        	sysMsgInfoBuilder.setUName(""+_info.get("n"));
        }
        sysMsgInfoBuilder.setEventTime(_sysMsgInfo.getAddTime());
        return sysMsgInfoBuilder;
    }




}

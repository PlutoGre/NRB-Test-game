package com.oldmountain.dzm.packethandler;

import java.util.List;
import net.sf.json.JSONArray;
import net.sf.json.JSONObject;
import org.apache.mina.common.ByteBuffer;
import org.apache.mina.common.IoSession;
import com.google.protobuf.InvalidProtocolBufferException;
import com.oldmountain.dzm.DZM;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.message.BackStagePushRewardInfo.OPBackStagePushRewardInfo;
import com.oldmountain.dzm.message.BackStagePushRewardInfo.OPBackStagePushRewardInfoRet;
import com.oldmountain.dzm.message.ErrorReporter.OPErrorReporter;
import com.oldmountain.dzm.message.OP;
import com.oldmountain.dzm.net.packet.Packet;
import com.oldmountain.dzm.services.RewardServices;
import com.oldmountain.dzm.util.Util;

public class BackStagePushRewardInfoHandler {
	public static void doPushReward(Packet packet,IoSession session){
		OPBackStagePushRewardInfo params = null;
		OPBackStagePushRewardInfoRet.Builder builder = OPBackStagePushRewardInfoRet.newBuilder();
		
		for(int i=0;i<packet.data.array().length;++i)
		{
			packet.data.array()[i]=(byte) (packet.data.array()[i] ^ 0xA5);
		}
		
		try {
			params = OPBackStagePushRewardInfo.parseFrom(packet.data.array());
		} catch (InvalidProtocolBufferException e) {
			e.printStackTrace();
			return;
		}
		
	
		int version = params.getVersion();
		String rewardname = params.getRewardname();
		String rewardcontent = params.getRewardcontent();
		String field = params.getField();
		int logintime = params.getLogintime();
		String names = params.getNames();
		int levelStart = params.getLevelStart();
		int levelEnd = params.getLevelEnd();
		int rankStart = params.getRankStart();
		int rankEnd = params.getRankEnd();
		String channel = params.getChannel();
		try{
			if("compensate".equals(field))
			{
                 String msg=rewardname;
                 String content=rewardcontent;
                 JSONObject rewardInfo=new JSONObject();
                 JSONArray rewardContent=new JSONArray();
                 int status=0;
                 int lastLogin=-1;
                 int playerId=-1;
                 if(!"".equals(content)&&content.contains(","))
                 {
                     String[]contentArr=content.split(",");
                     for (String aContentArr : contentArr)
                     {
                         if (!"".equals(aContentArr) && aContentArr.contains(":"))
                         {
                             String[] rewardItem = aContentArr.split(":");
                             if (rewardItem.length < 3)
                             {
                                 OPErrorReporter.Builder errorBuilder = OPErrorReporter.newBuilder();
                                 errorBuilder.setErrorid(OP.code.OPCODE_UPDATE_USERINFO_BY_GM_RET_S_VALUE);
                                 errorBuilder.setErrormsg("Compensate GM Command Invalid Content Item Length Error!");
                                 return;
                             }
                             else
                             {
                                 JSONObject item = new JSONObject();
                                 item.put("t", rewardItem[0]);
                                 item.put("i", rewardItem[1]);
                                 item.put("q", rewardItem[2]);
                                 rewardContent.add(item);
                             }
                         }
                         else
                         {
                             OPErrorReporter.Builder errorBuilder = OPErrorReporter.newBuilder();
                             errorBuilder.setErrorid(OP.code.OPCODE_UPDATE_USERINFO_BY_GM_RET_S_VALUE);
                             errorBuilder.setErrormsg("Compensate GM Command Invalid Content Item Error!");
                             return;
                         }
                     }
                     if(rewardContent.size()==0)
                     {
                         status=1;
                     }

                     lastLogin=logintime;
                     
                     if(lastLogin==-1)
                     {
                         OPErrorReporter.Builder errorBuilder = OPErrorReporter.newBuilder();
                         errorBuilder.setErrorid(OP.code.OPCODE_UPDATE_USERINFO_BY_GM_RET_S_VALUE);
                         errorBuilder.setErrormsg("Compensate GM Command Invalid lastLogin Error!");
                         return;
                     }

                 }
                 else if(content==null||"".equals(content))
                 {
	                 rewardInfo.put("m", 1);
                 }
                 else
                 {
                     OPErrorReporter.Builder errorBuilder = OPErrorReporter.newBuilder();
                     errorBuilder.setErrorid(OP.code.OPCODE_UPDATE_USERINFO_BY_GM_RET_S_VALUE);
                     errorBuilder.setErrormsg("Compensate GM Command Invalid Content Error!");
                     return;
                 }
                 if("-1".equals(names)){
                	 RewardServices.createCompensateReward(msg,rewardInfo,rewardContent,lastLogin,levelStart,levelEnd,rankStart,rankEnd,status,-1,channel);
                 }
                 else {
	                 String[] nameArr = names.split(",");
	                 
	                 for(int i=0;i<nameArr.length;i++){
	                     try
	                     {
	                		
	                		Player player;
	                		List<Object> listbyName = DZM.getEntityManager().limitQuery("from Player where name = ?", 0, 1, nameArr[i]);
	                		if (listbyName.size() == 1) 
	                		{
	                			player = (Player)listbyName.get(0);
	                		}
	                		else 
	                		{
	                			player = new Player();
	                		}
	                		playerId = player.getId();
	
	                    	RewardServices.createCompensateReward(msg,rewardInfo,rewardContent,lastLogin,levelStart,levelEnd,rankStart,rankEnd,status,playerId,channel);
	                     }
	                     catch (Exception e)
	                     {
	                    	 e.printStackTrace();
	                     }
	                 }
                 }
                 
			}
			else if("".equals(field)){
			}
			
			builder.setServertime(Util.getServerTime());
			
		
		}catch(Exception e){
			e.printStackTrace();
		}
		
		if (version != 1) {
			return;
		}
		
		Packet pt = new Packet(OP.code.OPCODE_HEARTBEATRET_S_VALUE, builder.build().toByteArray());
		
		ByteBuffer datasrc = pt.data;
		int size = datasrc.limit();
		ByteBuffer data = ByteBuffer.allocate(size);
		
		datasrc.rewind();
		
		for(int i=0;i<size;++i)
		{
			byte info = (byte) (datasrc.get() ^ 0xA5);
			data.put(info);
		}
		
		data.rewind();
		pt.data = data;
		
		session.write(pt);
		return;
	}
	
	/**
	 * function: 根据传入的阵容ID 获取参战弟子的信息 params: 1. Player 需要修改的玩家 2. int battle的ID
	 * return: Disciple 参战的弟子
	 * */
	/*private static Disciple findDiscipleByID(Player player, int index) 
	{
		String battleids = player.getBattleids();
		if (StringUtils.isBlank(battleids)) 
		{
			return null;
		}
		String[] ids = battleids.split(",");
		if (ids.length < index) 
		{
			return null;
		}
		int battleid = Integer.parseInt(ids[index - 1]);
		HashMap<Integer, Battle> battlearray = player.getBattlearray();
		Battle battle = battlearray.get(battleid);
		HashMap<Integer, Disciple> disciples = player.getDisciples();
		Disciple disciple = disciples.get(battle.getDiscipleid());
		return disciple;
	}
	//增加所有上阵弟子经验
	private static void addAllDiscipleExp(Player player, int amount) {
		Iterator<Entry<Integer, Battle>> iterator = player.getBattlearray().entrySet().iterator();
		int index=0;
		while (iterator.hasNext()) 
		{
			if(index>8)
			{
				break;
			}
			Entry<Integer, Battle> entry = iterator.next();
			Disciple disciple = player.getDisciples().get(entry.getValue().getDiscipleid());
			if(disciple==null)
			{
				continue;
			}
			Util.getlevelByExp(disciple, amount);
		}
	}
	
	//根据等级计算还需要多少经验能达到
	private static int getUserAddExp( int lvlEnd , Player player) 
	{
		int levelCount = XMLTemplateService.getUserLevelCount();
		if(lvlEnd>levelCount){
			lvlEnd=levelCount;
		}
		int addExp=XMLTemplateService.getUserLevelExp(lvlEnd-1)-player.getExp();
		return addExp;
	}*/
}

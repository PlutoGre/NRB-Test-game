package com.oldmountain.dzm.packethandler;

import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Random;
import java.util.Map.Entry;

import org.apache.commons.lang.StringUtils;
import org.apache.mina.common.IoSession;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.google.protobuf.InvalidProtocolBufferException;
import com.oldmountain.dzm.DZM;
import com.oldmountain.dzm.entity.ActivityTimeConfig;
import com.oldmountain.dzm.entity.Bag;
import com.oldmountain.dzm.entity.Cdkey;
import com.oldmountain.dzm.entity.CdkeyXmlTemplate;
import com.oldmountain.dzm.entity.DeepWaterSelecter;
import com.oldmountain.dzm.entity.Disciple;
import com.oldmountain.dzm.entity.DiscipleXmlTemplate;
import com.oldmountain.dzm.entity.Equip;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.entity.Skill;
import com.oldmountain.dzm.entity.Soul;
import com.oldmountain.dzm.entity.ToolContainReward;
import com.oldmountain.dzm.entity.UserStats;
import com.oldmountain.dzm.message.CdkeyReward;
import com.oldmountain.dzm.message.DeepwaterExplore;
import com.oldmountain.dzm.message.OP;
import com.oldmountain.dzm.message.UserSkillInfo;
import com.oldmountain.dzm.message.UserToolInfo;
import com.oldmountain.dzm.message.CdkeyReward.OPCdkeyRewardRet;
import com.oldmountain.dzm.message.ContinueLogin.OPContinueLoginRet;
import com.oldmountain.dzm.message.DeepwaterExplore.OPDeepWaterExploreRet;
import com.oldmountain.dzm.message.ErrorReporter.OPErrorReporter;
import com.oldmountain.dzm.message.SkillBookInfo.OPGetSkillBookInfoRet;
import com.oldmountain.dzm.message.UserDiscipleInfo.OPUserDiscipleInfoRet;
import com.oldmountain.dzm.message.UserEquipInfo.OPUserEquipInfoRet;
import com.oldmountain.dzm.message.UserRewardInfo.OPRewardInfoRet;
import com.oldmountain.dzm.message.UserSkillInfo.OPUserSkillInfoRet;
import com.oldmountain.dzm.message.UserSoulInfo.OPUserSoulInfoRet;
import com.oldmountain.dzm.message.UserToolInfo.OPUserToolInfoRet;
import com.oldmountain.dzm.net.ObjectAccessor;
import com.oldmountain.dzm.net.packet.Packet;
import com.oldmountain.dzm.services.BagToolServices;
import com.oldmountain.dzm.services.EquipServices;
import com.oldmountain.dzm.services.PayToolServices;
import com.oldmountain.dzm.services.SkillBookServices;
import com.oldmountain.dzm.services.SkillServices;
import com.oldmountain.dzm.services.SoulServices;
import com.oldmountain.dzm.services.UserPropertyServices;
import com.oldmountain.dzm.util.BagUtil;
import com.oldmountain.dzm.util.DeepWaterUtil;
import com.oldmountain.dzm.util.DiscipleUtil;
import com.oldmountain.dzm.util.EquipUtil;
import com.oldmountain.dzm.util.SkillUtil;
import com.oldmountain.dzm.util.SoulUtil;
import com.oldmountain.dzm.util.UserToolConst;
import com.oldmountain.dzm.util.Util;
import com.oldmountain.dzm.util.XMLTemplateService;
/**
 * 兑换cdkey奖励
 * 状态码status:
 * 			0 : cdkey错误
 * 			1 : 兑换成功
 * 			2 : cdkey已使用
 * 			3 : 已使用过同类cdkey
 * */
public class DeepWaterExploreHandler {
	private static final Logger log = LoggerFactory.getLogger(DeepWaterExploreHandler.class);
	public static SimpleDateFormat df = new SimpleDateFormat("yyyy-MM-dd_HH:mm:ss");

	/**
	 * @param packet
	 * @param session
	 */
	public static void getInfo(Packet packet, IoSession session) {		
		DeepwaterExplore.OPDeepWaterExplore params = null;
		DeepwaterExplore.OPDeepWaterExploreRet.Builder builder =DeepwaterExplore.OPDeepWaterExploreRet.newBuilder();
		try{
			params = DeepwaterExplore.OPDeepWaterExplore.parseFrom(packet.data.array());
		}catch (InvalidProtocolBufferException e) {
			e.printStackTrace();
			return;
		}
		long current_time = Util.getServerMillTime();
		int version = params.getVersion();
		int uuid = (Integer)session.getAttribute("playerid");
		Player player = ObjectAccessor.getOnlineUser(uuid);
		int playerid=player.getId();
		UserStats userStats=player.getUserStats();
		
		int termId=Util.getActivityTermId(current_time, XMLTemplateService.getDeepWaterTimeConfigs());
		//判断活动是否开启,没开启则表明非正常进入
		if(termId==0)
		{
			OPErrorReporter.Builder eBuilder = OPErrorReporter.newBuilder();
			eBuilder.setErrorid(OP.code.OPCODE_DEEP_WATER_EXPLORERET_S_VALUE);
			eBuilder.setErrormsg("activity is not open ");
			player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, eBuilder);
		}
		//重置/初始化活动信息
		else if(termId!=userStats.getDeepWaterTermId() || "{}".equals(userStats.getDeepWaterHeap()) ||"{}".equals(userStats.getDeepWaterLastTimes()))
		{
			DeepWaterUtil.initDeepWaterInfo(userStats);
			userStats.setDeepWaterTermId(termId);
		}
		//重置深海探宝抽取次数
		if(userStats.getDwNextRefreshTime()<Util.getServerTime()){
			DeepWaterUtil.resetDeepWaterTimes(userStats);
			userStats.setDwNextRefreshTime(Util.getNextDayMarshal(current_time/1000));
		}
		boolean isExplore=params.hasKind();
        int status=1;	//使用结果状态码
        log.info("deep water explore handler received: {}, {}, {}", new Object[]{ playerid , version});
        //剩余天数
        int lastDays=DeepWaterUtil.getLastDays(current_time, termId);
        Map<String, Integer> user_deepInfo= Util.jsonToMap(userStats.getDeepWaterLastTimes());
        //获取活动信息
        if(!isExplore)
        {
        	HashMap<Integer, int[]> costs=XMLTemplateService.getDeepWaterConfigs();
        	Iterator<Entry<Integer,int[]>>costs_iterator=costs.entrySet().iterator();
        	while(costs_iterator.hasNext()){
        		OPDeepWaterExploreRet.CostInfo.Builder costInfo=OPDeepWaterExploreRet.CostInfo.newBuilder();
        		Entry<Integer, int[]> costs_entry=costs_iterator.next();
        		int[]cost=costs_entry.getValue();
        		costInfo.setKind(cost[0]);
        		costInfo.setCostType(cost[1]);
        		costInfo.setPrice(cost[2]);
        		builder.addCostInfo(costInfo);
        	}
        }
        //抽取奖励
        else
        {
        	int kind=params.getKind();
        	int[] config=XMLTemplateService.getDeepWaterConfig(kind);
        	int costType=config[1];	//1:贝里 2:钻石
        	int price=config[2];
        	int countLimit=config[3];
        	int fortureTime=0;
        	int select_id=-1;//选中的id
        	int select_index=-1;//选中的角标
        	int lastTimes=user_deepInfo.get(""+kind);
        	if(lastTimes<1){
        		OPErrorReporter.Builder eBuilder = OPErrorReporter.newBuilder();
    			eBuilder.setErrorid(OP.code.OPCODE_DEEP_WATER_EXPLORERET_S_VALUE);
    			eBuilder.setErrormsg("times already used up ");
    			player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, eBuilder);
    			return;
        	}
        	Map<String, Integer> deepWaterHeap=Util.jsonToMap(userStats.getDeepWaterHeap());
        	DeepWaterSelecter deepWaterSelecter=XMLTemplateService.getDeepWaterSelecter(kind);
        	HashMap<Integer, Integer> fortureTimes=XMLTemplateService.getDeepWaterForsureTime(kind);
        	int heap=deepWaterHeap.get(""+kind);
        	Iterator<Entry<Integer, Integer>> forsure_iteraator = fortureTimes.entrySet().iterator();
        	//进行保底判定
        	while(forsure_iteraator.hasNext())
        	{
        		Entry<Integer, Integer>forsure_entry=forsure_iteraator.next();
        		fortureTime=forsure_entry.getKey();
        		//如果已抽取次数达到必中次数-1次,则此次抽取必中该id
        		if(heap==fortureTime-1)
        		{
        			select_id=forsure_entry.getValue();
        			break;
        		}
        	}
        	//如果抽取的是必中
        	if(select_id!=-1)
        	{
        		List<Integer> ids=deepWaterSelecter.getId();
        		for(int i=0;i<ids.size();i++){
        			if(select_id==ids.get(i)){
        				select_index=i;
        				break;
        			}
        		}
        		
        	}
        	//正常权值抽取
        	else{
        		Random random=new Random();
        		List<Integer> rates=deepWaterSelecter.getRate();
        		if(rates.size()>0)
        		{
        			int random_count=random.nextInt(rates.get(rates.size()-1))+1;
        			if(random_count>0 && random_count<=rates.get(0))
        			{
        				select_index=0;
        			}
        			else
        			{
        				for(int i=1;i<rates.size();i++)
        				{
        					if(random_count>rates.get(i-1) && random_count<=rates.get(i))
        					{
        						select_index=i;
        						break;
        					}
        				}
        			}
        		}
        		select_id=deepWaterSelecter.getId().get(select_index);
        	}
        	//如果选取的id或者角标是-1,表明没有正常选取
        	if(select_id==-1 || select_index==-1){
        		OPErrorReporter.Builder eBuilder = OPErrorReporter.newBuilder();
    			eBuilder.setErrorid(OP.code.OPCODE_DEEP_WATER_EXPLORERET_S_VALUE);
    			eBuilder.setErrormsg("error select ");
    			player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, eBuilder);
    			return;
        	}
        	int typeid=deepWaterSelecter.getTypeId().get(select_index);
        	int rewardType=deepWaterSelecter.getRewardType().get(select_index);
        	int lvl=deepWaterSelecter.getLvl().get(select_index);
        	int itemId=deepWaterSelecter.getItemId().get(select_index);
        	int count=deepWaterSelecter.getCount().get(select_index);
        	//进行钻石或贝里扣除
        	switch (costType) {
        	case 1:
        		if(player.getSilvercoins()<price){
        			OPErrorReporter.Builder eBuilder = OPErrorReporter.newBuilder();
        			eBuilder.setErrorid(OP.code.OPCODE_DEEP_WATER_EXPLORERET_S_VALUE);
        			eBuilder.setErrormsg("silvercoins not enough ");
        			player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, eBuilder);
        			return;
        		}
        		else{
        			player.setSilvercoins(player.getSilvercoins()-price);
        		}
        		break;
        	case 2:
        		if(player.getGoldCoins()<price){
        			OPErrorReporter.Builder eBuilder = OPErrorReporter.newBuilder();
        			eBuilder.setErrorid(OP.code.OPCODE_DEEP_WATER_EXPLORERET_S_VALUE);
        			eBuilder.setErrormsg("gold not enough ");
        			player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, eBuilder);
        			return;
        		}
        		player.consumeGold(price);
        		break;
        		
        	default:
        		break;
        	}
        	DeepWaterUtil.getReward(player , builder , kind, rewardType, lvl , itemId ,count);
        	deepWaterHeap.put(""+kind, heap+1);
        	user_deepInfo.put(""+kind, lastTimes-1);
        	userStats.setDeepWaterHeap(Util.mapToString(deepWaterHeap));
        	userStats.setDeepWaterLastTimes(Util.mapToString(user_deepInfo));
        	builder.setSilvercoins(player.getSilvercoins());
        	builder.setGoldcoins(player.getGoldCoins());
        	builder.setSelectedId(select_index);
        	log.info("deep water explore handler received: termId-{}, select_index-{}, select_id{}, costType-{}, price-{}, heap-{}, lastDays-{}, lastTimes-{}", new Object[]{ termId ,select_index ,select_id , costType ,price, heap , lastDays , lastTimes});
        }
        
        
        player.setUserStats(userStats);
        builder.setServertime(Util.getServerTime());
        builder.setLastDays(lastDays);
        builder.setLastTimes1(user_deepInfo.get("1"));
        builder.setLastTimes2(user_deepInfo.get("2"));
        builder.setLastTimes3(user_deepInfo.get("3"));
        player.setLastVerifyTime(Util.getServerTime());
        player.sendPacket(OP.code.OPCODE_DEEP_WATER_EXPLORERET_S_VALUE, builder);// 发送返回包
		long current_time_2 = Util.getServerMillTime();
		long taken_time=current_time_2-current_time;
		log.info("deep water explore handler ret send: {}, {}, {}", new Object[]{playerid ,taken_time});
	}
	

}

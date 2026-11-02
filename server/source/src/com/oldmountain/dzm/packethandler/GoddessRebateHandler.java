package com.oldmountain.dzm.packethandler;


import gnu.trove.map.TIntObjectMap;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.apache.mina.common.IoSession;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.google.protobuf.InvalidProtocolBufferException;
import com.oldmountain.dzm.configEntity.SkillBookConfig;
import com.oldmountain.dzm.entity.ActivityInfo;
import com.oldmountain.dzm.entity.Disciple;
import com.oldmountain.dzm.entity.DiscipleXmlTemplate;
import com.oldmountain.dzm.entity.GoddessRebateConfig;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.entity.Skill;
import com.oldmountain.dzm.entity.SkillBook;
import com.oldmountain.dzm.entity.Soul;
import com.oldmountain.dzm.message.OP;
import com.oldmountain.dzm.message.UserSkillInfo;
import com.oldmountain.dzm.message.ErrorReporter.OPErrorReporter;
import com.oldmountain.dzm.message.GoddessRebate.OPGoddessGiveFlower;
import com.oldmountain.dzm.message.GoddessRebate.OPGoddessGiveFlowerRet;
import com.oldmountain.dzm.message.GoddessRebate.OPGoddessRebateAddRet;
import com.oldmountain.dzm.message.GoddessRebate.OPGoddessRebateInfo;
import com.oldmountain.dzm.message.GoddessRebate.OPGoddessRebateInfoRet;
import com.oldmountain.dzm.message.UserDiscipleInfo.OPUserDiscipleInfoRet;
import com.oldmountain.dzm.message.UserEquipInfo.OPUserEquipInfoRet;
import com.oldmountain.dzm.message.UserSkillInfo.OPUserSkillInfoRet;
import com.oldmountain.dzm.message.UserSoulInfo.OPUserSoulInfoRet;
import com.oldmountain.dzm.message.UserToolInfo.OPUserToolInfoRet;
import com.oldmountain.dzm.net.ObjectAccessor;
import com.oldmountain.dzm.net.packet.Packet;
import com.oldmountain.dzm.services.ActivityServices;
import com.oldmountain.dzm.services.EquipServices;
import com.oldmountain.dzm.services.PayToolServices;
import com.oldmountain.dzm.services.SkillBookServices;
import com.oldmountain.dzm.services.SkillServices;
import com.oldmountain.dzm.services.SoulServices;
import com.oldmountain.dzm.services.UserPropertyServices;
import com.oldmountain.dzm.util.BagUtil;
import com.oldmountain.dzm.util.DiscipleUtil;
import com.oldmountain.dzm.util.SkillUtil;
import com.oldmountain.dzm.util.UserToolConst;
import com.oldmountain.dzm.util.Util;
import com.oldmountain.dzm.util.XMLTemplateService;

public class GoddessRebateHandler {
	private static final Logger logger = LoggerFactory.getLogger(GoddessRebateHandler.class);

	public static void getGoddessRebateInfo(Packet packet, IoSession session) {
		OPGoddessRebateInfo params = null;
		OPGoddessRebateInfoRet.Builder builder = OPGoddessRebateInfoRet.newBuilder();
		try {
			params = OPGoddessRebateInfo.parseFrom(packet.data.array());
		} catch (InvalidProtocolBufferException e) {
			e.printStackTrace();
			return;
		}
		int version=params.getVersion();
		int playerId = (Integer)session.getAttribute("playerid");
		Player player = ObjectAccessor.getOnlineUser(playerId);
		logger.info("fortune come handler received: {}, {}, {}", new Object[]{playerId , version});
		long current_time = Util.getServerMillTime();
		int termId=ActivityServices.getTermIdById(ActivityServices.GODDESS_REBATE, current_time);
		if(termId==0){
			//活动未开启
			OPErrorReporter.Builder eBuilder = OPErrorReporter.newBuilder();
			eBuilder.setErrorid(OP.code.OPCODE_GODDESS_REBATE_INFORET_S_VALUE);
			eBuilder.setErrormsg("activity not open ");
			player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, eBuilder);
			return;
		}
		ActivityInfo activityInfo=player.getActivityInfo();
		//检测是否需要清理失效信息
		ActivityServices.checkNeedToClearup(activityInfo);
		Map<String, Integer> info=ActivityServices.getActivityInfoById(activityInfo, ActivityServices.GODDESS_REBATE);
		//如果本次活动的信息不存在,则进行初始化
		boolean needRefresh=false;
		if(info==null ||!info.containsKey("termId") ||termId!=info.get("termId")){
			info=ActivityServices.initActivityInfo(ActivityServices.GODDESS_REBATE, termId);
			needRefresh=true;
		}
		int size=XMLTemplateService.getGoddessConfigsSize();
		TIntObjectMap<GoddessRebateConfig> configs=XMLTemplateService.getGoddessRebateConfig();
		//添加各女神亲密度,亲密度上限,亲密度价格的相关信息
		for(int i=1;i<=size;i++){
			OPGoddessRebateInfoRet.BasicInfo.Builder basicInfoBuilder=OPGoddessRebateInfoRet.BasicInfo.newBuilder();
			GoddessRebateConfig goddessRebateConfig=configs.get(i);
			basicInfoBuilder.setId(i);
			basicInfoBuilder.setIntimacy(info.get("intimacy"+i));
			basicInfoBuilder.setPrice(goddessRebateConfig.getPrice());
			basicInfoBuilder.setNeedTimes(goddessRebateConfig.getNeedTimes());
			builder.addBasicInfo(basicInfoBuilder);
		}
		//如果进行过初始化,则修改玩家的活动信息
		if(needRefresh){
			activityInfo=ActivityServices.updateActivityInfos(activityInfo,ActivityServices.GODDESS_REBATE,info);
			player.setActivityInfo(activityInfo);
		}
		builder.setServertime(Util.getServerTime());
		builder.setTotleCount(info.get("totalCount"));
		
		player.sendPacket(OP.code.OPCODE_GODDESS_REBATE_INFORET_S_VALUE, builder);// 发送返回包
		long current_time_2 = Util.getServerMillTime();
		long taken_time = current_time_2 - current_time;
		logger.info("godness rebate get info: {}, {}, {}", new Object[] {playerId, taken_time, termId});
		
	}
	
	//献花
	public static void giveFlower(Packet packet, IoSession session) {
		OPGoddessGiveFlower params = null;
		OPGoddessGiveFlowerRet.Builder builder = OPGoddessGiveFlowerRet.newBuilder();
		try {
			params = OPGoddessGiveFlower.parseFrom(packet.data.array());
		} catch (InvalidProtocolBufferException e) {
			e.printStackTrace();
			return;
		}
		int version=params.getVersion();
		int kind=params.getKind();
		
		int playerId = (Integer)session.getAttribute("playerid");
		Player player = ObjectAccessor.getOnlineUser(playerId);
		long current_time = Util.getServerMillTime();
		int termId=ActivityServices.getTermIdById(ActivityServices.GODDESS_REBATE, current_time);
		//活动未开启
		if(termId==0){
			OPErrorReporter.Builder eBuilder = OPErrorReporter.newBuilder();
			eBuilder.setErrorid(OP.code.OPCODE_GODDESS_GIVEFLOWERRET_S_VALUE);
			eBuilder.setErrormsg("activity not open ");
			player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, eBuilder);
			return;
		}
		int size=XMLTemplateService.getGoddessConfigsSize();
		//判定前端传来的kind是否有效
		if(kind>size || kind<0){
			//接收到的kind不正确
			OPErrorReporter.Builder eBuilder = OPErrorReporter.newBuilder();
			eBuilder.setErrorid(OP.code.OPCODE_GODDESS_GIVEFLOWERRET_S_VALUE);
			eBuilder.setErrormsg("kind is wrong ");
			player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, eBuilder);
			return;
		}
		TIntObjectMap<GoddessRebateConfig> configs=XMLTemplateService.getGoddessRebateConfig();
		GoddessRebateConfig config=configs.get(kind);
		int needTimes=config.getNeedTimes();
		int price=config.getPrice();
		ArrayList<int[]> rewardList=config.getRewardList();
		ActivityInfo activityInfo=player.getActivityInfo();
		//检测是否需要清理失效信息
		ActivityServices.checkNeedToClearup(activityInfo);
		Map<String, Integer> info=ActivityServices.getActivityInfoById(activityInfo, ActivityServices.GODDESS_REBATE);
		//如果本次活动的信息不存在,则进行初始化
		if(info==null ||!info.containsKey("termId") ||termId!=info.get("termId")){
			info=ActivityServices.initActivityInfo(ActivityServices.GODDESS_REBATE, termId);
		}
		int totalCount=info.get("totalCount");
		if(totalCount<price){
			//鲜花数量不足
			OPErrorReporter.Builder eBuilder = OPErrorReporter.newBuilder();
			eBuilder.setErrorid(OP.code.OPCODE_GODDESS_GIVEFLOWERRET_S_VALUE);
			eBuilder.setErrormsg("flower not enough");
			player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, eBuilder);
			return;
		}
		int intimacy=info.get("intimacy"+kind);
		intimacy=intimacy+1;
		totalCount=totalCount-price;
		logger.info("godness rebate intimacy and count : {}, {}, {}, {}", new Object[] {player.getId(), totalCount, intimacy});
		//如果亲密度达到上限,则能获得奖励
		if(intimacy>=needTimes)
		{
			intimacy=intimacy-needTimes;
			addReward(builder, player, rewardList);
		}
		info.put("totalCount", totalCount);
		info.put("intimacy"+kind, intimacy);
		//修改活动信息
		activityInfo=ActivityServices.updateActivityInfos(activityInfo, ActivityServices.GODDESS_REBATE, info);
		player.setActivityInfo(activityInfo);
		//回包中添加亲密度信息
		for(int i=1;i<=size;i++){
			OPGoddessGiveFlowerRet.BasicInfo.Builder basicInfoBuilder=OPGoddessGiveFlowerRet.BasicInfo.newBuilder();
			basicInfoBuilder.setId(i);
			basicInfoBuilder.setNum(info.get("intimacy"+i));
			builder.addIntimacy(basicInfoBuilder);
		}
		builder.setServertime(Util.getServerTime());
		builder.setTotalCount(totalCount);
		builder.setSilvercoins(player.getSilvercoins());
		builder.setGoldcoins(player.getGoldCoins());
		player.sendPacket(OP.code.OPCODE_GODDESS_GIVEFLOWERRET_S_VALUE,builder);
		long current_time_2 = Util.getServerMillTime();
		long taken_time = current_time_2 - current_time;
		logger.info("godness rebate give flower: {}, {}, {}", new Object[] {playerId, taken_time });
	}
	
	//充值获取鲜花
	public static void addFlower(Packet packet, IoSession session, int add_count, Player player, int termId, long current_time){
		//增加数量不能为负
		if(add_count<0){
			OPErrorReporter.Builder eBuilder = OPErrorReporter.newBuilder();
			eBuilder.setErrorid(OP.code.OPCODE_GODDESS_REBATE_ADD_RET_VALUE);
			eBuilder.setErrormsg("addcount can not less than 0");
			player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, eBuilder);
			return;
		}
		ActivityInfo activityInfo=player.getActivityInfo();
		//检测是否需要清理失效信息
		ActivityServices.checkNeedToClearup(activityInfo);
		Map<String, Integer> info=ActivityServices.getActivityInfoById(activityInfo, ActivityServices.GODDESS_REBATE);
		//如果本次活动的信息不存在,则进行初始化
		if(info==null ||!info.containsKey("termId") ||termId!=info.get("termId")){
			info=ActivityServices.initActivityInfo(ActivityServices.GODDESS_REBATE, termId);
		}
		int totalCount=info.get("totalCount");
		int old_count=totalCount;
		totalCount=totalCount+add_count;
		info.put("totalCount", totalCount);
		OPGoddessRebateAddRet.Builder builder=OPGoddessRebateAddRet.newBuilder();
		builder.setAddCount(add_count);
		builder.setTotalCount(totalCount);
		builder.setServertime(Util.getServerTime());
		//同步玩家的活动信息
		activityInfo=ActivityServices.updateActivityInfos(activityInfo, ActivityServices.GODDESS_REBATE, info);
		player.setActivityInfo(activityInfo);
		player.sendPacket(OP.code.OPCODE_GODDESS_REBATE_ADD_RET_VALUE, builder);
		long current_time_2 = Util.getServerMillTime();
		long taken_time = current_time_2 - current_time;
		logger.info("godness rebate add flower: {}, {}, {}, {}, {}, {}", new Object[] {player.getId(), taken_time,old_count, add_count,totalCount ,info});
	}
	
	
	//解析奖励信息
	private static void addReward(OPGoddessGiveFlowerRet.Builder builder , Player player ,ArrayList<int[]> rewardList){
		for(int i=0;i<rewardList.size();i++){
			OPGoddessGiveFlowerRet.RewardInfo.Builder rewardBuilder=OPGoddessGiveFlowerRet.RewardInfo.newBuilder();
			int[] reward_info=rewardList.get(i);
			int type=reward_info[0];
			int itemId=reward_info[1];
			int count=reward_info[2];
			rewardBuilder.setType(type);
			rewardBuilder.setItemid(itemId);
			rewardBuilder.setCount(count);
			builder.addRewardInfo(rewardBuilder);
			getReward(player, builder, type, itemId, count);
		}
			
	}
	//添加相关奖励和回包信息
	public static void getReward(Player player, OPGoddessGiveFlowerRet.Builder builder,int rewardType, int itemId, int rewardCount) {
		int type =UserToolConst.getType(rewardType);
		int skill_level=1;
        int giftItemId = itemId;
        switch (type)
        {
            case UserToolConst.USER_PROPERTY_TYPE:
                UserPropertyServices.changeProperty(player,giftItemId,rewardCount);
                switch (giftItemId)
                {
                    case UserToolConst.USER_PROPERTY_GOLD_COINS:
                    	builder.setAddGoldcoins(rewardCount);
                        break;
                    case UserToolConst.USER_PROPERTY_SILVER_COINS:
                    	builder.setAddSilvercoins(rewardCount);
                        break;
                    default:
                        break;
                }
                break;
            case UserToolConst.DISCIPLE_TYPE:
            	boolean has= false;
            	List<Integer> itemIds=player.getDiscipleitems();
            	if(itemIds!=null && itemIds.size()>0){
            		for(int j=0;j<itemIds.size();j++){
            			if(itemIds.get(j)==giftItemId){
            				has=true;
            				break;
            			}
            		}
            	}
            	if(!has){
            		Disciple disciple = DiscipleUtil.newDisciple(player, itemId, 1, 0);
            		//船员信息
                    OPUserDiscipleInfoRet.DiscipleInfo.Builder discipleBuilder=DiscipleUtil.buildDiscipleBuilder(disciple);
                    OPGoddessGiveFlowerRet.DiscipleInfo.Builder discipleInfo=OPGoddessGiveFlowerRet.DiscipleInfo.newBuilder();
                    discipleInfo.setId(discipleBuilder.getId());
                    discipleInfo.setItemid(discipleBuilder.getItemid());
                    discipleInfo.setLevel(discipleBuilder.getLevel());
                    discipleInfo.setExp(discipleBuilder.getExp());
                    discipleInfo.setHealth(discipleBuilder.getHealth());
                    discipleInfo.setAttack(discipleBuilder.getAttack());
                    discipleInfo.setDefence(discipleBuilder.getDefence());
                    discipleInfo.setRp(discipleBuilder.getRp());
                    discipleInfo.setUpgradelevel(discipleBuilder.getUpgradelevel());
                    discipleInfo.setPotentiality(discipleBuilder.getPotentiality());
                    discipleInfo.setSkill1(discipleBuilder.getSkill1());
                    discipleInfo.setBattleid(discipleBuilder.getBattleid());
                    builder.addDiscipleInfo(discipleInfo);
            		//船员技能信息
            		Skill defaultSkill = player.getSkills().get(disciple.getSkill1());
            		OPUserSkillInfoRet.SkillInfo.Builder skillbuilder = SkillUtil.buildSkillBuilder(defaultSkill);
            		OPGoddessGiveFlowerRet.SkillInfo.Builder skillInfo=OPGoddessGiveFlowerRet.SkillInfo.newBuilder();
    	            skillInfo.setId(skillbuilder.getId());
    	            skillInfo.setItemid(skillbuilder.getItemid());
    	            skillInfo.setLevel(skillbuilder.getLevel());
    	            skillInfo.setQuantity(skillbuilder.getQuantity());
    	            skillInfo.setConsume(skillbuilder.getConsume());
    	            skillInfo.setIsborn(skillbuilder.getIsborn());
    	            builder.addSkillInfo(skillInfo);
            	}
            	else{
            		HashMap<Integer, Soul> userSouls = player.getSouls();
            		DiscipleXmlTemplate discipleXmlTemplate = XMLTemplateService.getDiscipleTemplate(giftItemId);
        			Soul soul = new Soul();
        			int count = XMLTemplateService.getSysBasicConfig().getSoulCounts().get(discipleXmlTemplate.getQuality());
        			if (userSouls.containsKey(giftItemId)) {
        				soul = userSouls.get(giftItemId);
        				soul.setCount(soul.getCount() + count);
        				player.updateOneSoul(soul);
        			}
        			else {
        				soul.setItemid(giftItemId);
        				soul.setPlayerid(player.getId());
        				soul.setCount(count);
        				player.addOneSoul(soul);
        			}
        			OPGoddessGiveFlowerRet.SoulInfo.Builder soulInfo=OPGoddessGiveFlowerRet.SoulInfo.newBuilder();
                    OPUserSoulInfoRet.SoulInfo.Builder soulBuilder=SoulServices.addSoul(player, itemId, 0);//前面已完成魂魄数量添加,故添加次数为0
                    soulInfo.setId(soulBuilder.getId());
                    soulInfo.setItemid(soulBuilder.getItemid());
                    soulInfo.setCount(soulBuilder.getCount());
                    builder.addSoulInfo(soulInfo);
            	}
            	break;
            case UserToolConst.SOUL_TYPE:
            	//魂魄信息
            	OPGoddessGiveFlowerRet.SoulInfo.Builder soulInfo=OPGoddessGiveFlowerRet.SoulInfo.newBuilder();
                OPUserSoulInfoRet.SoulInfo.Builder soulBuilder=SoulServices.addSoul(player, itemId, rewardCount);
                soulInfo.setId(soulBuilder.getId());
                soulInfo.setItemid(soulBuilder.getItemid());
                soulInfo.setCount(soulBuilder.getCount());
                builder.addSoulInfo(soulInfo);
                break;
            case UserToolConst.SKILL_TYPE:
            	//技能信息
	            UserSkillInfo.OPUserSkillInfoRet.SkillInfo.Builder skillbuilder=SkillServices.createSkillWithLevel(player, itemId, skill_level);
	            OPGoddessGiveFlowerRet.SkillInfo.Builder skillInfo=OPGoddessGiveFlowerRet.SkillInfo.newBuilder();
	            skillInfo.setId(skillbuilder.getId());
	            skillInfo.setItemid(skillbuilder.getItemid());
	            skillInfo.setLevel(skillbuilder.getLevel());
	            skillInfo.setQuantity(skillbuilder.getQuantity());
	            skillInfo.setConsume(skillbuilder.getConsume());
	            skillInfo.setIsborn(skillbuilder.getIsborn());
	            builder.addSkillInfo(skillInfo);
                break;
            case UserToolConst.EQUIP_TYPE:
            	//装备信息
            	OPGoddessGiveFlowerRet.EquipInfo.Builder equipInfo=OPGoddessGiveFlowerRet.EquipInfo.newBuilder();
	            OPUserEquipInfoRet.EquipInfo.Builder equipBuilder=EquipServices.createEquitAlone(player, itemId);
	            OPUserEquipInfoRet.EquipInfo.PropertyInfo.Builder propertyBuilder=equipBuilder.getPInfoBuilder(0);
	            OPGoddessGiveFlowerRet.EquipInfo.PropertyInfo.Builder propertyInfo=OPGoddessGiveFlowerRet.EquipInfo.PropertyInfo.newBuilder();
	            propertyInfo.setType(propertyBuilder.getType());
	            propertyInfo.setNum(propertyBuilder.getNum());
	            equipInfo.addPInfo(propertyInfo);
	            equipInfo.setId(equipBuilder.getId());
	            equipInfo.setItemid(equipBuilder.getItemid());
	            equipInfo.setLevel(equipBuilder.getLevel());
	            equipInfo.setRefinexp(equipBuilder.getRefinexp());
	            equipInfo.setRefinelevel(equipBuilder.getRefinelevel());
                for(int i=0;i<equipBuilder.getStoneInfoList().size();++i)
                {
                    equipInfo.addStoneInfo(equipBuilder.getStoneInfoList().get(i));
                }
                equipInfo.setBuffvalue(equipBuilder.getBuffvalue());
                List<Integer> list = equipBuilder.getStoneInfoList();
        		for (int i = 0; i < list.size(); i++) {
        			equipInfo.addStoneInfo(list.get(i));
        		}
                builder.addEquipInfo(equipInfo);
                break;
            case UserToolConst.TOOLS_TYPE:
            	//道具信息
            	OPGoddessGiveFlowerRet.ToolInfo.Builder toolInfo=OPGoddessGiveFlowerRet.ToolInfo.newBuilder();
                OPUserToolInfoRet.ToolInfo.Builder toolBuilder=PayToolServices.BuyToolsAlone(player, itemId, rewardCount);
                toolInfo.setId(toolBuilder.getId());
                toolInfo.setItemid(toolBuilder.getItemid());
                toolInfo.setCount(toolBuilder.getCount());
                builder.addToolInfo(toolInfo);
                //添加宝箱运气池的运气值
                if(UserToolConst.getType(giftItemId)==UserToolConst.TREASURE_TOOL)
                {
                    BagUtil.getInstance().addLuckPool(player,giftItemId,rewardCount);
                }
                break;
            case UserToolConst.SKILL_PIECE_TYPE:
            	//残章信息
            	OPGoddessGiveFlowerRet.SkillBookPartItem.Builder partItem=OPGoddessGiveFlowerRet.SkillBookPartItem.newBuilder();
                SkillBookConfig bookConfig=XMLTemplateService.getSkillBookConfigById(itemId);
                SkillBookServices.obtainSkillBook(player,bookConfig.getSkillBookId());
                SkillBook skillBook=player.getSkillBooks().get(bookConfig.getSkillId());
                partItem.setSkillId(bookConfig.getSkillId());
                partItem.setPartId(bookConfig.getSkillBookId());
                partItem.setCount(skillBook.getSkillBookCountBySkillBookId(bookConfig.getSkillBookId()));
                builder.addSkillbookInfo(partItem);
        }
		
	}
}

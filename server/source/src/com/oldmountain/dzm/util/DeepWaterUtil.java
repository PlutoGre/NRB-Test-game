package com.oldmountain.dzm.util;

import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;


import com.oldmountain.dzm.configEntity.SkillBookConfig;
import com.oldmountain.dzm.entity.ActivityTimeConfig;
import com.oldmountain.dzm.entity.Disciple;
import com.oldmountain.dzm.entity.DiscipleXmlTemplate;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.entity.Skill;
import com.oldmountain.dzm.entity.SkillBook;
import com.oldmountain.dzm.entity.SkillSelecter;
import com.oldmountain.dzm.entity.SkillbookSelecter;
import com.oldmountain.dzm.entity.Soul;
import com.oldmountain.dzm.entity.UserStats;
import com.oldmountain.dzm.message.UserSkillInfo;
import com.oldmountain.dzm.message.DeepwaterExplore.OPDeepWaterExploreRet;
import com.oldmountain.dzm.message.DeepwaterExplore.OPDeepWaterExploreRet.Builder;
import com.oldmountain.dzm.message.UserDiscipleInfo.OPUserDiscipleInfoRet;
import com.oldmountain.dzm.message.UserEquipInfo.OPUserEquipInfoRet;
import com.oldmountain.dzm.message.UserSkillInfo.OPUserSkillInfoRet;
import com.oldmountain.dzm.message.UserSoulInfo.OPUserSoulInfoRet;
import com.oldmountain.dzm.message.UserToolInfo.OPUserToolInfoRet;
import com.oldmountain.dzm.services.EquipServices;
import com.oldmountain.dzm.services.PayToolServices;
import com.oldmountain.dzm.services.SkillBookServices;
import com.oldmountain.dzm.services.SkillServices;
import com.oldmountain.dzm.services.SoulServices;
import com.oldmountain.dzm.services.UserPropertyServices;
import net.sf.json.JSONArray;

public class DeepWaterUtil {
	public static SimpleDateFormat df = new SimpleDateFormat("yyyy-MM-dd_HH:mm:ss");
	//获取活动剩余天数
	public static int getLastDays(long current_time,int termId){
		ActivityTimeConfig config=XMLTemplateService.getDeepWaterTimeConfig(termId);
		long endTime = 0;
        long startTime = 0;
        try
        {
        	startTime =  df.parse(config.getBeginTime()).getTime();
            endTime = df.parse(config.getEndTime()).getTime();
        }catch (ParseException e)
        {
            e.printStackTrace();
        }
        int lastDays=(Util.getNextDayMarshal(endTime/1000)-Util.getNextDayMarshal(current_time/1000))/(24*60*60)+1;
        return lastDays;
	}
	
	//每日充值深海探宝活动信息
	public static void resetDeepWaterTimes(UserStats userStats){
		Map<String, Integer> deepWaterInfo= Util.jsonToMap(userStats.getDeepWaterLastTimes());
		int[] configer1 = XMLTemplateService.getDeepWaterConfig(1);
		int[] configer2 = XMLTemplateService.getDeepWaterConfig(2);
		int[] configer3 = XMLTemplateService.getDeepWaterConfig(3);
		deepWaterInfo.put("1", configer1[3]);
		deepWaterInfo.put("2", configer2[3]);
		deepWaterInfo.put("3", configer3[3]);
		userStats.setDeepWaterLastTimes(Util.mapToString(deepWaterInfo));
	}
	//重置深海探宝信息和用户抽取积累额度
	public static void initDeepWaterInfo(UserStats userStats){
		resetDeepWaterTimes(userStats);
		Map<String, Integer> deepWaterHeap=Util.jsonToMap(userStats.getDeepWaterHeap());
		deepWaterHeap.put("1", 0);
		deepWaterHeap.put("2", 0);
		deepWaterHeap.put("3", 0);
		userStats.setDeepWaterHeap(Util.mapToString(deepWaterHeap));
	}
	public static void getReward(Player player, Builder builder, int kind ,int rewardType, int lvl, int itemId, int rewardCount) {
		int type =UserToolConst.getType(rewardType);
		int skill_level=1;
		if(lvl>0){
			skill_level=lvl;
		}
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
                    case UserToolConst.USER_PROPERTY_EXP:
                    	Util.getUserLevelByExp(player, rewardCount);
                        builder.setAddExp(rewardCount);
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
                    OPDeepWaterExploreRet.DiscipleInfo.Builder discipleInfo=OPDeepWaterExploreRet.DiscipleInfo.newBuilder();
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
            		OPDeepWaterExploreRet.SkillInfo.Builder skillInfo=OPDeepWaterExploreRet.SkillInfo.newBuilder();
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
        			OPDeepWaterExploreRet.SoulInfo.Builder soulInfo=OPDeepWaterExploreRet.SoulInfo.newBuilder();
                    OPUserSoulInfoRet.SoulInfo.Builder soulBuilder=SoulServices.addSoul(player, itemId, 0);//前面已完成魂魄数量添加,故添加次数为0
                    soulInfo.setId(soulBuilder.getId());
                    soulInfo.setItemid(soulBuilder.getItemid());
                    soulInfo.setCount(soulBuilder.getCount());
                    builder.addSoulInfo(soulInfo);
            	}
            	break;
            case UserToolConst.SOUL_TYPE:
            	//魂魄信息
                OPDeepWaterExploreRet.SoulInfo.Builder soulInfo=OPDeepWaterExploreRet.SoulInfo.newBuilder();
                OPUserSoulInfoRet.SoulInfo.Builder soulBuilder=SoulServices.addSoul(player, itemId, rewardCount);
                soulInfo.setId(soulBuilder.getId());
                soulInfo.setItemid(soulBuilder.getItemid());
                soulInfo.setCount(soulBuilder.getCount());
                builder.addSoulInfo(soulInfo);
                break;
            case UserToolConst.SKILL_TYPE:
            	//如果没有指定id,则进行随机抽取
            	if(itemId==0){
            		SkillSelecter skillSelecter=null;
            		switch (rewardType) {
					case UserToolConst.SKILL:
						skillSelecter=XMLTemplateService.getAllSkillSelecter();
						break;
					case UserToolConst.A_SKILL:
						skillSelecter=XMLTemplateService.getSkillSelecter(1);
						break;
					case UserToolConst.B_SKILL:
						skillSelecter=XMLTemplateService.getSkillSelecter(2);
						break;
					case UserToolConst.C_SKILL:
						skillSelecter=XMLTemplateService.getSkillSelecter(3);
						break;
					case UserToolConst.D_SKILL:
						skillSelecter=XMLTemplateService.getSkillSelecter(4);
						break;
					default:
						break;
					}
            		List<Integer> skillRateList=new ArrayList<Integer>();
            		switch (kind) {
					case 1:
						skillRateList=skillSelecter.getDeepWaterRat1();
						break;
					case 2:
						skillRateList=skillSelecter.getDeepWaterRat2();
						break;
					case 3:
						skillRateList=skillSelecter.getDeepWaterRat3();
						break;

					default:
						break;
					}
            		double skillRandom = Math.random()*(skillRateList.get(skillRateList.size()-1));
                    int skillLen = skillRateList.size();
                    int skillSelectedNo=0;
                    for(int i=0;i<skillLen;i++){
                        if(skillRandom<skillRateList.get(i)){
                            skillSelectedNo = i;
                            break;
                        }
                    }
                    itemId=skillSelecter.getId().get(skillSelectedNo);
            	}
            	
            	//技能信息
	            UserSkillInfo.OPUserSkillInfoRet.SkillInfo.Builder skillbuilder=SkillServices.createSkillWithLevel(player, itemId, skill_level);
	            OPDeepWaterExploreRet.SkillInfo.Builder skillInfo=OPDeepWaterExploreRet.SkillInfo.newBuilder();
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
	            OPDeepWaterExploreRet.EquipInfo.Builder equipInfo=OPDeepWaterExploreRet.EquipInfo.newBuilder();
	            OPUserEquipInfoRet.EquipInfo.Builder equipBuilder=EquipServices.createEquitAlone(player, itemId);
	            OPUserEquipInfoRet.EquipInfo.PropertyInfo.Builder propertyBuilder=equipBuilder.getPInfoBuilder(0);
	            OPDeepWaterExploreRet.EquipInfo.PropertyInfo.Builder propertyInfo=OPDeepWaterExploreRet.EquipInfo.PropertyInfo.newBuilder();
	            propertyInfo.setType(propertyBuilder.getType());
	            propertyInfo.setNum(propertyBuilder.getNum());
	            equipInfo.addPInfo(propertyInfo);
	            equipInfo.setId(equipBuilder.getId());
	            equipInfo.setItemid(equipBuilder.getItemid());
	            equipInfo.setLevel(equipBuilder.getLevel());
	            equipInfo.setRefinexp(equipBuilder.getRefinexp());
	            equipInfo.setRefinelevel(equipBuilder.getRefinelevel());
	            //equipInfo.setStoneinlaidinfo(equipBuilder.getStoneinlaidinfo());
                for(int i=0;i<equipBuilder.getStoneInfoList().size();++i)
                {
                    equipInfo.addStoneInfo(equipBuilder.getStoneInfoList().get(i));
                }
                equipInfo.setBuffvalue(equipBuilder.getBuffvalue());
                builder.addEquipInfo(equipInfo);
                break;
            case UserToolConst.TOOLS_TYPE:
            	//道具信息
                OPDeepWaterExploreRet.ToolInfo.Builder toolInfo=OPDeepWaterExploreRet.ToolInfo.newBuilder();
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
                OPDeepWaterExploreRet.SkillBookPartItem.Builder partItem=OPDeepWaterExploreRet.SkillBookPartItem.newBuilder();
                if(itemId==0){
                	List<Integer> skillbookRateList=new ArrayList<Integer>();
                	SkillbookSelecter skillbookSelecter=XMLTemplateService.getSkillbookSelecter();
            		switch (kind) {
					case 1:
						skillbookRateList=skillbookSelecter.getDeepWaterRat1();
						break;
					case 2:
						skillbookRateList=skillbookSelecter.getDeepWaterRat2();
						break;
					case 3:
						skillbookRateList=skillbookSelecter.getDeepWaterRat3();
						break;

					default:
						break;
					}
            		double skillRandom = Math.random()*(skillbookRateList.get(skillbookRateList.size()-1));
                    int skillLen = skillbookRateList.size();
                    int skillbookSelectedNo=0;
                    for(int i=0;i<skillLen;i++){
                        if(skillRandom<skillbookRateList.get(i)){
                            skillbookSelectedNo = i;
                            break;
                        }
                    }
                    itemId=skillbookSelecter.getId().get(skillbookSelectedNo);
                }
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

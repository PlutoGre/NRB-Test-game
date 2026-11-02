package com.oldmountain.dzm.util;

import com.oldmountain.dzm.configEntity.DiamondConfig;
import gnu.trove.map.TIntObjectMap;

import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.Date;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Random;

import com.oldmountain.dzm.entity.*;
import com.oldmountain.dzm.message.UserRewardInfo;
import com.oldmountain.dzm.packethandler.AdventureContinueLoginHandler;
import com.oldmountain.dzm.packethandler.UserInvitedHandler;
import com.oldmountain.dzm.services.SkillBookServices;
import net.sf.json.JSONObject;

import com.oldmountain.dzm.entity.ActivityTimeConfig;
import com.oldmountain.dzm.entity.Battle;
import com.oldmountain.dzm.entity.Career;
import com.oldmountain.dzm.entity.ChargeRebateXmlTemplate;
import com.oldmountain.dzm.entity.ContinueLoginXmlTemplate;
import com.oldmountain.dzm.entity.Disciple;
import com.oldmountain.dzm.entity.DiscipleXmlTemplate;
import com.oldmountain.dzm.entity.Equip;
import com.oldmountain.dzm.entity.EquipXmlTemplate;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.entity.Skill;
import com.oldmountain.dzm.entity.SkillXmlTemplate;
import com.oldmountain.dzm.entity.Soul;
import com.oldmountain.dzm.entity.UserStats;
import com.oldmountain.dzm.message.OP;
import com.oldmountain.dzm.message.UserBattleArray.OPUserBattleArrayRet;
import com.oldmountain.dzm.message.UserUpgrade.OPUserUpgrade;
import com.oldmountain.dzm.net.ObjectAccessor;

public class Util {
	/**
	 * 判断两个时间time1和time2是否为同一天
	 * @param time1 timestamp 秒数
	 * @param time2 timestamp 秒数
	 * @return
	 */
	private static int todayZeroTimeStamp = 0;
	
	public static int getTodayZeroTimeStamp(){
		if (getServerTime() - todayZeroTimeStamp >= 86400) {
			Calendar calendar = Calendar.getInstance();
			calendar.set(Calendar.HOUR_OF_DAY, 0);
			calendar.set(Calendar.MINUTE, 0);
			calendar.set(Calendar.SECOND, 0);
			calendar.set(Calendar.MILLISECOND, 0);
			todayZeroTimeStamp=(int)(Math.floor(calendar.getTimeInMillis()/1000));
		}
		return todayZeroTimeStamp;
	}
	
	public static Boolean isSameDay(int time1,int time2){
		if (Math.abs(time1-time2) > 24 * 3600) {
			return false;
		}
		Calendar calendar = Calendar.getInstance();
		calendar.setTimeInMillis((long)time1 * 1000);
		int day1 = calendar.get(Calendar.DAY_OF_MONTH);
		calendar.setTimeInMillis((long)time2 * 1000);
		int day2 = calendar.get(Calendar.DAY_OF_MONTH);
		if (day1 != day2) {
			return false;
		}
		return true;
	}
	
	public static Boolean isNextDay(int time1,int time2){
		if (Math.abs(time1-time2) > 48 * 3600) {
			return false;
		}
		SimpleDateFormat formatter = new SimpleDateFormat("yyyyMMdd");
        
		Date date1 = new Date((long)time1 * 1000);
		Date date2 = new Date((long)time2 * 1000);
		String dateString1 = formatter.format(date1);
		String dateString2 = formatter.format(date2);
		
		try {
			Long ctime1 = formatter.parse(dateString1).getTime();
			Long ctime2 = formatter.parse(dateString2).getTime();
			if (Math.abs(ctime1 - ctime2) == 24 * 3600 * 1000) {
				return true;
			}
		} catch (Exception e) {
			e.printStackTrace();
		}
		return false;
	}
	
	public static Map jsonToMap(String jsonString){
		JSONObject jb = JSONObject.fromObject(jsonString);
		Iterator keyIter = jb.keys();  
		String key;  
        Object value;  
        Map valueMap = new HashMap();  
        while (keyIter.hasNext()) {  
            key = (String)keyIter.next();  
            value = jb.get(key);  
            valueMap.put(key, value);  
        } 
        return valueMap;
	}
	
	public static String mapToString(Map jsonMap){
		JSONObject jsonString = JSONObject.fromObject(jsonMap);
		return jsonString.toString();
	}
	
	public static void getUserLevelByExp(Player player,int addExp){
		int addLevel = 0;
		int level = 1;
		int levelCount = XMLTemplateService.getUserLevelCount();
		int currentLevel = player.getLevel();
		if (currentLevel >= levelCount) {
			int exp = player.getExp() + addExp;
			if (exp > XMLTemplateService.getUserLevelExp(levelCount) - 1) {
				exp = XMLTemplateService.getUserLevelExp(levelCount) - 1;
			}
			player.setLevel(levelCount);
			player.setExp(exp);
		}
		else 
		{
			int exp = player.getExp() + addExp;
			player.setExp(exp);
			int currentLevelExp = XMLTemplateService.getUserLevelExp(currentLevel);
			if (exp >= XMLTemplateService.getUserLevelExp(levelCount)) {
				level = levelCount;
			}
			else if (exp < currentLevelExp) {
				level = currentLevel;
			}
			else {
				level = getUserLevel(currentLevel + 1,levelCount,exp);
			}
			String battleIds = player.getBattleids();
			player.setExp(exp);
			if (level > currentLevel) {//玩家升级
				OPUserUpgrade.Builder builder = OPUserUpgrade.newBuilder();
				int positionCount = BattleUtil.getBattleCountByLevel(currentLevel);
				int newCount = BattleUtil.getBattleCountByLevel(level);
				if (newCount > positionCount) {
					int addCount = newCount - positionCount;
					if (addCount > 1) {
						System.out.println(addCount);
					}
					for (int i = 0; i < addCount; i++) {
						Battle battle = new Battle();
						battle.setPlayerid(player.getId());
						player.addOneBattle(battle);
						battleIds = battleIds + "," + battle.getId();
						ObjectAccessor.getBattleArray(player.getId()).getBattlearrays().add(battle);
						OPUserBattleArrayRet.BattleArrayInfo.Builder battleBuilder = OPUserBattleArrayRet.BattleArrayInfo.newBuilder();
						BattleUtil.buildBattleBuilder(battleBuilder, battle, positionCount+i);
						builder.setNewBattle(battleBuilder);
					}
	                player.setBattleCount(newCount);
					player.setBattleids(battleIds);
					builder.setBattleCount(newCount);
				}
				Challenge challenge = player.getChallenge();
				if (challenge != null) {
					challenge.setLevel(level);
					player.setChallenge(challenge);
				}
				addLevel = level - currentLevel;
				int addCoins = XMLTemplateService.getUserLevelConfig(level).getCoins1() * 5;
				builder.setLevel(level);
				builder.setExp(exp);
				builder.setAddSilverCoins(addCoins);
				builder.setAddGold(10 * addLevel);
				player.setSysgoldcoins(player.getSysgoldcoins() + 10 * addLevel);
				player.setSilvercoins(player.getSilvercoins() + addCoins);
				player.setLevel(level);//***********
				player.sendPacket(OP.code.OPCODE_USER_UPGRADE_S_VALUE, builder);
				
				//判断是否是12级，需要给邀请者增加计数
				if (level == 12 && player.getInvitedid() != 0) {
					UserInvitedHandler.updateInviteStatus(player.getInvitedid(),2, 1);
				}
			}
		}
	}
	
	private static int getUserLevel(int start,int end,int exp){
		int level = 0;
		if (start + 1 >= end) {
			level = end;
		}
		else if (exp <= XMLTemplateService.getUserLevelExp(start)) {
			level = start;
		}
		else if (exp == XMLTemplateService.getUserLevelExp(end)) {
			level = end;
		}
		else {
			int checkPosition = (int)Math.floor((start + end)/2);
			if (exp >XMLTemplateService.getUserLevelExp(checkPosition)) {
				start = checkPosition;
				return getUserLevel(start, end, exp);
			}
			else if (exp == XMLTemplateService.getUserLevelExp(checkPosition)) {
				level = checkPosition + 1;
			}
			else {
				end = checkPosition;
				return getUserLevel(start, end, exp);
			}
		}
		return level;
	}
	
	
	public static int getlevelByExp(Disciple disciple,int addExp){
		int itemID = disciple.getItemid();
		int levelCount = XMLTemplateService.getDiscipleLevelCount();
		DiscipleXmlTemplate discipleInfo = XMLTemplateService.getDiscipleTemplate(itemID);
		int newLevel = disciple.getLevel();
		int newExp = disciple.getExp() + addExp;
		
		if (disciple.getLevel() < levelCount) {
			newLevel = getDiscipleLevel(newExp, discipleInfo.getDefaultExp(),disciple.getLevel()); 
		}
		int addLevel = newLevel - disciple.getLevel();
		
		if (newLevel >= levelCount) {
			if ((float)newExp/discipleInfo.getDefaultExp() >= XMLTemplateService.getDiscipleLevelConfig(levelCount)) {
				newExp = (int)Math.floor(XMLTemplateService.getDiscipleLevelConfig(levelCount) * discipleInfo.getDefaultExp());
			}
		}
		
		disciple.setExp(newExp);
		
		if (addLevel > 0) {
			disciple.setLevel(newLevel);
			disciple.setAttack(disciple.getAttack() + addLevel * discipleInfo.getGrowAttack());
			disciple.setHealth(disciple.getHealth() + addLevel * discipleInfo.getGrowHealth());
			disciple.setDefence(disciple.getDefence() + addLevel * discipleInfo.getGrowDefence());
			disciple.setRp(disciple.getRp() + addLevel * discipleInfo.getGrowRp());
			disciple.setPotentiality(disciple.getPotentiality() + addLevel * discipleInfo.getDefaultPotentiality());
		}
		
		return addLevel;
	}
	
	public static int getDiscipleLevel(int exp,int baseExp,int current_level){
		int level = 1;
		float multi = (float)(exp) / baseExp;
		int levelCount = XMLTemplateService.getDiscipleLevelCount();
		if (multi >= XMLTemplateService.getDiscipleLevelConfig(levelCount)) {//超过表最大等级
			level = levelCount;
		}
		else if(multi < XMLTemplateService.getDiscipleLevelConfig(current_level)){//还不够升级
			level = current_level;
		}
		else {
			level = getLevel(current_level, levelCount, multi);
		}
		
		return level;
	}
	
	private static int getLevel(int start,int end,float seed){
		int level = 0;
		if (start + 1 == end) {
			level = end;
		}
		else if (seed == XMLTemplateService.getDiscipleLevelConfig(start)) {
			level = start+1;
		}
		else if (seed == XMLTemplateService.getDiscipleLevelConfig(end)) {
			level = end;
		}
		else {
			int checkPosition = (int)Math.floor((start + end)/2);
			if (seed >XMLTemplateService.getDiscipleLevelConfig(checkPosition)) {
				start = checkPosition;
				return getLevel(start, end, seed);
			}
			else if (seed == XMLTemplateService.getDiscipleLevelConfig(checkPosition)) {
				level = checkPosition + 1;
			}
			else {
				end = checkPosition;
				return getLevel(start, end, seed);
			}
			
		}
		return level;
	}
	
	public static int getServerTime(){
		int current_time = (int)(Math.floor(System.currentTimeMillis()/1000));
		return current_time;
	}
	
	public static long getServerMillTime(){
		return System.currentTimeMillis();
	}
	
	public static int getRandResult(List<Integer> randList){
		List<Integer> rand = new ArrayList<Integer>();
		int rate = 0;
		for (int i = 0; i < randList.size(); i++) {
			rate += randList.get(i);
			rand.add(rate);
		}
		
		Random random = new Random();
		int a = random.nextInt(rate) + 1;
		int index = 0;
		for (int i = 0; i < rand.size(); i++) {
			if (a <= rand.get(i)) {
				index = i;
				break;
			}
		}
		return index;
	}

	
	public static int rewardEquip(Player player,int id){
		EquipXmlTemplate equipInfo = XMLTemplateService.getEquipTemplate(id);
		Equip equip = new Equip();
		equip.setPlayerid(player.getId());
		equip.setType(equipInfo.getType());
		equip.setItemid(id);
		if (equipInfo.getType() == 1) {
			equip.setAttack(equipInfo.getQuantity());
		}
		else if (equipInfo.getType() == 2) {
			equip.setDefence(equipInfo.getQuantity());
		}
		else {
			equip.setHealth(equipInfo.getQuantity());
		}
        //modify by dylan at 20130710 because 宝石镶嵌改造
        if(DiamondConfig.DiamondOpen())
        {
            equip.generateDiamonInfo();
        }
		player.addOneEquip(equip);
		return equip.getId();
	}
	
	public static int rewardSkill(Player player,int id){
		SkillXmlTemplate skillInfo = XMLTemplateService.getSkillTemplate(id);
		Skill skill = new Skill();
		skill.setPlayerid(player.getId());
		skill.setItemid(id);
		skill.setQuantity(skillInfo.getQuantity());
		skill.setType(skillInfo.getType());
		player.addOneSkill(skill);
		return skill.getId();
	}
	public static int rewardSkillWithLevel(Player player,int id,int level){
		SkillXmlTemplate skillInfo = XMLTemplateService.getSkillTemplate(id);
		Skill skill = new Skill();
		skill.setPlayerid(player.getId());
		skill.setItemid(id);
		skill.setQuantity(skillInfo.getQuantity()+(level-1)*skillInfo.getGrowth());
		skill.setType(skillInfo.getType());
		int consume=(int) Math.scalb(skillInfo.getQuality(), level);
		skill.setConsume(consume);
		skill.setLevel(level);
		player.addOneSkill(skill);
		return skill.getId();
	}

    public static int rewardSoul(Player player,int itemId,int count){
        HashMap<Integer, Soul> userSouls = player.getSouls();
        Soul soul;
        if(userSouls.containsKey(itemId))
        {
            soul = userSouls.get(itemId);
            soul.setCount(soul.getCount() + count);
            player.updateOneSoul(soul);
        }else
        {
            soul = new Soul();
            soul.setItemid(itemId);
            soul.setPlayerid(player.getId());
            soul.setCount(count);
            player.addOneSoul(soul);
        }
        return soul.getCount();
    }
    public static int rewardBagTool(Player player,int itemId,int count){
        HashMap<Integer, Bag> userBagTools = player.getBags();
        Bag tool = new Bag();
        if(userBagTools.containsKey(itemId))
        {
            tool = userBagTools.get(itemId);
            tool.setCount(tool.getCount() + count);
            player.updateOneBag(tool);
        }else
        {
            int current_time = (int)(Math.floor(System.currentTimeMillis()/1000));
            tool.setCount(count);
            tool.setItemid(itemId);
            tool.setPlayerid(player.getId());
            tool.setCreatetime(current_time);
            player.addOneBag(tool);
        }
        return tool.getCount();
    }
    
    public static int decreaseBagTool(Player player,int id,int count){
        HashMap<Integer, Bag> userTools = player.getBags();
        Bag bag = userTools.get(id);
        if (bag.getCount() < count) {
           return -1;
        }
        bag.setCount(bag.getCount() - count);
        player.updateOneBag(bag);
        return bag.getCount();
    }
    public static UserRewardInfo.OPRewardInfoRet.RewardInfo.Builder getRewardInfoBuilder(int type,int id,int count){
        UserRewardInfo.OPRewardInfoRet.RewardInfo.Builder _rewardBuilder = UserRewardInfo.OPRewardInfoRet.RewardInfo.newBuilder();
        _rewardBuilder.setItemType(type);
        _rewardBuilder.setItemId(id);
        _rewardBuilder.setCount(count);
        return _rewardBuilder;
    }

	
	public static ArrayList<Integer> addRewards(Player player,int[] rewards){
		int rewardType = rewards[0];
		int rewardId = rewards[1];
		int rewardCount = rewards[2];
		ArrayList<Integer> returnList = new ArrayList<Integer>();
		switch (rewardType) {
			case 11001:
				switch (rewardId) {
					case 1001://奖励元宝
						player.setSysgoldcoins(player.getSysgoldcoins() + rewardCount);
						returnList.add(5);
						break;
				}
				break;
			case 12001:
				switch (rewardId) {
					case 2002://奖励银两
						UserLevelConfig config = XMLTemplateService.getUserLevelConfig(player.getLevel());
						rewardCount *= config.getCoins1();
						player.setSilvercoins(player.getSilvercoins() + rewardCount);
						returnList.add(6);
						break;
				}
				break;
			case 21001:
				break;
			case 22001:
				break;
			case 31000:
			case 31001:
			case 31002:
			case 31003:
			case 31004:
				break;
			case 32000:
			case 32001:
			case 32002:
			case 32003:
			case 32004:
				rewardSoul(player, rewardId, rewardCount);
				returnList.add(4);
				returnList.add(rewardId);
				player.setFirstGainByType("soul");
				break;
			
			case 41000:
			case 41001:
			case 41002:
			case 41003:
			case 41004:
				int skillId = rewardSkill(player, rewardId);
				returnList.add(3);
				returnList.add(skillId);
				break;
			case 42000:
			case 42001:
			case 42002:
			case 42003:
			case 42004:
                SkillBookServices.obtainSkillBook(player,rewardId);
                player.setFirstGainByType("skillBook");
                returnList.add(5);
                returnList.add(rewardId);
				break;
			case 50000:
			case 51000:
			case 51001:
			case 51002:
			case 51003:
			case 51004:
			case 52000:
			case 52001:
			case 52002:
			case 52003:
			case 52004:
			case 53000:
			case 53001:
			case 53002:
			case 53003:
			case 53004:
				int equipId = rewardEquip(player, rewardId);
				returnList.add(2);
				returnList.add(equipId);
				break;
			case 61001:
				rewardBagTool(player, rewardId, rewardCount);
				returnList.add(0);
				returnList.add(rewardId);
				break;
			case 71001:
				break;
				
		}
		returnList.add(rewardCount);
		return returnList;
	}
	
	public static void dailyFirstLogin(Player player){
		if (player.getLastlogin() > getTodayZeroTimeStamp()) {
			return;
		}
		//重置十里抽一次数
		player.setTodayleavetimes(5);
		
		//重置江湖次数统计
		Career career = player.getCareer();
		career.setPointcount("{}");
		player.setCareer(career);
		
		//重置今日吃鸡数量
		player.setTodaychickennum(0);
		player.setTodaypelletnum(0);
		player.setTodaybuychickennum(0);
		player.setTodaybuypelletnum(0);
		
		//重置连续登陆奖励
		UserStats stats = player.getUserStats();
		stats.setGetreward(0);
        stats.setHasVisited(0);
        boolean _isIncrease=false;
		if (isNextDay(getServerTime(), player.getLastlogin()))
        {
			stats.setLogincount(stats.getLogincount() + 1);
            _isIncrease=true;
		}
		else {
			stats.setLogincount(1);
		}
        AdventureContinueLoginHandler.setAdventureContinueLoginStats(player,_isIncrease,false);
        if (stats.getLogincount() > 3) {
			stats.setLogincount(3);
		}
		String rewards = getContinueReward(stats.getLogincount(), player.getLevel());
		stats.setLoginreward(rewards);
		
		//充值奇遇中叫花鸡使用情况
		stats.setAmeatfood(0);
		stats.setPmeatfood(0);
		
		//重置论剑挑战次数
		///VIP10以上的尊享礼包内容翻倍		VIP10以上所有特权+10，（例：原VIP10每日初始挑战14次+10次）
		int addCount = 4;
		if (player.getViplevel() > 9)
		{
			addCount = 10;
		}
		
		stats.setChallengetimes(player.getViplevel() + addCount);
		
		//重置充值返利奖励
		stats.setGetRebate(0);
		
		player.setApplyLeaguaNum(0);
		
		player.setUserStats(stats);
		
		if (getTodayZeroTimeStamp() - player.getRegistertime() < 24 * 3600 && player.getInvitedid() != 0) {
			UserInvitedHandler.updateInviteStatus(player.getInvitedid(), 1, 1);
		}
	}
	
	private static String getContinueReward(int loginDays,int level){
		String string = "";
		Random random = new Random();
		for (int i = 1; i <= loginDays; i++) {
			ContinueLoginXmlTemplate config = XMLTemplateService.getContinueLoginConfig(i);
			ArrayList<Integer> rates = config.getRates();
			int a = random.nextInt(rates.get(rates.size() - 1)) + 1;
			int index = rates.size() - 1;
			for (int j = 0; j < rates.size(); j++) {
				if (rates.get(j) > a) {
					index = j;
					break;
				}
			}
			int type = config.getTypes().get(index);
			int id = config.getIds().get(index);
			int count = config.getCounts().get(index);
			if (id == 0) {
				int[] rewards = getRewardIdByType(type, id, count, level, 1,-1);
				id = rewards[0];
				count = rewards[1];
			}
			if (i == 1) {
				string = type + ":" + id + ":" + count;
			}
			else {
				string +=  "," + type + ":" + id + ":" + count;
			}
		}
		return string;
	}
	
	public static int[] getRewardIdByType(int type,int id,int count,int level,int pool,int chapterId){
		switch (type) {
			case 12001://等级对应属性
				if (id == 2002) {
					count *= XMLTemplateService.getUserLevelConfig(level).getCoins1();
				}
				break;
			case 21001:
				break;
			case 22001:
				break;
			case 31000:
				break;
			case 31001:
				break;
			case 31002:
				break;
			case 31003:
				break;
			case 31004:
				break;
			case 32000:
				break;
			case 32001:
				if (id == 0) {
					id = SoulUtil.randSoulId(1, pool);
				}
				break;
			case 32002:
				if (id == 0) {
					id = SoulUtil.randSoulId(2, pool);
				}
				break;
			case 32003:
				if (id == 0) {
					id = SoulUtil.randSoulId(3, pool);
				}
				break;
			case 32004:
				if (id == 0) {
					id = SoulUtil.randSoulId(4, pool);
				}
				break;
			case 41000:
				break;
			case 41001:
				break;
			case 41002:
				break;
			case 41003:
				break;
			case 41004:
				break;
			case 42000:
                if(id==0)
                {
                    id = SkillBookServices.getRandomSkillBookByChapter(chapterId,-1);
                }
				break;
			case 42001:
                if(id==0)
                {
                    id = SkillBookServices.getRandomSkillBookByChapter(chapterId,1);
                }
				break;
			case 42002:
                if(id==0)
                {
                    id = SkillBookServices.getRandomSkillBookByChapter(chapterId,2);
                }
				break;
			case 42003:
                if(id==0)
                {
                    id = SkillBookServices.getRandomSkillBookByChapter(chapterId,3);
                }
				break;
			case 42004:
                if(id==0)
                {
                    id = SkillBookServices.getRandomSkillBookByChapter(chapterId,4);
                }
				break;
			case 50000:
				break;
			case 51000:
				break;
			case 51001:
				break;
			case 51002:
				break;
			case 51003:
				break;
			case 51004:
				break;
			case 52000:
				break;
			case 52001:
				break;
			case 52002:
				break;
			case 52003:
				break;
			case 52004:
				break;
			case 53000:
				break;
			case 53001:
				break;
			case 53002:
				break;
			case 53003:
				break;
			case 53004:
				break;
			case 71001:
				break;
		}
		int[] returns = new int[]{id,count};
		return returns;
	}
	
	public static int getChallengeCountLimitByVipLevel(int vipLevel){
		
		///VIP10以上的尊享礼包内容翻倍		VIP10以上所有特权+10，（例：原VIP10每日初始挑战14次+10次）
		int addCount = 4;
		if (vipLevel > 9)
		{
			addCount = 10;
		}
		
		return vipLevel + addCount;
	}
	
	public static void calcPowerAndVality(Player player){
		int serverTime = Util.getServerTime();
		if (player.getPowerbychicken() + player.getPowerbytime() >= 30) {
			player.setPoweraddtime(Util.getServerTime());
		}
		else {
			int maxAdd = 30 - player.getPowerbytime() - player.getPowerbychicken();
			int canAdd = (serverTime - player.getPoweraddtime())/1800;
			if (canAdd > 0) {
				int addNum = Math.min(maxAdd, canAdd);
				player.setPowerbytime(player.getPowerbytime() + addNum);
				player.setPoweraddtime(player.getPoweraddtime() + canAdd * 1800);
			}
		}
		
		if (player.getVitalitybypellet() + player.getVitalitybytime() >= 12) {
			player.setVitalityaddtime(Util.getServerTime());
		}
		else {
			int maxAdd = 12 - player.getVitalitybypellet() - player.getVitalitybytime();
			int canAdd = (serverTime - player.getVitalityaddtime())/1800;
			if (canAdd > 0) {
				int addNum = Math.min(maxAdd, canAdd);
				player.setVitalitybytime(player.getVitalitybytime() + addNum);
				player.setVitalityaddtime(player.getVitalityaddtime() + canAdd * 1800);
			}
		}
	}

    public static <T> int getRandomResultIndexFromInArray(ArrayList<T> list){
        int index = 0;
        int skillLen = list.size();
        double temp = 0.0;
        ArrayList<Double> rateList = new ArrayList<Double>();
        for(int j=0;j<skillLen;j++)
        {
            temp = Double.parseDouble(list.get(j).toString())+temp;
            rateList.add(j,temp);
        }
        double skillRandom = Math.random()*(temp);
        for(int i=0;i<skillLen;i++)
        {
            if(skillRandom<rateList.get(i))
            {
                index = i;
                break;
            }
        }
        return index;
    }
    //获取当天夜晚24:00的时间
    public static int getNextDayMarshal(long time){
    	Date date=new Date(time*1000);
    	SimpleDateFormat dateFormat=new SimpleDateFormat("yyyy-MM-dd");
    	String day=dateFormat.format(date);
    	try {
			date=dateFormat.parse(day);
		} catch (ParseException e) {
			e.printStackTrace();
		}
		int time2=(int)(Math.floor(date.getTime()/1000))+24*60*60;
    	return time2;
    }
    //玩家处在充值返利活动--储值阶段,返回的是期数(0表示没在此阶段)
    public static int inChargeDuration(Player player,int currentTime){
    	int times=0;
    	int instructionFinishTime=player.getInstructionFinishTime();
    	int primaryTime=getNextDayMarshal((long)instructionFinishTime);
    	int size=XMLTemplateService.getChargeRebateXmlTemplatesSize();
    	for(int i=1;i<=size;i++){
    		ChargeRebateXmlTemplate template=XMLTemplateService.getChargeRebateXmlTemplate(i);
    		int activeTimeInterval=template.getActiveTimeInterval();
    		int chargeDuration =template.getChargeDuration();
    		int begainTime=0;
    		int endTime=0;
    		if(i==1){
    			if(activeTimeInterval==0){
    				begainTime=instructionFinishTime;
    				endTime=primaryTime+(activeTimeInterval+chargeDuration)*24*60*60;
    			}
    			else{
    				begainTime=primaryTime+activeTimeInterval*24*60*60;
    				endTime=begainTime+chargeDuration*24*60*60;
    			}
    		}else{
	    		begainTime=primaryTime+activeTimeInterval*24*60*60;
	    		endTime=begainTime+chargeDuration*24*60*60;
    		}
    		if(currentTime>=begainTime && currentTime<endTime){
    			times=i;
    			break;
    		}
    	}
    	return times;
    }
    //玩家处在充值返利活动--返利阶段,返回的是期数(0表示没在此阶段)
    public static int inRebateDuration(Player player,int currentTime){
    	int times=0;
    	int instructionFinishTime=player.getInstructionFinishTime();
    	int primaryTime=getNextDayMarshal((long)instructionFinishTime);
    	int size=XMLTemplateService.getChargeRebateXmlTemplatesSize();
    	int minLimit=0;
    	for(int i=1;i<=size;i++){
    		ChargeRebateXmlTemplate template=XMLTemplateService.getChargeRebateXmlTemplate(i);
    		int activeTimeInterval=template.getActiveTimeInterval();
    		int chargeDuration =template.getChargeDuration();
    		int rebateDuration =template.getRebateDuration();
    		int minRecharge =template.getMinRecharge();
    		int begainTime=primaryTime+(activeTimeInterval+chargeDuration)*24*60*60;
    		int endTime=begainTime+rebateDuration*24*60*60;
    		
    		if(currentTime>=begainTime && currentTime<endTime){
    			minLimit=minRecharge;
    			times=i;
    			break;
    		}
    	}
    	if(times!=0 && player.getUserStats().getChargeHeap()<minLimit){
    		times=0;
    	}
    	return times;
    }
    public static int[] getChargeRebateBeginEndTime(Player player , int termId){
    	ChargeRebateXmlTemplate chargeRebateXmlTemplate=XMLTemplateService.getChargeRebateXmlTemplate(termId);
		int activeTimeInterval=chargeRebateXmlTemplate.getActiveTimeInterval();	//活动间隔时间(天)
		int chargeDuration=chargeRebateXmlTemplate.getChargeDuration();	//储值累积时间(天)
		int rebateDuration=chargeRebateXmlTemplate.getRebateDuration();	//返利时间(天)
		
		int instructionFinishTime=player.getInstructionFinishTime();	//玩家完成新手指引的时间+48小时
		int primaryTime=Util.getNextDayMarshal((long)instructionFinishTime); //指定时间晚上24:00:00的时间
		int beginTime=0;	//当前返利开始时间
		int endTime=0;	//当前返利结束时间
		if(termId==1){
			//新手充值返利
			if(activeTimeInterval==0){
				beginTime=instructionFinishTime;
				endTime=primaryTime+(activeTimeInterval+chargeDuration+rebateDuration)*24*60*60;
			}
			else{
				beginTime=primaryTime+activeTimeInterval*24*60*60;
				endTime=beginTime+(chargeDuration+rebateDuration)*24*60*60;
			}
		}else{
			beginTime=primaryTime+activeTimeInterval*24*60*60;
			endTime=beginTime+(chargeDuration+rebateDuration)*24*60*60;
		}
		int []beginEndTime=new int[2];
		beginEndTime[0]=beginTime;
		beginEndTime[1]=endTime;
    	return beginEndTime;
    }
    public static int getGiveSoulTermId(long current_time){
    	SimpleDateFormat df = new SimpleDateFormat("yyyy-MM-dd_HH:mm:ss");
    	int termId=0;
    	long beginTime=0;
    	long endTime=0;
    	int size=XMLTemplateService.getGiveSoulConfigSize();
    	for(int i=1;i<=size;i++){
    		GiveSoulConfig config=XMLTemplateService.getGiveSoulConfig(i);
    		try {
				beginTime=df.parse(config.getBeginTime()).getTime();
				endTime=df.parse(config.getEndTime()).getTime();
				if(current_time>=beginTime && current_time<=endTime){
					termId=config.getId();
					break;
				}
			} catch (ParseException e) {
				e.printStackTrace();
			}
    	}
    	return termId;
    }
    //adventure中相关活动的开启id查询
    public static int getActivityTermId(long current_time,TIntObjectMap<ActivityTimeConfig> configs){
    	SimpleDateFormat df = new SimpleDateFormat("yyyy-MM-dd_HH:mm:ss");
    	int termId=0;
    	long beginTime=0;
    	long endTime=0;
    	for(int i=1;i<=configs.size();i++){
    		ActivityTimeConfig config=configs.get(i);
    		try {
				beginTime=df.parse(config.getBeginTime()).getTime();
				endTime=df.parse(config.getEndTime()).getTime();
				if(current_time>=beginTime && current_time<=endTime){
					termId=config.getId();
					break;
				}
			} catch (ParseException e) {
				e.printStackTrace();
			}
    	}
    	return termId;
    }
    //获取adventure中相关活动的起止时间
    public static int[] getActivityTimeByTermid(TIntObjectMap<ActivityTimeConfig> configs,int termid){
    	SimpleDateFormat df = new SimpleDateFormat("yyyy-MM-dd_HH:mm:ss");
    	String beginTime=configs.get(termid).getBeginTime();
    	String endTime=configs.get(termid).getEndTime();
    	int[]times={0,0};
    	try {
			times[0]=(int)(Math.floor(df.parse(beginTime).getTime()/1000));
			times[1]=(int)(Math.floor(df.parse(endTime).getTime()/1000));
		} catch (ParseException e) {
			e.printStackTrace();
		}
    	
    	return times;
    	
    }
    //初始化玩家在幸运星活动的相关数据
    public static boolean initLuckyStarActivit(long current_time,Player player){
    	boolean needClear=false;
    	UserStats userStats=player.getUserStats();
    	int player_termId=userStats.getLuckyStarTermid();
    	int termId=Util.getActivityTermId(current_time, XMLTemplateService.getluckyStarTimeConfigs());
    	if(termId==0){
    		HashMap<Integer, Bag> bags=player.getBags();
    		LuckyStarXmlTemplate luckyStarXmlTemplate1=XMLTemplateService.getLuckyStarXmlTemplate(1);
    		LuckyStarXmlTemplate luckyStarXmlTemplate2=XMLTemplateService.getLuckyStarXmlTemplate(2);
    		Bag bag1=bags.get(luckyStarXmlTemplate1.getItemId());
    		Bag bag2=bags.get(luckyStarXmlTemplate2.getItemId());
    		if(bag1!=null){
    			bag1.setCount(0);
    			bags.put(luckyStarXmlTemplate1.getItemId(), bag1);
    			needClear=true;
    		}
    		if(bag2!=null){
    			bag2.setCount(0);
    			bags.put(luckyStarXmlTemplate2.getItemId(), bag2);
    			needClear=true;
    		}
    		player.setBags(bags);
    	}
    	//如果玩家的幸运星活动id与当前活动id不符,则进行初始化,并将id设为当前id
    	if( player_termId!=termId){
    		userStats.setLuckyStarDropTimes1(0);
    		userStats.setLuckyStarDropTimes2(0);
    		userStats.setLuckyStarTermid(termId);
    		userStats.setLuckyStayHeap1(0);
    		userStats.setLuckyStayHeap2(0);
    	}
    	player.setUserStats(userStats);
    	return needClear;
    }
}

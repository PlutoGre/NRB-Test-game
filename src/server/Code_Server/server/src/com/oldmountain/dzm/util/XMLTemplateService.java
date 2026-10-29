package com.oldmountain.dzm.util;

import com.oldmountain.dzm.configEntity.*;
import com.oldmountain.dzm.entity.*;
import com.oldmountain.dzm.message.FightEndInfo;
import com.oldmountain.dzm.message.GoddessRebate.OPGoddessGiveFlowerRet;
import com.oldmountain.dzm.packethandler.AdventureContinueLoginHandler;
import com.oldmountain.dzm.services.SkillBookServices;

import gnu.trove.iterator.TIntObjectIterator;
import gnu.trove.map.TIntObjectMap;
import gnu.trove.map.hash.TIntObjectHashMap;

import java.io.File;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.concurrent.ConcurrentHashMap;

import net.sf.json.JSONArray;
import net.sf.json.JSONObject;

import org.apache.commons.configuration.ConfigurationException;
import org.apache.commons.configuration.HierarchicalConfiguration;
import org.apache.commons.configuration.XMLConfiguration;
import org.apache.commons.lang.StringUtils;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import sun.print.resources.serviceui;

import com.oldmountain.dzm.DZM;


/**
 * 注意, 从config.xml中读取的路径应该是有效路径
 * @author miaoshengli
 *
 */
public class XMLTemplateService  {
	
	private static Logger log = LoggerFactory.getLogger(XMLTemplateService.class);
	
	private static HashMap<String, Integer> xmlLoadTime = new HashMap<String, Integer>();
	
	private static TIntObjectMap<ItemXmlTemplate> itemTemplates = new TIntObjectHashMap<ItemXmlTemplate>();
	private static TIntObjectMap<SkillXmlTemplate> skillTemplates = new TIntObjectHashMap<SkillXmlTemplate>();
	private static TIntObjectMap<EquipXmlTemplate> equipTemplates = new TIntObjectHashMap<EquipXmlTemplate>();
	private static TIntObjectMap<DiscipleXmlTemplate> discipleTemplates = new TIntObjectHashMap<DiscipleXmlTemplate>();
	private static TIntObjectMap<FateXmlTemplate> fateTemplates = new TIntObjectHashMap<FateXmlTemplate>();
	private static TIntObjectMap<CareerXmlTemplate> careerTemplates = new TIntObjectHashMap<CareerXmlTemplate>();
	private static TIntObjectMap<KindSelecter> kindSelecters = new TIntObjectHashMap<KindSelecter>();
	private static TIntObjectMap<KindSelecter> discipleSelecters = new TIntObjectHashMap<KindSelecter>();
	private static TIntObjectMap<LevelUpXmlTemplate> levelUpTemplates = new TIntObjectHashMap<LevelUpXmlTemplate>();
	private static TIntObjectMap<TreasureSelecter>  treasureSelecters= new TIntObjectHashMap<TreasureSelecter>();
   	private static TIntObjectMap<ToolsXmlTemplate> toolsTemplates = new TIntObjectHashMap<ToolsXmlTemplate>();
	private static TIntObjectMap<Float> discipleLevels = new TIntObjectHashMap<Float>();
	private static TIntObjectMap<Integer> userLevels = new TIntObjectHashMap<Integer>();
	private static TIntObjectMap<UserLevelConfig> userLevelConfigs = new TIntObjectHashMap<UserLevelConfig>();
	private static TIntObjectMap<TeamBuffTemplate> teamBuffTemplates = new TIntObjectHashMap<TeamBuffTemplate>();
	private static TIntObjectMap<TeamBuffBoldnessTemplate> teamBuffBoldnessTemplates = new TIntObjectHashMap<TeamBuffBoldnessTemplate>();
	private static SysBasicConfig sysBasicConfig = new SysBasicConfig();
    private static LevelSilversXmlTemplate lvSilversTemplate = new LevelSilversXmlTemplate();
    private static TIntObjectMap<SkillSelecter> skillSelecters = new TIntObjectHashMap<SkillSelecter>();
    private static TIntObjectMap<EquipSelecter> equipSelecters = new TIntObjectHashMap<EquipSelecter>();
    private static HashMap<String, ArrayList<Integer>> usePowerReward = new HashMap<String, ArrayList<Integer>>();
    //血战到底基础奖励以及额外奖励配置
    private static TIntObjectMap<FightEndGeneral> fightEndGeneralsTemplates = new TIntObjectHashMap<FightEndGeneral>();
    //血战可挑战的NPC组配置信息
    private static TIntObjectMap<FightEndNpc> fightEndNpcTemplates = new TIntObjectHashMap<FightEndNpc>();
    //血战NPC等级配置信息
    private static TIntObjectMap<FightEndNpcLevel> fightEndNpcLevelTemplates = new TIntObjectHashMap<FightEndNpcLevel>();
    //血战排行奖励信息
    private static TIntObjectMap<FightEndReward> fightEndRewardTemplates = new TIntObjectHashMap<FightEndReward>();
    private static HashMap<Integer, Integer> baseMap = new HashMap<Integer, Integer>();
    private static TIntObjectMap<ContinueLoginXmlTemplate> continueLoginTypes = new TIntObjectHashMap<ContinueLoginXmlTemplate>();
    private static TIntObjectMap<SoulSelecter> soulSelecters = new TIntObjectHashMap<SoulSelecter>();
    private static HashMap<Integer, HashMap<Integer, TitleXmlTemplate>> titleConfigs = new HashMap<Integer, HashMap<Integer,TitleXmlTemplate>>();
    private static TIntObjectMap<VisitRewardSelecter> visitRewardTemplates = new TIntObjectHashMap<VisitRewardSelecter>();
    private static HashMap<Integer, RechargeXmlTemplate> rechargeTemplates = new HashMap<Integer, RechargeXmlTemplate>();
    private static HashMap<String, HashMap<Integer, RechargeXmlTemplate>> otherRechargesHashMap = new HashMap<String, HashMap<Integer,RechargeXmlTemplate>>();
    private static List<Adventures> adventureTemplates = new ArrayList<Adventures>();
    private static TIntObjectMap<AdventureContinueLogin> adventureContinueLoginTemplates = new TIntObjectHashMap<AdventureContinueLogin>();
    private static HashMap<String,CdkeyXmlTemplate> cdkeyTemplates = new HashMap<String,CdkeyXmlTemplate>();
    private static TIntObjectMap<SkillBookConfig> skillBookTemplates = new TIntObjectHashMap<SkillBookConfig>();
    private static TIntObjectMap<ArrayList<Integer>> skillBookSkillRelationTemplates = new TIntObjectHashMap<ArrayList<Integer>>();
    private static TIntObjectMap<JSONObject> skillBookSnatchRadio = new TIntObjectHashMap<JSONObject>();
    private static TIntObjectMap<SkillBookSelecter> skillBookSelecters = new TIntObjectHashMap<SkillBookSelecter>();
    private static TIntObjectMap<GuideConfigerXmlTemplate> guideConfigerXmlTemplates= new TIntObjectHashMap<GuideConfigerXmlTemplate>();
    private static TIntObjectMap<ChargeRebateXmlTemplate> chargeRebateXmlTemplates = new TIntObjectHashMap<ChargeRebateXmlTemplate>();
    private static TIntObjectMap<FortuneComeXmlTemplate> fortuneComeXmlTemplas = new TIntObjectHashMap<FortuneComeXmlTemplate>();
    private static final ConcurrentHashMap<Integer,Battle> NpcBattleMap = new ConcurrentHashMap<Integer, Battle>(500);
    private static HashMap<Integer, InviteConfig> inviteConfigs = new HashMap<Integer, InviteConfig>();
    private static WorldBossConfig worldBossConfig = new WorldBossConfig();
    private static TIntObjectMap<GiveSoulSelecter> giveSoulSelecters = new TIntObjectHashMap<GiveSoulSelecter>();
    private static TIntObjectMap<GiveSoulStage> giveSoulStages = new TIntObjectHashMap<GiveSoulStage>();
    private static TIntObjectMap<GiveSoulConfig> giveSoulConfigs = new TIntObjectHashMap<GiveSoulConfig>();
    private static HashMap<String,Boolean> platActiveCodeCtrlConfigs = new HashMap<String, Boolean>();
    private static TIntObjectMap<WorldBossLvHp> worldBossLvHps=new TIntObjectHashMap<WorldBossLvHp>();
    private static TIntObjectMap<LuckyStarXmlTemplate> luckyStarXmlTemplates= new TIntObjectHashMap<LuckyStarXmlTemplate>();
    private static TIntObjectMap<ActivityTimeConfig> luckyStarTimeConfigs= new TIntObjectHashMap<ActivityTimeConfig>();
    private static TIntObjectMap<ActivityTimeConfig> dropTimeConfigs=new TIntObjectHashMap<ActivityTimeConfig>();
    //江湖掉落统计 index:0-type 1-id
    private static ArrayList<Integer> dropCountConfig=new ArrayList<Integer>();
    private static TIntObjectMap<ActivityTimeConfig> tiaozhanTimeConfigs=new TIntObjectHashMap<ActivityTimeConfig>();
    //论剑额外奖励兑换配置
    private static TiaozhanPoolConfig tiaozhanPoolConfig=new TiaozhanPoolConfig();
    private static HashMap<String, PlatInitReward> platsInitReward = new HashMap<String, PlatInitReward>();
    private static QuickCareerConfig quickCareerConfig = new QuickCareerConfig();
    private static TIntObjectMap<Diamond> diamonds=new TIntObjectHashMap<Diamond>();
    private static GuildConfig guildConfig = new GuildConfig();
    
    private static TIntObjectMap<ActivityTimeConfig> deepWaterTimeConfigs= new TIntObjectHashMap<ActivityTimeConfig>();
    private static HashMap<Integer, HashMap<Integer, Integer>> deepWaterForSureTimes=new HashMap<Integer, HashMap<Integer,Integer>>();//深海探宝必出(类型,必出配置)
    private static HashMap<Integer, int[]> deepWaterConfigs=new HashMap<Integer, int[]>();//深海探宝花费(抽取类型,花费类型(钻石/贝里),数量)
    private static HashMap<Integer, DeepWaterSelecter> deepWaterSelecters=new HashMap<Integer, DeepWaterSelecter>();//深海探宝抽取配置
    private static SkillSelecter allSkillSelecters = new SkillSelecter(); //全技能随机    
    private static SkillbookSelecter skillbookSelecter=new SkillbookSelecter();
    private static TIntObjectMap<ActivityTimeConfig> goddessRebateTimeConfigs= new TIntObjectHashMap<ActivityTimeConfig>();//女神回馈时间配置
    private static TIntObjectMap<GoddessRebateConfig> goddessRebateConfigs=new TIntObjectHashMap<GoddessRebateConfig>();
    
    private static int marketToolVersion;
    public static void initTemplateData(int type) throws Exception
    {
		if (type == 0)
        {
			loadSkillTemplates();
			loadEquipTemplates();
			loadDiscipleTemplates();
			loadFateTemplates();
			loadCareerTemplates();
			loadKindSelector();
			loadDiscipleSelecter();
			loadSysBasicConfig();
			loadLevelUpTemplates();
			loadDiscipleLevel();
			loadUserLevel();
			loadUserLevelConfig();
			loadTreasureSelecter();
       		loadToolsTemplates();			
            loadToolsTemplates();
            loadLevelSilvers();
            loadSkillSelecter();
            loadEquipSelecter();
            loadEquipLevelBaseTemplates();
            loadFightEndConfig();
            loadContinueLogin();
            loadSoulSelecters();
            loadTitleConfig();
            loadVisitRewardSelecter();
            loadRechargeTemplates();
            loadAdventuresConfigTemplates();
            loadUsePowerReward();
            loadAdventureContinueLoginTemplates();
            loadSkillBookTemplates();
            loadSkillBookNpcConfig();
            loadSkillBookSnatchRadio();
            loadSkillBookSelecter();
            checkXMLForeignKey();
            loadCdkeyTemplates();
            loadGuideTemplates();
            loadDiscipleBattle();
            loadInviteConfig();
            loadWorldBossConfig();
            loadChargeRebateXmlTemplates();
            loadFortuneComeXmlTemplates();
            loadServerConfig();
            loadTesterList();
            loadGiveSoulSelecter();
            loadGiveSoulStage();
            loadGiveSoulConfig();
            loadPlatActiveCodeCtrlConfig();
            loadWorldBossLvHP();
            loadLluckyStarXmlTemplate();
            loadPlatInitReward();
            loadQuickCareerConfig();
            loadDiamonds();
            loadDiamondConfig();
            loadDiamondAdditionConfig();
            loadGuildBaseConfig();
            loadGuildLvlConfig();
            loadGuildPropsConfig();
            loadGuildBuildConfig();
            loadDeepWaterXmlTemplate();
            loadGuildShopSellConfig();
            loadGuildNpc();
            loadGoddessRebateConfig();
        }
		else if (type == 2) {
			loadSkillTemplates();
		}
		else if (type ==3) {
			loadEquipTemplates();
		}
		else if (type == 4) {
			loadDiscipleTemplates();
		}
		else if (type == 5) {
			loadFateTemplates();
		}
		else if (type == 6) {
			loadCareerTemplates();
		}
		else if (type == 7) {
			loadKindSelector();
		}
		else if (type == 8) {
			loadDiscipleSelecter();
		}
		else if (type == 9) {
			loadSysBasicConfig();
		}
		else if (type == 10) {
			loadLevelUpTemplates();
		}
		else if (type == 11) {
			loadTeamBuffTemplates();
		}
		else if (type == 12) {
			loadTeamBuffBoldnessTemplates();
		}
        else if(type==13)
        {
           loadFightEndGeneralConfig();
        }
        else if(type==14)
        {
           loadFightEndNpcConfig();
        }
        else if(type==15)
        {
           loadFightEndNpcLevelConfig();
        }
        else if(type==18)
        {
        	loadLevelSilvers();
        }
        else if(type==19)
        {
        	loadDiscipleLevel();
        }
        else if(type==20)
        {
        	loadUserLevel();
        }
        else if(type==21)
        {
        	loadUserLevelConfig();
        }
        else if(type==22)
        {
        	loadTreasureSelecter();
        }
        else if(type==23)
        {
        	loadToolsTemplates();
        }
        else if(type==24)
        {
        	loadLevelSilvers();
        }
        else if(type==25)
        {
        	loadSkillSelecter();
        }
        else if(type==26)
        {
        	loadEquipSelecter();
        }
        else if(type==27)
        {
        	loadFightEndRewardConfig();
        }
        else if(type==28)
        {
        	loadContinueLogin();
        }
        else if(type==29)
        {
        	loadSoulSelecters();
        }
        else if(type==30)
        {
        	loadTitleConfig();
        }
        else if(type==31)
        {
        	loadVisitRewardSelecter();
        }
        else if(type==32)
        {
        	loadRechargeTemplates();
        }
        else if(type==33)
        {
        	loadAdventuresConfigTemplates();
        }
        else if(type==34)
        {
        	loadUsePowerReward();
        }
        else if(type==35)
        {
            loadAdventureContinueLoginTemplates();
        }
        else if(type==36)
        {
            loadSkillBookTemplates();
        }
        else if(type==37)
        {
            loadSkillBookNpcConfig();
        }
        else if(type==38)
        {
            loadSkillBookSnatchRadio();
        }
        else if(type==39)
        {
            loadSkillBookSelecter();
        }
        else if(type==40)
        {
        	loadCdkeyTemplates();
        }
        else if(type==41)
        {
        	loadGuideTemplates();
        }
        else if(type==42)
        {
        	loadInviteConfig();
        }
        else if(type==43) 
        {
			loadWorldBossConfig();
		}
        else if(type==44) 
        {
        	loadChargeRebateXmlTemplates();
        }
        else if(type==45) 
        {
        	loadFortuneComeXmlTemplates();
        }
        else if(type==46) 
        {
        	loadServerConfig();
        }
        else if(type==47) 
        {
        	loadGiveSoulSelecter();
        }
        else if(type==48) 
        {
        	loadGiveSoulStage();
        }
        else if(type==49) 
        {
        	loadGiveSoulConfig();
        }
        else if(type==50)
        {
            loadPlatActiveCodeCtrlConfig();
        }
        else if(type==51){
        	loadWorldBossLvHP();
        }
        else if(type==52)
        {
        	loadLluckyStarXmlTemplate();
        }
        else if(type==53)
        {
            loadDiamonds();
        }
        else if(type==54)
        {
            loadDiamondConfig();
        }
        else if(type==55)
        {
            loadDiamondAdditionConfig();
        }
        else if(type==56)
        {
        	loadQuickCareerConfig();
        }
        else if (type == 57) {
        	loadGuildBaseConfig();
		}
        else if (type == 58) {
        	loadGuildLvlConfig();
		}
        else if (type == 59) {
        	loadGuildPropsConfig();
		}
        else if (type == 60) {
        	loadGuildBuildConfig();
		}
        else if (type == 61)
        {
        	loadDeepWaterXmlTemplate();
        }
        else if (type == 62)
        {
        	loadGuildShopSellConfig();
        }
        else if (type == 63) 
        {
			loadGuildNpc();
		}
        else if (type == 64) 
        {
			loadPlatInitReward();
		}
        else if (type == 65) 
        {
        	loadGoddessRebateConfig();
        }
    }

    /**
     * 基础弟子阵容信息，攻血战、残章NPC基础阵容调整
     */
    private static void loadDiscipleBattle()
    {
        TIntObjectIterator itDisciple = (TIntObjectIterator) discipleTemplates.iterator();
        int i=1;
        while (itDisciple.hasNext())
        {
            itDisciple.advance();
            DiscipleXmlTemplate disciple=(DiscipleXmlTemplate)itDisciple.value();
            Battle _battle=new Battle();
            //获取弟子信息
            _battle.setId(i);
            _battle.setItemid(disciple.getId());
            _battle.setLevel(1);
            _battle.setDefaultskillitem(disciple.getDefaultSkill());
            SkillXmlTemplate skillItem=XMLTemplateService.getSkillTemplate(disciple.getDefaultSkill());
            _battle.setDefaultskilltype(skillItem.getType());
            _battle.setDefaultskillquantity(skillItem.getQuantity());
            _battle.setDiscipleid(disciple.getId());
            NpcBattleMap.put(disciple.getId(), _battle);
            ++i;
        }
    }

    public static synchronized void checkXMLForeignKey()
    {
        //check 弟子表技能与缘分关联id是否存在

        TIntObjectIterator itDisciple = (TIntObjectIterator) discipleTemplates.iterator();
        while (itDisciple.hasNext())
        {
           itDisciple.advance();
           DiscipleXmlTemplate dt=(DiscipleXmlTemplate)itDisciple.value();
           //check defaultSkill
            if(!skillTemplates.containsKey(dt.getDefaultSkill()))
            {
                System.out.println("人物：["+dt.getId()+"]默认技能 ：["+dt.getDefaultSkill()+"]不存在！");
            }
           //check fates
            String fates[]=dt.getFates().split(",");
            for(int i=0;i<fates.length;++i)
            {
                int fatesId=fates[i].length()>0?Integer.parseInt(fates[i]):0;
                if(fatesId!=0)
                {
                    if(!fateTemplates.containsKey(fatesId))
                    {
                        System.out.println("人物：["+dt.getId()+"]位置["+(i+1)+"] ：缘分["+fates[i]+"]不存在！");
                    }
                }
            }
        }
        //check 江湖表内章节id<未检测>、奖励物品、关卡npc<未检测>、解锁关卡是否存在
        TIntObjectIterator itCareer = (TIntObjectIterator) careerTemplates.iterator();
        while (itCareer.hasNext())
        {
            itCareer.advance();
            CareerXmlTemplate career=(CareerXmlTemplate)itCareer.value();
            //check unlock custompass
            if(career.getUnlockId()!=0)
            {
                if(!careerTemplates.containsKey(career.getUnlockId()))
                {
                    System.out.println("关卡：["+career.getId()+"]解锁关卡 ：["+career.getUnlockId()+"]不存在！");
                }
            }

            //check drop goods

            ArrayList<int[]> dropItems=(ArrayList<int[]>)career.getDropItems().clone();
            for (int[] dropGoods : dropItems)
            {
                if (dropGoods.length < 3)
                {
                    System.out.println("江湖关卡：[" + career.getId() + "]奖励物品配置异常！");
                }
                else
                {
                    if(dropGoods[1]==0)
                    {
                        continue;
                    }
                    switch (dropGoods[0])
                    {
                        case 41000:
                            if (!skillTemplates.containsKey(dropGoods[1]))
                            {
                                System.out.println("江湖关卡：[" + career.getId() + "]掉落技能[" + dropGoods[1] + "]不存在！");
                            }
                            break;
                        case 50000:
                            if (!equipTemplates.containsKey(dropGoods[1]))
                            {
                                System.out.println("江湖关卡：[" + career.getId() + "]掉落装备[" + dropGoods[1] + "]不存在！");
                            }
                            break;
                        default:
                            System.out.println("江湖关卡：[" + career.getId() + "]掉落物品类型[" + dropGoods[0] + "]不存在！");
                    }
                }
            }
        }

    }
    
    public static HashMap<String, ArrayList<Integer>> getUsePowerReward(){
    	return usePowerReward;
    }
	
	public static HashMap<Integer, TitleXmlTemplate> getTypeTitleConfig(int type){
		return titleConfigs.get(type);
	}
	
	public static SoulSelecter getSoulSelecter(int type){
		return soulSelecters.get(type);
	}
	
	public static ContinueLoginXmlTemplate getContinueLoginConfig(int day){
		return continueLoginTypes.get(day);
	}
	
	public static ItemXmlTemplate getItemTemplate(int itemTemplateId) {
		return itemTemplates.get(itemTemplateId);
	}
	   
	public static Item getNewItemByTemplateId(int templateId){
		ItemXmlTemplate it = itemTemplates.get(templateId);
		Item item = new Item();   
		item.setTemplateId(templateId);
		item.setItemTemplate(it);
		return item;
	} 
	
	public static SkillXmlTemplate getSkillTemplate(int skillid){
		return skillTemplates.get(skillid);
	}
	
	public static EquipXmlTemplate getEquipTemplate(int equipId){
		return equipTemplates.get(equipId);
	}
	
	public static DiscipleXmlTemplate getDiscipleTemplate(int discipleId){
		return discipleTemplates.get(discipleId);
	}
	
	public static FateXmlTemplate getFateTemplate(int fateId){
		return fateTemplates.get(fateId);
	}
	
	public static CareerXmlTemplate getCareerTemplate(int careerId){
		return careerTemplates.get(careerId);
	}
	
	public static LevelUpXmlTemplate getLevelUpTemplate(int levelUpId){
		return levelUpTemplates.get(levelUpId);
	}
	
	public static int getLevelUpTemplateSize(){
		return levelUpTemplates.size();
	}
	
	public static HashMap<String, Integer> getModifyTime(){
		return xmlLoadTime;
	}
	
	public static SysBasicConfig getSysBasicConfig(){
		return sysBasicConfig;
	}
	
	public static KindSelecter getKindSelecter(int kind){
		return kindSelecters.get(kind);
	}
	
	public static KindSelecter getDiscipleSelecter(int kind){
		return discipleSelecters.get(kind);
	}
	
	public static float getDiscipleLevelConfig(int level){
		return discipleLevels.get(level);
	}
	
	public static int getDiscipleLevelCount(){
		return discipleLevels.size();
	}
	
	public static int getUserLevelExp(int level){
		return userLevels.get(level);
	}
	
	public static int getUserLevelCount(){
		return userLevels.size();
	}
	
	public static UserLevelConfig getUserLevelConfig(int level){
		return userLevelConfigs.get(level);
	}
	
	public static ToolsXmlTemplate getToolTemplate(int toolId){
		return toolsTemplates.get(toolId);
	}
	
	public static CdkeyXmlTemplate getCdkeyXmlTemplate(String typeId){
		return cdkeyTemplates.get(typeId);
	}
	
	public static GuideConfigerXmlTemplate getGuideConfigerXmlTemplate(int id){
		return guideConfigerXmlTemplates.get(id);
	}
	public static LuckyStarXmlTemplate getLuckyStarXmlTemplate(int id){
		return luckyStarXmlTemplates.get(id);
	}
	public static ChargeRebateXmlTemplate getChargeRebateXmlTemplate(int id){
		return chargeRebateXmlTemplates.get(id);
	}
	public static int getChargeRebateXmlTemplatesSize(){
		return chargeRebateXmlTemplates.size();
	}
	public static FortuneComeXmlTemplate getFortuneComeXmlTemplate(int id){
		return fortuneComeXmlTemplas.get(id);
	}
	public static int getFortuneComeXmlTemplatesSize(){
		return fortuneComeXmlTemplas.size();
	}
	public static TreasureSelecter getTreasureSelecter(int type){
        return treasureSelecters.get(type);
    }
	
    public static LevelSilversXmlTemplate getLevelSilversTemplate()
    {
        return lvSilversTemplate;
    }
    
    public static SkillSelecter getSkillSelecter(int quality){
        return skillSelecters.get(quality);
    }
    
    public static EquipSelecter getEquitSelecter(int type){
        return equipSelecters.get(type);
    }
    
    public static int getEquipLevelBase(int level){
    	return baseMap.get(level);
    }
    public static WorldBossLvHp getWorldBossLvHp(int level){
    	return worldBossLvHps.get(level);
    }
    public static VisitRewardSelecter getVisitRewardSelecter(int times){
        int size = visitRewardTemplates.size();
        if(times+1>=visitRewardTemplates.get(size-1).getTimes())
        {
            return visitRewardTemplates.get(size-1);
        }
        return visitRewardTemplates.get(times+1);
    }

    public static Boolean getPlatActiveCodeStatus(String platName)
    {
        if(!platActiveCodeCtrlConfigs.containsKey(platName))
        {
            return false;
        }
        return platActiveCodeCtrlConfigs.get(platName);
    }

    /**
     * 返回血战到底阶段关卡奖池信息
     * @param barrier
     * @return
     */
    public static FightEndGeneral getFightEndGeneralByBarrier(int barrier)
    {
        return fightEndGeneralsTemplates.get(barrier);
    }

    /**
     *通过npc编号获得npc配置相关信息
     * @param id
     * @return
     */
    public static synchronized  FightEndNpc getFightEndNpcById(int id)
    {
        return fightEndNpcTemplates.get(id);
    }

    /**
     * 通过血战到底关卡获得npc加成信息
     * @param barrier
     * @return
     */
    public static synchronized  FightEndNpcLevel getFightEndNpcLevelByBarrier(int barrier)
    {
        return fightEndNpcLevelTemplates.get(barrier);
    }

    public static synchronized  FightEndReward getFightEndRewardByLevel(int level)
    {
        return fightEndRewardTemplates.get(level);
    }
	
	public static int getTeamBuffTemplateSize(){
		return teamBuffTemplates.size();
	}
	
	public static int getTeamBuffBoldnessTemplateSize(){
		return teamBuffBoldnessTemplates.size();
	}
	
	public static HashMap<Integer, RechargeXmlTemplate> getAllRecharges(){
		return (HashMap<Integer, RechargeXmlTemplate>) rechargeTemplates;
	}
	
	public static RechargeXmlTemplate getRechargeConfig(int id){
		return rechargeTemplates.get(id);
	}
    public static List<Adventures> getAdventureTemplates(){
        return adventureTemplates;
    }

    public static AdventureContinueLogin getAdventureContinueLoginById(int id)
    {
        return adventureContinueLoginTemplates.get(id);
    }

    /**
     * 获取奇遇连续登陆大小
     * @return
     */
    public static int adventureContinueLoginTemplatesSize()
    {
        return adventureContinueLoginTemplates.size();
    }

    /**
     * 获取技能编号根据残章编号
     * @param skillBookId
     * @return
     */
    public static int getSkillIdBySkillBookId(int skillBookId)
    {
        int id=-1;
        try
        {
            id=skillBookTemplates.get(skillBookId).getSkillId();
        }
        catch (Exception e)
        {
             id=-1;
        }
        return id;
    }

    /**
     * 获取残章列表根据技能id;
     * @param skillId
     * @return
     */
    public static ArrayList<Integer> getSkillBookListBySkillId(int skillId)
    {
        ArrayList<Integer> _list=null;
        if(skillId==-1)
        {
            return _list;
        }
        try
        {
            _list=skillBookSkillRelationTemplates.get(skillId);
        }
        catch (Exception e)
        {
            _list=null;
        }
        return _list;
    }

    /**
     * 获取残章列表根据残章编号
     * @param skillBookId
     * @return
     */
    public static ArrayList<Integer> getSkillBookListBySkillBookId(int skillBookId)
    {
        return getSkillBookListBySkillId(getSkillIdBySkillBookId(skillBookId));
    }

    /**
     * 获取残章抢夺概率根据品质
     * @param quality
     * @return
     */
    public static JSONObject getSkillBookSnatchByQuality(int quality)
    {
        return skillBookSnatchRadio.get(quality);
    }

    /**
     * 获取残章配置信息
     * @param bookId
     * @return
     */
    public static SkillBookConfig getSkillBookConfigById(int bookId)
    {
        return skillBookTemplates.get(bookId);
    }

    /**
     * 获取残章奖池信息
     * @param id
     * @return
     */
    public static SkillBookSelecter getSkillBookSelecterById(int id)
    {
        return skillBookSelecters.get(id);
    }

    public static Battle getBattleByDiscipleId(int id)
    {
        return NpcBattleMap.get(id);
    }
    
    public static InviteConfig getInviteConfig(int type) {
		return inviteConfigs.get(type);
	}
    
    public static WorldBossConfig getWorldBossConfig(){
    	return worldBossConfig;
    }
    public static GiveSoulSelecter getGiveSoulSelecter(int typeid){
        return giveSoulSelecters.get(typeid);
    }
    public static GiveSoulStage getGiveSoulStage(int typeid){
    	return giveSoulStages.get(typeid);
    }
    public static GiveSoulConfig getGiveSoulConfig(int id){
    	return giveSoulConfigs.get(id);
    }
    public static int getGiveSoulConfigSize(){
    	return giveSoulConfigs.size();
    }
    public static ActivityTimeConfig getluckyStarTimeConfig(int id){
    	return luckyStarTimeConfigs.get(id);
    }
    public static int getLuckyStarConfigSize(){
    	return luckyStarTimeConfigs.size();
    }
    public static TIntObjectMap<ActivityTimeConfig> getluckyStarTimeConfigs(){
    	return luckyStarTimeConfigs;
    }
    //获取论剑新增奖励信息
    public static TiaozhanPoolConfig getTiaozhanPoolConfig(){
    	return tiaozhanPoolConfig;
    }
    //获取论剑新增奖励开启时间配置
    public static TIntObjectMap<ActivityTimeConfig> getTiamzhanTimeConfigs(){
    	return tiaozhanTimeConfigs;
    }
    //获取江湖掉落统计开启时间配置
    public static TIntObjectMap<ActivityTimeConfig> getDropTimeConfigs(){
    	return dropTimeConfigs;
    }
    //获取江湖掉落统计对象
    public static ArrayList<Integer> getDropCountConfig(){
    	return dropCountConfig;
    }

    public static QuickCareerConfig getQuickCareerConfig(){
    	return quickCareerConfig;
    }
    public static TIntObjectMap<ActivityTimeConfig> getDeepWaterTimeConfigs(){
    	return deepWaterTimeConfigs;
    }
    public static ActivityTimeConfig getDeepWaterTimeConfig(int id){
    	return deepWaterTimeConfigs.get(id);
    }
    public static HashMap<Integer, int[]> getDeepWaterConfigs(){
    	return deepWaterConfigs;
    }
    public static int[] getDeepWaterConfig(int kind){
    	return deepWaterConfigs.get(kind);
    }
    public static DeepWaterSelecter getDeepWaterSelecter(int typeid){
    	return deepWaterSelecters.get(typeid);
    }
    public static HashMap<Integer, Integer> getDeepWaterForsureTime(int typeid){
    	return deepWaterForSureTimes.get(typeid);
    }
    public static SkillSelecter getAllSkillSelecter(){
    	return allSkillSelecters;
    }
    public static SkillbookSelecter getSkillbookSelecter(){
    	return skillbookSelecter;
    }
    public static int getMarketToolVersion(){
    	return marketToolVersion;
    }
    public static TIntObjectMap<ActivityTimeConfig> getGoddesRebateTimeConfigs(){
    	return goddessRebateTimeConfigs;
    }
    public static ActivityTimeConfig getGoddesRebateTimeConfigById(int id){
    	return goddessRebateTimeConfigs.get(id);
    }
    //获取女神反馈活动配置
    public static TIntObjectMap<GoddessRebateConfig> getGoddessRebateConfig(){
    	return goddessRebateConfigs;
    }
    //获取女神反馈活动女神数量
    public static int getGoddessConfigsSize(){
    	return goddessRebateConfigs.size();
    }
    public static PlatInitReward getPlatInitRewardConfigByPlatform(String platform){
    	PlatInitReward config = null;
    	if (platsInitReward.containsKey(platform)) {
			config = platsInitReward.get(platform);
		}
    	
    	return config;
    }
    
    /**
     * 获取宝石信息
     * @param itemId
     * @return
     */
    public static Diamond getDiamondByItemId(int itemId)
    {
       if(diamonds.containsKey(itemId))
       {
           return diamonds.get(itemId);
       }
        return null;
    }
    
    public static GuildConfig getGuildConfig(){
    	return guildConfig;
    }

	private static void loadSkillTemplates() throws Exception{
		String itemConfPath = DZM.basePath + "/xml/skill.xml";
		log.info("load item config file path :{}",itemConfPath);
		XMLConfiguration conf = new XMLConfiguration(itemConfPath);
		List<HierarchicalConfiguration> fields = conf.configurationsAt("Skills.Skill");
		if (fields != null && fields.size() > 0) {
			HierarchicalConfiguration sub;
			SkillXmlTemplate it = new SkillXmlTemplate();
			for (int i = 0; i < fields.size(); i++) {
				it = new SkillXmlTemplate();
				sub = fields.get(i);
				int id = sub.getInt("[@id]");
				int quality = sub.getInt("[@quality]");
				int type = sub.getInt("[@type]");
				float rate = sub.getFloat("[@rate]");
				float quantity = sub.getFloat("[@quantity]");
				float growth = sub.getFloat("[@growth]");
				it.setId(id);
				it.setQuality(quality);
				it.setType(type);
				it.setRate(rate);
				it.setQuantity(quantity);
				it.setGrowth(growth);
				skillTemplates.put(id, it);
			}
		}
		xmlLoadTime.put("skill", Util.getServerTime());
	}
	
	private static void loadEquipTemplates() throws Exception{
		String itemConfPath = DZM.basePath + "/xml/equip.xml";
		log.info("load item config file path :{}",itemConfPath);
		XMLConfiguration conf = new XMLConfiguration(itemConfPath);
		List<HierarchicalConfiguration> fields = conf.configurationsAt("Equips.Equip");
		if (fields != null && fields.size() > 0) {
			HierarchicalConfiguration sub;
			EquipXmlTemplate it = new EquipXmlTemplate();
			for (int i = 0; i < fields.size(); i++) {
				it = new EquipXmlTemplate();
				sub = fields.get(i);
				int id = sub.getInt("[@id]");
				int quality = sub.getInt("[@quality]");
				int type = sub.getInt("[@type]");
				float quantity = sub.getFloat("[@quantity]");
				float growth = sub.getFloat("[@growth]");
				it.setId(id);
				it.setQuality(quality);
				it.setType(type);
				it.setQuantity(quantity);
				it.setGrowth(growth);
				equipTemplates.put(id, it);
			}
		}
		xmlLoadTime.put("equip", Util.getServerTime());
	}
	
	private static void loadEquipLevelBaseTemplates() throws Exception{
		String itemConfPath = DZM.basePath + "/xml/equip_level_base.xml";
		log.info("load item config file path :{}",itemConfPath);
		XMLConfiguration conf = new XMLConfiguration(itemConfPath);
		List<HierarchicalConfiguration> fields = conf.configurationsAt("Levels.Level");
		if (fields != null && fields.size() > 0) {
			HierarchicalConfiguration sub;
			for (int i = 0; i < fields.size(); i++) {
				sub = fields.get(i);
				int id = sub.getInt("[@id]");
				int base = sub.getInt("[@base]");
				baseMap.put(id, base);
			}
		}
		xmlLoadTime.put("equipLevelBase", Util.getServerTime());
	}
	
	private static void loadDiscipleTemplates() throws Exception{
		String itemConfPath = DZM.basePath + "/xml/disciple.xml";
		log.info("load item config file path :{}",itemConfPath);
		XMLConfiguration conf = new XMLConfiguration(itemConfPath);
		List<HierarchicalConfiguration> fields = conf.configurationsAt("Disciples.Disciple");
		if (fields != null && fields.size() > 0) {
			HierarchicalConfiguration sub;
			DiscipleXmlTemplate it = new DiscipleXmlTemplate();
			for (int i = 0; i < fields.size(); i++) {
				it = new DiscipleXmlTemplate();
				sub = fields.get(i);
				int id = sub.getInt("[@id]");
				int quality = sub.getInt("[@quality]");
				int group = sub.getInt("[@group]");
				int defaultSkill = sub.getInt("[@defaultSkill]");
				int defaultHealth = sub.getInt("[@defaultHealth]");
				int defaultAttack = sub.getInt("[@defaultAttack]");
				int defaultDefence = sub.getInt("[@defaultDefence]");
				int defaultRp = sub.getInt("[@defaultRp]");
				int defaultPotentiality = sub.getInt("[@defaultPotentiality]");
				int defaultExp = sub.getInt("[@defaultExp]");
				float growHealth = sub.getFloat("[@growHealth]");
				float growAttack = sub.getFloat("[@growAttack]");
				float growDefence = sub.getFloat("[@growDefence]");
				float growRp = sub.getFloat("[@growRp]");
				int fate1 = sub.getInt("[@fate1]");
				int fate2 = sub.getInt("[@fate2]");
				int fate3 = sub.getInt("[@fate3]");
				int fate4 = sub.getInt("[@fate4]");
				int fate5 = sub.getInt("[@fate5]");
				int fate6 = sub.getInt("[@fate6]");
				it.setId(id);
				it.setQuality(quality);
				it.setGroup(group);
				it.setDefaultSkill(defaultSkill);
				it.setDefaultHealth(defaultHealth);
				it.setDefaultAttack(defaultAttack);
				it.setDefaultDefence(defaultDefence);
				it.setDefaultRp(defaultRp);
				it.setDefaultPotentiality(defaultPotentiality);
				it.setDefaultExp(defaultExp);
				it.setGrowHealth(growHealth);
				it.setGrowAttack(growAttack);
				it.setGrowDefence(growDefence);
				it.setGrowRp(growRp);
				String fates = "";
				if (fate1 != 0) {
					fates += fate1;
				}
				if (fate2 != 0) {
					fates = fates + "," + fate2;
				}
				if (fate3 != 0) {
					fates = fates + "," + fate3;
				}
				if (fate4 != 0) {
					fates = fates + "," + fate4;
				}
				if (fate5 != 0) {
					fates = fates + "," + fate5;
				}
				if (fate6 != 0) {
					fates = fates + "," + fate6;
				}
				it.setFates(fates);
				
				discipleTemplates.put(id, it);
			}
		}
		xmlLoadTime.put("disciple", Util.getServerTime());
	}

	private static void loadFateTemplates() throws Exception{
		String itemConfPath = DZM.basePath + "/xml/fate.xml";
		log.info("load item config file path :{}",itemConfPath);
		XMLConfiguration conf = new XMLConfiguration(itemConfPath);
		List<HierarchicalConfiguration> fields = conf.configurationsAt("Fates.Fate");
		if (fields != null && fields.size() > 0) {
			HierarchicalConfiguration sub;
			FateXmlTemplate it = new FateXmlTemplate();
			for (int i = 0; i < fields.size(); i++) {
				it = new FateXmlTemplate();
				sub = fields.get(i);
				int id = sub.getInt("[@id]");
				int type = sub.getInt("[@type]");
				String require = sub.getString("[@require]");
				int upgradeType = sub.getInt("[@upgradeType]");
				float quantity = sub.getFloat("[@quantity]");
				
				it.setId(id);
				it.setType(type);
				it.setRequire(require);
				it.setUpgradeType(upgradeType);
				it.setQuantity(quantity);
				
				fateTemplates.put(id, it);
			}
		}
		xmlLoadTime.put("fate", Util.getServerTime());
	}
	
	private static void loadCareerTemplates() throws Exception{
		String itemConfPath = DZM.basePath + "/xml/career_group.xml";
		log.info("load item config file path :{}",itemConfPath);
		XMLConfiguration conf = new XMLConfiguration(itemConfPath);
		List<HierarchicalConfiguration> fields = conf.configurationsAt("Groups.Group");
		HashMap<Integer, BattleArrayObject> groups = new HashMap<Integer, BattleArrayObject>();
		HashMap<Integer, ArrayList<Float>> groupRates = new HashMap<Integer, ArrayList<Float>>();
		if (fields != null && fields.size() > 0) {
			HierarchicalConfiguration sub;
			for (int i = 0; i < fields.size(); i++) {
				BattleArrayObject bo = new BattleArrayObject();
				Battle battle = new Battle();
				ArrayList<Battle> battlearrays = new ArrayList<Battle>();
				ArrayList<Float> rates = new ArrayList<Float>();
				sub = fields.get(i);
				int gid = sub.getInt("[@id]");
				int momentum = sub.getInt("[@momentum]");
				
				
				int id = sub.getInt("[@cid1]");
				int level = sub.getInt("[@clevel1]");
				float rate = sub.getFloat("[@crate1]");
				rates.add(rate);
				battle = getCareerBattle(id, level);
				battlearrays.add(battle);
				
				id = sub.getInt("[@cid2]");
				level = sub.getInt("[@clevel2]");
				if (id != 0) {
					rate = sub.getFloat("[@crate2]");
					rates.add(rate);
					battle = getCareerBattle(id, level);
					battlearrays.add(battle);
				}
				
				id = sub.getInt("[@cid3]");
				level = sub.getInt("[@clevel3]");
				if (id != 0) {
					rate = sub.getFloat("[@crate3]");
					rates.add(rate);
					battle = getCareerBattle(id, level);
					battlearrays.add(battle);
				}
				
				id = sub.getInt("[@cid4]");
				level = sub.getInt("[@clevel4]");
				if (id != 0) {
					rate = sub.getFloat("[@crate4]");
					rates.add(rate);
					battle = getCareerBattle(id, level);
					battlearrays.add(battle);
				}
				
				id = sub.getInt("[@cid5]");
				level = sub.getInt("[@clevel5]");
				if (id != 0) {
					rate = sub.getFloat("[@crate5]");
					rates.add(rate);
					battle = getCareerBattle(id, level);
					battlearrays.add(battle);
				}
				
				id = sub.getInt("[@cid6]");
				level = sub.getInt("[@clevel6]");
				if (id != 0) {
					rate = sub.getFloat("[@crate6]");
					rates.add(rate);
					battle = getCareerBattle(id, level);
					battlearrays.add(battle);
				}
				
				id = sub.getInt("[@cid7]");
				level = sub.getInt("[@clevel7]");
				if (id != 0) {
					rate = sub.getFloat("[@crate7]");
					rates.add(rate);
					battle = getCareerBattle(id, level);
					battlearrays.add(battle);
				}
				
				id = sub.getInt("[@cid8]");
				level = sub.getInt("[@clevel8]");
				if (id != 0) {
					rate = sub.getFloat("[@crate8]");
					rates.add(rate);
					battle = getCareerBattle(id, level);
					battlearrays.add(battle);
				}
				
				bo.setPlayerid(gid);
				bo.setMomentum(momentum);
				bo.setBattlearrays(battlearrays);
				groups.put(gid, bo);
				
				groupRates.put(gid, rates);
			}
		}
		
		itemConfPath = DZM.basePath + "/xml/career.xml";
		XMLConfiguration.setDefaultListDelimiter(' ');
		conf = new XMLConfiguration(itemConfPath);
		fields = conf.configurationsAt("Careers.Career");
		if (fields != null && fields.size() > 0) {
			HierarchicalConfiguration sub;
			for (int i = 0; i < fields.size(); i++) {
				sub = fields.get(i);
				CareerXmlTemplate careerTemplate = new CareerXmlTemplate();
				int id = sub.getInt("[@id]");
				int chapterId = sub.getInt("[@chapterId]");
				boolean isOpen = sub.getBoolean("[@isOpen]");
				int boosType = sub.getInt("[@boosType]");
				int costPower = sub.getInt("[@costPower]");
				int countLimit = sub.getInt("[@countLimit]");
				int gainExp = (int) (sub.getInt("[@gainExp]")*1.3);   /// add by lh经验掉落提高百分之三十
				int gainCoins = sub.getInt("[@gainCoins]")*2;           /// 胜利后奖励银两双倍
				ArrayList<int[]> list = new ArrayList<int[]>();
				String dropItems = sub.getString("[@dropItems]");
				String[] itemArray = dropItems.split(",");
				int count = itemArray.length;
				for (int j = 0; j < count; j++) {
					String[] a = itemArray[j].split("_");
					int[] b = new int[3];
					for (int k = 0; k < a.length; k++) {
						b[k] = Integer.parseInt(a[k]);
					}
					
					b[2]*=2;			/// 胜利后奖励银两双倍   add by lh  人物升级所需经验降低30%					战斗掉落提示1倍
					
					list.add(b);
				}
				
				ArrayList<Integer> rList = new ArrayList<Integer>();
				String dropRates = sub.getString("[@dropRates]");
				String[] rArray = dropRates.split(",");
				int rate = 0;
				for (int j = 0; j < rArray.length; j++) {
					rList.add(Integer.parseInt(rArray[j]) + rate);
					rate += Integer.parseInt(rArray[j]);
				}
				int groupId = sub.getInt("[@groupId]");
				int unlockId = sub.getInt("[@unlockId]");
				
				careerTemplate.setId(id);
				careerTemplate.setChapterId(chapterId);
				careerTemplate.setIsOpen(isOpen);
				careerTemplate.setBoosType(boosType);
				careerTemplate.setCostPower(costPower);
				careerTemplate.setCountLimit(countLimit);
				careerTemplate.setGainExp(gainExp);
				careerTemplate.setGainCoins(gainCoins);
				careerTemplate.setDropItems(list);
				careerTemplate.setDropRates(rList);
				careerTemplate.setBattleInfo(groups.get(groupId));
				careerTemplate.setUnlockId(unlockId);
				careerTemplate.setSkillRates(groupRates.get(groupId));
				
				
				careerTemplates.put(id, careerTemplate);
			}
		}
		
		xmlLoadTime.put("career", Util.getServerTime());
	}
	
	private static Battle getCareerBattle(int id,int level){
		Battle battle = new Battle();
		DiscipleXmlTemplate discipleInfo = getDiscipleTemplate(id);
		battle.setDiscipleid(id);
		battle.setItemid(discipleInfo.getId());
		battle.setLevel(level);
		battle.setAttack(discipleInfo.getDefaultAttack() + Math.round(discipleInfo.getGrowAttack() * level));
		battle.setDefence(discipleInfo.getDefaultDefence() + Math.round(discipleInfo.getGrowDefence() * level));
		battle.setHealth(discipleInfo.getDefaultHealth() + Math.round(discipleInfo.getGrowHealth() * level));
		battle.setRp(discipleInfo.getDefaultRp() + Math.round(discipleInfo.getGrowRp() * level));
		battle.setDefaultskill(discipleInfo.getDefaultSkill());
		battle.setDefaultskillitem(discipleInfo.getDefaultSkill());
		SkillXmlTemplate skillInfo = getSkillTemplate(discipleInfo.getDefaultSkill());
		battle.setDefaultskilltype(skillInfo.getType());
		battle.setDefaultskillquantity(skillInfo.getQuantity());
		return battle;
	}
	
	private static void loadKindSelector() throws Exception{
		String itemConfPath = DZM.basePath + "/xml/kindSelecter.xml";
		log.info("load item config file path :{}",itemConfPath);
		XMLConfiguration conf = new XMLConfiguration(itemConfPath);
		List<HierarchicalConfiguration> fields = conf.configurationsAt("Selects.Select");
		if (fields != null && fields.size() > 0) {
			HierarchicalConfiguration sub;
			KindSelecter it1 = new KindSelecter();
			KindSelecter it2 = new KindSelecter();
			KindSelecter it3 = new KindSelecter();
			for (int i = 0; i < fields.size(); i++) {
				sub = fields.get(i);
				int type = sub.getInt("[@types]");
				int kind = sub.getInt("[@kind]");
				int rate1 = sub.getInt("[@rate1]");
				int rate2 = sub.getInt("[@rate2]");
				int rate3 = sub.getInt("[@rate3]");
				int rate4 = sub.getInt("[@rate4]");
				int rate5 = sub.getInt("[@rate5]");
				int rate6 = sub.getInt("[@rate6]");
				
				if (type == 1) {
					it1.getKinds().add(kind);
						it1.getRate1().add(rate1);
						it1.getRate2().add(rate2);
						it1.getRate3().add(rate3);
						it1.getRate4().add(rate4);
						it1.getRate5().add(rate5);
						it1.getRate6().add(rate6);
				}
				else if (type == 2) {
					it2.getKinds().add(kind);
						it2.getRate1().add(rate1);
						it2.getRate2().add(rate2);
						it2.getRate3().add(rate3);
						it2.getRate4().add(rate4);
						it2.getRate5().add(rate5);
						it2.getRate6().add(rate6);
				}
				else if (type == 3) {
					it3.getKinds().add(kind);
						it3.getRate1().add(rate1);
						it3.getRate2().add(rate2);
						it3.getRate3().add(rate3);
						it3.getRate4().add(rate4);
						it3.getRate5().add(rate5);
						it3.getRate6().add(rate6);
				}
			}
			kindSelecters.put(1, it1);
			kindSelecters.put(2, it2);
			kindSelecters.put(3, it3);
		}
		xmlLoadTime.put("kindSelecter", Util.getServerTime());
	}
	
	private static void loadDiscipleSelecter() throws Exception{
		String itemConfPath = DZM.basePath + "/xml/discipleSelecter.xml";
		log.info("load item config file path :{}",itemConfPath);
		XMLConfiguration conf = new XMLConfiguration(itemConfPath);
		List<HierarchicalConfiguration> fields = conf.configurationsAt("Selects.Select");
		if (fields != null && fields.size() > 0) {
			HierarchicalConfiguration sub;
			KindSelecter it1 = new KindSelecter();
			KindSelecter it2 = new KindSelecter();
			KindSelecter it3 = new KindSelecter();
			KindSelecter it4 = new KindSelecter();
			for (int i = 0; i < fields.size(); i++) {
				sub = fields.get(i);
				int discipleId = sub.getInt("[@discipleid]");
				int kind = sub.getInt("[@kind]");
				int rate1 = sub.getInt("[@rate1]");
				int rate2 = sub.getInt("[@rate2]");
				int rate3 = sub.getInt("[@rate3]");
				int rate4 = sub.getInt("[@rate4]");
				int rate5 = sub.getInt("[@rate5]");
				int rate6 = sub.getInt("[@rate6]");
				
				if (kind == 1) {
					it1.getKinds().add(discipleId);
						it1.getRate1().add(rate1);
						it1.getRate2().add(rate2);
						it1.getRate3().add(rate3);
						it1.getRate4().add(rate4);
						it1.getRate5().add(rate5);
						it1.getRate6().add(rate6);
				}
				else if (kind == 2) {
					it2.getKinds().add(discipleId);
						it2.getRate1().add(rate1);
						it2.getRate2().add(rate2);
						it2.getRate3().add(rate3);
						it2.getRate4().add(rate4);
						it2.getRate5().add(rate5);
						it2.getRate6().add(rate6);
				}
				else if (kind == 3) {
					it3.getKinds().add(discipleId);
						it3.getRate1().add(rate1);
						it3.getRate2().add(rate2);
						it3.getRate3().add(rate3);
						it3.getRate4().add(rate4);
						it3.getRate5().add(rate5);
						it3.getRate6().add(rate6);
				}
				else if (kind == 4) {
					it4.getKinds().add(discipleId);
						it4.getRate1().add(rate1);
						it4.getRate2().add(rate2);
						it4.getRate3().add(rate3);
						it4.getRate4().add(rate4);
						it4.getRate5().add(rate5);
						it4.getRate6().add(rate6);
				}
			}
			discipleSelecters.put(1, it1);
			discipleSelecters.put(2, it2);
			discipleSelecters.put(3, it3);
			discipleSelecters.put(4, it4);
		}
		xmlLoadTime.put("discipleSelecter", Util.getServerTime());
	}
	
	private static void loadLevelUpTemplates() throws Exception{
		String levelUpConfPath = DZM.basePath + "/xml/levelUp.xml";
		log.info("load item config file path :{}",levelUpConfPath);
		XMLConfiguration conf = new XMLConfiguration(levelUpConfPath);
		List<HierarchicalConfiguration> fields = conf.configurationsAt("LevelUps.LevelUp");
		if (fields != null && fields.size() > 0) {
		    HierarchicalConfiguration sub;
			for (int i = 0; i < fields.size(); i++) {
			    LevelUpXmlTemplate it = new LevelUpXmlTemplate();
				sub = fields.get(i);
				int id = sub.getInt("[@id]");
				int needLevel = sub.getInt("[@needLevel]");
				int goldcoins = sub.getInt("[@goldcoins]");
				int upVipLevel = sub.getInt("[@upVipLevel]");
				int rechargenum = sub.getInt("[@rechargenum]");
				
				it.setId(id);
				it.setNeedLevel(needLevel);
				it.setGoldcoins(goldcoins);
				it.setUpVipLevel(upVipLevel);
				it.setRechargenum(rechargenum);
			    levelUpTemplates.put(id, it);
		    }
		    sub = null;
		}
		xmlLoadTime.put("levelUp", Util.getServerTime());
	}
	
	private static void loadTeamBuffTemplates() throws Exception{
		String teamBuffConfPath = DZM.basePath + "/xml/teamBuff.xml";
		log.info("load item config file path :{}",teamBuffConfPath);
		XMLConfiguration conf = new XMLConfiguration(teamBuffConfPath);
		List<HierarchicalConfiguration> fields = conf.configurationsAt("TeamBuffs.TeamBuff");
		if (fields != null && fields.size() > 0) {
		    HierarchicalConfiguration sub;
			for (int i = 0; i < fields.size(); i++) {
			    TeamBuffTemplate it = new TeamBuffTemplate();
				sub = fields.get(i);

				int id = sub.getInt("[@id]");
				String name = sub.getString("[@name]");
				int increaseType = sub.getInt("[@increaseType]");
				int unlockLevel = sub.getInt("[@unlockLevel]");
				int initLevel = sub.getInt("[@initLevel]");
				int increaseLevel = sub.getInt("[@increaseLevel]");
				String icon = sub.getString("[@icon]");
				String smallIcon = sub.getString("[@smallIcon]");
				
				it.setId(id);
				it.setName(name);
				it.setIncreaseType(increaseType);
				it.setUnlockLevel(unlockLevel);
				it.setInitLevel(initLevel);
				it.setIncreaseLevel(increaseLevel);
				it.setIcon(icon);
				it.setSmallIcon(smallIcon);
				
			    teamBuffTemplates.put(id, it);
		    }
		    sub = null;
		}
		xmlLoadTime.put("teamBuff", Util.getServerTime());
	}
	
	private static void loadTeamBuffBoldnessTemplates() throws Exception{
		String teamBuffBoldnessConfPath = DZM.basePath + "/xml/teamBuffBoldness.xml";
		log.info("load item config file path :{}",teamBuffBoldnessConfPath);
		XMLConfiguration conf = new XMLConfiguration(teamBuffBoldnessConfPath);
		List<HierarchicalConfiguration> fields = conf.configurationsAt("TeamBuffsBoldnesss.TeamBuffsBoldness");
		if (fields != null && fields.size() > 0) {
		    HierarchicalConfiguration sub;
			for (int i = 0; i < fields.size(); i++) {
			    TeamBuffBoldnessTemplate it = new TeamBuffBoldnessTemplate();
				sub = fields.get(i);

				int quality = sub.getInt("[@quality]");
				int soulCourage = sub.getInt("[@soulCourage]");
				int discipleCourage = sub.getInt("[@discipleCourage]");
				
				it.setQuality(quality);
				it.setSoulCourage(soulCourage);
				it.setDiscipleCourage(discipleCourage);
				
			    teamBuffBoldnessTemplates.put(quality, it);
		    }
		    sub = null;
		}
		xmlLoadTime.put("teamBuffBoldness", Util.getServerTime());
	}

	private static void loadSysBasicConfig(){
		try {
			String itemConfPath = DZM.basePath + "/xml/sysbasicconfig.xml";
			XMLConfiguration xmlConf = new XMLConfiguration(itemConfPath);
			sysBasicConfig.setTenCoolTime(xmlConf.getInt("tenCoolTime"));
			sysBasicConfig.setTenCostGold(xmlConf.getInt("tenCostGold"));
			sysBasicConfig.setHundredCoolTime(xmlConf.getInt("hundredCoolTime"));
			sysBasicConfig.setHundredCostGold(xmlConf.getInt("hundredCostGold"));
			sysBasicConfig.setWanliCoolTime(xmlConf.getInt("wanliCoolTime"));
			sysBasicConfig.setWanliCostGold(xmlConf.getInt("wanliCostGold"));
			List<Integer> lists = new ArrayList<Integer>();
			lists.add(0);
			int limit = xmlConf.getInt("pool1");
			lists.add(limit);
			limit = xmlConf.getInt("pool2");
			lists.add(limit);
			limit = xmlConf.getInt("pool3");
			lists.add(limit);
			limit = xmlConf.getInt("pool4");
			lists.add(limit);
			limit = xmlConf.getInt("pool5");
			lists.add(limit);
			limit = xmlConf.getInt("pool6");
			lists.add(limit);
			sysBasicConfig.setPoolLimits(lists);
			
			lists = new ArrayList<Integer>();
            lists.add(0);
            limit = xmlConf.getInt("soulcount1");
            lists.add(limit);
            limit = xmlConf.getInt("soulcount2");
            lists.add(limit);
            limit = xmlConf.getInt("soulcount3");
            lists.add(limit);
            limit = xmlConf.getInt("soulcount4");
            lists.add(limit);
            sysBasicConfig.setSoulCounts(lists);

            lists = new ArrayList<Integer>();
            lists.add(0);
            limit = xmlConf.getInt("treasurePoolNeed1");
            lists.add(limit);
            limit = xmlConf.getInt("treasurePoolNeed2");
            lists.add(limit);
            limit = xmlConf.getInt("treasurePoolNeed3");
            lists.add(limit);
            limit = xmlConf.getInt("treasurePoolNeed4");
            lists.add(limit);
            sysBasicConfig.setTreasurePoolNeed(lists);

            lists = new ArrayList<Integer>();
            limit = xmlConf.getInt("treasureClearCount2");
            lists.add(limit);
            limit = xmlConf.getInt("treasureClearCount3");
            lists.add(limit);
            sysBasicConfig.setTreasureClearCount(lists);

            sysBasicConfig.setCareerLuckyProportion(xmlConf.getFloat("careerLuckyProportion"));
            sysBasicConfig.setFightEndLuckyProportion(xmlConf.getFloat("fightEndLuckyProportion"));
            sysBasicConfig.setEmptyLuckPoolProportion(xmlConf.getFloat("emptyLuckPoolProportion"));
            
            List<HierarchicalConfiguration> fields2 = xmlConf.configurationsAt("DeepWaterConfigs.DeepWaterConfig");
        	if (fields2 != null && fields2.size() > 0) {
            	HierarchicalConfiguration sub;
            	for (int i = 0; i < fields2.size(); i++)
                {
    		        sub = fields2.get(i);
                    int typeid = sub.getInt("[@typeid]");
                    int costType = sub.getInt("[@costType]");
                    int price = sub.getInt("[@price]");
                    int countLimit=sub.getInt("[@countLimit]");
                    int[] config=new int[4];
                    config[0]=typeid;
                    config[1]=costType;
                    config[2]=price;
                    config[3]=countLimit;
                    deepWaterConfigs.put(typeid, config);
                }
            }
		} catch (ConfigurationException e) {
			e.printStackTrace();
		}
		xmlLoadTime.put("sysBasicConfig", Util.getServerTime());
	}
	
	private static void loadDiscipleLevel(){
		try {
			String itemConfPath = DZM.basePath + "/xml/disciple_exp_level.xml";
			log.info("load item config file path :{}",itemConfPath);
			XMLConfiguration conf = new XMLConfiguration(itemConfPath);
			List<HierarchicalConfiguration> fields = conf.configurationsAt("Levels.Level");
			if (fields != null && fields.size() > 0) {
				HierarchicalConfiguration sub;
				for (int i = 0; i < fields.size(); i++) {
					sub = fields.get(i);
					int id = sub.getInt("[@id]");
					float multiple = sub.getFloat("[@quality]");
					discipleLevels.put(id, multiple);
				}
			}
		} catch (ConfigurationException e) {
			e.printStackTrace();
		}
		xmlLoadTime.put("discipleLevel", Util.getServerTime());
	}
	
	private static void loadUserLevel(){
		try {
			String itemConfPath = DZM.basePath + "/xml/user_level.xml";
			log.info("load item config file path :{}",itemConfPath);
			XMLConfiguration conf = new XMLConfiguration(itemConfPath);
			List<HierarchicalConfiguration> fields = conf.configurationsAt("Levels.Level");
			if (fields != null && fields.size() > 0) {
				HierarchicalConfiguration sub;
				for (int i = 0; i < fields.size(); i++) {
					sub = fields.get(i);
					int id = sub.getInt("[@id]");
					int multiple = sub.getInt("[@quality]");
					// int multiple = (int) (sub.getInt("[@quality]") * 0.7f);   ///1人物升级所需经验降低30%
					userLevels.put(id, multiple);
				}
			}
		} catch (ConfigurationException e) {
			e.printStackTrace();
		}
		xmlLoadTime.put("userLevel", Util.getServerTime());
	}
	
	private static void loadUserLevelConfig(){
		try {
			String itemConfPath = DZM.basePath + "/xml/user_level_config.xml";
			log.info("load item config file path :{}",itemConfPath);
			XMLConfiguration conf = new XMLConfiguration(itemConfPath);
			List<HierarchicalConfiguration> fields = conf.configurationsAt("Levels.Level");
			if (fields != null && fields.size() > 0) {
				HierarchicalConfiguration sub;
				for (int i = 0; i < fields.size(); i++) {
					UserLevelConfig it = new UserLevelConfig();
					sub = fields.get(i);
					it.setLevel(sub.getInt("[@id]"));
					it.setCareerXp(sub.getInt("[@careerXp]"));
					it.setChallengeUserXp(sub.getInt("[@challengeUserXp]"));
					it.setChallengeDiscipleXp(sub.getInt("[@challengeDiscipleXp]"));
					it.setChallengeCoins(sub.getInt("[@challengeCoins]"));
					it.setCoins1(sub.getInt("[@coins1]"));
					it.setWorldBossRumCount(sub.getInt("[@worldBossRumCount]"));
					it.setWorldBossRumProbability(sub.getInt("[@worldBossRumProbability]"));
					
					userLevelConfigs.put(sub.getInt("[@id]"), it);
				}
			}
			conf.clear();
		} catch (ConfigurationException e) {
			e.printStackTrace();
		}
		xmlLoadTime.put("userLevelConfig", Util.getServerTime());
	}
	
	private static void loadTreasureSelecter() throws Exception{
      	  String itemConfPath = DZM.basePath + "/xml/treasureSelecter.xml";
        log.info("load item config file path :{}",itemConfPath);
        XMLConfiguration conf = new XMLConfiguration(itemConfPath);
        List<HierarchicalConfiguration> fields = conf.configurationsAt("Selects.Select");
        if (fields != null && fields.size() > 0) {
            HierarchicalConfiguration sub;
            TreasureSelecter it1 = new TreasureSelecter();
            TreasureSelecter it2 = new TreasureSelecter();
            TreasureSelecter it3 = new TreasureSelecter();
            TreasureSelecter it4 = new TreasureSelecter();
            TreasureSelecter it101 = new TreasureSelecter();
            TreasureSelecter it102 = new TreasureSelecter();
            for (int i = 0; i < fields.size(); i++) {
                sub = fields.get(i);
                int id = sub.getInt("[@id]");
                int type = sub.getInt("[@types]");
                int itemType = sub.getInt("[@itemType]");
                int propId = sub.getInt("[@propId]");
                int count = sub.getInt("[@count]");
                int worth = sub.getInt("[@worth]");
                float rate1 = sub.getFloat("[@rate1]");
                float rate2 = sub.getFloat("[@rate2]");
                float rate3 = sub.getFloat("[@rate3]");
                float rate4 = sub.getFloat("[@rate4]");
                if (type == 1) {
                    it1.getId().add(id);
                    it1.getType().add(type);
                    it1.getItemType().add(itemType);
                    it1.getPropId().add(propId);
                    it1.getCount().add(count);
                    it1.getWorth().add(worth);
                    if (it1.getRate1().size() == 0) {
                        it1.getRate1().add(rate1);
                        it1.getRate2().add(rate2);
                        it1.getRate3().add(rate3);
                        it1.getRate4().add(rate4);
                    }
                    else {
                        int size = it1.getRate1().size();
                        it1.getRate1().add(rate1 + it1.getRate1().get(size - 1));
                        it1.getRate2().add(rate2 + it1.getRate2().get(size - 1));
                        it1.getRate3().add(rate3 + it1.getRate3().get(size - 1));
                        it1.getRate4().add(rate4 + it1.getRate4().get(size - 1));
                    }
                }
                else if (type == 2) {
                    it2.getId().add(id);
                    it2.getType().add(type);
                    it2.getItemType().add(itemType);
                    it2.getPropId().add(propId);
                    it2.getCount().add(count);
                    it2.getWorth().add(worth);
                    if (it2.getRate1().size() == 0) {
                        it2.getRate1().add(rate1);
                        it2.getRate2().add(rate2);
                        it2.getRate3().add(rate3);
                        it2.getRate4().add(rate4);
                    }
                    else {
                        int size = it2.getRate1().size();
                        it2.getRate1().add(rate1 + it2.getRate1().get(size - 1));
                        it2.getRate2().add(rate2 + it2.getRate2().get(size - 1));
                        it2.getRate3().add(rate3 + it2.getRate3().get(size - 1));
                        it2.getRate4().add(rate4 + it2.getRate4().get(size - 1));
                    }
                }
                else if (type == 3) {
                    it3.getId().add(id);
                    it3.getType().add(type);
                    it3.getItemType().add(itemType);
                    it3.getPropId().add(propId);
                    it3.getCount().add(count);
                    it3.getWorth().add(worth);
                    if (it3.getRate1().size() == 0) {
                        it3.getRate1().add(rate1);
                        it3.getRate2().add(rate2);
                        it3.getRate3().add(rate3);
                        it3.getRate4().add(rate4);
                    }
                    else {
                        int size = it3.getRate1().size();
                        it3.getRate1().add(rate1 + it3.getRate1().get(size - 1));
                        it3.getRate2().add(rate2 + it3.getRate2().get(size - 1));
                        it3.getRate3().add(rate3 + it3.getRate3().get(size - 1));
                        it3.getRate4().add(rate4 + it3.getRate4().get(size - 1));
                    }
                }
                else if (type == 4) {
                    it4.getId().add(id);
                    it4.getType().add(type);
                    it4.getItemType().add(itemType);
                    it4.getPropId().add(propId);
                    it4.getCount().add(count);
                    it4.getWorth().add(worth);
                    if (it4.getRate1().size() == 0) {
                        it4.getRate1().add(rate1);
                        it4.getRate2().add(rate2);
                        it4.getRate3().add(rate3);
                        it4.getRate4().add(rate4);
                    }
                    else {
                        int size = it4.getRate1().size();
                        it4.getRate1().add(rate1 + it4.getRate1().get(size - 1));
                        it4.getRate2().add(rate2 + it4.getRate2().get(size - 1));
                        it4.getRate3().add(rate3 + it4.getRate3().get(size - 1));
                        it4.getRate4().add(rate4 + it4.getRate4().get(size - 1));
                    }
                }
                else if (type == 101) {
                    it101.getId().add(id);
                    it101.getType().add(type);
                    it101.getItemType().add(itemType);
                    it101.getPropId().add(propId);
                    it101.getCount().add(count);
                    it101.getWorth().add(worth);
                    if (it101.getRate1().size() == 0) {
                    	it101.getRate1().add(rate1);
                    	it101.getRate2().add(rate2);
                    	it101.getRate3().add(rate3);
                    	it101.getRate4().add(rate4);
                    }
                    else {
                        int size = it101.getRate1().size();
                        it101.getRate1().add(rate1 + it101.getRate1().get(size - 1));
                        it101.getRate2().add(rate2 + it101.getRate2().get(size - 1));
                        it101.getRate3().add(rate3 + it101.getRate3().get(size - 1));
                        it101.getRate4().add(rate4 + it101.getRate4().get(size - 1));
                    }
                }
                else if (type == 102) {
                	it102.getId().add(id);
                	it102.getType().add(type);
                	it102.getItemType().add(itemType);
                	it102.getPropId().add(propId);
                	it102.getCount().add(count);
                	it102.getWorth().add(worth);
                    if (it102.getRate1().size() == 0) {
                    	it102.getRate1().add(rate1);
                    	it102.getRate2().add(rate2);
                    	it102.getRate3().add(rate3);
                    	it102.getRate4().add(rate4);
                    }
                    else {
                        int size = it102.getRate1().size();
                        it102.getRate1().add(rate1 + it102.getRate1().get(size - 1));
                        it102.getRate2().add(rate2 + it102.getRate2().get(size - 1));
                        it102.getRate3().add(rate3 + it102.getRate3().get(size - 1));
                        it102.getRate4().add(rate4 + it102.getRate4().get(size - 1));
                    }
                }

            }
            treasureSelecters.put(1, it1);
            treasureSelecters.put(2, it2);
            treasureSelecters.put(3, it3);
            treasureSelecters.put(4, it4);
            treasureSelecters.put(101, it101);
            treasureSelecters.put(102, it102);
        }
        xmlLoadTime.put("treasureSelecter", Util.getServerTime());
    }
    
    private static void loadToolsTemplates() throws Exception{
        String itemConfPath = DZM.basePath + "/xml/tools.xml";
        log.info("load item config file path :{}",itemConfPath);
        XMLConfiguration conf = new XMLConfiguration(itemConfPath);
        List<HierarchicalConfiguration> fields = conf.configurationsAt("Tools.Tool");
        if (fields != null && fields.size() > 0) {
            HierarchicalConfiguration sub;

            for (int i = 0; i < fields.size(); i++)
            {
                ToolsXmlTemplate it = new ToolsXmlTemplate();
                sub = fields.get(i);
                int toolId = sub.getInt("[@toolId]");
                int opType = sub.getInt("[@opType]");
                int priceType = sub.getInt("[@priceType]");
                int price = sub.getInt("[@price]");
                int buyTimes = sub.getInt("[@buyTimes]");
                int vipLevel = sub.getInt("[@vipLevel]");
                String containTools = sub.getString("[@containTools]");
                it.setToolId(toolId);
                it.setOpType(opType);
                it.setPriceType(priceType);
                it.setPrice(price);
                it.setBuyTimes(buyTimes);
                it.setContainTools(containTools);
                it.setVipLevel(vipLevel);
                toolsTemplates.put(toolId, it);
            }
        }
        xmlLoadTime.put("tools", Util.getServerTime());
    }
    private static void loadCdkeyTemplates() throws Exception{
        String itemConfPath = DZM.basePath + "/xml/cdkeys.xml";
        log.info("load item config file path :{}",itemConfPath);
        XMLConfiguration conf = new XMLConfiguration(itemConfPath);
        List<HierarchicalConfiguration> fields = conf.configurationsAt("Cdkeys.Cdkey");
        if (fields != null && fields.size() > 0) {
            HierarchicalConfiguration sub;

            for (int i = 0; i < fields.size(); i++)
            {
                CdkeyXmlTemplate it = new CdkeyXmlTemplate();
                sub = fields.get(i);
                
                String typeid=sub.getString("[@typeId]");
                String rewards = sub.getString("[@rewards]");
                it.setType(typeid);
                it.setRewards(rewards);
                cdkeyTemplates.put(typeid, it);
            }
        }
        xmlLoadTime.put("cdkeys", Util.getServerTime());
    }
    private static void loadGuideTemplates() throws Exception{
    	String itemConfPath = DZM.basePath + "/xml/guideConfiger.xml";
    	log.info("load item config file path :{}",itemConfPath);
    	XMLConfiguration conf = new XMLConfiguration(itemConfPath);
    	List<HierarchicalConfiguration> fields = conf.configurationsAt("Guides.Guide");
    	 if (fields != null && fields.size() > 0) {
             HierarchicalConfiguration sub;
             for (int i = 0; i < fields.size(); i++)
             {
                 GuideConfigerXmlTemplate it = new GuideConfigerXmlTemplate();
                 sub = fields.get(i);
                 int id=sub.getInt("[@id]");
                 String type=sub.getString("[@type]");
                 int itemId = sub.getInt("[@itemId]");
                 int price = sub.getInt("[@price]");
                 int count = sub.getInt("[@count]");
                 it.setId(id);
                 it.setType(type);
                 it.setItemId(itemId);
                 it.setPrice(price);
                 it.setCount(count);
                 guideConfigerXmlTemplates.put(id, it);
             }
         }
    	xmlLoadTime.put("guideConfiger", Util.getServerTime());
    }
    private static void loadChargeRebateXmlTemplates() throws Exception{
    	String itemConfPath = DZM.basePath + "/xml/chargeRebateConfig.xml";
    	log.info("load item config file path :{}",itemConfPath);
    	XMLConfiguration conf = new XMLConfiguration(itemConfPath);
    	List<HierarchicalConfiguration> fields = conf.configurationsAt("ChargeRebates.ChargeRebate");
    	if (fields != null && fields.size() > 0) {
    		HierarchicalConfiguration sub;
    		for (int i = 0; i < fields.size(); i++)
    		{
    			ChargeRebateXmlTemplate it = new ChargeRebateXmlTemplate();
    			sub = fields.get(i);
    			int id=sub.getInt("[@id]");
    			int activeTimeInterval=sub.getInt("[@activeTimeInterval]");
    			int chargeDuration = sub.getInt("[@chargeDuration]");
    			int rebateDuration = sub.getInt("[@rebateDuration]");
    			int minRecharge = sub.getInt("[@minRecharge]");
    			int maxRecharge = sub.getInt("[@maxRecharge]");
    			float refundRate = sub.getFloat("[@refundRate]");
    			it.setId(id);
    			it.setActiveTimeInterval(activeTimeInterval);
    			it.setChargeDuration(chargeDuration);
    			it.setRebateDuration(rebateDuration);
    			it.setMinRecharge(minRecharge);
    			it.setMaxRecharge(maxRecharge);
    			it.setRefundRate(refundRate);
    			chargeRebateXmlTemplates.put(id, it);
    		}
    	}
    	xmlLoadTime.put("chargeRebateConfig", Util.getServerTime());
    }
    
    private static void loadFortuneComeXmlTemplates() throws Exception{
        	String itemConfPath = DZM.basePath + "/xml/fortuneComeConfig.xml";
        	log.info("load item config file path :{}",itemConfPath);
        	XMLConfiguration conf = new XMLConfiguration(itemConfPath);
        	List<HierarchicalConfiguration> fields = conf.configurationsAt("FortuneComes.FortuneCome");
        	if (fields != null && fields.size() > 0) {
        		HierarchicalConfiguration sub;
        		for (int i = 0; i < fields.size(); i++)
        		{
        			FortuneComeXmlTemplate it = new FortuneComeXmlTemplate();
        			sub = fields.get(i);
        			int id=sub.getInt("[@id]");
        			int needGold=sub.getInt("[@needGold]");
        			int minReward = sub.getInt("[@minReward]");
        			int maxReward = sub.getInt("[@maxReward]");
        			it.setId(id);
        			it.setNeedGold(needGold);
        			it.setMinReward(minReward);
        			it.setMaxReward(maxReward);
        			fortuneComeXmlTemplas.put(id, it);
        		}
        	}
        	xmlLoadTime.put("fortuneCome", Util.getServerTime());
    }
    
    
    private static void loadLevelSilvers() throws Exception{
        String itemConfPath = DZM.basePath + "/xml/levelSilvers.xml";
        log.info("load item config file path :{}",itemConfPath);
        XMLConfiguration conf = new XMLConfiguration(itemConfPath);
        List<HierarchicalConfiguration> fields = conf.configurationsAt("Silvers.Silver");
        if (fields != null && fields.size() > 0) {
            HierarchicalConfiguration sub;
            for (int i = 0; i < fields.size(); i++) {
                sub = fields.get(i);
                int count = sub.getInt("[@count]");
                lvSilversTemplate.getSilvers().add(count);
            }
        }
        xmlLoadTime.put("levelSilvers", Util.getServerTime());
    }
    private static void loadSkillSelecter() throws Exception{
        String itemConfPath = DZM.basePath + "/xml/skillSelecter.xml";
        log.info("load item config file path :{}",itemConfPath);
        XMLConfiguration conf = new XMLConfiguration(itemConfPath);
        List<HierarchicalConfiguration> fields = conf.configurationsAt("SkillSelecters.SkillSelecter");
        if (fields != null && fields.size() > 0) {
            HierarchicalConfiguration sub;
            SkillSelecter it1 = new SkillSelecter();
            SkillSelecter it2 = new SkillSelecter();
            SkillSelecter it3 = new SkillSelecter();
            SkillSelecter it4 = new SkillSelecter();
            SkillSelecter allSkill=new SkillSelecter();
            for (int i = 0; i < fields.size(); i++) {
                sub = fields.get(i);
                int id = sub.getInt("[@id]");
                int quality = sub.getInt("[@quality]");
                int skillType = sub.getInt("[@skillType]");
                int rate1 = sub.getInt("[@rate1]");
                int rate2 = sub.getInt("[@rate2]");
                int rate3 = sub.getInt("[@rate3]");
                int rate4 = sub.getInt("[@rate4]");
                int continuesLoginRate = sub.getInt("[@continuesLoginRate]");
                int visitRate = sub.getInt("[@visitRate]");
                int deepWaterRat1 = sub.getInt("[@deepWaterRat1]");
                int deepWaterRat2 = sub.getInt("[@deepWaterRat2]");
                int deepWaterRat3 = sub.getInt("[@deepWaterRat3]");
                allSkill.getId().add(id);
                allSkill.getSkillType().add(skillType);
                if (allSkill.getRate1().size() == 0) {
                	allSkill.getRate1().add(rate1);
                	allSkill.getRate2().add(rate2);
                	allSkill.getRate3().add(rate3);
                	allSkill.getRate4().add(rate4);
                	allSkill.getContinuesLoginRate().add(continuesLoginRate);
                	allSkill.getVisitRate().add(visitRate);
                	allSkill.getDeepWaterRat1().add(deepWaterRat1);
                	allSkill.getDeepWaterRat2().add(deepWaterRat2);
                	allSkill.getDeepWaterRat3().add(deepWaterRat3);
                }
                else {
                    int size = allSkill.getRate1().size();
                    allSkill.getRate1().add(rate1 + allSkill.getRate1().get(size - 1));
                    allSkill.getRate2().add(rate2 + allSkill.getRate2().get(size - 1));
                    allSkill.getRate3().add(rate3 + allSkill.getRate3().get(size - 1));
                    allSkill.getRate4().add(rate4 + allSkill.getRate4().get(size - 1));
                    allSkill.getContinuesLoginRate().add(continuesLoginRate+allSkill.getContinuesLoginRate().get(size-1));
                    allSkill.getVisitRate().add(visitRate+allSkill.getVisitRate().get(size-1));
                    allSkill.getDeepWaterRat1().add(deepWaterRat1+allSkill.getDeepWaterRat1().get(size-1));
                    allSkill.getDeepWaterRat2().add(deepWaterRat2+allSkill.getDeepWaterRat2().get(size-1));
                    allSkill.getDeepWaterRat3().add(deepWaterRat3+allSkill.getDeepWaterRat3().get(size-1));
                }
                
                if (quality == 1) {
                    it1.getId().add(id);
                    it1.getSkillType().add(skillType);
                    if (it1.getRate1().size() == 0) {
                        it1.getRate1().add(rate1);
                        it1.getRate2().add(rate2);
                        it1.getRate3().add(rate3);
                        it1.getRate4().add(rate4);
                        it1.getContinuesLoginRate().add(continuesLoginRate);
                        it1.getVisitRate().add(visitRate);
                        it1.getDeepWaterRat1().add(deepWaterRat1);
                        it1.getDeepWaterRat2().add(deepWaterRat2);
                        it1.getDeepWaterRat3().add(deepWaterRat3);
                    }
                    else {
                        int size = it1.getRate1().size();
                        it1.getRate1().add(rate1 + it1.getRate1().get(size - 1));
                        it1.getRate2().add(rate2 + it1.getRate2().get(size - 1));
                        it1.getRate3().add(rate3 + it1.getRate3().get(size - 1));
                        it1.getRate4().add(rate4 + it1.getRate4().get(size - 1));
                        it1.getContinuesLoginRate().add(continuesLoginRate+it1.getContinuesLoginRate().get(size-1));
                        it1.getVisitRate().add(visitRate+it1.getVisitRate().get(size-1));
                        it1.getDeepWaterRat1().add(deepWaterRat1+it1.getDeepWaterRat1().get(size-1));
                        it1.getDeepWaterRat2().add(deepWaterRat2+it1.getDeepWaterRat2().get(size-1));
                        it1.getDeepWaterRat3().add(deepWaterRat3+it1.getDeepWaterRat3().get(size-1));
                    }
                }
                else if (quality == 2) {
                    it2.getId().add(id);
                    it2.getSkillType().add(skillType);
                    if (it2.getRate1().size() == 0) {
                        it2.getRate1().add(rate1);
                        it2.getRate2().add(rate2);
                        it2.getRate3().add(rate3);
                        it2.getRate4().add(rate4);
                        it2.getContinuesLoginRate().add(continuesLoginRate);
                        it2.getVisitRate().add(visitRate);
                        it2.getDeepWaterRat1().add(deepWaterRat1);
                        it2.getDeepWaterRat2().add(deepWaterRat2);
                        it2.getDeepWaterRat3().add(deepWaterRat3);
                    }
                    else {
                        int size = it2.getRate1().size();
                        it2.getRate1().add(rate1 + it2.getRate1().get(size - 1));
                        it2.getRate2().add(rate2 + it2.getRate2().get(size - 1));
                        it2.getRate3().add(rate3 + it2.getRate3().get(size - 1));
                        it2.getRate4().add(rate4 + it2.getRate4().get(size - 1));
                        it2.getContinuesLoginRate().add(continuesLoginRate+it2.getContinuesLoginRate().get(size-1));
                        it2.getVisitRate().add(visitRate+it2.getVisitRate().get(size-1));
                        it2.getDeepWaterRat1().add(deepWaterRat1+it2.getDeepWaterRat1().get(size-1));
                        it2.getDeepWaterRat2().add(deepWaterRat2+it2.getDeepWaterRat2().get(size-1));
                        it2.getDeepWaterRat3().add(deepWaterRat3+it2.getDeepWaterRat3().get(size-1));
                    }
                }
                else if (quality == 3) {
                    it3.getId().add(id);
                    it3.getSkillType().add(skillType);
                    if (it3.getRate1().size() == 0) {
                        it3.getRate1().add(rate1);
                        it3.getRate2().add(rate2);
                        it3.getRate3().add(rate3);
                        it3.getRate4().add(rate4);
                        it3.getContinuesLoginRate().add(continuesLoginRate);
                        it3.getVisitRate().add(visitRate);
                        it3.getDeepWaterRat1().add(deepWaterRat1);
                        it3.getDeepWaterRat2().add(deepWaterRat2);
                        it3.getDeepWaterRat3().add(deepWaterRat3);
                    }
                    else {
                        int size = it3.getRate1().size();
                        it3.getRate1().add(rate1 + it3.getRate1().get(size - 1));
                        it3.getRate2().add(rate2 + it3.getRate2().get(size - 1));
                        it3.getRate3().add(rate3 + it3.getRate3().get(size - 1));
                        it3.getRate4().add(rate4 + it3.getRate4().get(size - 1));
                        it3.getContinuesLoginRate().add(continuesLoginRate+it3.getContinuesLoginRate().get(size-1));
                        it3.getVisitRate().add(visitRate+it3.getVisitRate().get(size-1));
                        it3.getDeepWaterRat1().add(deepWaterRat1+it3.getDeepWaterRat1().get(size-1));
                        it3.getDeepWaterRat2().add(deepWaterRat2+it3.getDeepWaterRat2().get(size-1));
                        it3.getDeepWaterRat3().add(deepWaterRat3+it3.getDeepWaterRat3().get(size-1));
                    }
                }
                else if (quality == 4) {
                    it4.getId().add(id);
                    it4.getSkillType().add(skillType);
                    if (it4.getRate1().size() == 0) {
                        it4.getRate1().add(rate1);
                        it4.getRate2().add(rate2);
                        it4.getRate3().add(rate3);
                        it4.getRate4().add(rate4);
                        it4.getContinuesLoginRate().add(continuesLoginRate);
                        it4.getVisitRate().add(visitRate);
                        it4.getDeepWaterRat1().add(deepWaterRat1);
                        it4.getDeepWaterRat2().add(deepWaterRat2);
                        it4.getDeepWaterRat3().add(deepWaterRat3);
                    }
                    else {
                        int size = it4.getRate1().size();
                        it4.getRate1().add(rate1 + it4.getRate1().get(size - 1));
                        it4.getRate2().add(rate2 + it4.getRate2().get(size - 1));
                        it4.getRate3().add(rate3 + it4.getRate3().get(size - 1));
                        it4.getRate4().add(rate4 + it4.getRate4().get(size - 1));
                        it4.getContinuesLoginRate().add(continuesLoginRate+it4.getContinuesLoginRate().get(size-1));
                        it4.getVisitRate().add(visitRate+it4.getVisitRate().get(size-1));
                        it4.getDeepWaterRat1().add(deepWaterRat1+it4.getDeepWaterRat1().get(size-1));
                        it4.getDeepWaterRat2().add(deepWaterRat2+it4.getDeepWaterRat2().get(size-1));
                        it4.getDeepWaterRat3().add(deepWaterRat3+it4.getDeepWaterRat3().get(size-1));
                    }
                }

            }
            skillSelecters.put(1, it1);
            skillSelecters.put(2, it2);
            skillSelecters.put(3, it3);
            skillSelecters.put(4, it4);
            allSkillSelecters=allSkill;
        }
        xmlLoadTime.put("skillSelecter", Util.getServerTime());
    }
    private static void loadEquipSelecter() throws Exception{
        String itemConfPath = DZM.basePath + "/xml/equipSelecter.xml";
        log.info("load item config file path :{}",itemConfPath);
        XMLConfiguration conf = new XMLConfiguration(itemConfPath);
        List<HierarchicalConfiguration> fields = conf.configurationsAt("EquipSelecters.EquipSelecter");
        if (fields != null && fields.size() > 0) {
            HierarchicalConfiguration sub;
            EquipSelecter it1 = new EquipSelecter();
            EquipSelecter it2 = new EquipSelecter();
            EquipSelecter it3 = new EquipSelecter();
            EquipSelecter it4 = new EquipSelecter();
            EquipSelecter it5 = new EquipSelecter();
            EquipSelecter it6 = new EquipSelecter();
            EquipSelecter it7 = new EquipSelecter();
            EquipSelecter it8 = new EquipSelecter();
            EquipSelecter it9 = new EquipSelecter();
            EquipSelecter it10 = new EquipSelecter();
            EquipSelecter it11 = new EquipSelecter();
            EquipSelecter it12 = new EquipSelecter();
            for (int i = 0; i < fields.size(); i++) {
                sub = fields.get(i);
                int quality = sub.getInt("[@quality]");
                int equipType = sub.getInt("[@equipType]");
                if (quality == 1) {
                    switch (equipType)
                    {
                        case 1:
                            addEquipSelecterItem(it1,sub);
                            break;
                        case 2:
                            addEquipSelecterItem(it2,sub);
                            break;
                        case 3:
                            addEquipSelecterItem(it3,sub);
                            break;
                        default:
                            break;
                    }

                }
                else if (quality == 2) {
                    switch (equipType)
                    {
                        case 1:
                            addEquipSelecterItem(it4,sub);
                            break;
                        case 2:
                            addEquipSelecterItem(it5,sub);
                            break;
                        case 3:
                            addEquipSelecterItem(it6,sub);
                            break;
                        default:
                            break;
                    }
                }
                else if (quality == 3) {
                    switch (equipType)
                    {
                        case 1:
                            addEquipSelecterItem(it7,sub);
                            break;
                        case 2:
                            addEquipSelecterItem(it8,sub);
                            break;
                        case 3:
                            addEquipSelecterItem(it9,sub);
                            break;
                        default:
                            break;
                    }
                }
                else if (quality == 4) {
                    switch (equipType)
                    {
                        case 1:
                            addEquipSelecterItem(it10,sub);
                            break;
                        case 2:
                            addEquipSelecterItem(it11,sub);
                            break;
                        case 3:
                            addEquipSelecterItem(it12,sub);
                            break;
                        default:
                            break;
                    }
                }

            }
            equipSelecters.put(UserToolConst.A_WEAPON, it1);
            equipSelecters.put(UserToolConst.A_ARMOR, it2);
            equipSelecters.put(UserToolConst.A_ACCESSORIES, it3);
            equipSelecters.put(UserToolConst.B_WEAPON, it4);
            equipSelecters.put(UserToolConst.B_ARMOR, it5);
            equipSelecters.put(UserToolConst.B_ACCESSORIES, it6);
            equipSelecters.put(UserToolConst.C_WEAPON, it7);
            equipSelecters.put(UserToolConst.C_ARMOR, it8);
            equipSelecters.put(UserToolConst.C_ACCESSORIES, it9);
            equipSelecters.put(UserToolConst.D_WEAPON, it10);
            equipSelecters.put(UserToolConst.D_ARMOR, it11);
            equipSelecters.put(UserToolConst.D_ACCESSORIES, it12);
        }
        xmlLoadTime.put("equipSelecter", Util.getServerTime());
    }
    private static void addEquipSelecterItem(EquipSelecter selecter, HierarchicalConfiguration sub)
    {
        int id = sub.getInt("[@id]");
        int quality = sub.getInt("[@quality]");
        int equipType = sub.getInt("[@equipType]");
        int rate1 = sub.getInt("[@rate1]");
        int rate2 = sub.getInt("[@rate2]");
        int rate3 = sub.getInt("[@rate3]");
        int rate4 = sub.getInt("[@rate4]");
        int continuesLoginRate = sub.getInt("[@continuesLoginRate]");
        int visitRate = sub.getInt("[@visitRate]");
        selecter.getId().add(id);
        selecter.getQuality().add(quality);
        selecter.getEquipType().add(equipType);
        if (selecter.getRate1().size() == 0) {
            selecter.getRate1().add(rate1);
            selecter.getRate2().add(rate2);
            selecter.getRate3().add(rate3);
            selecter.getRate4().add(rate4);
            selecter.getContinuesLoginRate().add(continuesLoginRate);
            selecter.getVisitRate().add(visitRate);
        }
        else {
            int size = selecter.getRate1().size();
            selecter.getRate1().add(rate1 + selecter.getRate1().get(size - 1));
            selecter.getRate2().add(rate2 + selecter.getRate2().get(size - 1));
            selecter.getRate3().add(rate3 + selecter.getRate3().get(size - 1));
            selecter.getRate4().add(rate4 + selecter.getRate4().get(size - 1));
            selecter.getContinuesLoginRate().add(continuesLoginRate+selecter.getContinuesLoginRate().get(size-1));
            selecter.getVisitRate().add(visitRate+selecter.getVisitRate().get(size-1));
        }
    }
    private static void loadFightEndConfig() throws Exception
    {
        loadFightEndGeneralConfig();
        loadFightEndNpcConfig();
        loadFightEndNpcLevelConfig();
        loadFightEndRewardConfig();
    }

    private static void loadFightEndRewardConfig() throws Exception
    {
        String itemConfPath = DZM.basePath + "/xml/FightEndRankReward.xml";
        log.info("load item config file path :{}",itemConfPath);
        XMLConfiguration conf = new XMLConfiguration(itemConfPath);
        List<HierarchicalConfiguration> fields = conf.configurationsAt("FightEndRewards.item");
        if (fields != null && fields.size() > 0)
        {
            HierarchicalConfiguration sub;

            for (int i = 0; i < fields.size(); i++)
            {
                FightEndReward it = new FightEndReward();
                sub = fields.get(i);
                int level = sub.getInt("[@level]");
                JSONObject reward=new JSONObject();
                for(int j=5;j<=8;++j)
                {
                   String key="[@battle"+j+"Reward]";
                   String battleReward = sub.getString(key);
                   if(battleReward.length()>0&&battleReward.contains(","))
                   {
                       String[] arr = battleReward.split(",");
                       try
                       {
                           for (String anArr : arr)
                           {
                               String[] arrTmp = anArr.split(":");
                               JSONObject rewardItem = new JSONObject();
                               rewardItem.put("t", Integer.parseInt(arrTmp[0]));
                               rewardItem.put("i", Integer.parseInt(arrTmp[1]));
                               rewardItem.put("q", Integer.parseInt(arrTmp[2]));
                               if (!reward.containsKey(String.valueOf(j)))
                               {
                                   reward.put(j, new JSONArray());
                               }
                               reward.getJSONArray(String.valueOf(j)).add(rewardItem);
                           }
                       }
                       catch (Exception e)
                       {
                       }
                   }
                }
                it.setLevel(level);
                it.setRewardContent(reward);
                fightEndRewardTemplates.put(level, it);
            }
        }
        xmlLoadTime.put("fightEndRewards", Util.getServerTime());
    }
    private static void loadFightEndGeneralConfig() throws Exception
    {
        String itemConfPath = DZM.basePath + "/xml/FightEndGeneral.xml";
        log.info("load item config file path :{}",itemConfPath);
        XMLConfiguration conf = new XMLConfiguration(itemConfPath);
        List<HierarchicalConfiguration> fields = conf.configurationsAt("FightEndGenerals.item");
        if (fields != null && fields.size() > 0)
        {
            HierarchicalConfiguration sub;

            for (int i = 0; i < fields.size(); i++)
            {
                FightEndGeneral it = new FightEndGeneral();
                sub = fields.get(i);
                int barrier = sub.getInt("[@barrier]");
                int battle5Silver = sub.getInt("[@battle5Silver]");
                int battle6Silver = sub.getInt("[@battle6Silver]");
                int battle7Silver = sub.getInt("[@battle7Silver]");
                int battle8Silver = sub.getInt("[@battle8Silver]");
                String extra5Reward = sub.getString("[@extra5Reward]");
                String extra15Reward = sub.getString("[@extra15Reward]");
                String extra30Reward = sub.getString("[@extra30Reward]");
                String extra45Reward = sub.getString("[@extra45Reward]");
                it.setBarrier(barrier);
                it.setBattleSilver(5,battle5Silver);
                it.setBattleSilver(6,battle6Silver);
                it.setBattleSilver(7,battle7Silver);
                it.setBattleSilver(8,battle8Silver);
                it.setBattlerReward(5,extra5Reward);
                it.setBattlerReward(15,extra15Reward);
                it.setBattlerReward(30,extra30Reward);
                it.setBattlerReward(45,extra45Reward);
                fightEndGeneralsTemplates.put(barrier, it);
            }
        }
        xmlLoadTime.put("fightEndGenerals", Util.getServerTime());
    }
    private static void loadFightEndNpcConfig() throws Exception
    {
        String itemConfPath = DZM.basePath + "/xml/FightEndNpc.xml";
        log.info("load item config file path :{}",itemConfPath);
        XMLConfiguration conf = new XMLConfiguration(itemConfPath);

        List<HierarchicalConfiguration> fields = conf.configurationsAt("FightEndNpc.item");

        if (fields != null && fields.size() > 0)
        {
            HierarchicalConfiguration sub;
            JSONObject json=new JSONObject();
            for(int i=5;i<=8;++i)
            {
                JSONObject _battle=new JSONObject();
                for(int j=1;i<=3;j++)
                {
                    _battle.put(j,_battle);
                }
                json.put(i,_battle);
            }
            for (int i = 0; i < fields.size(); i++)
            {
                FightEndNpc it = new FightEndNpc();
                sub = fields.get(i);
                int id = sub.getInt("[@id]");
                int battle = sub.getInt("[@battle]");
                int pos_index = sub.getInt("[@pos_index]");
                int skill_rate = sub.getInt("[@skill_rate]");
                String npc_arr = sub.getString("[@npc_arr]");
                it.setId(id);
                it.setBattle(battle);
                it.setPos_index(pos_index);
                it.setAddition(FightEndInfo.OPFightEndInfoRet.Addition_Type.HP_VALUE, sub.getFloat("[@addition_hp]"));
                it.setAddition(FightEndInfo.OPFightEndInfoRet.Addition_Type.ATTATCK_VALUE, sub.getFloat("[@addition_attack]"));
                it.setAddition(FightEndInfo.OPFightEndInfoRet.Addition_Type.DEFENSE_VALUE, sub.getFloat("[@addition_defense]"));
                it.setAddition(FightEndInfo.OPFightEndInfoRet.Addition_Type.WILL_VALUE, sub.getFloat("[@addition_will]"));
                it.setSkill_rate(skill_rate);
                it.setNpcArr(npc_arr);
                fightEndNpcTemplates.put(id, it);
                json.getJSONObject(String.valueOf(battle)).accumulate(String.valueOf(pos_index),id);
            }
            AdventureFightEndUtil.getInstance().setBarrierIndexNpcObj(json);
        }
        xmlLoadTime.put("fightEndNpcTemplates", Util.getServerTime());
    }
    private static void loadFightEndNpcLevelConfig() throws Exception
    {
        String itemConfPath = DZM.basePath + "/xml/FightEndNpcLevel.xml";
        log.info("load item config file path :{}",itemConfPath);
        XMLConfiguration conf = new XMLConfiguration(itemConfPath);
        List<HierarchicalConfiguration> fields = conf.configurationsAt("FightEndNpcLevels.item");
        if (fields != null && fields.size() > 0)
        {
            HierarchicalConfiguration sub;

            for (int i = 0; i < fields.size(); i++)
            {
                FightEndNpcLevel it = new FightEndNpcLevel();
                sub = fields.get(i);
                int barrier = sub.getInt("[@barrier]");
                int battle5 = sub.getInt("[@battle5]");
                int battle6 = sub.getInt("[@battle6]");
                int battle7 = sub.getInt("[@battle7]");
                int battle8 = sub.getInt("[@battle8]");
                it.setBarrier(barrier);
                it.setBattleLevel(5, battle5);
                it.setBattleLevel(6, battle6);
                it.setBattleLevel(7, battle7);
                it.setBattleLevel(8, battle8);
                fightEndNpcLevelTemplates.put(barrier, it);
            }
        }
        xmlLoadTime.put("fightEndNpcLevelTemplates", Util.getServerTime());
    }
    
    private static void loadContinueLogin() throws Exception{
    	String levelUpConfPath = DZM.basePath + "/xml/continue_loging.xml";
		log.info("load item config file path :{}",levelUpConfPath);
		XMLConfiguration conf = new XMLConfiguration(levelUpConfPath);
		List<HierarchicalConfiguration> fields = conf.configurationsAt("Pools.Pool");
		if (fields != null && fields.size() > 0) {
		    HierarchicalConfiguration sub;
		    ArrayList<Integer> types1 = new ArrayList<Integer>();
		    ArrayList<Integer> types2 = new ArrayList<Integer>();
		    ArrayList<Integer> types3 = new ArrayList<Integer>();
		    
		    ArrayList<Integer> ids1 = new ArrayList<Integer>();
		    ArrayList<Integer> ids2 = new ArrayList<Integer>();
		    ArrayList<Integer> ids3 = new ArrayList<Integer>();
		    
		    ArrayList<Integer> counts1 = new ArrayList<Integer>();
		    ArrayList<Integer> counts2 = new ArrayList<Integer>();
		    ArrayList<Integer> counts3 = new ArrayList<Integer>();
		    
		    ArrayList<Integer> rates1 = new ArrayList<Integer>();
		    ArrayList<Integer> rates2 = new ArrayList<Integer>();
		    ArrayList<Integer> rates3 = new ArrayList<Integer>();
		    
			for (int i = 0; i < fields.size(); i++) {
			    LevelUpXmlTemplate it = new LevelUpXmlTemplate();
				sub = fields.get(i);
				int pool = sub.getInt("[@pool]");
				int reward_type = sub.getInt("[@reward_type]");
				int reward_id = sub.getInt("[@reward_id]");
				int reward_count = sub.getInt("[@reward_count]");
				int rate = sub.getInt("[@rate]");
				
				if (pool == 1) {
					types1.add(reward_type);
					ids1.add(reward_id);
					counts1.add(reward_count);
					if (rates1.size() == 0) {
						rates1.add(rate);
					}
					else {
						rates1.add(rates1.get(rates1.size() - 1) + rate);
					}
				}
				else if (pool == 2) {
					types2.add(reward_type);
					ids2.add(reward_id);
					counts2.add(reward_count);
					if (rates2.size() == 0) {
						rates2.add(rate);
					}
					else {
						rates2.add(rates2.get(rates2.size() - 1) + rate);
					}
				}
				else if (pool == 3) {
					types3.add(reward_type);
					ids3.add(reward_id);
					counts3.add(reward_count);
					if (rates3.size() == 0) {
						rates3.add(rate);
					}
					else {
						rates3.add(rates3.get(rates3.size() - 1) + rate);
					}
				}
		    }
			ContinueLoginXmlTemplate continueLoginXmlTemplate = new ContinueLoginXmlTemplate();
			continueLoginXmlTemplate.setTypes(types1);
			continueLoginXmlTemplate.setIds(ids1);
			continueLoginXmlTemplate.setCounts(counts1);
			continueLoginXmlTemplate.setRates(rates1);
			continueLoginTypes.put(1, continueLoginXmlTemplate);
			
			continueLoginXmlTemplate = new ContinueLoginXmlTemplate();
			continueLoginXmlTemplate.setTypes(types2);
			continueLoginXmlTemplate.setIds(ids2);
			continueLoginXmlTemplate.setCounts(counts2);
			continueLoginXmlTemplate.setRates(rates2);
			continueLoginTypes.put(2, continueLoginXmlTemplate);
			
			continueLoginXmlTemplate = new ContinueLoginXmlTemplate();
			continueLoginXmlTemplate.setTypes(types3);
			continueLoginXmlTemplate.setIds(ids3);
			continueLoginXmlTemplate.setCounts(counts3);
			continueLoginXmlTemplate.setRates(rates3);
			continueLoginTypes.put(3, continueLoginXmlTemplate);
		}
		xmlLoadTime.put("continueLogin", Util.getServerTime());
    }
    
    private static void loadSoulSelecters() throws Exception{
    	String levelUpConfPath = DZM.basePath + "/xml/soulSelecter.xml";
		log.info("load item config file path :{}",levelUpConfPath);
		XMLConfiguration conf = new XMLConfiguration(levelUpConfPath);
		List<HierarchicalConfiguration> fields = conf.configurationsAt("Souls.Soul");
		if (fields != null && fields.size() > 0) {
		    HierarchicalConfiguration sub;
		    SoulSelecter it1 = new SoulSelecter();
		    SoulSelecter it2 = new SoulSelecter();
		    SoulSelecter it3 = new SoulSelecter();
		    SoulSelecter it4 = new SoulSelecter();
		    
			for (int i = 0; i < fields.size(); i++) {
				sub = fields.get(i);
				int id = sub.getInt("[@id]");
				int quality = sub.getInt("[@quality]");
				int loginRate = sub.getInt("[@loginRate]");
				int careerRate = sub.getInt("[@careerRate]");
				
				switch (quality) {
					case 1:
						it1.getSoulIds().add(id);
						if (it1.getLoginRates().size() != 0) {
							loginRate += it1.getLoginRates().get(it1.getLoginRates().size() - 1);
						}
						it1.getLoginRates().add(loginRate);
						if (it1.getCareerRates().size() != 0) {
							careerRate += it1.getCareerRates().get(it1.getCareerRates().size() - 1);
						}
						it1.getCareerRates().add(careerRate);
						break;
					case 2:
						it2.getSoulIds().add(id);
						if (it2.getLoginRates().size() != 0) {
							loginRate += it2.getLoginRates().get(it2.getLoginRates().size() - 1);
						}
						it2.getLoginRates().add(loginRate);
						if (it2.getCareerRates().size() != 0) {
							careerRate += it2.getCareerRates().get(it2.getCareerRates().size() - 1);
						}
						it2.getCareerRates().add(careerRate);
						break;
					case 3:
						it3.getSoulIds().add(id);
						if (it3.getLoginRates().size() != 0) {
							loginRate += it3.getLoginRates().get(it3.getLoginRates().size() - 1);
						}
						it3.getLoginRates().add(loginRate);
						if (it3.getCareerRates().size() != 0) {
							careerRate += it3.getCareerRates().get(it3.getCareerRates().size() - 1);
						}
						it3.getCareerRates().add(careerRate);
						break;
					case 4:
						it4.getSoulIds().add(id);
						if (it4.getLoginRates().size() != 0) {
							loginRate += it4.getLoginRates().get(it4.getLoginRates().size() - 1);
						}
						it4.getLoginRates().add(loginRate);
						if (it4.getCareerRates().size() != 0) {
							careerRate += it4.getCareerRates().get(it4.getCareerRates().size() - 1);
						}
						it4.getCareerRates().add(careerRate);
						break;
				}
		    }
			
			soulSelecters.put(1, it1);
			soulSelecters.put(2, it2);
			soulSelecters.put(3, it3);
			soulSelecters.put(4, it4);
		}
		xmlLoadTime.put("soulSelecter", Util.getServerTime());
    }
    
    private static void loadTitleConfig() throws Exception{
    	String levelUpConfPath = DZM.basePath + "/xml/Title.xml";
		log.info("load item config file path :{}",levelUpConfPath);
		XMLConfiguration conf = new XMLConfiguration(levelUpConfPath);
		List<HierarchicalConfiguration> fields = conf.configurationsAt("Titles.Title");
		HashMap<Integer, TitleXmlTemplate> title1 = new HashMap<Integer, TitleXmlTemplate>();
		HashMap<Integer, TitleXmlTemplate> title2 = new HashMap<Integer, TitleXmlTemplate>();
		HashMap<Integer, TitleXmlTemplate> title3 = new HashMap<Integer, TitleXmlTemplate>();
		HashMap<Integer, TitleXmlTemplate> title4 = new HashMap<Integer, TitleXmlTemplate>();
		HashMap<Integer, TitleXmlTemplate> title5 = new HashMap<Integer, TitleXmlTemplate>();
		HashMap<Integer, TitleXmlTemplate> title6 = new HashMap<Integer, TitleXmlTemplate>();
		HashMap<Integer, TitleXmlTemplate> title7 = new HashMap<Integer, TitleXmlTemplate>();
		HashMap<Integer, TitleXmlTemplate> title8 = new HashMap<Integer, TitleXmlTemplate>();
		HashMap<Integer, TitleXmlTemplate> title9 = new HashMap<Integer, TitleXmlTemplate>();
		if (fields != null && fields.size() > 0) {
			HierarchicalConfiguration sub;
			for (int i = 0; i < fields.size(); i++) {
				sub = fields.get(i);
				TitleXmlTemplate it = new TitleXmlTemplate();
				
				int id = sub.getInt("[@id]");
				int type = sub.getInt("[@type]");
				int addType = sub.getInt("[@addType]");
				int timeLine = sub.getInt("[@timeLine]");
				int require = sub.getInt("[@require]");
				float quantity = sub.getFloat("[@quantity]");
				
				it.setId(id);
				it.setType(type);
				it.setAddType(addType);
				it.setTimeLine(timeLine);
				it.setRequire(require);
				it.setQuantity(quantity);
				
				switch (type) {
					case 1:
						title1.put(require, it);
						break;
					case 2:
						title2.put(require, it);
						break;
					case 3:
						title3.put(require, it);
						break;
					case 4:
						title4.put(require, it);
						break;
					case 5:
						title5.put(require, it);
						break;
					case 6:
						title6.put(require, it);
						break;
					case 7:
						title7.put(require, it);
						break;
					case 8:
						title8.put(require, it);
						break;
					case 9:
						title9.put(require, it);
						break;
				}
			}
			
			titleConfigs.put(1, title1);
			titleConfigs.put(2, title2);
			titleConfigs.put(3, title3);
			titleConfigs.put(4, title4);
			titleConfigs.put(5, title5);
			titleConfigs.put(6, title6);
			titleConfigs.put(7, title7);
			titleConfigs.put(8, title8);
			titleConfigs.put(9, title9);
		}
		
		xmlLoadTime.put("titleConfig", Util.getServerTime());
    }
    private static void loadVisitRewardSelecter() throws Exception{
        String levelUpConfPath = DZM.basePath + "/xml/VisitRewardSelecter.xml";
        log.info("load item config file path :{}",levelUpConfPath);
        XMLConfiguration conf = new XMLConfiguration(levelUpConfPath);
        List<HierarchicalConfiguration> fields = conf.configurationsAt("VisitRewardSelecters.VisitRewardSelecter");
        if (fields != null && fields.size() > 0) {
            HierarchicalConfiguration sub;
            for (int i = 0; i < fields.size(); i++) {
                VisitRewardSelecter it = new VisitRewardSelecter();
                sub = fields.get(i);
                int times = sub.getInt("[@times]");
                int continueDay = sub.getInt("[@continueDay]");
                String rewardTypes = sub.getString("[@rewardTypes]");
                String rates = sub.getString("[@rates]");

                it.setTimes(times);
                it.setContinueDay(continueDay);
                String[] typesArr = rewardTypes.split(",");
                int tLen = typesArr.length;
                for(int j = 0;j<tLen;j++)
                {
                    it.getRewardTypes().add(Integer.parseInt(typesArr[j]));
                }
                String[] ratesArr = rewardTypes.split(",");
                int rLen = ratesArr.length;
                for(int k = 0;k<rLen;k++)
                {
                    if(k==0)
                    {
                        it.getRates().add(Float.parseFloat(ratesArr[k]));
                    }else
                    {
                        it.getRates().add(it.getRates().get(k-1)+Float.parseFloat(ratesArr[k]));
                    }
                }
                visitRewardTemplates.put(times, it);
            }
            sub = null;
        }
        xmlLoadTime.put("VisitRewardSelecter", Util.getServerTime());
    }
    
    private static void loadRechargeTemplates() throws Exception{
    	String levelUpConfPath = DZM.basePath + "/xml/rechargeConfig.xml";
    	log.info("load item config file path :{}",levelUpConfPath);
        XMLConfiguration conf = new XMLConfiguration(levelUpConfPath);
        List<HierarchicalConfiguration> fields = conf.configurationsAt("Recharges.Recharge");
        if (fields != null && fields.size() > 0) {
        	HierarchicalConfiguration sub;
        	for (int i = 0; i < fields.size(); i++) {
        		 sub = fields.get(i);
        		 RechargeXmlTemplate it = new RechargeXmlTemplate();
        		 int id = sub.getInt("[@id]");
        		 int amount = sub.getInt("[@amount]");
        		 int addNum = sub.getInt("[@addNum]");
        		 int costMoney = sub.getInt("[@costMoney]");
        		 
        		 it.setId(id);
        		 it.setAmount(amount);
        		 it.setAddNum(addNum);
        		 it.setCostMoney(costMoney);
        		 rechargeTemplates.put(id, it);
        	}
        }
        xmlLoadTime.put("rechargeConfig", Util.getServerTime());
    }
    private static void loadAdventuresConfigTemplates() throws Exception
    {
        String levelUpConfPath = DZM.basePath + "/xml/adventuresConfig.xml";
        log.info("load item config file path :{}",levelUpConfPath);
        XMLConfiguration conf = new XMLConfiguration(levelUpConfPath);
        List<HierarchicalConfiguration> fields = conf.configurationsAt("adventures.adventure");
        if (fields != null && fields.size() > 0)
        {
            HierarchicalConfiguration sub;
            adventureTemplates.clear();
            for (int i = 0; i < fields.size(); i++)
            {
                sub = fields.get(i);
                Adventures adventures = new Adventures();
                int id = sub.getInt("[@id]");
                int timeType = sub.getInt("[@timeType]");
                int levelLimit = sub.getInt("[@levelLimit]");
                int lastTime = sub.getInt("[@lastTime]");
                String beginTime =  sub.getString("[@beginTime]");
                String endTime =  sub.getString("[@endTime]");
                int version=-1;
                try
                {
                    version=  sub.getInt("[@version]");
                }
                catch (Exception e)
                {
                    version=-1;
                }
                adventures.setId(id);
                adventures.setTimeType(timeType);
                adventures.setLevelLimit(levelLimit);
                adventures.setLastTime(lastTime);
                adventures.setBeginTime(beginTime);
                adventures.setEndTime(endTime);
                adventures.setVersion(version);
                if(id== AdventureContinueLoginHandler.AdventureContinueLoginId)
                {
                    AdventureContinueLoginHandler.adventures=adventures;
                }
                adventureTemplates.add(adventures);
            }
        }
        //幸运星活动的开启配置
        List<HierarchicalConfiguration> fields2 = conf.configurationsAt("LuckyStarConfigs.LuckyStarConfig");
    	if (fields2 != null && fields2.size() > 0) {
    		HierarchicalConfiguration sub;
    		for (int i = 0; i < fields2.size(); i++)
    		{
    			ActivityTimeConfig it = new ActivityTimeConfig();
    			sub = fields2.get(i);
    			int id=sub.getInt("[@id]");
    			String beginTime = sub.getString("[@beginTime]");
    			String endTime = sub.getString("[@endTime]");
    			it.setId(id);
    			it.setBeginTime(beginTime);
    			it.setEndTime(endTime);
    			luckyStarTimeConfigs.put(id, it);
    		}
    	}
    	//论剑新增奖励开启配置
    	List<HierarchicalConfiguration> fields3 = conf.configurationsAt("tiaozhanConfigs.tiaozhanConfig");
    	if (fields3 != null && fields3.size() > 0) {
    		HierarchicalConfiguration sub;
    		for (int i = 0; i < fields3.size(); i++)
    		{
    			ActivityTimeConfig it = new ActivityTimeConfig();
    			sub = fields3.get(i);
    			int id=sub.getInt("[@id]");
    			String beginTime = sub.getString("[@beginTime]");
    			String endTime = sub.getString("[@endTime]");
    			it.setId(id);
    			it.setBeginTime(beginTime);
    			it.setEndTime(endTime);
    			tiaozhanTimeConfigs.put(id, it);
    		}
    	}
    	//论剑新增奖励配置
    	List<HierarchicalConfiguration> fields4 = conf.configurationsAt("tiaozhanConfigs.tiaozhanPoolConfig");
    	HierarchicalConfiguration sub_tiaozhan=fields4.get(0);
    	tiaozhanPoolConfig.setType(sub_tiaozhan.getInt("[@Type]"));
    	tiaozhanPoolConfig.setArenaRewardIndex(sub_tiaozhan.getInt("[@ArenaRewardIndex]"));
    	tiaozhanPoolConfig.setArenaRewardType(sub_tiaozhan.getInt("[@ArenaRewardType]"));
    	tiaozhanPoolConfig.setArenaRewardId(sub_tiaozhan.getInt("[@ArenaRewardId]"));
    	tiaozhanPoolConfig.setArenaCost(sub_tiaozhan.getInt("[@ArenaCost]"));
    	tiaozhanPoolConfig.setRankLimit(sub_tiaozhan.getInt("[@RankLimit]"));
    	tiaozhanPoolConfig.setRewardCount(sub_tiaozhan.getInt("[@RewardCount]"));
    	
    	//江湖掉落统计的开启配置
    	List<HierarchicalConfiguration> fields5 = conf.configurationsAt("dropConfigs.dropConfig");
    	if (fields5 != null && fields5.size() > 0) {
    		HierarchicalConfiguration sub;
    		for (int i = 0; i < fields5.size(); i++)
    		{
    			ActivityTimeConfig it = new ActivityTimeConfig();
    			sub = fields5.get(i);
    			int id=sub.getInt("[@id]");
    			String beginTime = sub.getString("[@beginTime]");
    			String endTime = sub.getString("[@endTime]");
    			it.setId(id);
    			it.setBeginTime(beginTime);
    			it.setEndTime(endTime);
    			dropTimeConfigs.put(id, it);
    		}
    	}
    	//江湖掉落统计对象
    	List<HierarchicalConfiguration> fields6= conf.configurationsAt("dropConfigs.dropCount");
    	String dropConfigs=fields6.get(0).getString("[@PoolId]");
    	String[] dropConfig_str=dropConfigs.split(":");
    	ArrayList<Integer> dropList=new ArrayList<Integer>();
    	dropList.add(Integer.parseInt(dropConfig_str[0]));
    	dropList.add(Integer.parseInt(dropConfig_str[1]));
    	dropCountConfig=dropList;
    	//深海探宝活动开启配置
    	 List<HierarchicalConfiguration> fields7 = conf.configurationsAt("DeepWaterConfigs.DeepWaterConfig");
     	if (fields7 != null && fields7.size() > 0) {
     		HierarchicalConfiguration sub;
     		for (int i = 0; i < fields7.size(); i++)
     		{
     			ActivityTimeConfig it = new ActivityTimeConfig();
     			sub = fields7.get(i);
     			int id=sub.getInt("[@id]");
     			String beginTime = sub.getString("[@beginTime]");
     			String endTime = sub.getString("[@endTime]");
     			it.setId(id);
     			it.setBeginTime(beginTime);
     			it.setEndTime(endTime);
     			deepWaterTimeConfigs.put(id, it);
     		}
     	}
     	//女神回馈活动开启配置
     	List<HierarchicalConfiguration> fields8 = conf.configurationsAt("GoddessConfigs.GoddessConfig");
     	if (fields8 != null && fields8.size() > 0) {
     		HierarchicalConfiguration sub;
     		for (int i = 0; i < fields8.size(); i++)
     		{
     			ActivityTimeConfig it = new ActivityTimeConfig();
     			sub = fields8.get(i);
     			int id=sub.getInt("[@id]");
     			String beginTime = sub.getString("[@beginTime]");
     			String endTime = sub.getString("[@endTime]");
     			it.setId(id);
     			it.setBeginTime(beginTime);
     			it.setEndTime(endTime);
     			goddessRebateTimeConfigs.put(id, it);
     		}
     	}
     	//商店相关活动版本号
     	marketToolVersion=conf.getInt("MarketToolVersion");
        xmlLoadTime.put("adventuresConfig", Util.getServerTime());
    }
    
    private static void loadUsePowerReward() throws Exception { 
    	String usePowerConfPath = DZM.basePath + "/xml/usePowerReward.xml";
    	log.info("load item config file path :{}",usePowerConfPath);
    	XMLConfiguration conf = new XMLConfiguration(usePowerConfPath);
    	List<HierarchicalConfiguration> fields = conf.configurationsAt("Rewards.Reward");
        if (fields != null && fields.size() > 0) {
        	ArrayList<Integer> typesArrayList = new ArrayList<Integer>();
        	ArrayList<Integer> idArrayList = new ArrayList<Integer>();
        	ArrayList<Integer> quantityArrayList = new ArrayList<Integer>();
        	ArrayList<Integer> rateArrayList = new ArrayList<Integer>();
        	
        	int size = 0;
        	
            HierarchicalConfiguration sub;
            for (int i = 0; i < fields.size(); i++) {
                sub = fields.get(i);
                int type = sub.getInt("[@type]");
                int id = sub.getInt("[@id]");
                int quantity = sub.getInt("[@quantity]");
                int rate = sub.getInt("[@rate]");
                
                typesArrayList.add(type);
                idArrayList.add(id);
                quantityArrayList.add(quantity);
                rateArrayList.add(rate + size);
                size += rate;
            }
            
            usePowerReward.put("types", typesArrayList);
            usePowerReward.put("ids", idArrayList);
            usePowerReward.put("quantities", quantityArrayList);
            usePowerReward.put("rates",	rateArrayList);
        }
        xmlLoadTime.put("usePowerReward", Util.getServerTime());
    }

    /**
     * 奇遇连续登陆配置表
     * @throws ConfigurationException
     */
    private static void loadAdventureContinueLoginTemplates() throws ConfigurationException
    {
        String dataPath = DZM.basePath + "/xml/adventuresContinueLogin.xml";
        log.info("load item config file path :{}",dataPath);
        XMLConfiguration conf = new XMLConfiguration(dataPath);
        List<HierarchicalConfiguration> fields = conf.configurationsAt("continueLogins.item");
        if (fields != null && fields.size() > 0)
        {
            HierarchicalConfiguration sub;
            for (int i = 0; i < fields.size(); i++)
            {
                AdventureContinueLogin item=new AdventureContinueLogin();
                sub = fields.get(i);
                int id = sub.getInt("[@id]");
                int count = sub.getInt("[@dayCount]");
                String rewards = sub.getString("[@rewards]");

                item.setId(id);
                item.setDayCount(count);
                item.setRewards(rewards);
                adventureContinueLoginTemplates.put(id,item);
            }
        }
        xmlLoadTime.put("adventuresContinueLogin", Util.getServerTime());
    }

    /**
     * 技能书配置
     * @throws ConfigurationException
     */
    private static void loadSkillBookTemplates() throws ConfigurationException
    {
        String dataPath = DZM.basePath + "/xml/skillBookConfig.xml";
        log.info("load item config file path :{}",dataPath);
        XMLConfiguration conf = new XMLConfiguration(dataPath);
        List<HierarchicalConfiguration> fields = conf.configurationsAt("SkillBooks.item");
        if (fields != null && fields.size() > 0)
        {
            HierarchicalConfiguration sub;
            for (int i = 0; i < fields.size(); i++)
            {
                SkillBookConfig item=new SkillBookConfig();
                sub = fields.get(i);
                int id = sub.getInt("[@id]");
                int skillId = sub.getInt("[@skillId]");
                int quality = sub.getInt("[@quality]");
                item.setSkillBookId(id);
                item.setSkillId(skillId);
                item.setSkillQuality(quality);
                skillBookTemplates.put(id,item);
                if(skillBookSkillRelationTemplates.containsKey(skillId))
                {
                    skillBookSkillRelationTemplates.get(skillId).add(id);
                }
                else
                {
                    skillBookSkillRelationTemplates.put(skillId,new ArrayList<Integer>());
                    skillBookSkillRelationTemplates.get(skillId).add(id);
                }
                SkillBookServices.initSkillBookPlayerMap(id);
            }
        }
        xmlLoadTime.put("skillBookConfig", Util.getServerTime());
    }

    /**
     *技能书NPC参数配置
     * @throws ConfigurationException
     */
    public static void loadSkillBookNpcConfig() throws ConfigurationException
    {
        String dataPath = DZM.basePath + "/xml/skillBookNpcConfig.xml";
        log.info("load item config file path :{}",dataPath);
        XMLConfiguration conf = new XMLConfiguration(dataPath);
        SkillBookNpcConfig _config=new SkillBookNpcConfig();
        _config.setMinDiscipleId(conf.getInt("minDiscipleId"));
        _config.setMaxDiscipleId(conf.getInt("maxDiscipleId"));
        _config.setHpAddition(conf.getFloat("hpAddition"));
        _config.setAttackAddition(conf.getFloat("attackAddition"));
        _config.setDefenseAddition(conf.getFloat("defenseAddition"));
        _config.setRpAddition(conf.getFloat("rpAddition"));
        _config.setFloorCaptainLevel(conf.getInt("floorCaptainLevel"));
        _config.setCeilCaptainLevel(conf.getInt("ceilCaptainLevel"));
        _config.setFloorTeamLevel(conf.getInt("floorTeamLevel"));
        _config.setCeilTeamLevel(conf.getInt("ceilTeamLevel"));
        _config.setTeamLevelRatio(conf.getInt("teamLevelRatio"));
        SkillBookServices.initNpcConfig(_config);
        xmlLoadTime.put("skillBookNpcConfig", Util.getServerTime());
    }

    /**
     * 技能书抢夺概率配置
     * @throws ConfigurationException
     */
    public static void loadSkillBookSnatchRadio() throws  ConfigurationException
    {
        String dataPath = DZM.basePath + "/xml/skillBookSnatchConfig.xml";
        log.info("load item config file path :{}",dataPath);
        XMLConfiguration conf = new XMLConfiguration(dataPath);
        List<HierarchicalConfiguration> fields = conf.configurationsAt("SkillBooks.item");
        if (fields != null && fields.size() > 0)
        {
            HierarchicalConfiguration sub;
            for (int i = 0; i < fields.size(); i++)
            {
                JSONObject radio=new JSONObject();
                sub = fields.get(i);
                int quality = sub.getInt("[@quality]");
                int normal = sub.getInt("[@normal]");
                int two = sub.getInt("[@surplusTwo]");
                int one = sub.getInt("[@surplusOne]");
                radio.put(3,normal);
                radio.put(2,two);
                radio.put(1,one);
                skillBookSnatchRadio.put(quality, radio);
            }
        }
        xmlLoadTime.put("skillBookSnatchConfig", Util.getServerTime());
    }

    /**
     * 技能书掉落奖池
     * @throws ConfigurationException
     */
    private static void loadSkillBookSelecter() throws ConfigurationException
    {
        String dataPath = DZM.basePath + "/xml/skillbookSelecter.xml";
        log.info("load item config file path :{}",dataPath);
        XMLConfiguration conf = new XMLConfiguration(dataPath);
        List<HierarchicalConfiguration> fields = conf.configurationsAt("SkillBooks.item");
        if (fields != null && fields.size() > 0)
        {
            HierarchicalConfiguration sub;
            //check 江湖表内章节id<未检测>、奖励物品、关卡npc<未检测>、解锁关卡是否存在
            TIntObjectIterator itCareer = (TIntObjectIterator) careerTemplates.iterator();
            ArrayList<Integer> _careerIdArr=new ArrayList<Integer>();
            while (itCareer.hasNext())
            {
                itCareer.advance();
                CareerXmlTemplate career=(CareerXmlTemplate)itCareer.value();
                if(!_careerIdArr.contains(career.getChapterId()))
                {
                    _careerIdArr.add(career.getChapterId());
                }
            }
            skillBookSelecters.clear();
            for (int i = 0; i < fields.size(); i++)
            {
                sub = fields.get(i);
                int id = sub.getInt("[@id]");
                int deepWaterRat1 = sub.getInt("[@deepWaterRat1]");
                int deepWaterRat2 = sub.getInt("[@deepWaterRat2]");
                int deepWaterRat3 = sub.getInt("[@deepWaterRat3]");
                
                skillbookSelecter.getId().add(id);
                
                if (skillbookSelecter.getDeepWaterRat1().size() == 0) {
                	skillbookSelecter.getDeepWaterRat1().add(deepWaterRat1);
                	skillbookSelecter.getDeepWaterRat2().add(deepWaterRat2);
                	skillbookSelecter.getDeepWaterRat3().add(deepWaterRat3);
                }
                else {
                    int size = skillbookSelecter.getDeepWaterRat1().size();
                    skillbookSelecter.getDeepWaterRat1().add(deepWaterRat1 + skillbookSelecter.getDeepWaterRat1().get(size - 1));
                    skillbookSelecter.getDeepWaterRat2().add(deepWaterRat2 + skillbookSelecter.getDeepWaterRat2().get(size - 1));
                    skillbookSelecter.getDeepWaterRat3().add(deepWaterRat3 + skillbookSelecter.getDeepWaterRat3().get(size - 1));
                }
                
                for(int m=0;m<_careerIdArr.size();++m)
                {
                    int _chapterId=_careerIdArr.get(m);
                    String chaperKey="[@chapterWeight"+_chapterId+"]";
                    int weight = sub.getInt(chaperKey);
                    if(weight>0)
                    {
                        if(!skillBookSelecters.containsKey(_chapterId))
                        {
                            SkillBookSelecter _selecter=new SkillBookSelecter();
                            _selecter.setId(_chapterId);
                            skillBookSelecters.put(_chapterId,_selecter);
                        }
                        skillBookSelecters.get(_chapterId).getBookIdArr().add(id);
                        skillBookSelecters.get(_chapterId).getWeightArr().add(weight);
                    }

                }
                try
                {
                    int acWeight = sub.getInt("[@activity]");
                    
                    if(acWeight>0)
                    {
                        if(!skillBookSelecters.containsKey(SkillBookServices.activitySelecterId))
                        {
                            SkillBookSelecter _selecter=new SkillBookSelecter();
                            _selecter.setId(SkillBookServices.activitySelecterId);
                            skillBookSelecters.put(SkillBookServices.activitySelecterId,_selecter);
                        }
                        skillBookSelecters.get(SkillBookServices.activitySelecterId).getBookIdArr().add(id);
                        skillBookSelecters.get(SkillBookServices.activitySelecterId).getWeightArr().add(acWeight);
                    }

                }
                catch (Exception e)
                {
                     //catch error;
                }
            }
        }
        xmlLoadTime.put("skillbookSelecter", Util.getServerTime());
    }
    
    public static ArrayList<String> getPersistNames(){
    	ArrayList<String> persistNames = new ArrayList<String>();
    	try {
	    	String dataPath = DZM.basePath + "/xml/persistNames.xml";
	        log.info("load item config file path :{}",dataPath);
	        XMLConfiguration conf = new XMLConfiguration(dataPath);
	        List<HierarchicalConfiguration> fields = conf.configurationsAt("Names.Name");
	        if (fields != null && fields.size() > 0)
	        {
	            HierarchicalConfiguration sub;
	            for (int i = 0; i < fields.size(); i++)
	            {
	                sub = fields.get(i);
	                String name = sub.getString("[@name]");
	
	                persistNames.add(name);
	            }
	        }
    	} catch (Exception e) {
			e.printStackTrace();
		}
    	return persistNames;
    }
    
    public static void loadInviteConfig() throws Exception{
    	String itemConfPath = DZM.basePath + "/xml/inviteConfig.xml";
        log.info("load item config file path :{}",itemConfPath);
        XMLConfiguration conf = new XMLConfiguration(itemConfPath);
        List<HierarchicalConfiguration> fields = conf.configurationsAt("Kinds.kind");
        if (fields != null && fields.size() > 0) {
            HierarchicalConfiguration sub;
            for (int i = 0; i < fields.size(); i++)
            {
                InviteConfig it = new InviteConfig();
                sub = fields.get(i);
                int type = sub.getInt("[@type]");
                int required = sub.getInt("[@required]");
                String rewards = sub.getString("[@rewards]");
                it.setType(type);
                it.setRequired(required);
                it.setContainTools(rewards);
                inviteConfigs.put(type, it);
            }
        }
        xmlLoadTime.put("inviteConfig", Util.getServerTime());
    }
    
    public static void loadWorldBossConfig() throws Exception{
    	String itemConfPath = DZM.basePath + "/xml/worldbossConfig.xml";
    	log.info("load item config file path :{}",itemConfPath);
        XMLConfiguration conf = new XMLConfiguration(itemConfPath);
        List<HierarchicalConfiguration> fields = conf.configurationsAt("BossInfo");
        HierarchicalConfiguration sub = fields.get(0);
        worldBossConfig.setStartTime(sub.getInt("[@startTime]"));
        worldBossConfig.setEndTime(sub.getInt("[@endTime]"));
        worldBossConfig.setBaseHealth(sub.getInt("[@baseHealth]"));
        worldBossConfig.setHealthRatio(sub.getFloat("[@healthRatio]"));
        worldBossConfig.setMaxLevel(sub.getInt("[@maxLevel]"));
        worldBossConfig.setTimeLimitMax(sub.getInt("[@timeLimitMax]"));
        worldBossConfig.setTimeLimitMin(sub.getInt("[@timeLimitMin]"));
        worldBossConfig.setReliveCost(sub.getInt("[@reliveCost]"));
        worldBossConfig.setAshesCost(sub.getInt("[@ashesCost]"));
        worldBossConfig.setAshesIncrease(sub.getInt("[@ashesIncrease]"));
        worldBossConfig.setAshesAddType(sub.getInt("[@ashesAddType]"));
        worldBossConfig.setAshesAddQuantity(sub.getFloat("[@ashesAddQuantity]"));
        worldBossConfig.setSilverCoinsBase(sub.getFloat("[@silverCoinsBase]"));
        worldBossConfig.setRumRewardRatio(sub.getFloat("[@rumRewardRatio]"));
        worldBossConfig.setBossKillRatio(sub.getFloat("[@bossKillRatio]"));
        worldBossConfig.setPerAttackRewardNum(sub.getInt("[@perAttackRewardNum]"));
        worldBossConfig.setPerAttackRewardProbability(sub.getInt("[@perAttackRewardProbability]"));
        worldBossConfig.setTotalRewardMax(sub.getInt("[@totalRewardMax]"));
        worldBossConfig.setStartLevel(sub.getInt("[@startLevel]"));
        worldBossConfig.setNpcItemId(sub.getInt("[@npcItemId]"));
        xmlLoadTime.put("worldBossConfig", Util.getServerTime());
    }
    
    private static void loadServerConfig() throws Exception{
    	String itemConfPath = DZM.basePath + "/xml/ServerConfig.xml";
    	log.info("load item config file path :{}",itemConfPath);
        XMLConfiguration conf = new XMLConfiguration(itemConfPath);
        List<HierarchicalConfiguration> fields = conf.configurationsAt("ServerInfo");
        boolean getServerConfig = false;
        if (fields != null && fields.size() > 0) {
        	HierarchicalConfiguration sub;
        	for (int i = 0; i < fields.size(); i++)
            {
		        sub = fields.get(i);
		        String serverDis = sub.getString("[@platform]") + "_" + sub.getString("[@serverId]");
		        if (serverDis.equals(DZM.platForm + "_" + DZM.serverId)) {
		        	DZM.serverMaxUser = sub.getInt("[@serverMaxUser]");
			        DZM.serverStartDate = sub.getString("[@serverOpenDate]");
			        DZM.serverStat = sub.getInt("[@serverStat]");
			        String closeDate = sub.getString("[@serverCloseTime]");
			        if (!"".equals(closeDate)) {
			        	java.sql.Timestamp timestamp = java.sql.Timestamp.valueOf(closeDate.replace('|', ' '));
			        	DZM.serverCloseTime = (int)(timestamp.getTime()/1000);
			        	DZM.closeNeedTime = sub.getInt("[@closeNeedMinutes]");
					}
			        xmlLoadTime.put("serverConfig", Util.getServerTime());
			        getServerConfig = true;
				}
            }
        }
        if (!getServerConfig) {
			log.info("No server config found for platform:{},serverid:{}",DZM.platForm,DZM.serverId);
			throw new Exception("No server config found for platform:" + DZM.platForm + ",serverid:" + DZM.serverId);
		}
    }
    
    private static void loadPlatInitReward() throws Exception {
    	String itemConfPath = DZM.basePath + "/xml/platInitReward.xml";
    	log.info("load item config file path :{}",itemConfPath);
    	XMLConfiguration conf = new XMLConfiguration(itemConfPath);
    	List<HierarchicalConfiguration> fields = conf.configurationsAt("PlatInfo");
    	if (fields != null && fields.size() > 0) {
    		HierarchicalConfiguration sub;
        	for (int i = 0; i < fields.size(); i++)
            {
		        sub = fields.get(i);
		        String platform = sub.getString("[@platform]");
		        int startTime = sub.getInt("[@rewardStartTime]");
		        int endTime = sub.getInt("[@rewardEndTime]");
		        String rewardInfo = sub.getString("[@rewardInfo]");
		        String rewardMsg = sub.getString("[@rewardMsg]");
		        
		        PlatInitReward rewardConfig = new PlatInitReward();
		        rewardConfig.setRewardStartTime(startTime);
		        rewardConfig.setRewardEndTime(endTime);
		        rewardConfig.setRewardInfo(rewardInfo);
		        rewardConfig.setRewardMsg(rewardMsg);
		        
		        platsInitReward.put(platform, rewardConfig);
            }
    	}
    	xmlLoadTime.put("platInitReward", Util.getServerTime());
    }
    
    private static void loadTesterList() throws Exception{
    	String itemConfPath = DZM.basePath + "/xml/TesterList.xml";
    	log.info("load item config file path :{}",itemConfPath);
        XMLConfiguration conf = new XMLConfiguration(itemConfPath);
        List<HierarchicalConfiguration> fields = conf.configurationsAt("Tester");
        boolean getServerConfig = false;
        if (fields != null && fields.size() > 0) {
        	HierarchicalConfiguration sub;
        	for (int i = 0; i < fields.size(); i++)
            {
		        sub = fields.get(i);
		        String puid = sub.getString("[@puid]");
		        DZM.testerArray.add(puid);
            }
        }
    }
    //送魂魄活动抽取配置
    private static void loadGiveSoulSelecter()throws Exception{
    	String itemConfPath = DZM.basePath + "/xml/giveSoulSelecter.xml";
    	log.info("load item config file path :{}",itemConfPath);
        XMLConfiguration conf = new XMLConfiguration(itemConfPath);
        List<HierarchicalConfiguration> fields = conf.configurationsAt("GiveSoulSelecters.GiveSoulSelecter");
        if (fields != null && fields.size() > 0) {
        	HierarchicalConfiguration sub;
        	GiveSoulSelecter gs1 = new GiveSoulSelecter();
        	GiveSoulSelecter gs2 = new GiveSoulSelecter();
        	GiveSoulSelecter gs3 = new GiveSoulSelecter();
        	for (int i = 0; i < fields.size(); i++)
            {
		        sub = fields.get(i);
                int id = sub.getInt("[@id]");
                int typeid = sub.getInt("[@typeid]");
                int soulItemid = sub.getInt("[@soulItemid]");
                int count1 = sub.getInt("[@count1]");
                int count2 = sub.getInt("[@count2]");
                int rate1 = sub.getInt("[@rate1]");
                int rate2 = sub.getInt("[@rate2]");
                int rate3 = sub.getInt("[@rate3]");
                int rate4 = sub.getInt("[@rate4]");
                int rate5 = sub.getInt("[@rate5]");
                if (typeid == 2) {
                	 gs2.getId().add(id);
                	 gs2.getTypeid().add(typeid);
                	 gs2.getSoulItemid().add(soulItemid);
                	 gs2.getCount1().add(count1);
                	 gs2.getCount2().add(count2);
                     if (gs2.getRate1().size() == 0) {
                    	 gs2.getRate1().add(rate1);
                    	 gs2.getRate2().add(rate2);
                    	 gs2.getRate3().add(rate3);
                    	 gs2.getRate4().add(rate4);
                    	 gs2.getRate5().add(rate5);
                     }
                     else {
                         int size = gs2.getRate1().size();
                         gs2.getRate1().add(rate1 + gs2.getRate1().get(size - 1));
                         gs2.getRate2().add(rate2 + gs2.getRate2().get(size - 1));
                         gs2.getRate3().add(rate3 + gs2.getRate3().get(size - 1));
                         gs2.getRate4().add(rate4 + gs2.getRate4().get(size - 1));
                         gs2.getRate5().add(rate5 + gs2.getRate5().get(size - 1));
                     }
                }
                else if (typeid == 3) {
                	gs3.getId().add(id);
                    gs3.getTypeid().add(typeid);
                    gs3.getSoulItemid().add(soulItemid);
                    gs3.getCount1().add(count1);
                    gs3.getCount2().add(count2);
                    if (gs3.getRate1().size() == 0) {
                    	gs3.getRate1().add(rate1);
                    	gs3.getRate2().add(rate2);
                    	gs3.getRate3().add(rate3);
                    	gs3.getRate4().add(rate4);
                    	gs3.getRate5().add(rate5);
                    }
                    else {
                        int size = gs3.getRate1().size();
                        gs3.getRate1().add(rate1 + gs3.getRate1().get(size - 1));
                        gs3.getRate2().add(rate2 + gs3.getRate2().get(size - 1));
                        gs3.getRate3().add(rate3 + gs3.getRate3().get(size - 1));
                        gs3.getRate4().add(rate4 + gs3.getRate4().get(size - 1));
                        gs3.getRate5().add(rate5 + gs3.getRate5().get(size - 1));
                    }
                }
            }
        	giveSoulSelecters.put(1, gs1);
            giveSoulSelecters.put(2, gs2);
            giveSoulSelecters.put(3, gs3);
        }
        xmlLoadTime.put("giveSoulSelecter", Util.getServerTime());
    }
    //送魂魄活动阶段判定
    private static void loadGiveSoulStage()throws Exception{
    	String itemConfPath = DZM.basePath + "/xml/giveSoulStage.xml";
    	log.info("load item config file path :{}",itemConfPath);
    	XMLConfiguration conf = new XMLConfiguration(itemConfPath);
    	List<HierarchicalConfiguration> fields = conf.configurationsAt("GiveSoulStages.GiveSoulStage");
    	if (fields != null && fields.size() > 0) {
    		HierarchicalConfiguration sub;
    		GiveSoulStage gss1 = new GiveSoulStage();
    		GiveSoulStage gss2 = new GiveSoulStage();
    		GiveSoulStage gss3 = new GiveSoulStage();
    		for (int i = 0; i < fields.size(); i++)
    		{
    			sub = fields.get(i);
    			int id = sub.getInt("[@id]");
    			int typeid = sub.getInt("[@typeid]");
    			int stage = sub.getInt("[@stage]");
    			String condition = sub.getString("[@condition]");
    			int balanceSoulId = sub.getInt("[@balanceSoulId]");
    			int balanceTimes = sub.getInt("[@balanceTimes]");
    			int balanceId = sub.getInt("[@balanceId]");
    			int condition_soulId = 0;
    			int condition_count = 0;
    			String[]strs = condition.split(":");
    			condition_soulId=Integer.parseInt(strs[0]);
    			if(strs.length>0){
    				condition_count=Integer.parseInt(strs[1]);
    			}
    			
    			if (typeid == 2) {
    				gss2.getId().add(id);
    				gss2.getTypeid().add(typeid);
    				gss2.getStage().add(stage);
    				gss2.getCondition_soulId().add(condition_soulId);
    				gss2.getCondition_count().add(condition_count);
    				gss2.getBalanceSoulId().add(balanceSoulId);
    				gss2.getBalanceTimes().add(balanceTimes);
    				gss2.getBalanceId().add(balanceId);
    			}
    			else if (typeid == 3) {
    				gss3.getId().add(id);
    				gss3.getTypeid().add(typeid);
    				gss3.getStage().add(stage);
    				gss3.getCondition_soulId().add(condition_soulId);
    				gss3.getCondition_count().add(condition_count);
    				gss3.getBalanceSoulId().add(balanceSoulId);
    				gss3.getBalanceTimes().add(balanceTimes);
    				gss3.getBalanceId().add(balanceId);
    			}
    		}
    		giveSoulStages.put(1, gss1);
    		giveSoulStages.put(2, gss2);
    		giveSoulStages.put(3, gss3);
    	}
    	xmlLoadTime.put("giveSoulStage", Util.getServerTime());
    }
    //送魂魄活动开启配置
    private static void loadGiveSoulConfig()throws Exception{
    	String itemConfPath = DZM.basePath + "/xml/adventuresConfig.xml";
    	log.info("load item config file path :{}",itemConfPath);
    	XMLConfiguration conf = new XMLConfiguration(itemConfPath);
    	List<HierarchicalConfiguration> fields = conf.configurationsAt("GiveSoulConfigs.GiveSoulConfig");
    	if (fields != null && fields.size() > 0) {
    		HierarchicalConfiguration sub;
    		for (int i = 0; i < fields.size(); i++)
    		{
    			GiveSoulConfig gsc = new GiveSoulConfig();
    			sub = fields.get(i);
    			int id = sub.getInt("[@id]");
    			String beginTime = sub.getString("[@beginTime]");
    			String endTime = sub.getString("[@endTime]");
    			gsc.setId(id);
    			gsc.setBeginTime(beginTime);
    			gsc.setEndTime(endTime);
    			giveSoulConfigs.put(id, gsc);
    		}
    	}
    	xmlLoadTime.put("giveSoulConfig", Util.getServerTime());
    }

    //安卓平台激活码功能控制
    private static void loadPlatActiveCodeCtrlConfig() throws Exception{
        String path = DZM.basePath + "/xml/platActiveCodeCtrl.xml";
        log.info("load platActiveCodeCtrl config file path :{}",path);
        XMLConfiguration conf = new XMLConfiguration(path);
        List<HierarchicalConfiguration> fields = conf.configurationsAt("platforms.plat");
        if (fields != null && fields.size() > 0) {
            HierarchicalConfiguration sub;
            for (int i = 0; i < fields.size(); i++)
            {
                sub = fields.get(i);
                String name = sub.getString("[@name]");
                Boolean isOpen = sub.getBoolean("[@isopen]");
                platActiveCodeCtrlConfigs.put(name, isOpen);
            }
        }
        xmlLoadTime.put("platActiveCodeCtrlConfig", Util.getServerTime());
    }
    private static void loadWorldBossLvHP()throws Exception{
    	String itemConfPath = DZM.basePath + "/xml/worldbossLvlHp.xml";
    	log.info("load item config file path :{}",itemConfPath);
        XMLConfiguration conf = new XMLConfiguration(itemConfPath);
        List<HierarchicalConfiguration> fields = conf.configurationsAt("BossHp.LvlHp");
        if (fields != null && fields.size() > 0) {
        	HierarchicalConfiguration sub;
        	for (int i = 0; i < fields.size(); i++) {
        		 sub = fields.get(i);
        		 WorldBossLvHp bossLvHp=new WorldBossLvHp();
        		 int level = sub.getInt("[@lvl]");
        		 long hp = sub.getLong("[@hp]");
        		 bossLvHp.setLevel(level);
        		 bossLvHp.setHp(hp);
        		 worldBossLvHps.put(level, bossLvHp);
        	}
        }
        xmlLoadTime.put("worldbossLvlHp", Util.getServerTime());
    }
    private static void loadLluckyStarXmlTemplate() throws Exception{
    	String itemConfPath = DZM.basePath + "/xml/luckyStarConfig.xml";
    	log.info("load item config file path :{}",itemConfPath);
    	XMLConfiguration conf = new XMLConfiguration(itemConfPath);
    	List<HierarchicalConfiguration> fields = conf.configurationsAt("LuckyStarConfigs.LuckyStarConfig");
    	 if (fields != null && fields.size() > 0) {
             HierarchicalConfiguration sub;
             for (int i = 0; i < fields.size(); i++)
             {
                 LuckyStarXmlTemplate it = new LuckyStarXmlTemplate();
                 sub = fields.get(i);
                 int id=sub.getInt("[@id]");
                 int itemId = sub.getInt("[@itemId]");
                 int maxDropCount = sub.getInt("[@maxDropCount]");
                 int balanceTimes = sub.getInt("[@balanceTimes]");
                 int balanceType = sub.getInt("[@balanceType]");
                 int balanceId = sub.getInt("[@balanceId]");
                 int treasureType = sub.getInt("[@treasureType]");
                 it.setId(id);
                 it.setItemId(itemId);
                 it.setMaxDropCount(maxDropCount);
                 it.setBalanceId(balanceId);
                 it.setBalanceTimes(balanceTimes);
                 it.setBalanceType(balanceType);
                 it.setTreasureType(treasureType);
                 luckyStarXmlTemplates.put(id, it);
             }
         }
    	xmlLoadTime.put("luckStarConfig", Util.getServerTime());
    }
    
    
    public static HashMap<Integer, RechargeXmlTemplate> getRechargeConfigByPlayform(String platform) throws Exception{
    	HashMap<Integer, RechargeXmlTemplate> config = null;
    	if (otherRechargesHashMap.containsKey(platform)) {
    		config = otherRechargesHashMap.get(platform);
		}
    	else {
			String filePath = DZM.basePath + "/xml/rechargeConfig/rechargeConfig_"+platform+".xml";
			File file = new File(filePath);
			if (file.exists()) {
				HashMap<Integer, RechargeXmlTemplate>platRecharge = new HashMap<Integer, RechargeXmlTemplate>();
				XMLConfiguration conf = new XMLConfiguration(filePath);
		        List<HierarchicalConfiguration> fields = conf.configurationsAt("Recharges.Recharge");
		        if (fields != null && fields.size() > 0) {
		        	HierarchicalConfiguration sub;
		        	for (int i = 0; i < fields.size(); i++) {
		        		 sub = fields.get(i);
		        		 RechargeXmlTemplate it = new RechargeXmlTemplate();
		        		 int id = sub.getInt("[@id]");
		        		 int amount = sub.getInt("[@amount]");
		        		 int addNum = sub.getInt("[@addNum]");
		        		 int costMoney = sub.getInt("[@costMoney]");
		        		 
		        		 it.setId(id);
		        		 it.setAmount(amount);
		        		 it.setAddNum(addNum);
		        		 it.setCostMoney(costMoney);
		        		 platRecharge.put(id, it);
		        	}
		        }
		        config = platRecharge;
		        otherRechargesHashMap.put(platform, platRecharge);
			}
		}
    	return config;
    }
    private static void loadQuickCareerConfig() throws Exception{
    	String itemConfPath = DZM.basePath + "/xml/quickCareerConfig.xml";
    	log.info("load item config file path :{}",itemConfPath);
        XMLConfiguration conf = new XMLConfiguration(itemConfPath);
        List<HierarchicalConfiguration> fields = conf.configurationsAt("QuickCareerConfigs.QuickCareerConfig");
        HierarchicalConfiguration sub = fields.get(0);
        quickCareerConfig.setBasecoldTime(sub.getInt("[@basecoldTime]"));
        quickCareerConfig.setBasePrice(sub.getInt("[@basePrice]"));
        quickCareerConfig.setVipLimit(sub.getInt("[@vipLimit]"));
        quickCareerConfig.setVipResetLimit(sub.getInt("[@vipResetLimit]"));
        quickCareerConfig.setDivisor(sub.getInt("[@divisor]"));
        xmlLoadTime.put("quickCareerConfig", Util.getServerTime());
    }


    /**
     * 宝石属性信息
     * @throws Exception
     */
    private static void loadDiamonds() throws Exception
    {
        String path = DZM.basePath + "/xml/Diamond.xml";
        log.info("load Diamond config file path :{}",path);
        XMLConfiguration conf = new XMLConfiguration(path);
        List<HierarchicalConfiguration> fields = conf.configurationsAt("Diamonds.Diamond");
        if (fields != null && fields.size() > 0)
        {
            HierarchicalConfiguration sub;
            for (int i = 0; i < fields.size(); i++)
            {
                sub = fields.get(i);
                try
                {
                    int  itemId = sub.getInt("[@itemId]");
                    int  colorType = sub.getInt("[@colorType]");
                    int  level = sub.getInt("[@level]");
                    int  weanponAttr = sub.getInt("[@weanponAttr]");
                    int  armorAttr = sub.getInt("[@armorAttr]");
                    int  jewelryAttr = sub.getInt("[@jewelryAttr]");
                    int  upgradeConsumeSilver = sub.getInt("[@upgradeConsumeSilver]");
                    int  upgradeConsumeDiamon = sub.getInt("[@upgradeConsumeDiamon]");
                    int  dismantleConsume = sub.getInt("[@dismantleConsume]");
                    int  nextItemId = sub.getInt("[@nextItemId]");
                    Diamond _diamon=new Diamond();
                    _diamon.setId(itemId);
                    _diamon.setColorType(colorType);
                    _diamon.setLevel(level);
                    _diamon.setWeanponAttr(weanponAttr);
                    _diamon.setArmorAttr(armorAttr);
                    _diamon.setJewelryAttr(jewelryAttr);
                    _diamon.setUpgradeConsumeSilver(upgradeConsumeSilver);
                    _diamon.setUpgradeConsumeDiamon(upgradeConsumeDiamon);
                    _diamon.setDismantleConsume(dismantleConsume);
                    _diamon.setNextItemId(nextItemId);
                    diamonds.put(itemId, _diamon);
                }
                catch (Exception e)
                {
                    log.info("XMLTemplateService.loadDiamonds:"+e.getMessage());
                }

            }
        }
        xmlLoadTime.put("Diamond", Util.getServerTime());
    }

    /**
     * load 钻石基础属性配置
     * @throws Exception
     */
    private static void loadDiamondConfig() throws Exception
    {
        String dataPath = DZM.basePath + "/xml/DiamondConfig.xml";
        log.info("load item config file path :{}",dataPath);
        XMLConfiguration conf = new XMLConfiguration(dataPath);
        DiamondConfig.PundConsume[0]=conf.getInt("hole_1_consume");
        DiamondConfig.PundConsume[1]=conf.getInt("hole_2_consume");
        DiamondConfig.PundConsume[2]=conf.getInt("hole_3_consume");
        DiamondConfig.PundConsume[3]=conf.getInt("hole_4_consume");
        DiamondConfig.DiamondOpenLevel=conf.getInt("DiamondOpenLevel");
        DiamondConfig.DiamondOpenToolsId=conf.getInt("DiamondOpenToolsId");
        DiamondConfig.DiamondRemoveToolsId=conf.getInt("DiamondRemoveToolsId");
        DiamondConfig.DiamondOpenStatus=conf.getInt("DiamondOpenStatus");
        xmlLoadTime.put("DiamondConfig", Util.getServerTime());
    }

    private static void loadDiamondAdditionConfig() throws Exception
    {
        String dataPath = DZM.basePath + "/xml/DiamondAdditionConfig.xml";
        log.info("load item config file path :{}",dataPath);
        XMLConfiguration conf = new XMLConfiguration(dataPath);
        List<HierarchicalConfiguration> fields = conf.configurationsAt("Additions.Addition");
        if (fields != null && fields.size() > 0)
        {
            HierarchicalConfiguration sub;
            JSONObject _object=new JSONObject();
            for (int i = 0; i < fields.size(); i++)
            {
                sub = fields.get(i);
                try
                {
                    int  quality = sub.getInt("[@quality]");
                    int  type = sub.getInt("[@equipType]");
                    int  weight = sub.getInt("[@weight]");
                    String  value = sub.getString("[@value]");
                    String _key=quality+"_"+type;
                    if(!_object.containsKey(String.valueOf(_key)))
                    {
                        JSONObject _type=new JSONObject();
                        _type.put("w",new JSONArray());
                        _type.put("v",new JSONArray());
                        _object.put(String.valueOf(_key),_type);

                    }
                    _object.getJSONObject(String.valueOf(_key)).getJSONArray("w").add(weight);
                    _object.getJSONObject(String.valueOf(_key)).getJSONArray("v").add(value);
                }
                catch (Exception e)
                {
                    log.info("XMLTemplateService.loadDiamonds:"+e.getMessage());
                }

            }
            DiamondConfig.setDiamondAdditionConfig(_object);
        }
        xmlLoadTime.put("DiamondAdditionConfig", Util.getServerTime());
    }

   
    private static void loadDeepWaterXmlTemplate()throws Exception{
    	String itemConfPath = DZM.basePath + "/xml/deepWaterConfig.xml";
    	log.info("load item config file path :{}",itemConfPath);
    	XMLConfiguration conf = new XMLConfiguration(itemConfPath);
    	List<HierarchicalConfiguration> fields1 = conf.configurationsAt("DeepWaterSelecters.DeepWaterSelecter");
    	if (fields1 != null && fields1.size() > 0) {
        	HierarchicalConfiguration sub;
        	DeepWaterSelecter dws1 = new DeepWaterSelecter();
        	DeepWaterSelecter dws2 = new DeepWaterSelecter();
        	DeepWaterSelecter dws3 = new DeepWaterSelecter();
        	HashMap<Integer, Integer> forsureTimes1=new HashMap<Integer, Integer>();
        	HashMap<Integer, Integer> forsureTimes2=new HashMap<Integer, Integer>();
        	HashMap<Integer, Integer> forsureTimes3=new HashMap<Integer, Integer>();
        	
        	for (int i = 0; i < fields1.size(); i++)
            {
		        sub = fields1.get(i);
                int id = sub.getInt("[@id]");
                int typeid = sub.getInt("[@typeid]");
                int rewardType = sub.getInt("[@rewardType]");
                int itemId = sub.getInt("[@itemId]");
                int count = sub.getInt("[@count]");
                int rate = sub.getInt("[@rate]");
                int lvl = sub.getInt("[@lvl]");
                String forSureTimes = sub.getString("[@forSureTimes]");
                String[] forSureTimes_str=null;
                ArrayList<Integer> forSureTimes_int=new ArrayList<Integer>();
                if(StringUtils.isNotBlank(forSureTimes)){
                	forSureTimes_str=forSureTimes.split(",");
                	for(int j=0;j<forSureTimes_str.length;j++){
                		int times=0;
                		try {
							times=Integer.parseInt(forSureTimes_str[j]);
						} catch (Exception e) {
							continue;
						}
						if(times!=0){
							if(typeid==1){
								forsureTimes1.put(times, id);
							}
							else if(typeid==2){
								forsureTimes2.put(times, id);
							}
							else if(typeid==3){
								forsureTimes3.put(times, id);
							}
						}
                	}
                }
                
                if (typeid == 1) {
                	dws1.getId().add(id);
                	dws1.getTypeId().add(typeid);
                	dws1.getRewardType().add(rewardType);
                	dws1.getItemId().add(itemId);
                	dws1.getCount().add(count);
                	dws1.getLvl().add(lvl);
                	 
                     if (dws1.getRate().size() == 0) {
                    	 dws1.getRate().add(rate);
                     }
                     else {
                         int size = dws1.getRate().size();
                         dws1.getRate().add(rate+dws1.getRate().get(size-1));
                     }
                }
                else if (typeid == 2) {
                	dws2.getId().add(id);
                	dws2.getTypeId().add(typeid);
                	dws2.getRewardType().add(rewardType);
                	dws2.getItemId().add(itemId);
                	dws2.getCount().add(count);
                	dws2.getLvl().add(lvl);
                	 
                     if (dws2.getRate().size() == 0) {
                    	 dws2.getRate().add(rate);
                     }
                     else {
                         int size = dws2.getRate().size();
                         dws2.getRate().add(rate+dws2.getRate().get(size-1));
                     }
                }
                else if (typeid == 3) {
                	dws3.getId().add(id);
                	dws3.getTypeId().add(typeid);
                	dws3.getRewardType().add(rewardType);
                	dws3.getItemId().add(itemId);
                	dws3.getCount().add(count);
                	dws3.getLvl().add(lvl);
                	 
                     if (dws3.getRate().size() == 0) {
                    	 dws3.getRate().add(rate);
                     }
                     else {
                         int size = dws3.getRate().size();
                         dws3.getRate().add(rate+dws3.getRate().get(size-1));
                     }
                }
            }
        	deepWaterSelecters.put(1, dws1);
        	deepWaterSelecters.put(2, dws2);
        	deepWaterSelecters.put(3, dws3);
        	deepWaterForSureTimes.put(1, forsureTimes1);
        	deepWaterForSureTimes.put(2, forsureTimes2);
        	deepWaterForSureTimes.put(3, forsureTimes3);
        }
    	xmlLoadTime.put("deepWaterConfig", Util.getServerTime());
    }
    
    private static void loadGuildBaseConfig() throws Exception{
    	String itemConfPath = DZM.basePath + "/xml/GuildBase.xml";
    	log.info("load item config file path :{}",itemConfPath);
        XMLConfiguration conf = new XMLConfiguration(itemConfPath);
        List<HierarchicalConfiguration> fields = conf.configurationsAt("Guild");
        
        HierarchicalConfiguration sub = fields.get(0);
        guildConfig.setCreateGold(sub.getInt("[@costGold]"));
        guildConfig.setLevelLimit(sub.getInt("[@levelLimit]"));
        guildConfig.setDefaultBrodcast(sub.getString("[@defaultBrodcast]"));
        guildConfig.setTotemRepairNeedSeconds(sub.getInt("[@totemRepairNeedSeconds]"));
        guildConfig.setDailyMaxAttackTimes(sub.getInt("[@dailyMaxAttackTimes]"));
        guildConfig.setDailyMaxRewardTimes(sub.getInt("[@dailyMaxRewardTimes]"));
        guildConfig.setAttackCoolSeconds(sub.getInt("[@attackCoolSeconds]"));
        guildConfig.setAttckHp(sub.getInt("[@attckHp]"));
        guildConfig.setShopRefreshCD(sub.getString("[@shopFresh]"));
        guildConfig.setShopWinFreshCost(sub.getInt("[@shopWinFreshCost]"));
        guildConfig.setMemberUpgradeCD(sub.getInt("[@memberUpgradeCD]"));
        guildConfig.setGetGuildCoinRatio(sub.getFloat("[@getGuildCoinRatio]"));
        guildConfig.setChangeRate(sub.getString("[@exchangeRate]"));
        guildConfig.setBroadcastRefreshCD(sub.getInt("[@broadcastRefreshCD]"));
        guildConfig.setAttackAdjustmentRatio(sub.getFloat("[@attackAdjustmentRatio]"));
        guildConfig.setAttackRankingRatio1(sub.getFloat("[@attackRankingRatio1]"));
        guildConfig.setAttackRankingRatio2(sub.getFloat("[@attackRankingRatio2]"));
        xmlLoadTime.put("guildBase", Util.getServerTime());
    }
    
    private static void loadGuildLvlConfig() throws Exception{
    	String itemConfPath = DZM.basePath + "/xml/GuildLvl.xml";
    	log.info("load item config file path :{}",itemConfPath);
    	XMLConfiguration conf = new XMLConfiguration(itemConfPath);
    	List<HierarchicalConfiguration> fields = conf.configurationsAt("GuildLvls.GuildLvl");
    	if (fields != null && fields.size() > 0) {
             HierarchicalConfiguration sub;
             HashMap<Integer, GuildLevelConfig> configs = new HashMap<Integer, GuildLevelConfig>();
             for (int i = 0; i < fields.size(); i++)
             {
                 GuildLevelConfig it = new GuildLevelConfig();
                 sub = fields.get(i);
                 
                 it.setLevel(sub.getInt("[@lvl]"));
                 it.setGuildMaxMember(sub.getInt("[@guildMaxMember]"));
                 it.setGuildXp(sub.getInt("[@guildXp]"));
                 it.setShopXp(sub.getInt("[@shopXp]"));
                 it.setCatalogCount(sub.getInt("[@shopWin]"));
                 it.setShoppingCD(sub.getInt("[@shoppingCD]"));
                 it.setBankXp(sub.getInt("[@bankXp]"));
                 it.setBankStorage(sub.getInt("[@bankStorage]"));
                 it.setTotemBuff(sub.getFloat("[@totemBuff]"));
                 it.setTotemHp(sub.getInt("[@totemHp]"));
                 it.setTotemXp(sub.getInt("[@totemXp]"));
                 it.setHpAltar(sub.getInt("[@hpAltar]"));
                 it.setAtcAltar(sub.getInt("[@atcAltar]"));
                 it.setDefAltar(sub.getInt("[@defAltar]"));
                 it.setWillAltar(sub.getInt("[@willAltar]"));
                 it.setAltarXp(sub.getInt("[@altarXp]"));
                 
                 configs.put(sub.getInt("[@lvl]"), it);
                 
                 if (sub.getInt("[@lvl]") > guildConfig.getLeaguaMaxLevel()) {
					 guildConfig.setLeaguaMaxLevel(sub.getInt("[@lvl]"));
				 }
             }
             guildConfig.setLevelConfigs(configs);
         }
    	xmlLoadTime.put("guildLevelConfig", Util.getServerTime());
    }
    
    private static void loadGuildPropsConfig() throws Exception{
    	String itemConfPath = DZM.basePath + "/xml/GuildProps.xml";
    	log.info("load item config file path :{}",itemConfPath);
    	XMLConfiguration conf = new XMLConfiguration(itemConfPath);
    	List<HierarchicalConfiguration> fields = conf.configurationsAt("GuildProps.Prop");
    	if (fields != null && fields.size() > 0) {
	        HierarchicalConfiguration sub;
	        HashMap<Integer, ArrayList<Integer>> configs = new HashMap<Integer, ArrayList<Integer>>();
	        for (int i = 0; i < fields.size(); i++)
	        {
	            sub = fields.get(i);
	             
	            int id = sub.getInt("[@id]");
	            int type = sub.getInt("[@types]");
	             
	            if (!configs.containsKey(type)) {
	            	configs.put(type, new ArrayList<Integer>());
	            }
	            
	            configs.get(type).add(id);
	        }
	        guildConfig.setAllMedals(configs);
	    }
    	xmlLoadTime.put("guildProps", Util.getServerTime());
    }
    
    private static void loadGuildBuildConfig() throws Exception{
    	String itemConfPath = DZM.basePath + "/xml/GuildBuildInfo.xml";
    	log.info("load item config file path :{}",itemConfPath);
    	XMLConfiguration conf = new XMLConfiguration(itemConfPath);
    	List<HierarchicalConfiguration> fields = conf.configurationsAt("GuildBuildInfos.Info");
    	if (fields != null && fields.size() > 0) {
	        HierarchicalConfiguration sub;
	        HashMap<Integer, ArrayList<Integer>> configs = new HashMap<Integer, ArrayList<Integer>>();
	        for (int i = 0; i < fields.size(); i++)
	        {
	            sub = fields.get(i);
	             
	            int id = sub.getInt("[@id]");
	            int type = sub.getInt("[@type]");
	             
	            if (!configs.containsKey(type)) {
	            	configs.put(type, new ArrayList<Integer>());
	            }
	            
	            configs.get(type).add(id);
	        }
	        guildConfig.setAllBuilds(configs);
	    }
    	xmlLoadTime.put("guildBuilds", Util.getServerTime());
    }
    
    private static void loadGuildShopSellConfig() throws Exception {
    	String itemConfPath = DZM.basePath + "/xml/GuildShopSell.xml";
    	log.info("load item config file path :{}",itemConfPath);
    	XMLConfiguration conf = new XMLConfiguration(itemConfPath);
    	List<HierarchicalConfiguration> fields = conf.configurationsAt("GuildShopSpells.ShopSpell");
    	if (fields != null && fields.size() > 0) {
    		HierarchicalConfiguration sub;
    		HashMap<Integer, HashMap<Integer, GuildShopSell>> regularShopConfigs = new HashMap<Integer, HashMap<Integer,GuildShopSell>>();
    		HashMap<Integer, HashMap<Integer, GuildShopSell>> occupyShopConfigs = new HashMap<Integer, HashMap<Integer,GuildShopSell>>();
    		for (int i = 0; i < fields.size(); i++){
    			sub = fields.get(i);
    			
    			int id = sub.getInt("[@id]");
    			int itemType = sub.getInt("[@itemType]");
    			int propId = sub.getInt("[@propId]");
    			int count = sub.getInt("[@count]");
    			int price = sub.getInt("[@price]");
    			int shopType = sub.getInt("[@shopType]");
    			int dropPriority = sub.getInt("[@dropPriority]");
    			int shopLvl = sub.getInt("[@shopLvl]");
    			int buyPermission = sub.getInt("[@buyPermission]");
    			int rate = sub.getInt("[@rate]");
    			
    			GuildShopSell sell = new GuildShopSell();
    			sell.setId(id);
    			sell.setItemType(itemType);
    			sell.setPropId(propId);
    			sell.setCount(count);
    			sell.setPrice(price);
    			sell.setShopType(shopType);
    			sell.setDropPriority(dropPriority);
    			sell.setShopLvl(shopLvl);
    			sell.setBuyPermission(buyPermission);
    			sell.setRate(rate);
    			
    			if (shopType == 1) {//普通商店出现
					if (!regularShopConfigs.containsKey(shopLvl)) {
						regularShopConfigs.put(shopLvl, new HashMap<Integer, GuildShopSell>());
					}
					
					regularShopConfigs.get(shopLvl).put(id, sell);
				}
    			else if (shopType == 2) {//占领区域商店出现
					
				}
    		}
    		guildConfig.setRegularShopSell(regularShopConfigs);
    		guildConfig.setOccupyShopSell(occupyShopConfigs);
    	}
    	xmlLoadTime.put("guildShopSell", Util.getServerTime());
    }
    
    private static void loadGuildNpc() throws Exception{
    	String itemConfPath = DZM.basePath + "/xml/GuildNPC.xml";
    	log.info("load item config file path :{}",itemConfPath);
    	XMLConfiguration conf = new XMLConfiguration(itemConfPath);
    	List<HierarchicalConfiguration> fields = conf.configurationsAt("GuildNPCs.GuildNpc");
    	if (fields != null && fields.size() > 0) {
    		HierarchicalConfiguration sub;
    		HashMap<Integer, HashMap<Integer, GuildNpcConfig>> totemNpcConfigs = new HashMap<Integer, HashMap<Integer,GuildNpcConfig>>();
    		for (int i = 0; i < fields.size(); i++){
    			sub = fields.get(i);
    			int totemId = sub.getInt("[@totemId]");
    			int totemLv = sub.getInt("[@totemLv]");
    			String totemNpcId = sub.getString("[@totemNpcId]");
    			int totemNpcLv = sub.getInt("[@totemNpcLv]");
    			float totemNpcSkillRatio = sub.getFloat("[@totemNpcSkillRatio]");
    			float HpBuff = sub.getFloat("[@HpBuff]");
    			float atcBuff = sub.getFloat("[@atcBuff]");
    			float DefBuff = sub.getFloat("[@DefBuff]");
    			float willBuff = sub.getFloat("[@willBuff]");
    			int totemNpcRate = sub.getInt("[@totemNpcRate]");
    			int GuildMemberRate = sub.getInt("[@GuildMemberRate]");
    			
    			GuildNpcConfig npcConfig = new GuildNpcConfig();
    			npcConfig.setNpcIds(totemNpcId);
    			npcConfig.setNpcLevel(totemNpcLv);
    			npcConfig.setSkillRatios(totemNpcSkillRatio);
    			npcConfig.setHpBuff(HpBuff);
    			npcConfig.setAttackBuff(atcBuff);
    			npcConfig.setDefenceBuff(DefBuff);
    			npcConfig.setWillBuff(willBuff);
    			npcConfig.setNpcRate(totemNpcRate);
    			npcConfig.setmemberRate(GuildMemberRate);
    			
    			if (!totemNpcConfigs.containsKey(totemId)) {
    				totemNpcConfigs.put(totemId, new HashMap<Integer,GuildNpcConfig>());
				}
    			totemNpcConfigs.get(totemId).put(totemLv, npcConfig);
    		}
    		guildConfig.setNpcConfigs(totemNpcConfigs);
    	}
    	xmlLoadTime.put("guildNpc", Util.getServerTime());
    }
    private static void loadGoddessRebateConfig() throws Exception{
    	String itemConfPath = DZM.basePath + "/xml/GoddessRebateConfig.xml";
    	log.info("load item config file path :{}",itemConfPath);
    	XMLConfiguration conf = new XMLConfiguration(itemConfPath);
    	List<HierarchicalConfiguration> fields = conf.configurationsAt("GoddessRebateConfigs.GoddessRebateConfig");
    	 if (fields != null && fields.size() > 0) {
             HierarchicalConfiguration sub;
             for (int i = 0; i < fields.size(); i++)
             {
                 GoddessRebateConfig it = new GoddessRebateConfig();
                 sub = fields.get(i);
                 int id=sub.getInt("[@id]");
                 int price = sub.getInt("[@price]");
                 int needTimes = sub.getInt("[@needTimes]");
                 String rewards = sub.getString("[@rewards]");
                 ArrayList<int[]> rewardList=new ArrayList<int[]>();
                 String[] rewards_str=rewards.split(",");
         		for(int j=0;j<rewards_str.length;j++){
         			String reward_str=rewards_str[j];
         			String[] reward_info=reward_str.split(":");
         			int type=Integer.parseInt( reward_info[0].trim());
         			int itemId=Integer.parseInt(reward_info[1].trim());
         			int count=Integer.parseInt(reward_info[2].trim());
         			int[]reward=new int[3];
         			reward[0]=type;
         			reward[1]=itemId;
         			reward[2]=count;
         			rewardList.add(reward);
         		}                 
                 
                 it.setId(id);
                 it.setPrice(price);
                 it.setNeedTimes(needTimes);
                 it.setRewardList(rewardList);
                 goddessRebateConfigs.put(id, it);
             }
         }
    	xmlLoadTime.put("goddessRebateConfig", Util.getServerTime());
    }
}

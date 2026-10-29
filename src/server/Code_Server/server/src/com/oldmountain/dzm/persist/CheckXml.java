package com.oldmountain.dzm.persist;

import java.io.File;
import java.util.HashMap;
import java.util.logging.Level;
import java.util.logging.Logger;

import com.oldmountain.dzm.DZM;
import com.oldmountain.dzm.util.XMLTemplateService;

public class CheckXml implements Runnable{
	private static final Logger logger = Logger.getLogger(DZM.class.getName());
	long preTime;
	long currentTime;
	
	public void run(){
		try {
			logger.info("check xml file");
			HashMap<String, Integer> modifyTime = XMLTemplateService.getModifyTime();
			String itemConfPath = DZM.basePath + "/xml/item.xml";
			File file = new File(itemConfPath);
			int time = (int)(file.lastModified()/1000);
			
			
			itemConfPath = DZM.basePath + "/xml/skill.xml";
			file = new File(itemConfPath);
			time = (int)(file.lastModified()/1000);
			if (time > modifyTime.get("skill")) {
				XMLTemplateService.initTemplateData(2);
			}
			
			itemConfPath = DZM.basePath + "/xml/equip.xml";
			file = new File(itemConfPath);
			time = (int)(file.lastModified()/1000);
			if (time > modifyTime.get("equip")) {
				XMLTemplateService.initTemplateData(3);
			}
			
			itemConfPath = DZM.basePath + "/xml/disciple.xml";
			file = new File(itemConfPath);
			time = (int)(file.lastModified()/1000);
			if (time > modifyTime.get("disciple")) {
				XMLTemplateService.initTemplateData(4);
			}
			
			itemConfPath = DZM.basePath + "/xml/fate.xml";
			file = new File(itemConfPath);
			time = (int)(file.lastModified()/1000);
			if (time > modifyTime.get("fate")) {
				XMLTemplateService.initTemplateData(5);
			}
			
			itemConfPath = DZM.basePath + "/xml/career.xml";
			file = new File(itemConfPath);
			int time1 = (int)(file.lastModified()/1000);
			
			itemConfPath = DZM.basePath + "/xml/career_group.xml";
			file = new File(itemConfPath);
			int time2 = (int)(file.lastModified()/1000);
			
			if (time1 > modifyTime.get("career") || time2 > modifyTime.get("career")) {
				XMLTemplateService.initTemplateData(6);
			}
			
			itemConfPath = DZM.basePath + "/xml/kindSelecter.xml";
			file = new File(itemConfPath);
			time = (int)(file.lastModified()/1000);
			if (time > modifyTime.get("kindSelecter")) {
				XMLTemplateService.initTemplateData(7);
			}
			
			itemConfPath = DZM.basePath + "/xml/discipleSelecter.xml";
			file = new File(itemConfPath);
			time = (int)(file.lastModified()/1000);
			if (time > modifyTime.get("discipleSelecter"))
            {
				XMLTemplateService.initTemplateData(8);
			}

            itemConfPath = DZM.basePath + "/xml/sysbasicconfig.xml";
            file = new File(itemConfPath);
            time = (int)(file.lastModified()/1000);
            if (time > modifyTime.get("sysBasicConfig"))
            {
                XMLTemplateService.initTemplateData(9);
            }

            itemConfPath = DZM.basePath + "/xml/levelUp.xml";
            file = new File(itemConfPath);
            time = (int)(file.lastModified()/1000);
            if (time > modifyTime.get("levelUp"))
            {
                XMLTemplateService.initTemplateData(10);
            }

            itemConfPath = DZM.basePath + "/xml/FightEndGeneral.xml";
            file = new File(itemConfPath);
            time = (int)(file.lastModified()/1000);
            if (time > modifyTime.get("fightEndGenerals"))
            {
                XMLTemplateService.initTemplateData(13);
            }

            itemConfPath = DZM.basePath + "/xml/FightEndNpc.xml";
            file = new File(itemConfPath);
            time = (int)(file.lastModified()/1000);
            if (time > modifyTime.get("fightEndNpcTemplates"))
            {
                XMLTemplateService.initTemplateData(14);
            }

            itemConfPath = DZM.basePath + "/xml/FightEndNpcLevel.xml";
            file = new File(itemConfPath);
            time = (int)(file.lastModified()/1000);
            if (time > modifyTime.get("fightEndNpcLevelTemplates"))
            {
                XMLTemplateService.initTemplateData(15);
            }
            
            itemConfPath = DZM.basePath + "/xml/levelUp.xml";
            file = new File(itemConfPath);
            time = (int)(file.lastModified()/1000);
            if (time > modifyTime.get("levelUp"))
            {
            	XMLTemplateService.initTemplateData(18);
            }
            
            itemConfPath = DZM.basePath + "/xml/disciple_exp_level.xml";
            file = new File(itemConfPath);
            time = (int)(file.lastModified()/1000);
            if (time > modifyTime.get("discipleLevel"))
            {
            	XMLTemplateService.initTemplateData(19);
            }
            
            itemConfPath = DZM.basePath + "/xml/user_level.xml";
            file = new File(itemConfPath);
            time = (int)(file.lastModified()/1000);
            if (time > modifyTime.get("userLevel"))
            {
            	XMLTemplateService.initTemplateData(20);
            }
            
            itemConfPath = DZM.basePath + "/xml/user_level_config.xml";
            file = new File(itemConfPath);
            time = (int)(file.lastModified()/1000);
            if (time > modifyTime.get("userLevelConfig"))
            {
            	XMLTemplateService.initTemplateData(21);
            }
            
            itemConfPath = DZM.basePath + "/xml/treasureSelecter.xml";
            file = new File(itemConfPath);
            time = (int)(file.lastModified()/1000);
            if (time > modifyTime.get("treasureSelecter"))
            {
            	XMLTemplateService.initTemplateData(22);
            }
            
            itemConfPath = DZM.basePath + "/xml/tools.xml";
            file = new File(itemConfPath);
            time = (int)(file.lastModified()/1000);
            if (time > modifyTime.get("tools"))
            {
            	XMLTemplateService.initTemplateData(23);
            }
            
            itemConfPath = DZM.basePath + "/xml/levelSilvers.xml";
            file = new File(itemConfPath);
            time = (int)(file.lastModified()/1000);
            if (time > modifyTime.get("levelSilvers"))
            {
            	XMLTemplateService.initTemplateData(24);
            }
            
            itemConfPath = DZM.basePath + "/xml/skillSelecter.xml";
            file = new File(itemConfPath);
            time = (int)(file.lastModified()/1000);
            if (time > modifyTime.get("skillSelecter"))
            {
            	XMLTemplateService.initTemplateData(25);
            }
            
            itemConfPath = DZM.basePath + "/xml/equipSelecter.xml";
            file = new File(itemConfPath);
            time = (int)(file.lastModified()/1000);
            if (time > modifyTime.get("equipSelecter"))
            {
            	XMLTemplateService.initTemplateData(26);
            }
            
            itemConfPath = DZM.basePath + "/xml/FightEndRankReward.xml";
            file = new File(itemConfPath);
            time = (int)(file.lastModified()/1000);
            if (time > modifyTime.get("fightEndRewards"))
            {
            	XMLTemplateService.initTemplateData(26);
            }
            
            itemConfPath = DZM.basePath + "/xml/continue_loging.xml";
            file = new File(itemConfPath);
            time = (int)(file.lastModified()/1000);
            if (time > modifyTime.get("continueLogin"))
            {
            	XMLTemplateService.initTemplateData(28);
            }
            
            itemConfPath = DZM.basePath + "/xml/soulSelecter.xml";
            file = new File(itemConfPath);
            time = (int)(file.lastModified()/1000);
            if (time > modifyTime.get("soulSelecter"))
            {
            	XMLTemplateService.initTemplateData(29);
            }
            
            itemConfPath = DZM.basePath + "/xml/Title.xml";
            file = new File(itemConfPath);
            time = (int)(file.lastModified()/1000);
            if (time > modifyTime.get("titleConfig"))
            {
            	XMLTemplateService.initTemplateData(30);
            }
            
            itemConfPath = DZM.basePath + "/xml/VisitRewardSelecter.xml";
            file = new File(itemConfPath);
            time = (int)(file.lastModified()/1000);
            if (time > modifyTime.get("VisitRewardSelecter"))
            {
            	XMLTemplateService.initTemplateData(31);
            }
            
            itemConfPath = DZM.basePath + "/xml/rechargeConfig.xml";
            file = new File(itemConfPath);
            time = (int)(file.lastModified()/1000);
            if (time > modifyTime.get("rechargeConfig"))
            {
            	XMLTemplateService.initTemplateData(32);
            }
            
            itemConfPath = DZM.basePath + "/xml/adventuresConfig.xml";
            file = new File(itemConfPath);
            time = (int)(file.lastModified()/1000);
            if (time > modifyTime.get("adventuresConfig"))
            {
            	XMLTemplateService.initTemplateData(33);
            }
            
            itemConfPath = DZM.basePath + "/xml/usePowerReward.xml";
            file = new File(itemConfPath);
            time = (int)(file.lastModified()/1000);
            if (time > modifyTime.get("usePowerReward"))
            {
            	XMLTemplateService.initTemplateData(34);
            }

            itemConfPath = DZM.basePath + "/xml/adventuresContinueLogin.xml";
            file = new File(itemConfPath);
            time = (int)(file.lastModified()/1000);
            if (time > modifyTime.get("adventuresContinueLogin"))
            {
                XMLTemplateService.initTemplateData(35);
            }

            itemConfPath = DZM.basePath + "/xml/adventuresContinueLogin.xml";
            file = new File(itemConfPath);
            time = (int)(file.lastModified()/1000);
            if (time > modifyTime.get("adventuresContinueLogin"))
            {
                XMLTemplateService.initTemplateData(35);
            }

            itemConfPath = DZM.basePath + "/xml/skillBookConfig.xml";
            file = new File(itemConfPath);
            time = (int)(file.lastModified()/1000);
            if (time > modifyTime.get("skillBookConfig"))
            {
                XMLTemplateService.initTemplateData(36);
            }

            itemConfPath = DZM.basePath + "/xml/skillBookNpcConfig.xml";
            file = new File(itemConfPath);
            time = (int)(file.lastModified()/1000);
            if (time > modifyTime.get("skillBookNpcConfig"))
            {
                XMLTemplateService.initTemplateData(37);
            }

            itemConfPath = DZM.basePath + "/xml/skillBookSnatchConfig.xml";
            file = new File(itemConfPath);
            time = (int)(file.lastModified()/1000);
            if (time > modifyTime.get("skillBookSnatchConfig"))
            {
                XMLTemplateService.initTemplateData(38);
            }

            itemConfPath = DZM.basePath + "/xml/skillbookSelecter.xml";
            file = new File(itemConfPath);
            time = (int)(file.lastModified()/1000);
            if (time > modifyTime.get("skillbookSelecter"))
            {
                XMLTemplateService.initTemplateData(39);
            }

            itemConfPath = DZM.basePath + "/xml/cdkeys.xml";
            file = new File(itemConfPath);
            time = (int)(file.lastModified()/1000);
            if (time > modifyTime.get("cdkeys"))
            {
            	XMLTemplateService.initTemplateData(40);
            }
            
            itemConfPath = DZM.basePath + "/xml/guideConfiger.xml";
            file = new File(itemConfPath);
            time = (int)(file.lastModified()/1000);
            if (time > modifyTime.get("guideConfiger"))
            {
            	XMLTemplateService.initTemplateData(41);
            }
            
            itemConfPath = DZM.basePath + "/xml/inviteConfig.xml";
            file = new File(itemConfPath);
            time = (int)(file.lastModified()/1000);
            if (time > modifyTime.get("inviteConfig"))
            {
            	XMLTemplateService.initTemplateData(42);
            }
            
            itemConfPath = DZM.basePath + "/xml/worldbossConfig.xml";
            file = new File(itemConfPath);
            time = (int)(file.lastModified()/1000);
            if (time > modifyTime.get("worldBossConfig"))
            {
            	XMLTemplateService.initTemplateData(43);
            }
            
            itemConfPath = DZM.basePath + "/xml/chargeRebateConfig.xml";
            file = new File(itemConfPath);
            time = (int)(file.lastModified()/1000);
            if (time > modifyTime.get("chargeRebateConfig"))
            {
            	XMLTemplateService.initTemplateData(44);
            }
            
            itemConfPath = DZM.basePath + "/xml/fortuneComeConfig.xml";
            file = new File(itemConfPath);
            time = (int)(file.lastModified()/1000);
            if (time > modifyTime.get("fortuneCome"))
            {
            	XMLTemplateService.initTemplateData(45);
            }
            
            itemConfPath = DZM.basePath + "/xml/ServerConfig.xml";
            file = new File(itemConfPath);
            time = (int)(file.lastModified()/1000);
            if (time > modifyTime.get("serverConfig"))
            {
            	XMLTemplateService.initTemplateData(46);
            }
            
            itemConfPath = DZM.basePath + "/xml/giveSoulSelecter.xml";
            file = new File(itemConfPath);
            time = (int)(file.lastModified()/1000);
            if (time > modifyTime.get("giveSoulSelecter"))
            {
            	XMLTemplateService.initTemplateData(47);
            }
            
            itemConfPath = DZM.basePath + "/xml/giveSoulStage.xml";
            file = new File(itemConfPath);
            time = (int)(file.lastModified()/1000);
            if (time > modifyTime.get("giveSoulStage"))
            {
            	XMLTemplateService.initTemplateData(48);
            }
            
            itemConfPath = DZM.basePath + "/xml/adventuresConfig.xml";
            file = new File(itemConfPath);
            time = (int)(file.lastModified()/1000);
            if (time > modifyTime.get("giveSoulConfig"))
            {
            	XMLTemplateService.initTemplateData(49);
            }
            
            itemConfPath = DZM.basePath + "/xml/platActiveCodeCtrl.xml";
            file = new File(itemConfPath);
            time = (int)(file.lastModified()/1000);
            if (time > modifyTime.get("platActiveCodeCtrlConfig"))
            {
            	XMLTemplateService.initTemplateData(50);
            }
            
            itemConfPath = DZM.basePath + "/xml/worldbossLvlHp.xml";
            file = new File(itemConfPath);
            time = (int)(file.lastModified()/1000);
            if (time > modifyTime.get("worldbossLvlHp"))
            {
            	XMLTemplateService.initTemplateData(51);
            }
            
            itemConfPath = DZM.basePath + "/xml/luckyStarConfig.xml";
            file = new File(itemConfPath);
            time = (int)(file.lastModified()/1000);
            if (time > modifyTime.get("luckStarConfig"))
            {
            	XMLTemplateService.initTemplateData(52);
            }

            itemConfPath = DZM.basePath + "/xml/Diamond.xml";
            file = new File(itemConfPath);
            time = (int)(file.lastModified()/1000);
            if (time > modifyTime.get("Diamond"))
            {
                XMLTemplateService.initTemplateData(53);
            }

            itemConfPath = DZM.basePath + "/xml/DiamondConfig.xml";
            file = new File(itemConfPath);
            time = (int)(file.lastModified()/1000);
            if (time > modifyTime.get("DiamondConfig"))
            {
                XMLTemplateService.initTemplateData(54);
            }


            itemConfPath = DZM.basePath + "/xml/DiamondAdditionConfig.xml";
            file = new File(itemConfPath);
            time = (int)(file.lastModified()/1000);
            if (time > modifyTime.get("DiamondAdditionConfig"))
            {
                XMLTemplateService.initTemplateData(55);
            }

            itemConfPath = DZM.basePath + "/xml/quickCareerConfig.xml";
          file = new File(itemConfPath);
          time = (int)(file.lastModified()/1000);
          if (time > modifyTime.get("quickCareerConfig"))
          {
          	XMLTemplateService.initTemplateData(56);
          }
            
            itemConfPath = DZM.basePath + "/xml/GuildBase.xml";
            file = new File(itemConfPath);
            time = (int)(file.lastModified()/1000);
            if (time > modifyTime.get("guildBase"))
            {
            	XMLTemplateService.initTemplateData(57);
            }
            
            itemConfPath = DZM.basePath + "/xml/GuildLvl.xml";
            file = new File(itemConfPath);
            time = (int)(file.lastModified()/1000);
            if (time > modifyTime.get("guildLevelConfig"))
            {
            	XMLTemplateService.initTemplateData(58);
            }
            
            itemConfPath = DZM.basePath + "/xml/GuildProps.xml";
            file = new File(itemConfPath);
            time = (int)(file.lastModified()/1000);
            if (time > modifyTime.get("guildProps"))
            {
            	XMLTemplateService.initTemplateData(59);
            }
            
            itemConfPath = DZM.basePath + "/xml/GuildBuildInfo.xml";
            file = new File(itemConfPath);
            time = (int)(file.lastModified()/1000);
            if (time > modifyTime.get("guildBuilds"))
            {
            	XMLTemplateService.initTemplateData(60);
            }
            
            itemConfPath = DZM.basePath + "/xml/deepWaterConfig.xml";
            file = new File(itemConfPath);
            time = (int)(file.lastModified()/1000);
            if (time > modifyTime.get("deepWaterConfig"))
            {
            	XMLTemplateService.initTemplateData(61);
            }
            
            itemConfPath = DZM.basePath + "/xml/GuildShopSell.xml";
            file = new File(itemConfPath);
            time = (int)(file.lastModified()/1000);
            if (time > modifyTime.get("guildShopSell"))
            {
            	XMLTemplateService.initTemplateData(62);
            }
            
            itemConfPath = DZM.basePath + "/xml/GuildNPC.xml";
            file = new File(itemConfPath);
            time = (int)(file.lastModified()/1000);
            if (time > modifyTime.get("guildNpc"))
            {
            	XMLTemplateService.initTemplateData(63);
            }
            
            itemConfPath = DZM.basePath + "/xml/platInitReward.xml";
            file = new File(itemConfPath);
            time = (int)(file.lastModified()/1000);
            if (time > modifyTime.get("platInitReward"))
            {
            	XMLTemplateService.initTemplateData(64);
            }
            
            itemConfPath = DZM.basePath + "/xml/GoddessRebateConfig.xml";
            file = new File(itemConfPath);
            time = (int)(file.lastModified()/1000);
            if (time > modifyTime.get("goddessRebateConfig"))
            {
            	XMLTemplateService.initTemplateData(65);
            }
            
			currentTime = System.currentTimeMillis();
			preTime = currentTime ;
		}
		catch (Throwable e) {
			logger.log(Level.SEVERE, "An error occurred while loading the server.", e);
		}
	}
}

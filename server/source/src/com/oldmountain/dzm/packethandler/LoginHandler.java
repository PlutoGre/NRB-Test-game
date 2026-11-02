package com.oldmountain.dzm.packethandler;

import java.util.ArrayList;
import java.util.Date;
import java.util.HashMap;
import java.util.List;

import org.apache.mina.common.IoSession;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.google.protobuf.InvalidProtocolBufferException;
import com.oldmountain.dzm.DZM;
import com.oldmountain.dzm.configEntity.DiamondConfig;
import com.oldmountain.dzm.configEntity.PlatInitReward;
import com.oldmountain.dzm.entity.ActivityInfo;
import com.oldmountain.dzm.entity.Bag;
import com.oldmountain.dzm.entity.Battle;
import com.oldmountain.dzm.entity.BattleArrayObject;
import com.oldmountain.dzm.entity.Career;
import com.oldmountain.dzm.entity.Disciple;
import com.oldmountain.dzm.entity.Equip;
import com.oldmountain.dzm.entity.Handbook;
import com.oldmountain.dzm.entity.Mapping;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.entity.PlayerFrom;
import com.oldmountain.dzm.entity.RelationShip;
import com.oldmountain.dzm.entity.Reward;
import com.oldmountain.dzm.entity.Skill;
import com.oldmountain.dzm.entity.Soul;
import com.oldmountain.dzm.entity.TeamBuffer;
import com.oldmountain.dzm.entity.Title;
import com.oldmountain.dzm.entity.UserStats;
import com.oldmountain.dzm.message.Login.OPLogin;
import com.oldmountain.dzm.message.Login.OPLoginRet;
import com.oldmountain.dzm.message.OP;
import com.oldmountain.dzm.net.ObjectAccessor;
import com.oldmountain.dzm.net.packet.Packet;
import com.oldmountain.dzm.services.DaoManager;
import com.oldmountain.dzm.services.RewardServices;
import com.oldmountain.dzm.services.SkillBookServices;
import com.oldmountain.dzm.util.BattleUtil;
import com.oldmountain.dzm.util.HandbookUtil;
import com.oldmountain.dzm.util.Util;
import com.oldmountain.dzm.util.XMLTemplateService;
/**
 * 登陆包
 * 1， 解析数据包
 * 2， 查找数据库
 * 3， 设置在线状态
 * 4， 下发登陆成功
 * 
 * 
 * @author miaoshengli
 */
public class LoginHandler {
	public static void login(Packet packet, IoSession session) {
		LoginRunable loginRunable = new LoginRunable(packet, session);
		DZM.getThreadPool().execute(loginRunable);
	}
}

class LoginRunable implements Runnable{
	private static final Logger log = LoggerFactory.getLogger(LoginHandler.class);
	private static final org.slf4j.Logger logoutinfo_log = LoggerFactory.getLogger("PlayerLogoutInfo");
	private Packet packet;
	private IoSession session;
	public LoginRunable(Packet packet,IoSession session){
		this.packet = packet;
		this.session = session;
	}
	public void run(){
		OPLogin login = null;
		OPLoginRet.Builder builder = OPLoginRet.newBuilder();
		builder.setResourceVersion(DZM.resource_version);
		try {
			login = OPLogin.parseFrom(packet.data.array());
		} catch (InvalidProtocolBufferException e) {
			e.printStackTrace();
			return;
		}
		
		String puid = login.getPuid();
		String password = login.getPwd();
        String platName = login.getPlatform().toLowerCase();
        String[] platArr = platName.split("_");
        int arrLen = platArr.length;
        if (arrLen >= 2) {
			platName = platArr[arrLen-2] + "_" + platArr[arrLen - 1];
		}
        
        String deviceId = login.getDeviceid();
		
		if (DZM.serverStat == 2 && !DZM.testerArray.contains(puid)) {
			builder.setStatus(5);
			Packet pt = new Packet(OP.code.OPCODE_PLAYER_LOGINRET_S_VALUE, builder.build().toByteArray());
			session.write(pt);
			return;
		}

        if(XMLTemplateService.getPlatActiveCodeStatus(platName))
        {
            List<Object> list = DZM.getEntityManager().limitQuery("from ActiveCodeEntity where playerid = ?", 0, 1, puid.toLowerCase());
            if (list.size() < 1)
            {
                builder.setStatus(4);
                Packet pt = new Packet(OP.code.OPCODE_PLAYER_LOGINRET_S_VALUE, builder.build().toByteArray());
                session.write(pt);
                return;
            }
        }
		
		int version = login.getVersion();//version default = 1;
		
		log.info("login handler received: {}, {}, {}", new Object[]{puid, password, version});
		if(version < DZM.CURRENT_VERSION) {
			//notify need update;
		}
		
		Mapping mapping = ObjectAccessor.getMapping(puid);
		if (mapping == null) {
			List<Object> mappings = DZM.getEntityManager().limitQuery("from Mapping where puid = ?", 0, 1, puid);
			if (mappings.size() == 1) {
				mapping = (Mapping)mappings.get(0);
				ObjectAccessor.addMapping(mapping);
			}
		}
		
		long current_time = Util.getServerMillTime();
		int time = Util.getServerTime();
		Player player = new Player();
		int login_status = 1;
		
		if (mapping != null) {
			int playerId = mapping.getGameid();
			player = ObjectAccessor.getOnlineUser(playerId);
			if (player == null) {
				List<Object> list = DZM.getEntityManager().limitQuery("from Player where id = ?", 0, 1, playerId);
				if (list.size() == 1) {
					player = (Player)list.get(0);
					//将装备的信息加载到内存player对象中
					HashMap<Integer, Equip> userEquips = new HashMap<Integer, Equip>();
					List<Equip> equips = DZM.getEntityManager().query(Equip.class, "from Equip where playerid = ?", playerId);
                    long startTimes=Util.getServerMillTime();
                    ArrayList<Integer> _isCompatibleList=new ArrayList<Integer>();
					if (equips.size() > 0) {
						for (int i = 0; i < equips.size(); i++) {
							Equip equip = equips.get(i);
                            //宝石功能开启后才生成数据
                            if(DiamondConfig.DiamondOpen())
                            {
                                //add by dylan at 20130709 because 宝石镶嵌 兼容改造
                                if(!equip.isHavDiamonInfo())
                                {
                                    equip.generateDiamonInfo();
                                    _isCompatibleList.add(equip.getId());
                                }
                            }
                            userEquips.put((Integer)equip.getId(), equip);
						}
					}
					player.setEquips(userEquips);
                    //add by dylan at 20130709 because 宝石镶嵌 兼容改造
                    if(_isCompatibleList.size()>0)
                    {
                        for(int i=0;i<_isCompatibleList.size();++i)
                        {
                            player.updateOneEquip(userEquips.get(_isCompatibleList.get(i)));
                        }
                    }
                    long endTimes=Util.getServerMillTime();
                    log.info("login handler equip equipSize: {} ,costTime: {} ", new Object[]{_isCompatibleList.size(), endTimes-startTimes});
					//将用户的图鉴信息加载到内存player对象中
					Handbook userHandbook = new Handbook();
					List<Handbook> handbooks = DZM.getEntityManager().query(Handbook.class, "from Handbook where playerid = ? ", playerId);
					if(handbooks.size() > 0){
						for(int i = 0; i < handbooks.size(); i++){
							userHandbook = handbooks.get(i);
						}
						player.setHandbook(userHandbook);
					}else{
						HandbookUtil.insertOrUpdateHandbook(player, null, null, null);
						DZM.getEntityManager().createSync(player.getHandbook());
					}
					//将技能的信息加载到内存player对象中
					HashMap<Integer, Skill> userSkills = new HashMap<Integer, Skill>();
					List<Skill> skills = DZM.getEntityManager().query(Skill.class, "from Skill where playerid = ?", playerId);
					if (skills.size() > 0) {
						for (int i = 0; i < skills.size(); i++) {
							Skill skill = skills.get(i);
							userSkills.put((Integer)skill.getId(), skill);
						}
					}
					player.setSkills(userSkills);
					//将弟子的信息加载到内存player对象中
					HashMap<Integer, Disciple> userDisciples = new HashMap<Integer, Disciple>();
					List<Disciple> disciples = DZM.getEntityManager().query(Disciple.class, "from Disciple where playerid = ?", playerId);
					List<Integer> discipleItems = new ArrayList<Integer>();
					if (disciples.size() > 0) {
						for (int i = 0; i < disciples.size(); i++) {
							Disciple disciple = disciples.get(i);
							userDisciples.put((Integer)disciple.getId(), disciple);
							discipleItems.add(disciple.getItemid());
						}
					}
					player.setDisciples(userDisciples);
					player.setDiscipleitems(discipleItems);
					
					//将江湖的信息加载到内存player对象中
					Career userCareer = new Career();
					List<Object> career = DZM.getEntityManager().limitQuery("from Career where playerid = ?", 0, 1, playerId);
					if (career.size() == 1) {
						userCareer = (Career)career.get(0);
					}
					player.setCareer(userCareer);
					
					//将背包信息加载到内存player对象中
					HashMap<Integer, Bag> userBags = new HashMap<Integer, Bag>();
					List<Bag> bags = DZM.getEntityManager().query(Bag.class, "from Bag where playerid = ?", playerId);
					if (bags.size() > 0) {
						for (int i = 0; i < bags.size(); i++) {
							Bag bag = bags.get(i);
							userBags.put((Integer)bag.getItemid(), bag);
						}
					}
					player.setBags(userBags);
					
					//将魂魄信息加载到内存player对象中
					HashMap<Integer, Soul>userSouls = new HashMap<Integer, Soul>();
					List<Soul> souls = DZM.getEntityManager().query(Soul.class, "from Soul where playerid = ?", playerId);
					if (souls.size() > 0) {
						for (int i = 0; i < souls.size(); i++) {
							Soul soul = souls.get(i);
							userSouls.put((Integer)soul.getItemid(), soul);
						}
					}
					player.setSouls(userSouls);
					
					//将行为记录加载进来
					List<UserStats> statList = DZM.getEntityManager().limitQuery("from UserStats where id = ?", 0, 1, playerId);
					if (statList.size() == 1) {
						UserStats stats = statList.get(0);
						player.setUserStats(stats);
					}
					
					//将掌门决信息加载进来
					List<TeamBuffer> bufferList = DZM.getEntityManager().limitQuery("from TeamBuffer where playerid = ?", 0, 1, playerId);
					if (bufferList.size() == 1) {
						TeamBuffer teamBuffer = bufferList.get(0);
						player.setTeamBuffer(teamBuffer);
					}
					
					//将气势信息加载进来
					List<Title> titleList = DZM.getEntityManager().limitQuery("from Title where playerid = ?", 0, 1, playerId);
					if (titleList.size() == 1) {
						Title title = titleList.get(0);
						player.setTitle(title);
					}
//					//检测玩家vip等级称号是否正确 (可能存在问题,待定)
//					TitleUtil.checkVipTitle(player, 2, player.getViplevel());
                    //添加残章信息
                    SkillBookServices.loadSkillBook(player);
                    
                    //将恩怨的信息加载到内存player对象中
					RelationShip relationShip = new RelationShip();
					List<Object> relationList = DZM.getEntityManager().limitQuery("from RelationShip where playerid = ?", 0, 1, playerId);
					if (relationList.size() == 1) {
						relationShip = (RelationShip)relationList.get(0);
					}
					else {
						relationShip.setPlayerid(playerId);
						DZM.getEntityManager().createSync(relationShip);
					}
					player.setRelationShip(relationShip);
					
					//添加来源信息
					List<Object> fromInfo = DZM.getEntityManager().limitQuery("from PlayerFrom where playerid=?", 0, 1, playerId);
					if (fromInfo.size() == 1) {
						PlayerFrom from = new PlayerFrom();
						from = (PlayerFrom)fromInfo.get(0);
						player.setDeviceId(from.getDeviceid());
						player.setPlatform(from.getPlatform());
						if (DZM.platForm.equals("ios") || DZM.platForm.equals("91") || DZM.platForm.equals("pp") || DZM.platForm.equals("lo")) {
							if (puid.length() > 6 && puid.startsWith("PPUSR_")) {
								player.setPlatform("ios_pp");
							}
							else if (puid.length() > 6 && puid.startsWith("AGUSR_")) {
								player.setPlatform("ios_ag");
							}
							else if (puid.length() > 6 && puid.startsWith("TBUSR_")) {
								player.setPlatform("ios_tb");
							}
							else if (platName.endsWith("_91")) {
								player.setPlatform("ios_91");
							}
						}
					}
					//TODO see
					//将玩家活动数据的信息加载到内存player对象中
					ActivityInfo activityInfo = new ActivityInfo();
					List<Object> activityInfoList = DZM.getEntityManager().limitQuery("from ActivityInfo where playerid = ?", 0, 1, playerId);
					if (activityInfoList.size() == 1) {
						activityInfo = (ActivityInfo)activityInfoList.get(0);
					}
					else {
						activityInfo.setPlayerid(playerId);
						DZM.getEntityManager().createSync(activityInfo);
					}
					player.setActivityInfo(activityInfo);
					
					//将阵容信息加载到player对象中
					int limit = BattleUtil.getBattleCountByLevel(player.getLevel());
					List<Battle> battles = DZM.getEntityManager().limitQuery("from Battle where playerid = ?", 0,8, playerId);
					HashMap<Integer, Battle> allBattles = new HashMap<Integer, Battle>();
					HashMap<Integer, Battle> userBattles = new HashMap<Integer, Battle>();
					ArrayList<Battle> positionBattles = new ArrayList<Battle>();
					if (battles.size() > 0) {
						for (int i = 0; i < battles.size(); i++) {
							allBattles.put(battles.get(i).getId(), battles.get(i));
						}
					}
					String ids = player.getBattleids();
					String[] idsArray = ids.split(",");
					for (int i = 0; i < limit; i++) {
						int battleid = Integer.parseInt(idsArray[i]);
						positionBattles.add(allBattles.get(battleid));
						userBattles.put(battleid, allBattles.get(battleid));
						allBattles.remove(battleid);
					}
					
					player.setBattlearray(userBattles);
					BattleArrayObject battleArrayObject = new BattleArrayObject();
					battleArrayObject.setPlayerid(playerId);
					battleArrayObject.setMomentum(player.getMomentum());
					battleArrayObject.setBattlearrays(positionBattles);
					ObjectAccessor.addBattleArray(battleArrayObject);
				}
				else {
					player = new Player();
					player.setPuid(puid);
					player.setPassword(password);
					player.setPoweraddtime(time);
					player.setVitalityaddtime(time);
					player.setRegistertime(time);
					DZM.getEntityManager().createSync(player);
				}
			}
		}
		else {
			if (DZM.totalUserCount >= DZM.serverMaxUser) {
				builder.setStatus(3);
				Packet pt = new Packet(OP.code.OPCODE_PLAYER_LOGINRET_S_VALUE, builder.build().toByteArray());
				session.write(pt);
				return;
			}
			player = new Player();
			//创建mapping
			mapping = new Mapping();
			mapping.setPuid(puid);
			mapping.setPasswd(password);
			mapping.setName("player"+puid);
			DZM.getEntityManager().createSync(mapping);
			//创建阵容属性
			ArrayList<Battle> positionBattle = new ArrayList<Battle>();
			HashMap<Integer, Battle> userBattles = new HashMap<Integer, Battle>();
			String idsString = "";
			for (int i = 0; i < 2; i++) {
				Battle battle = new Battle();
				battle.setPlayerid(mapping.getGameid());
				DZM.getEntityManager().createSync(battle);
				if (i != 0) {
					idsString = idsString + "," + battle.getId();
				}
				else {
					idsString += battle.getId();
				}
				positionBattle.add(battle);
				userBattles.put(battle.getId(),battle);
			}
			player.setBattleids(idsString);
			player.setBattlearray(userBattles);
			BattleArrayObject battleObject = new BattleArrayObject();
			battleObject.setPlayerid(mapping.getGameid());
			battleObject.setMomentum(player.getMomentum());
			battleObject.setBattlearrays(positionBattle);
			ObjectAccessor.addBattleArray(battleObject);
			
			//创建player
			player.setPuid(puid);
			player.setPassword(password);
			player.setId(mapping.getGameid());
			player.setPoweraddtime(time);
			player.setVitalityaddtime(time);
			player.setRegistertime(time);
			DZM.getEntityManager().createSync(player);
			DZM.totalUserCount ++;
			//创建生涯属性
			Career career = new Career();
			career.setPlayerid(mapping.getGameid());
			career.setChecktime(time);
			DZM.getEntityManager().createSync(career);
			player.setCareer(career);
			
			//创建用户统计属性
			UserStats userStats = new UserStats();
			
			
			///VIP10以上的尊享礼包内容翻倍		VIP10以上所有特权+10，（例：原VIP10每日初始挑战14次+10次）
			int addCount = 4;
			if (player.getViplevel() > 9)
			{
				addCount = 10;
			}
			userStats.setChallengetimes(player.getViplevel() + addCount);
			
			
			userStats.setPlayerid(mapping.getGameid());
			DZM.getEntityManager().createSync(userStats);
			player.setUserStats(userStats);
			
			//创建掌门决信息
			TeamBuffer teamBuffer = new TeamBuffer();
			teamBuffer.setPlayerid(mapping.getGameid());
			DZM.getEntityManager().createSync(teamBuffer);
			player.setTeamBuffer(teamBuffer);
			
			//创建气势信息
			Title title = new Title();
			title.setPlayerid(mapping.getGameid());
			DZM.getEntityManager().createSync(title);
			player.setTitle(title);
			
			//创建来源信息
			PlayerFrom fromInfo = new PlayerFrom();
			fromInfo.setPuid(puid);
			if (DZM.platForm == "ios" || DZM.platForm == "91" || DZM.platForm == "pp") {
				if (puid.length() > 6 && puid.startsWith("PPUSR_")) {
					platName="ios_pp";
				}
				else if (puid.length() > 6 && puid.startsWith("AGUSR_")) {
					platName="ios_ag";
				}
				else if (puid.length() > 6 && puid.startsWith("TBUSR_")) {
					platName="ios_tb";
				}
				else if (platName.endsWith("_91")) {
					platName="ios_91";
				}
			}
			
			fromInfo.setPlayerid(mapping.getGameid());
			fromInfo.setPlatform(platName);
			fromInfo.setDeviceid(deviceId);
			fromInfo.setCreatetime(time);
			DZM.getEntityManager().createSync(fromInfo);
			
			player.setDeviceId(deviceId);
			player.setPlatform(platName);
			
			//创建恩怨信息
			RelationShip relationShip = new RelationShip();
			relationShip.setPlayerid(mapping.getGameid());
			player.setRelationShip(relationShip);
			DZM.getEntityManager().createSync(relationShip);
			//TODO see
			//创建玩家活动信息
			ActivityInfo activityInfo=new ActivityInfo();
			activityInfo.setPlayerid(mapping.getGameid());
			player.setActivityInfo(activityInfo);
			DZM.getEntityManager().createSync(activityInfo);
			
			//初始化图鉴信息
			HandbookUtil.insertOrUpdateHandbook(player, null, null, null);
			DZM.getEntityManager().createSync(player.getHandbook());
			//初始化一个武器
			Util.rewardEquip(player, 23);
			Util.rewardEquip(player, 32);			
			//初始化一个物品
			Util.rewardBagTool(player, 3503001, 1);
			//初始化5个魂魄
			Util.rewardSoul(player, 160, 5);
			//初始化一个欢迎传书
			Reward reward=new Reward();
			reward.setPlayerId(player.getId());
	        reward.setAddTime(time);
	        reward.setRewardTime(time);
	        reward.setStatus(1);
	        reward.setRewardType(RewardServices.COMPENSATE_REWARD);
			PlatInitReward initReward = XMLTemplateService.getPlatInitRewardConfigByPlatform(platName);
			if (initReward != null && time >= initReward.getRewardStartTime() && time < initReward.getRewardEndTime()) {
				reward.setStatus(0);
				reward.setRewardContent(initReward.getRewardInfo());
				reward.setRewardMsg(initReward.getRewardMsg());
			}
			else {
				reward.setRewardMsg("恭喜您在一个新游戏区创建了角色，如果您之前已经在其他区创建了角色，请在登录界面检查是否进入了正确的游戏区.");
			}
	        
	        DaoManager.getRewardDao().addReward(reward);
		}
		
		Util.dailyFirstLogin(player);
		
		session.setAttribute("playerid", player.getId());
		player.setSession(session);
		player.setLastlogin(time);
		ObjectAccessor.addOnlineUser(player);

		if (login.hasClientGuideFinished()) {
			boolean clientGuideFinished = login.getClientGuideFinished();
			if (player.getTutorialstep() < 1 && clientGuideFinished) {
				player.setTutorialstep(1);
			}
		}
		
		builder.setStatus(login_status);
		if (player.getTutorialstep() < 1) {
			builder.setServerGuideFinished(false);
		}

		player.sendPacket(OP.code.OPCODE_PLAYER_LOGINRET_S_VALUE, builder);// 发送返回包
		long current_time_2 = Util.getServerMillTime();
		player.setLastVerifyTime(time);

		long taken_time = current_time_2 - current_time;
		logoutinfo_log.info("player login info: {}, {}, {}, {}, {}, {}, {}, {}", new Object[]{player.getId() , player.getRegistertime() , player.getLevel() , player.getViplevel() , player.getExp() , Util.getServerTime() , player.getPlatform() , player.getDeviceId()});
		log.info("login handler ret send: {}, {}, {}", new Object[]{puid ,taken_time});
	}
}

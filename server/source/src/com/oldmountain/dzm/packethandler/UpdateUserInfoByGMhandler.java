package com.oldmountain.dzm.packethandler;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

import com.oldmountain.dzm.services.*;

import net.sf.json.JSONArray;
import net.sf.json.JSONObject;

import org.apache.commons.lang.StringUtils;
import org.apache.mina.common.IoSession;
import org.hibernate.SQLQuery;
import org.hibernate.Session;
import org.hibernate.Transaction;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.google.protobuf.InvalidProtocolBufferException;
import com.oldmountain.dzm.DZM;
import com.oldmountain.dzm.entity.Bag;
import com.oldmountain.dzm.entity.Battle;
import com.oldmountain.dzm.entity.BattleArrayObject;
import com.oldmountain.dzm.entity.Career;
import com.oldmountain.dzm.entity.Disciple;
import com.oldmountain.dzm.entity.DiscipleXmlTemplate;
import com.oldmountain.dzm.entity.Equip;
import com.oldmountain.dzm.entity.EquipSelecter;
import com.oldmountain.dzm.entity.KindSelecter;
import com.oldmountain.dzm.entity.Leagua;
import com.oldmountain.dzm.entity.LeaguaObject;
import com.oldmountain.dzm.entity.Mapping;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.entity.Skill;
import com.oldmountain.dzm.entity.SkillSelecter;
import com.oldmountain.dzm.entity.Soul;
import com.oldmountain.dzm.entity.TeamBuffer;
import com.oldmountain.dzm.entity.Title;
import com.oldmountain.dzm.entity.TreasureSelecter;
import com.oldmountain.dzm.entity.UserStats;
import com.oldmountain.dzm.message.OP;
import com.oldmountain.dzm.message.ErrorReporter.OPErrorReporter;
import com.oldmountain.dzm.message.UpdateUserInfoByGM.OPUpdateUserInfoByGM;
import com.oldmountain.dzm.message.UpdateUserInfoByGM.OPUpdateUserInfoByGMRet;
import com.oldmountain.dzm.message.UserBattleArray.OPUserBattleArrayRet;
import com.oldmountain.dzm.message.UserDiscipleInfo.OPUserDiscipleInfoRet;
import com.oldmountain.dzm.message.UserSkillInfo.OPUserSkillInfoRet;
import com.oldmountain.dzm.message.UserSoulInfo.OPUserSoulInfoRet;
import com.oldmountain.dzm.net.ObjectAccessor;
import com.oldmountain.dzm.net.packet.Packet;
import com.oldmountain.dzm.persist.DataLander;
import com.oldmountain.dzm.persist.EntityManagerImpl;
import com.oldmountain.dzm.services.BagToolServices;
import com.oldmountain.dzm.services.EquipServices;
import com.oldmountain.dzm.services.LeaguaServices;
import com.oldmountain.dzm.services.SkillServices;
import com.oldmountain.dzm.services.UserPropertyServices;
import com.oldmountain.dzm.util.BattleUtil;
import com.oldmountain.dzm.util.DiscipleUtil;
import com.oldmountain.dzm.util.EquipUtil;
import com.oldmountain.dzm.util.SkillUtil;
import com.oldmountain.dzm.util.SoulUtil;
import com.oldmountain.dzm.util.TitleUtil;
import com.oldmountain.dzm.util.UserToolConst;
import com.oldmountain.dzm.util.Util;
import com.oldmountain.dzm.util.XMLTemplateService;

public class UpdateUserInfoByGMhandler 
{
	private static final Logger logger2 = LoggerFactory.getLogger("ToolInfo");
	private static final Logger logger_box =LoggerFactory.getLogger("BoxRewardInfo");
	private static final Logger logger_recruitDisciple =LoggerFactory.getLogger("RecruitDiscipleInfo");
	public static void updateUserInfo(Packet packet, IoSession session) 
	{
		OPUpdateUserInfoByGM params = null;
		OPUpdateUserInfoByGMRet.Builder builder = OPUpdateUserInfoByGMRet.newBuilder();
		try 
		{
			params = OPUpdateUserInfoByGM.parseFrom(packet.data.array());
		} 
		catch (InvalidProtocolBufferException e) 
		{
			e.printStackTrace();
		}
		int self_id = (Integer) session.getAttribute("playerid");
		Player self_player = ObjectAccessor.getOnlineUser(self_id);
		String message = params.getMsg();
		String[] messages = message.split(" ");
		boolean self_flag = true;
		boolean other_online=true;
		Player other_player = null;
		//初步判断是否是GM指令
		if(messages.length<2 || (!"/clearBattle /repairAllTotem /add /to /tomorrow /gamble /darkroom /additem /cls /compensate".contains(messages[0])))
		{
			OPErrorReporter.Builder errorBuilder = OPErrorReporter.newBuilder();
			errorBuilder.setErrorid(OP.code.OPCODE_UPDATE_USERINFO_BY_GM_RET_S_VALUE);
			errorBuilder.setErrormsg("Is Not GM Command");
			self_player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errorBuilder);
			return;
		}
		
		// 判断是对谁进行操作
		if (message.contains("player")) 
		{
			self_flag = false;
			String []other_msg=message.split(" player ");
			if(other_msg.length<2){
				OPErrorReporter.Builder errorBuilder = OPErrorReporter.newBuilder();
				errorBuilder.setErrorid(OP.code.OPCODE_UPDATE_USERINFO_BY_GM_RET_S_VALUE);
				errorBuilder.setErrormsg("Message Error");
				self_player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errorBuilder);
				return;
			}
			String puid = message.split(" player ")[1];
			Mapping mapping = ObjectAccessor.getMapping(puid);
			//如果缓存中没有,就从数据库中查出来放入缓存
			if (mapping == null) {
				List<Object> mappings = DZM.getEntityManager().limitQuery("from Mapping where puid = ?", 0, 1, puid);
				if (mappings.size() == 1) {
					mapping = (Mapping)mappings.get(0);
					ObjectAccessor.addMapping(mapping);
				}
			}
			other_player = new Player();
			if (mapping != null) {
				int playerId = mapping.getGameid();
				other_player = ObjectAccessor.getOnlineUser(playerId);
				//如果玩家不在线,从数据库中查询出玩家信息
				if (other_player == null) {
					other_online=false;
					List<Object> list = DZM.getEntityManager().limitQuery("from Player where id = ?", 0, 1, playerId);
					if (list.size() == 1) {
						other_player = (Player)list.get(0);
						//将装备的信息加载到内存player对象中
						HashMap<Integer, Equip> userEquips = new HashMap<Integer, Equip>();
						List<Equip> equips = DZM.getEntityManager().query(Equip.class, "from Equip where playerid = ?", playerId);
						if (equips.size() > 0) {
							for (int i = 0; i < equips.size(); i++) {
								Equip equip = equips.get(i);
								userEquips.put((Integer)equip.getId(), equip);
							}
						}
						other_player.setEquips(userEquips);
						//将技能的信息加载到内存player对象中
						HashMap<Integer, Skill> userSkills = new HashMap<Integer, Skill>();
						List<Skill> skills = DZM.getEntityManager().query(Skill.class, "from Skill where playerid = ?", playerId);
						if (skills.size() > 0) {
							for (int i = 0; i < skills.size(); i++) {
								Skill skill = skills.get(i);
								userSkills.put((Integer)skill.getId(), skill);
							}
						}
						other_player.setSkills(userSkills);
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
						other_player.setDisciples(userDisciples);
						other_player.setDiscipleitems(discipleItems);
						
						//将江湖的信息加载到内存player对象中
						Career userCareer = new Career();
						List<Object> career = DZM.getEntityManager().limitQuery("from Career where playerid = ?", 0, 1, playerId);
						if (career.size() == 1) {
							userCareer = (Career)career.get(0);
						}
						other_player.setCareer(userCareer);
						
						//将背包信息加载到内存player对象中
						HashMap<Integer, Bag> userBags = new HashMap<Integer, Bag>();
						List<Bag> bags = DZM.getEntityManager().query(Bag.class, "from Bag where playerid = ?", playerId);
						if (bags.size() > 0) {
							for (int i = 0; i < bags.size(); i++) {
								Bag bag = bags.get(i);
								userBags.put((Integer)bag.getItemid(), bag);
							}
						}
						other_player.setBags(userBags);
						
						//将魂魄信息加载到内存player对象中
						HashMap<Integer, Soul>userSouls = new HashMap<Integer, Soul>();
						List<Soul> souls = DZM.getEntityManager().query(Soul.class, "from Soul where playerid = ?", playerId);
						if (souls.size() > 0) {
							for (int i = 0; i < souls.size(); i++) {
								Soul soul = souls.get(i);
								userSouls.put((Integer)soul.getItemid(), soul);
							}
						}
						other_player.setSouls(userSouls);
						
						//将行为记录加载进来
						List<UserStats> statList = DZM.getEntityManager().limitQuery("from UserStats where id = ?", 0, 1, playerId);
						if (statList.size() == 1) {
							UserStats stats = statList.get(0);
							other_player.setUserStats(stats);
						}
						
						//将掌门决信息加载进来
						List<TeamBuffer> bufferList = DZM.getEntityManager().limitQuery("from TeamBuffer where playerid = ?", 0, 1, playerId);
						if (bufferList.size() == 1) {
							TeamBuffer teamBuffer = bufferList.get(0);
							other_player.setTeamBuffer(teamBuffer);
						}
						
						//将气势信息加载进来
						List<Title> titleList = DZM.getEntityManager().limitQuery("from Title where playerid = ?", 0, 1, playerId);
						if (titleList.size() == 1) {
							Title title = titleList.get(0);
							other_player.setTitle(title);
						}
						
						//将阵容信息加载到player对象中
						int limit = BattleUtil.getBattleCountByLevel(other_player.getLevel());
						List<Battle> battles = DZM.getEntityManager().limitQuery("from Battle where playerid = ?", 0, 8, playerId);
						HashMap<Integer, Battle> userBattles = new HashMap<Integer, Battle>();
						ArrayList<Battle> positionBattles = new ArrayList<Battle>();
						if (battles.size() > 0) {
							for (int i = 0; i < battles.size(); i++) {
								userBattles.put(battles.get(i).getId(), battles.get(i));
							}
						}
						String ids = other_player.getBattleids();
						String[] idsArray = ids.split(",");
						for (int i = 0; i < limit; i++) {
							int battleid = Integer.parseInt(idsArray[i]);
							positionBattles.add(userBattles.get(battleid));
						}
						other_player.setBattlearray(userBattles);
						BattleArrayObject battleArrayObject = new BattleArrayObject();
						battleArrayObject.setPlayerid(playerId);
						battleArrayObject.setMomentum(other_player.getMomentum());
						battleArrayObject.setBattlearrays(positionBattles);
						ObjectAccessor.addBattleArray(battleArrayObject);
						ObjectAccessor.addOnlineUser(other_player);
					}
					else {
						OPErrorReporter.Builder errorBuilder = OPErrorReporter.newBuilder();
						errorBuilder.setErrorid(OP.code.OPCODE_UPDATE_USERINFO_BY_GM_RET_S_VALUE);
						errorBuilder.setErrormsg("player does not exist");
						self_player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errorBuilder);
						return;
					}
				}
			}
			else {
				OPErrorReporter.Builder errorBuilder = OPErrorReporter.newBuilder();
				errorBuilder.setErrorid(OP.code.OPCODE_UPDATE_USERINFO_BY_GM_RET_S_VALUE);
				errorBuilder.setErrormsg("player does not exist");
				self_player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errorBuilder);
				return;
			}
		}
		int amount = 0;
		int dis_id = 0;
		// 清除玩家的阵容信息
		if ("/clearBattle".equals(messages[0])) {
			int playerId = other_player.getId();
			//内存数据落地
			if (other_player.getIsDirty()) {
				DataLander.landOnePlayer(other_player);
			}
			ObjectAccessor.delOnlineUser(playerId);
			
			Session sqlSession=DZM.getEntityManager().getSessionFactory();
	        Transaction tx = sqlSession.beginTransaction( ) ;
	        //清楚阵容上的一切
			String sql = "update battle set discipleid=0,itemid=0,level=0,attack=0,defence=0,health=0,rp=0,defaultSkill=0,defaultSkillItem=0,defaultSkillType=0,defaultSkillQuantity=0,skill1=0,skill1item=0,skill1Type=0,skill1Quantity=0,skill2=0,skill2item=0,skill2Type=0,skill2Quantity=0,equip1=0,equip1item=0,equip2=0,equip2item=0,equip3=0,equip3item=0,fates='',fatesstatus='' where playerid=" + String.valueOf(playerId);
			SQLQuery query = sqlSession.createSQLQuery(sql) ;
			query.executeUpdate();
			sqlSession.clear();
            tx.commit() ;
            
            //清除所有装备的阵容属性
            sql = "update equip set battleid=0 where playerid=" + String.valueOf(playerId);
            query = sqlSession.createSQLQuery(sql) ;
			query.executeUpdate();
			sqlSession.clear();
            tx.commit() ;
            
            //清除所有技能的阵容属性
            sql = "update skill set battleid=0 where playerid=" + String.valueOf(playerId);
            query = sqlSession.createSQLQuery(sql) ;
			query.executeUpdate();
			sqlSession.clear();
            tx.commit() ;
            
            //清除所有弟子的阵容属性
            sql = "update disciple set battleid=0 where playerid=" + String.valueOf(playerId);
            query = sqlSession.createSQLQuery(sql) ;
			query.executeUpdate();
			sqlSession.clear();
            tx.commit() ;
		}
		// 判断修复所有联盟图腾
		else if ("/repairAllTotem".equals(messages[0])) {
			Iterator<LeaguaObject> it = LeaguaServices.allLeaguaObject.values().iterator();
			while(it.hasNext()){
				LeaguaObject p = it.next();
				Leagua leagua = p.getLeaguaInfo();
				JSONObject totemsInfo = leagua.getLeaguaTotemInfoInJson();
				Iterator<String> keys = totemsInfo.keys();
				while (keys.hasNext()) {
					String key = keys.next();
					JSONObject totemInfo = totemsInfo.getJSONObject(key);
					totemInfo.put("lh", totemInfo.get("mh"));
					if (!totemInfo.containsKey("rt")) {
						totemInfo.remove("rt");
					}
					totemsInfo.put(key, totemInfo);
				}
				leagua.setLeaguaTotemInfoFromJson(totemsInfo);
				p.updateLeaguaInfo(leagua);
			}
		}
		// 判断/add操作
		else if ("/add".equals(messages[0])) 
		{
			if(messages.length<3)
			{
				OPErrorReporter.Builder errorBuilder = OPErrorReporter.newBuilder();
				errorBuilder.setErrorid(OP.code.OPCODE_UPDATE_USERINFO_BY_GM_RET_S_VALUE);
				errorBuilder.setErrormsg("Message Error");
				self_player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errorBuilder);
				return;
			}
			// 判断add金币
			if ("gold".equals(messages[1])) 
			{
				amount=parseInt(messages[2]);
				if(amount<=0)
				{
					OPErrorReporter.Builder errorBuilder = OPErrorReporter.newBuilder();
					errorBuilder.setErrorid(OP.code.OPCODE_UPDATE_USERINFO_BY_GM_RET_S_VALUE);
					errorBuilder.setErrormsg("the num must larger than 0");
					self_player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errorBuilder);
					return;
				}
				if (self_flag) 
				{
					self_player.setRechargegoldcoins(self_player.getRechargegoldcoins()+amount);
					self_player.setRechargenum(self_player.getRechargenum()+amount);
					int vipLevel = RechargeHandler.getVipLevelByNum(self_player.getRechargenum());
					if (vipLevel > self_player.getViplevel()) {
						self_player.setViplevel(vipLevel);
					}
					int	goldcoins=self_player.getGoldCoins();
					TitleUtil.checkNewTitle(self_player, 2, vipLevel);
					builder.setGoldcoins(goldcoins);
					builder.setViplevel(vipLevel);
				} 
				else 
				{
					other_player.setRechargegoldcoins(other_player.getRechargegoldcoins()+amount);
					other_player.setRechargenum(other_player.getRechargenum()+amount/10);
					int vipLevel = RechargeHandler.getVipLevelByNum(other_player.getRechargenum());
					if (vipLevel > other_player.getViplevel()) {
						other_player.setViplevel(vipLevel);
					}
					TitleUtil.checkNewTitle(other_player, 2, vipLevel);
				}
			}
			// 判断add银币
			else if ("coin".equals(messages[1])) 
			{
				amount=parseInt(messages[2]);
				if (self_flag) 
				{
					self_player.setSilvercoins(self_player.getSilvercoins()+amount);
					long silvercoins=self_player.getSilvercoins();
					//如果GM添加的银两为负值,并导致银两小于等于0,则将银两数设为0
					if(silvercoins<=0)
					{
						self_player.setSilvercoins(0);
						silvercoins=0;
					}
					builder.setSilvercoins(silvercoins);
				} 
				else 
				{
					other_player.setSilvercoins(other_player.getSilvercoins()+amount);
					long silvercoins=other_player.getSilvercoins();
					if(silvercoins<=0)
					{
						other_player.setSilvercoins(0);
						silvercoins=0;
					}
				}
			}
			// 判断add体力
			else if ("power".equals(messages[1])) 
			{
				amount=parseInt(messages[2]);
				if (self_flag) 
				{
					self_player.increasePowerBySys(amount);
					int power = self_player.getPowerbychicken()+self_player.getPowerbytime();
					//如果GM添加的power为负值,并导致总power小于等于0,则将power设为0
					if(power<=0)
					{
						self_player.setPowerbychicken(0);
						self_player.setPowerbytime(0);
						power=0;
					}
					builder.setPower(power);
				} 
				else 
				{
					other_player.increasePowerBySys(amount);
					int power = other_player.getPowerbychicken()+other_player.getPowerbytime();
					if(power<=0)
					{
						other_player.setPowerbychicken(0);
						other_player.setPowerbytime(0);
						power=0;
					}
				}
			}
			// 判断add精力
			else if ("vitality".equals(messages[1])) 
			{
				amount=parseInt(messages[2]);
				if (self_flag) 
				{
					self_player.increaseVitalityBySys(amount);
					int vitality=self_player.getVitalitybypellet()+self_player.getVitalitybytime();
					//如果GM添加的vitality为负值,并导致总vitality小于等于0,则将vitality设为0
					if(vitality<=0)
					{
						self_player.setVitalitybypellet(0);
						self_player.setVitalitybytime(0);
						vitality=0;
					}
					builder.setVitality(vitality);
				} 
				else 
				{
					other_player.increaseVitalityBySys(amount);
					int vitality=other_player.getVitalitybypellet()+other_player.getVitalitybytime();
					if(vitality<=0)
					{
						other_player.setVitalitybypellet(0);
						other_player.setVitalitybytime(0);
						vitality=0;
					}
				}
			}
			// 判断add 玩家经验 
			else if("exp".equals(messages[1]))
			{
				amount = parseInt(messages[2]);
				if(self_flag)
				{
					Util.getUserLevelByExp(self_player, amount);
					builder.setExp(self_player.getExp());
					builder.setLevel(self_player.getLevel());
				}
				else 
				{
					Util.getUserLevelByExp(other_player, amount);
				}
			}
			// 判断add 全部上阵弟子经验
			else if("team".equals(messages[1]))
			{
				if(messages.length>=4)
				{
					amount = parseInt(messages[3]);
				}
				else
				{
					OPErrorReporter.Builder errorBuilder = OPErrorReporter.newBuilder();
					errorBuilder.setErrorid(OP.code.OPCODE_UPDATE_USERINFO_BY_GM_RET_S_VALUE);
					errorBuilder.setErrormsg("Message Error");
					self_player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errorBuilder);
					return;
				}
				if(self_flag)
				{
					addAllDiscipleExp(self_player, amount ,builder);
				}
				else 
				{
					Iterator<Entry<Integer, Battle>> iterator = other_player.getBattlearray().entrySet().iterator();
					int index=0;
					while (iterator.hasNext()) 
					{
						if(index>8)
						{
							break;
						}
						Entry<Integer, Battle> entry = iterator.next();
						Disciple disciple = other_player.getDisciples().get(entry.getValue().getDiscipleid());
						if(disciple==null)
						{
							continue;
						}
						Util.getlevelByExp(disciple, amount);
					}
				}
			}
			// 判断add 阵容中某个弟子经验值
			else if("member".equals(messages[1]))
			{
				if(messages.length<5)
				{
					OPErrorReporter.Builder errorBuilder = OPErrorReporter.newBuilder();
					errorBuilder.setErrorid(OP.code.OPCODE_UPDATE_USERINFO_BY_GM_RET_S_VALUE);
					errorBuilder.setErrormsg("Message Error");
					self_player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errorBuilder);
					return;
				}
				dis_id = parseInt(messages[2]);
				amount = parseInt(messages[4]);
				//如果修改自己数据
				if(self_flag)
				{
					Disciple disciple = findDiscipleByID(self_player, dis_id);
					if (disciple != null) 
					{
						int addLevel=Util.getlevelByExp(disciple, amount);
						Battle battle=findBattleByID(self_player, dis_id);
						if(addLevel>0){
							BattleUtil.resetBattleProperty(battle, disciple,self_player,null,0);
							self_player.updateOneBattle(battle);
						}
						OPUserDiscipleInfoRet.DiscipleInfo.Builder discipleBuilder = OPUserDiscipleInfoRet.DiscipleInfo.newBuilder();
						initDiscipleBuilder(disciple, discipleBuilder);
						builder.addDiscipleInfo(discipleBuilder);
					} 
					else 
					{
						//阵容中没有该弟子
						OPErrorReporter.Builder errorBuilder = OPErrorReporter.newBuilder();
						errorBuilder.setErrorid(OP.code.OPCODE_UPDATE_USERINFO_BY_GM_RET_S_VALUE);
						errorBuilder.setErrormsg("Disciple Cannot Find");
						self_player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errorBuilder);
						return;
					}
				}
				else 
				{
					Disciple disciple = findDiscipleByID(other_player, dis_id);
					if (disciple != null) 
					{
						Util.getlevelByExp(disciple, amount);
//						OPUserDiscipleInfoRet.DiscipleInfo.Builder discipleBuilder = OPUserDiscipleInfoRet.DiscipleInfo.newBuilder();
//						initDiscipleBuilder(disciple, discipleBuilder);
//						builder.addDiscipleInfo(discipleBuilder);
					} 
					else 
					{
						// 阵容中没有该弟子
						OPErrorReporter.Builder errorBuilder = OPErrorReporter.newBuilder();
						errorBuilder.setErrorid(OP.code.OPCODE_UPDATE_USERINFO_BY_GM_RET_S_VALUE);
						errorBuilder.setErrormsg("Disciple Cannot Find");
						self_player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errorBuilder);
						return;
					}
				}
			}
		}
		// 判断/to操作
		else if ("/to".equals(messages[0])) 
		{
			if(messages.length<3)
			{
				OPErrorReporter.Builder errorBuilder = OPErrorReporter.newBuilder();
				errorBuilder.setErrorid(OP.code.OPCODE_UPDATE_USERINFO_BY_GM_RET_S_VALUE);
				errorBuilder.setErrormsg("Message Error");
				self_player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errorBuilder);
				return;
			}
			// 设置玩家到某个等级
			if ("lvl".equals(messages[1])) 
			{
				amount = parseInt(messages[2]);
				if(amount <= self_player.getLevel())
				{
					OPErrorReporter.Builder errorBuilder = OPErrorReporter.newBuilder();
					errorBuilder.setErrorid(OP.code.OPCODE_UPDATE_USERINFO_BY_GM_RET_S_VALUE);
					errorBuilder.setErrormsg("num must larger than current_level");
					self_player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errorBuilder);
					return;
				}
				else if(amount>XMLTemplateService.getUserLevelCount())
				{
					OPErrorReporter.Builder errorBuilder = OPErrorReporter.newBuilder();
					errorBuilder.setErrorid(OP.code.OPCODE_UPDATE_USERINFO_BY_GM_RET_S_VALUE);
					errorBuilder.setErrormsg("you are already the top level");
					self_player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errorBuilder);
					return;
				}
				else
				{
					int addExp =getUserAddExp(amount,self_player);
					Util.getUserLevelByExp(self_player, addExp);
					builder.setExp(self_player.getExp());
					builder.setLevel(self_player.getLevel());
				}
			}
			// 全部参战弟子达到某个等级
			else if ("team".equals(messages[1])) 
			{
				if(messages.length==4)
				{
					amount = parseInt(messages[3]);
				}
				else
				{
					OPErrorReporter.Builder errorBuilder = OPErrorReporter.newBuilder();
					errorBuilder.setErrorid(OP.code.OPCODE_UPDATE_USERINFO_BY_GM_RET_S_VALUE);
					errorBuilder.setErrormsg("Message Error");
					self_player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errorBuilder);
					return;
				}
				amount = parseInt(messages[3]);
				if (amount > 0) 
				{
					setDiscipleLevelInBattle(amount, self_player,builder);
				}
			}
			// 阵容中{num1}弟子达到{num2}等级
			else if ("member".equals(messages[1])) 
			{
				if(messages.length!=5)
				{
					OPErrorReporter.Builder errorBuilder = OPErrorReporter.newBuilder();
					errorBuilder.setErrorid(OP.code.OPCODE_UPDATE_USERINFO_BY_GM_RET_S_VALUE);
					errorBuilder.setErrormsg("Message Error");
					self_player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errorBuilder);
					return;
				}
				dis_id = parseInt(messages[2]);
				amount = parseInt(messages[4]);
				Disciple disciple = findDiscipleByID(self_player, dis_id);
				if (disciple != null) 
				{
					if(amount<=disciple.getLevel())
					{
						OPErrorReporter.Builder errorBuilder = OPErrorReporter.newBuilder();
						errorBuilder.setErrorid(OP.code.OPCODE_UPDATE_USERINFO_BY_GM_RET_S_VALUE);
						errorBuilder.setErrormsg("num must larger than current_level");
						self_player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errorBuilder);
						return;
					}
					int addExp=getAddExp(amount, disciple);
					int addLevel= Util.getlevelByExp(disciple, addExp);
					Battle battle=findBattleByID(self_player, dis_id);
					if (addLevel > 0) {//弟子升级
						BattleUtil.resetBattleProperty(battle, disciple,self_player,null,0);
						self_player.updateOneBattle(battle);
					}
					OPUserDiscipleInfoRet.DiscipleInfo.Builder discipleBuilder = OPUserDiscipleInfoRet.DiscipleInfo.newBuilder();
					initDiscipleBuilder(disciple, discipleBuilder);
					builder.addDiscipleInfo(discipleBuilder);
				} 
				else 
				{
					//阵容中没有该弟子
					OPErrorReporter.Builder errorBuilder = OPErrorReporter.newBuilder();
					errorBuilder.setErrorid(OP.code.OPCODE_UPDATE_USERINFO_BY_GM_RET_S_VALUE);
					errorBuilder.setErrormsg("Disciple Cannot Find");
					self_player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errorBuilder);
					return;
				}
			}
		}
		//判断/gamble操作
		else if("/gamble".equals(messages[0]))
		{
			//删除信息
			if("clear".equals(messages[1]))
			{
				if(messages.length!=3)
				{
					OPErrorReporter.Builder errorBuilder = OPErrorReporter.newBuilder();
					errorBuilder.setErrorid(OP.code.OPCODE_UPDATE_USERINFO_BY_GM_RET_S_VALUE);
					errorBuilder.setErrormsg("Message Error");
					self_player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errorBuilder);
					return;
				}
				else
				{
					String puid=messages[2];
					ObjectAccessor.getMapping(puid);
					Mapping mapping = ObjectAccessor.getMapping(puid);
					if (mapping == null) {
						OPErrorReporter.Builder errorBuilder = OPErrorReporter.newBuilder();
						errorBuilder.setErrorid(OP.code.OPCODE_UPDATE_USERINFO_BY_GM_RET_S_VALUE);
						errorBuilder.setErrormsg("no player info in mapping");
						self_player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errorBuilder);
						return;
					}
					else
					{
						int playerId = mapping.getGameid();
						ObjectAccessor.delBattleArray(playerId);
						ObjectAccessor.delMapping(puid);
						ObjectAccessor.delOnlineUser(playerId);
					}
				}
			}
			else if("delete".equals(messages[1]))
			{
				if(messages.length!=4)
				{
					OPErrorReporter.Builder errorBuilder = OPErrorReporter.newBuilder();
					errorBuilder.setErrorid(OP.code.OPCODE_UPDATE_USERINFO_BY_GM_RET_S_VALUE);
					errorBuilder.setErrormsg("Message Error");
					self_player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errorBuilder);
					return;
				}
				else
				{
					String puid=messages[3];
					ObjectAccessor.getMapping(puid);
					Mapping mapping = ObjectAccessor.getMapping(puid);
					if (mapping == null) {
						OPErrorReporter.Builder errorBuilder = OPErrorReporter.newBuilder();
						errorBuilder.setErrorid(OP.code.OPCODE_UPDATE_USERINFO_BY_GM_RET_S_VALUE);
						errorBuilder.setErrormsg("no player info in mapping");
						self_player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errorBuilder);
						return;
					}
					else
					{
						int playerId = mapping.getGameid();
						ObjectAccessor.delBattleArray(playerId);
						ObjectAccessor.delMapping(puid);
						ObjectAccessor.delOnlineUser(playerId);
					}
				
					int other_playerId=other_player.getId();
					EntityManagerImpl entityManager=(EntityManagerImpl) DZM.getEntityManager();
					Session session2=entityManager.getSession();
					Transaction tr=session2.beginTransaction();
					try
					{
						SQLQuery query1=session2.createSQLQuery(" delete from adventurefightrank where playerId="+other_player.getId());
						SQLQuery query2=session2.createSQLQuery(" delete from adventurefightstats where playerId="+other_player.getId());
						SQLQuery query3=session2.createSQLQuery(" delete from bag where playerId="+other_player.getId());
						SQLQuery query4=session2.createSQLQuery(" delete from battle where playerId="+other_player.getId());
						SQLQuery query5=session2.createSQLQuery(" delete from career where playerId="+other_player.getId());
						SQLQuery query6=session2.createSQLQuery(" delete from challenge where playerId="+other_player.getId());
						SQLQuery query7=session2.createSQLQuery(" delete from disciple where playerId="+other_player.getId());
						SQLQuery query8=session2.createSQLQuery(" delete from equip where playerId="+other_player.getId());
						SQLQuery query9=session2.createSQLQuery(" delete from mapping where puid='"+puid+"'");
						SQLQuery query10=session2.createSQLQuery(" delete from moment where playerId="+other_player.getId());
						SQLQuery query11=session2.createSQLQuery(" delete from pay where playerId="+other_player.getId());
						SQLQuery query13=session2.createSQLQuery(" delete from rewards where playerId="+other_player.getId());
						SQLQuery query14=session2.createSQLQuery(" delete from skill where playerId="+other_player.getId());
						SQLQuery query15=session2.createSQLQuery(" delete from soul where playerId="+other_player.getId());
						SQLQuery query16=session2.createSQLQuery(" delete from stats where playerId="+other_player.getId());
						SQLQuery query17=session2.createSQLQuery(" delete from teambuffer where playerId="+other_player.getId());
						SQLQuery query18=session2.createSQLQuery(" delete from title where playerId="+other_player.getId());
						SQLQuery query19=session2.createSQLQuery(" delete from wpayopr201304 where playerId="+other_player.getId());
						SQLQuery query12=session2.createSQLQuery(" delete from player where id="+other_player.getId());
						SQLQuery query20=session2.createSQLQuery(" delete from invitestatus where playerId="+other_player.getId());
						SQLQuery query21=session2.createSQLQuery(" delete from skillbook where playerId="+other_player.getId());
						query1.executeUpdate();
						query2.executeUpdate();
						query3.executeUpdate();
						query4.executeUpdate();
						query5.executeUpdate();
						query6.executeUpdate();
						query7.executeUpdate();
						query8.executeUpdate();
						query9.executeUpdate();
						query10.executeUpdate();
						query11.executeUpdate();
						query12.executeUpdate();
						query13.executeUpdate();
						query14.executeUpdate();
						query15.executeUpdate();
						query16.executeUpdate();
						query17.executeUpdate();
						query18.executeUpdate();
						query19.executeUpdate();
						query20.executeUpdate();
						query21.executeUpdate();
						tr.commit();
					}
					catch (Exception e) {
						tr.rollback();
						OPErrorReporter.Builder errorBuilder = OPErrorReporter.newBuilder();
						errorBuilder.setErrorid(OP.code.OPCODE_UPDATE_USERINFO_BY_GM_RET_S_VALUE);
						errorBuilder.setErrormsg("delete player failed");
						self_player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errorBuilder);
						return;
					}
				}
			}
			else
			{
				if(messages.length!=4)
				{
					OPErrorReporter.Builder errorBuilder = OPErrorReporter.newBuilder();
					errorBuilder.setErrorid(OP.code.OPCODE_UPDATE_USERINFO_BY_GM_RET_S_VALUE);
					errorBuilder.setErrormsg("Message Error");
					self_player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errorBuilder);
					return;
				}
				int typeid=parseInt(messages[1]);
				int count=parseInt(messages[3]);
				
				// 判断开宝箱操作
				if("box".equals(messages[2]))
				{
					for(int i=0 ; i<count ; i++)
					{
						if(typeid!=1 && typeid!=2 && typeid!=3 && typeid!=4 )
						{
							OPErrorReporter.Builder errorBuilder = OPErrorReporter.newBuilder();
							errorBuilder.setErrorid(OP.code.OPCODE_UPDATE_USERINFO_BY_GM_RET_S_VALUE);
							errorBuilder.setErrormsg("Message Error");
							self_player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errorBuilder);
							return;
						}
							openTreasure(self_player, typeid, builder);
					}
				}
				//判断抽弟子操作
				else if("hero".equals(messages[2]))
				{
					if(typeid!=1 && typeid!=2 && typeid!=3)
					{
						OPErrorReporter.Builder errorBuilder = OPErrorReporter.newBuilder();
						errorBuilder.setErrorid(OP.code.OPCODE_UPDATE_USERINFO_BY_GM_RET_S_VALUE);
						errorBuilder.setErrormsg("Message Error");
						self_player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errorBuilder);
						return;
					}
					for(int i=0 ; i<count ; i++)
					{
						getDisciple(self_player, typeid, builder);
					}
				}
			}
		}
		//判断/tomorrow操作
		else if("/tomorrow".endsWith(messages[0]))
		{
			//判断向后穿越{num}天
			if(messages.length==2)
			{
				int num=parseInt(messages[1]);
				//TODO 设置穿越
			}
			//以每天都登陆的方式向后穿越{num}天
			else if(messages.length==3)
			{
				int num=parseInt(messages[2]);
				//TODO 设置每天都穿越
			}
			else
			{
				OPErrorReporter.Builder errorBuilder = OPErrorReporter.newBuilder();
				errorBuilder.setErrorid(OP.code.OPCODE_UPDATE_USERINFO_BY_GM_RET_S_VALUE);
				errorBuilder.setErrormsg("Message Error");
				self_player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errorBuilder);
				return;
			}
		}
		//判断/additem 操作
		else if ("/additem".equals(messages[0]))
		{
			if(messages.length<3)
			{
				OPErrorReporter.Builder errorBuilder = OPErrorReporter.newBuilder();
				errorBuilder.setErrorid(OP.code.OPCODE_UPDATE_USERINFO_BY_GM_RET_S_VALUE);
				errorBuilder.setErrormsg("Message Error");
				self_player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errorBuilder);
				return;
			}
			int id=parseInt(messages[2]);
			int id2=0;
			//判断是否批量操作
			boolean batch=false;
			if("to".equals(messages[3]))
			{
				batch=true;
				id2=parseInt(messages[4]);
				if(id2<id)
				{
					OPErrorReporter.Builder errorBuilder = OPErrorReporter.newBuilder();
					errorBuilder.setErrorid(OP.code.OPCODE_UPDATE_USERINFO_BY_GM_RET_S_VALUE);
					errorBuilder.setErrormsg("id");
					self_player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errorBuilder);
					return;
				}
			}
			if("tool".equals(messages[1]))
			{
				if (messages.length != 4) 
				{
					OPErrorReporter.Builder errorBuilder = OPErrorReporter.newBuilder();
					errorBuilder.setErrorid(OP.code.OPCODE_UPDATE_USERINFO_BY_GM_RET_S_VALUE);
					errorBuilder.setErrormsg("Message Error");
					self_player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE,errorBuilder);
					return;
				}
				else 
				{
					amount = parseInt(messages[3]);
					if (amount <= 0) {
						OPErrorReporter.Builder errorBuilder = OPErrorReporter.newBuilder();
						errorBuilder.setErrorid(OP.code.OPCODE_UPDATE_USERINFO_BY_GM_RET_S_VALUE);
						errorBuilder.setErrormsg("num must larger than 0");
						self_player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errorBuilder);
						return;
					} 
					else 
					{
						if (XMLTemplateService.getToolTemplate(id) == null) 
						{
							OPErrorReporter.Builder errorBuilder = OPErrorReporter.newBuilder();
							errorBuilder.setErrorid(OP.code.OPCODE_UPDATE_USERINFO_BY_GM_RET_S_VALUE);
							errorBuilder.setErrormsg("tool does not exist");
							self_player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE,errorBuilder);
							return;
						}
						builder.addToolInfo(BagToolServices.addBagTool(self_player, id, amount));
					}
				}
			}
			else if("hero".equals(messages[1]))
			{
				if(batch)
				{
					if(messages.length!=5)
					{
						OPErrorReporter.Builder errorBuilder = OPErrorReporter.newBuilder();
						errorBuilder.setErrorid(OP.code.OPCODE_UPDATE_USERINFO_BY_GM_RET_S_VALUE);
						errorBuilder.setErrormsg("Message Error");
						self_player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errorBuilder);
						return;
					}
					else
					{
						if(XMLTemplateService.getDiscipleTemplate(id)==null || XMLTemplateService.getDiscipleTemplate(id2)==null)
						{
							OPErrorReporter.Builder errorBuilder = OPErrorReporter.newBuilder();
							errorBuilder.setErrorid(OP.code.OPCODE_UPDATE_USERINFO_BY_GM_RET_S_VALUE);
							errorBuilder.setErrormsg("id is wrong");
							self_player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errorBuilder);
							return;
						}
						List<Integer> discipleitems =self_player.getDiscipleitems();
						for(int i=id ; i<=id2 ; i++)
						{
							if(discipleitems!=null && discipleitems.size()>0)
							{
								boolean has_flag=false;
								for(Integer item : discipleitems)
								{
									if(item==i){
										has_flag=true;
										break;
									}
								}
								if(has_flag)
								{
									continue;
								}
							}
							Disciple disciple=DiscipleUtil.newDisciple(self_player, i, 1, 0);
							Skill skill = self_player.getSkills().get(disciple.getSkill1());
							OPUserSkillInfoRet.SkillInfo.Builder skillBuilder = SkillUtil.buildSkillBuilder(skill);
							builder.addSkillInfo(skillBuilder);
							OPUserDiscipleInfoRet.DiscipleInfo.Builder discipleBuilder=DiscipleUtil.buildDiscipleBuilder(disciple);
							builder.addDiscipleInfo(discipleBuilder);
						}
					}
				}
				else
				{
					if(XMLTemplateService.getDiscipleTemplate(id)==null)
					{
						OPErrorReporter.Builder errorBuilder = OPErrorReporter.newBuilder();
						errorBuilder.setErrorid(OP.code.OPCODE_UPDATE_USERINFO_BY_GM_RET_S_VALUE);
						errorBuilder.setErrormsg("id is wrong");
						self_player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errorBuilder);
						return;
					}
					List<Integer> discipleitems =self_player.getDiscipleitems();
					if(discipleitems!=null && discipleitems.size()>0)
					{
						for(Integer item : discipleitems)
						{
							if(item==id){
								OPErrorReporter.Builder errorBuilder = OPErrorReporter.newBuilder();
								errorBuilder.setErrorid(OP.code.OPCODE_UPDATE_USERINFO_BY_GM_RET_S_VALUE);
								errorBuilder.setErrormsg("you have already got this disciple");
								self_player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errorBuilder);
								return;
							}
						}
					}
					Disciple disciple=DiscipleUtil.newDisciple(self_player, id, 1, 0);
					Skill skill = self_player.getSkills().get(disciple.getSkill1());
					OPUserSkillInfoRet.SkillInfo.Builder skillBuilder = SkillUtil.buildSkillBuilder(skill);
					builder.addSkillInfo(skillBuilder);
					OPUserDiscipleInfoRet.DiscipleInfo.Builder discipleBuilder=DiscipleUtil.buildDiscipleBuilder(disciple);
					builder.addDiscipleInfo(discipleBuilder);
				}
			}
			else if("soul".equals(messages[1]))
			{
				if(batch)
				{
					if(messages.length!=6)
					{
						OPErrorReporter.Builder errorBuilder = OPErrorReporter.newBuilder();
						errorBuilder.setErrorid(OP.code.OPCODE_UPDATE_USERINFO_BY_GM_RET_S_VALUE);
						errorBuilder.setErrormsg("Message Error");
						self_player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errorBuilder);
						return;
					}
					if(XMLTemplateService.getDiscipleTemplate(id)==null || XMLTemplateService.getDiscipleTemplate(id2)==null)
					{
						OPErrorReporter.Builder errorBuilder = OPErrorReporter.newBuilder();
						errorBuilder.setErrorid(OP.code.OPCODE_UPDATE_USERINFO_BY_GM_RET_S_VALUE);
						errorBuilder.setErrormsg("id is wrong");
						self_player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errorBuilder);
						return;
					}
					else
					{
						amount=parseInt(messages[5]);
						if(amount<=0)
						{
							OPErrorReporter.Builder errorBuilder = OPErrorReporter.newBuilder();
							errorBuilder.setErrorid(OP.code.OPCODE_UPDATE_USERINFO_BY_GM_RET_S_VALUE);
							errorBuilder.setErrormsg("num must larger than 0");
							self_player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errorBuilder);
							return;
						}
						HashMap<Integer, Soul> userSouls = self_player.getSouls();
						for(int i=id ; i<=id2 ; i++ )
						{
							Soul soul = new Soul();
							if (userSouls.containsKey(i)) {
								soul = userSouls.get(i);
								soul.setCount(soul.getCount() + amount);
								self_player.updateOneSoul(soul);
							}
							else {
								soul.setItemid(i);
								soul.setPlayerid(self_player.getId());
								soul.setCount(amount);
								self_player.addOneSoul(soul);
							}
							OPUserSoulInfoRet.SoulInfo.Builder soulBuilder=SoulUtil.buildSoulBuilder(soul);
							builder.addSoulInfo(soulBuilder);
						}
						
					}
				}
				else
				{
					if(messages.length!=4)
					{
						OPErrorReporter.Builder errorBuilder = OPErrorReporter.newBuilder();
						errorBuilder.setErrorid(OP.code.OPCODE_UPDATE_USERINFO_BY_GM_RET_S_VALUE);
						errorBuilder.setErrormsg("Message Error");
						self_player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errorBuilder);
						return;
					}
					if(XMLTemplateService.getDiscipleTemplate(id)==null)
					{
						OPErrorReporter.Builder errorBuilder = OPErrorReporter.newBuilder();
						errorBuilder.setErrorid(OP.code.OPCODE_UPDATE_USERINFO_BY_GM_RET_S_VALUE);
						errorBuilder.setErrormsg("id is wrong");
						self_player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errorBuilder);
						return;
					}
					else
					{
						amount=parseInt(messages[3]);
						if(amount<=0)
						{
							OPErrorReporter.Builder errorBuilder = OPErrorReporter.newBuilder();
							errorBuilder.setErrorid(OP.code.OPCODE_UPDATE_USERINFO_BY_GM_RET_S_VALUE);
							errorBuilder.setErrormsg("num must larger than 0");
							self_player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errorBuilder);
							return;
						}
						HashMap<Integer, Soul> userSouls = self_player.getSouls();
						Soul soul = new Soul();
						if (userSouls.containsKey(id)) {
							soul = userSouls.get(id);
							soul.setCount(soul.getCount() + amount);
							self_player.updateOneSoul(soul);
						}
						else {
							soul.setItemid(id);
							soul.setPlayerid(self_player.getId());
							soul.setCount(amount);
							self_player.addOneSoul(soul);
						}
						OPUserSoulInfoRet.SoulInfo.Builder soulBuilder=SoulUtil.buildSoulBuilder(soul);
						builder.addSoulInfo(soulBuilder);
					}
				}
			}
			else if("skillbook".equals(messages[1]))
			{
				if(batch)
				{
					if(messages.length!=6)
					{
						OPErrorReporter.Builder errorBuilder = OPErrorReporter.newBuilder();
						errorBuilder.setErrorid(OP.code.OPCODE_UPDATE_USERINFO_BY_GM_RET_S_VALUE);
						errorBuilder.setErrormsg("Message Error");
						self_player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errorBuilder);
						return;
					}
					else
					{
						amount=parseInt(messages[5]);
						if(amount<=0)
						{
							OPErrorReporter.Builder errorBuilder = OPErrorReporter.newBuilder();
							errorBuilder.setErrorid(OP.code.OPCODE_UPDATE_USERINFO_BY_GM_RET_S_VALUE);
							errorBuilder.setErrormsg("num must larger than 0");
							self_player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errorBuilder);
							return;
						}
						else
						{
							if( XMLTemplateService.getSkillTemplate(id)==null || XMLTemplateService.getSkillTemplate(id)==null){
								OPErrorReporter.Builder errorBuilder = OPErrorReporter.newBuilder();
								errorBuilder.setErrorid(OP.code.OPCODE_UPDATE_USERINFO_BY_GM_RET_S_VALUE);
								errorBuilder.setErrormsg("id is wrong");
								self_player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errorBuilder);
								return;
							}
							for(int i=id ; i<=id2 ; i++)
							{
								for(int j=0 ; j<amount ; j++)
								{
									builder.addSkillInfo(SkillUtil.buildSkillBuilder(SkillUtil.newSkill(self_player, i, 0)));
								}
							}
						}
					}
				}
				else
				{
					if(messages.length!=4)
					{
						OPErrorReporter.Builder errorBuilder = OPErrorReporter.newBuilder();
						errorBuilder.setErrorid(OP.code.OPCODE_UPDATE_USERINFO_BY_GM_RET_S_VALUE);
						errorBuilder.setErrormsg("Message Error");
						self_player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errorBuilder);
						return;
					}
					else
					{
						amount=parseInt(messages[3]);
						if(amount<=0)
						{
							OPErrorReporter.Builder errorBuilder = OPErrorReporter.newBuilder();
							errorBuilder.setErrorid(OP.code.OPCODE_UPDATE_USERINFO_BY_GM_RET_S_VALUE);
							errorBuilder.setErrormsg("num must larger than 0");
							self_player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errorBuilder);
							return;
						}
						else
						{
							if( XMLTemplateService.getSkillTemplate(id)==null){
								OPErrorReporter.Builder errorBuilder = OPErrorReporter.newBuilder();
								errorBuilder.setErrorid(OP.code.OPCODE_UPDATE_USERINFO_BY_GM_RET_S_VALUE);
								errorBuilder.setErrormsg("id is wrong");
								self_player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errorBuilder);
								return;
							}
							for(int j=0 ; j<amount ; j++)
							{
								builder.addSkillInfo(SkillUtil.buildSkillBuilder(SkillUtil.newSkill(self_player, id, 0)));
							}
						}
					}
				}
				
			}
			else if("equip".equals(messages[1]))
			{
				if(batch)
				{
					if(messages.length!=6)
					{
						OPErrorReporter.Builder errorBuilder = OPErrorReporter.newBuilder();
						errorBuilder.setErrorid(OP.code.OPCODE_UPDATE_USERINFO_BY_GM_RET_S_VALUE);
						errorBuilder.setErrormsg("Message Error");
						self_player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errorBuilder);
						return;
					}
					else
					{
						 amount=parseInt(messages[5]);
						 if(amount<=0){
							OPErrorReporter.Builder errorBuilder = OPErrorReporter.newBuilder();
							errorBuilder.setErrorid(OP.code.OPCODE_UPDATE_USERINFO_BY_GM_RET_S_VALUE);
							errorBuilder.setErrormsg("num must larger than 0");
							self_player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errorBuilder);
							return;
						 }
						 else
						 {
							if( XMLTemplateService.getEquipTemplate(id)==null || XMLTemplateService.getEquipTemplate(id2)==null){
								OPErrorReporter.Builder errorBuilder = OPErrorReporter.newBuilder();
								errorBuilder.setErrorid(OP.code.OPCODE_UPDATE_USERINFO_BY_GM_RET_S_VALUE);
								errorBuilder.setErrormsg("id is wrong");
								self_player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errorBuilder);
								return;
							}
							for(int i=id ; i<=id2 ; i++)
							{
								for(int j=0 ; j<amount ; j++)
								{
									builder.addEquipInfo(EquipUtil.buildReturn(EquipUtil.newEquip(self_player, i)));
								}
							}
							
						}
					}
				}
				else
				{
					if(messages.length!=4)
					{
						OPErrorReporter.Builder errorBuilder = OPErrorReporter.newBuilder();
						errorBuilder.setErrorid(OP.code.OPCODE_UPDATE_USERINFO_BY_GM_RET_S_VALUE);
						errorBuilder.setErrormsg("Message Error");
						self_player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errorBuilder);
						return;
					}
					else
					{
						 amount=parseInt(messages[3]);
						 if(amount<=0){
							OPErrorReporter.Builder errorBuilder = OPErrorReporter.newBuilder();
							errorBuilder.setErrorid(OP.code.OPCODE_UPDATE_USERINFO_BY_GM_RET_S_VALUE);
							errorBuilder.setErrormsg("num must larger than 0");
							self_player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errorBuilder);
							return;
						 }
						 else
						 {
							if( XMLTemplateService.getEquipTemplate(id)==null){
								OPErrorReporter.Builder errorBuilder = OPErrorReporter.newBuilder();
								errorBuilder.setErrorid(OP.code.OPCODE_UPDATE_USERINFO_BY_GM_RET_S_VALUE);
								errorBuilder.setErrormsg("id is wrong");
								self_player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errorBuilder);
								return;
							}
							for(int j=0 ; j<amount ; j++)
							{
								builder.addEquipInfo(EquipUtil.buildReturn(EquipUtil.newEquip(self_player, id)));
							}
						}
					}
				}
				
			}
		}
        //补偿GM命令->只有GM能做此操作
        else if("/compensate".equals(messages[0]))
        {
             if(messages.length<4)
             {
                 OPErrorReporter.Builder errorBuilder = OPErrorReporter.newBuilder();
                 errorBuilder.setErrorid(OP.code.OPCODE_UPDATE_USERINFO_BY_GM_RET_S_VALUE);
                 errorBuilder.setErrormsg("Compensate GM Command Invalid!");
                 self_player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errorBuilder);
                 return;
             }
             else
             {
                 String msg=messages[1];
                 String content=messages[2];
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
                                 self_player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errorBuilder);
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
                             self_player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errorBuilder);
                             return;
                         }
                     }
                     if(rewardContent.size()==0)
                     {
                         status=1;
                     }

                     try
                     {
                         lastLogin=Integer.parseInt(messages[3]);
                     }
                     catch (Exception e)
                     {

                     }
                     if(lastLogin==-1)
                     {
                         OPErrorReporter.Builder errorBuilder = OPErrorReporter.newBuilder();
                         errorBuilder.setErrorid(OP.code.OPCODE_UPDATE_USERINFO_BY_GM_RET_S_VALUE);
                         errorBuilder.setErrormsg("Compensate GM Command Invalid lastLogin Error!");
                         self_player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errorBuilder);
                         return;
                     }
                     if(messages.length>4)
                     {
                         try
                         {
                             playerId=Integer.parseInt(messages[4]);
                         }
                         catch (Exception e)
                         {

                         }
                     }
                     RewardServices.createCompensateReward(msg,rewardInfo,rewardContent,lastLogin,0,0,0,0,status,playerId,"_");
                 }
                 else
                 {
                     OPErrorReporter.Builder errorBuilder = OPErrorReporter.newBuilder();
                     errorBuilder.setErrorid(OP.code.OPCODE_UPDATE_USERINFO_BY_GM_RET_S_VALUE);
                     errorBuilder.setErrormsg("Compensate GM Command Invalid Content Error!");
                     self_player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errorBuilder);
                     return;
                 }
             }
        }
		if(message.contains("player")&&(!other_online)){
			ObjectAccessor.delBattleArray(other_player.getId());
			ObjectAccessor.delMapping(other_player.getPuid());
			ObjectAccessor.delOnlineUser(other_player.getId());
		}
		self_player.sendPacket(OP.code.OPCODE_UPDATE_USERINFO_BY_GM_RET_S_VALUE, builder);// 发送包内容
	}

	private static int getAddExp(int lvlEnd , Disciple disciple)
	{
		//最高级别限制
		int levelCount = XMLTemplateService.getDiscipleLevelCount();
		int itemID = disciple.getItemid(); 
		DiscipleXmlTemplate discipleInfo = XMLTemplateService.getDiscipleTemplate(itemID);
		int defaultExp=discipleInfo.getDefaultExp();
		if(lvlEnd>levelCount){
			lvlEnd=levelCount;
		}
		float multi=XMLTemplateService.getDiscipleLevelConfig(lvlEnd-1);
		int addExp=(int)(defaultExp*multi)-disciple.getExp() ;
		return addExp;
	}
	
	private static int getUserAddExp( int lvlEnd , Player player) 
	{
		int levelCount = XMLTemplateService.getUserLevelCount();
		if(lvlEnd>levelCount){
			lvlEnd=levelCount;
		}
		int addExp=XMLTemplateService.getUserLevelExp(lvlEnd-1)-player.getExp();
		return addExp;
	}

	//增加所有上阵弟子经验
	private static void addAllDiscipleExp(Player player, int amount , OPUpdateUserInfoByGMRet.Builder builder) {
		Iterator<Entry<Integer, Battle>> iterator = player.getBattlearray().entrySet().iterator();
		int i=0;
		while (iterator.hasNext()) 
		{
			Entry<Integer, Battle> entry = iterator.next();
			Battle battle=entry.getValue();
			Disciple disciple = player.getDisciples().get(battle.getDiscipleid());
			if(disciple==null)
			{
				continue;
			}
			int addLevel=Util.getlevelByExp(disciple, amount);
			if (addLevel > 0) {//弟子升级
				BattleUtil.resetBattleProperty(battle, disciple,player,null,0);
				player.updateOneBattle(battle);
			}
			OPUserDiscipleInfoRet.DiscipleInfo.Builder discipleBuilder = OPUserDiscipleInfoRet.DiscipleInfo.newBuilder();
			initDiscipleBuilder(disciple, discipleBuilder);
			builder.addDiscipleInfo(discipleBuilder);
			i++;
		}
	}

	//设定所有参战弟子等级
	private static void setDiscipleLevelInBattle(int amount, Player player , OPUpdateUserInfoByGMRet.Builder builder) 
	{
		Iterator<Map.Entry<Integer, Battle>> iterator = player.getBattlearray().entrySet().iterator();
		int i=0;
		while (iterator.hasNext()) 
		{
			Entry<Integer, Battle> entry = iterator.next();
			Battle battle=entry.getValue();
			Disciple disciple = player.getDisciples().get(battle.getDiscipleid());
			if(disciple==null){
				continue;
			}
			if(amount<=disciple.getLevel())
			{
				OPErrorReporter.Builder errorBuilder = OPErrorReporter.newBuilder();
				errorBuilder.setErrorid(OP.code.OPCODE_UPDATE_USERINFO_BY_GM_RET_S_VALUE);
				errorBuilder.setErrormsg("num must larger than current_level");
				player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errorBuilder);
				return;
			}
			int addExp=getAddExp(amount, disciple);
			int addLevel =Util.getlevelByExp(disciple, addExp);
			if (addLevel > 0) {//弟子升级
				BattleUtil.resetBattleProperty(battle, disciple,player,null,0);
				player.updateOneBattle(battle);
			}
			OPUserDiscipleInfoRet.DiscipleInfo.Builder discipleBuilder = OPUserDiscipleInfoRet.DiscipleInfo.newBuilder();
			initDiscipleBuilder(disciple, discipleBuilder);
			builder.addDiscipleInfo(discipleBuilder);
			i++;
		}
	}

	/**
	 * function: 根据传入的阵容ID 获取参战弟子的信息 params: 1. Player 需要修改的玩家 2. int battle的ID
	 * return: Disciple 参战的弟子
	 * */
	private static Disciple findDiscipleByID(Player player, int index) 
	{
		Battle battle=findBattleByID(player, index);
		HashMap<Integer, Disciple> disciples = player.getDisciples();
		Disciple disciple = disciples.get(battle.getDiscipleid());
		return disciple;
	}
	private static Battle findBattleByID(Player player, int index){
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
		return battle;
	}
	/**
	 * 将接受的{num}转化为int数值
	 * */
	private static int parseInt(String str) {
		return Integer.parseInt(str.trim());
	}
	

	
	/***********************************************抽取宝箱**********************************************/
	private static void openTreasure(Player player,int toolItemId,OPUpdateUserInfoByGMRet.Builder builder)
	{
		int treasureType = getTreasuretype(toolItemId);
		int addLuck = 0;
		switch (treasureType) 
		{
		case UserToolConst.D_QUALITY: // 木
			addLuck = 5;
			break;
		case UserToolConst.C_QUALITY: // 铜
			addLuck = 10;
			break;
		case UserToolConst.B_QUALITY: // 银
			addLuck = 20;
			break;
		case UserToolConst.A_QUALITY: // 金
			addLuck = 30;
			break;
		default:
			break;
		}

		TreasureSelecter selecter = XMLTemplateService.getTreasureSelecter(treasureType);
		List<Float> rateList;
		double randomNum = Math.random();
		int rewardsPoolNo = 1;
		UserStats userStats = player.getUserStats();
        logger2.info("-------------------->TreasureInfo    ReadyGo     :LuckyOne ---{}---,LuckyTwo ---{}---, treasureWorth---{}---,playerLevel---{}---,LuckPool---{}----", new Object[]{userStats.getLuckOne(),userStats.getLuckTwo(),userStats.getTreasureWorth(),player.getLevel(),userStats.getLuckPool()});
        if(userStats.getLuckPool()>0)
        {
            float remainLuck = userStats.getLuckPool()-addLuck;
            if(remainLuck<0)
            {
            	addLuck = (int)StrictMath.ceil(userStats.getLuckPool());
                userStats.setLuckPool(0.0f);
            }else
            {
                userStats.setLuckPool(remainLuck);
            }
        }
        else
        {
            addLuck =  (int)StrictMath.ceil(addLuck*XMLTemplateService.getSysBasicConfig().getEmptyLuckPoolProportion());
        }
		if (userStats.getLuckTwo()>=XMLTemplateService.getSysBasicConfig().getTreasurePoolNeed().get(4)) 
		{
			// 奖池4抽奖
			rateList = selecter.getRate4();
			userStats.setLuckTwo(0);
			rewardsPoolNo = 4;
		} 
		else 
		{
			if (player.getLevel() >= 20) {
				if (userStats.getLuckOne()>=XMLTemplateService.getSysBasicConfig().getTreasurePoolNeed().get(3)) 
				{
					// 奖池3抽奖
					rateList = selecter.getRate3();
					rewardsPoolNo = 3;
				} else {
					// 奖池1抽奖
					rateList = selecter.getRate1();
					rewardsPoolNo = 1;
				}
			} 
			else 
			{
				if (userStats.getLuckOne()>=XMLTemplateService.getSysBasicConfig().getTreasurePoolNeed().get(2)) 
				{
					// 奖池2抽奖
					rateList = selecter.getRate2();
					rewardsPoolNo = 2;
				} else {
					// 奖池1抽奖
					rateList = selecter.getRate1();
					rewardsPoolNo = 1;
				}
			}
		}
		logger2.info("---------------------->RewardPoolNo:----{}----", new Object[]{rewardsPoolNo});
		int listLen = rateList.size();
		int selectedNo = 0;
		for (int i = 0; i < listLen; i++) 
		{
			if (randomNum < rateList.get(i)) 
			{
				selectedNo = i;
				break;
			}
		}
		// 增加或者清空幸运值及累计额度
		if (rewardsPoolNo == 4) 
		{
			userStats.setTreasureWorth(0);
			if (player.getLevel() >= 20) 
			{
				if (userStats.getLuckOne()<XMLTemplateService.getSysBasicConfig().getTreasurePoolNeed().get(3)) 
				{
					userStats.setLuckOne(userStats.getLuckOne() + addLuck);
				}
			} 
			else 
			{
				if (userStats.getLuckOne()<XMLTemplateService.getSysBasicConfig().getTreasurePoolNeed().get(2)) 
				{
					userStats.setLuckOne(userStats.getLuckOne() + addLuck);
				}
			}
		} 
		else 
		{
			if (rewardsPoolNo > 1) 
			{
				int totalWorth = userStats.getTreasureWorth()
						+ selecter.getWorth().get(selectedNo);
				if (player.getLevel() >= 20) 
				{
					if (totalWorth >= XMLTemplateService.getSysBasicConfig().getTreasureClearCount().get(1)) 
					{
						totalWorth = 0;
						userStats.setLuckOne(0);
					}
				} 
				else 
				{
					if (totalWorth >= XMLTemplateService.getSysBasicConfig().getTreasureClearCount().get(0)) 
					{
						totalWorth = 0;
						userStats.setLuckOne(0);
					}
				}
				userStats.setTreasureWorth(totalWorth);
			} 
			else 
			{
				userStats.setLuckOne(userStats.getLuckOne() + addLuck);
			}
			userStats.setLuckTwo(userStats.getLuckTwo() + addLuck);
		}
		player.setUserStats(userStats);
        logger2.info("-------------------->TreasureInfo     After    :LuckyOne ---{}---,LuckyTwo ---{}---, treasureWorth---{}---,playerLevel---{}---,luckPool---{}---", new Object[]{userStats.getLuckOne(),userStats.getLuckTwo(),userStats.getTreasureWorth(),player.getLevel(),userStats.getLuckPool()});
		int propId = selecter.getPropId().get(selectedNo);
		int itemType = selecter.getItemType().get(selectedNo);
		int count = selecter.getCount().get(selectedNo);
		if (propId != 0) { // 直接给予道具和用户属性奖励
			switch (itemType) {
			case UserToolConst.USER_PROPERTY:
				if (propId == UserToolConst.USER_PROPERTY_GOLD_COINS) 
				{
					UserPropertyServices.changeProperty(player, propId, count);
					builder.setGoldcoins(player.getSysgoldcoins() + player.getRechargegoldcoins());
					logger_box.info("Open Box Reward Info: {}, {}, {} ,{}", new Object[]{player.getId() , itemType , propId , count });
				}
				break;
			case UserToolConst.USER_TOOLS:
				builder.addToolInfo(BagToolServices.addBagTool(player, propId, count));
				logger_box.info("Open Box Reward Info: {}, {}, {} ,{}", new Object[]{player.getId() , itemType , propId , count });
				break;
			case UserToolConst.USER_LEVEL_PROPERTY:
				if (propId == UserToolConst.USER_LEVEL_PROPERTY_SILVER_COINS) 
				{
					int lvSilvers = XMLTemplateService.getUserLevelConfig(player.getLevel()).getCoins1();
					int  coin_count=lvSilvers*selecter.getCount().get(selectedNo);
                    UserPropertyServices.changeProperty(player,propId,coin_count);
                    builder.setSilvercoins(player.getSilvercoins());
					logger_box.info("Open Box Reward Info: {}, {}, {} ,{}", new Object[]{player.getId() , itemType , propId , coin_count });
				}
				break;
			default:
				break;
			}
		} 
		else 
		{
			// 从技能奖池或者装备奖池给予奖励
			int type = UserToolConst.getType(itemType);
			if (type == UserToolConst.SKILL_TYPE) 
			{
				int quality = 0;
				switch (itemType) 
				{
					case UserToolConst.A_SKILL:
						quality = UserToolConst.A_QUALITY;
						break;
					case UserToolConst.B_SKILL:
						quality = UserToolConst.B_QUALITY;
						break;
					case UserToolConst.C_SKILL:
						quality = UserToolConst.C_QUALITY;
						break;
					case UserToolConst.D_SKILL:
						quality = UserToolConst.D_QUALITY;
						break;
					default:
						break;
				}
				SkillSelecter skillSelecter = XMLTemplateService.getSkillSelecter(quality);
				List<Integer> skillRateList;
				switch (rewardsPoolNo) 
				{
					case 1:
						skillRateList = skillSelecter.getRate1();
						break;
					case 2:
						skillRateList = skillSelecter.getRate2();
						break;
					case 3:
						skillRateList = skillSelecter.getRate3();
						break;
					case 4:
						skillRateList = skillSelecter.getRate4();
						break;
					default:
						skillRateList = skillSelecter.getRate1();
						break;
				}
				double skillRandom = Math.random()* (skillRateList.get(skillRateList.size() - 1));
				int skillLen = skillRateList.size();
				int skillSelectedNo = 0;
				for (int i = 0; i < skillLen; i++) 
				{
					if (skillRandom < skillRateList.get(i)) 
					{
						skillSelectedNo = i;
						break;
					}
				}
				int addSkillId = skillSelecter.getId().get(skillSelectedNo);
				propId = addSkillId;
				if (itemType == UserToolConst.A_SKILL) 
				{// 添加系统广播
					ArrayList<Integer> params = new ArrayList<Integer>(2);
					params.add(2);
					params.add(addSkillId);
					BoardCastInfoHandler.addBoardCase(BoardCastInfoHandler.OPENCASE_type, player.getName(), params);
				}
				builder.addSkillInfo(SkillServices.createSkillAlone(player,addSkillId));
				logger_box.info("Open Box Reward Info: {}, {}, {} ,{}", new Object[]{player.getId() , itemType , propId , count });
			} 
			else if (type == UserToolConst.EQUIP_TYPE) 
			{
				EquipSelecter equipSelecter = XMLTemplateService.getEquitSelecter(itemType);
				List<Integer> equipRateList;
				switch (rewardsPoolNo) 
				{
					case 1:
						equipRateList = equipSelecter.getRate1();
						break;
					case 2:
						equipRateList = equipSelecter.getRate2();
						break;
					case 3:
						equipRateList = equipSelecter.getRate3();
						break;
					case 4:
						equipRateList = equipSelecter.getRate4();
						break;
					default:
						equipRateList = equipSelecter.getRate1();
						break;
				}
				double equipRandom = Math.random()* (equipRateList.get(equipRateList.size() - 1));
				int equipLen = equipRateList.size();
				int equipSelectedNo = 0;
				for (int i = 0; i < equipLen; i++) 
				{
					if (equipRandom < equipRateList.get(i)) 
					{
						equipSelectedNo = i;
						break;
					}
				}
				int equipId = equipSelecter.getId().get(equipSelectedNo);
				propId = equipId;
				builder.addEquipInfo(EquipServices.createEquitAlone(player,
						equipId));
				if (itemType == UserToolConst.A_ARMOR || itemType == UserToolConst.A_WEAPON
													  || itemType == UserToolConst.A_ACCESSORIES) 
				{
					ArrayList<Integer> params = new ArrayList<Integer>(2);
					params.add(2);
					params.add(equipId);
					BoardCastInfoHandler.addBoardCase(BoardCastInfoHandler.OPENCASE_type, player.getName(), params);
				}
				logger_box.info("Open Box Reward Info: {}, {}, {} ,{}", new Object[]{player.getId() , itemType , propId , count });
			}
		}
	}
	private static int getTreasuretype(int boxId)
	{
	     if(boxId==1)
	     {
	         return UserToolConst.D_QUALITY;
	     }
	     else if(boxId==2)
	     {
	         return UserToolConst.C_QUALITY;
	     }
	     else if(boxId==3)
	     {
	         return UserToolConst.B_QUALITY;
	     }
	     else if(boxId==4)
	     {
	         return UserToolConst.A_QUALITY;
	     }
	    return 0;
	    
	}
    /***********************************************抽取宝箱**********************************************/
	/***********************************************抽取弟子**********************************************/
	private static void getDisciple(Player player , int kind , OPUpdateUserInfoByGMRet.Builder builder)
	{
		
		int playerId = player.getId();
		int id=player.getId();
		logger2.info("market recruit disciple handler received: {}, {}, {}", new Object[]{id , });
		int current_time = Util.getServerTime();
		UserStats userStats = player.getUserStats();
		int his_lucky=userStats.getDiscipleLucky();
		int poolId = 2;
		int addLucky = 0;
		
		int userGold = player.getSysgoldcoins() + player.getRechargegoldcoins();
		int cost = 0;
		switch (kind) {
			case 1:
				break;
			case 2:
				addLucky = 100;
				if (userStats.getFirstbaitime() == 0) {
					poolId = 1;
					addLucky = 0;
					userStats.setFirstbaitime(current_time);
					player.setUserStats(userStats);
				}
				break;
			case 3:
				addLucky = 300;
				if (userStats.getFirstwantime() == 0) {
					addLucky = 0;
					poolId = 1;
					userStats.setFirstwantime(current_time);
					player.setUserStats(userStats);
				}
				break;
		}
		int itemid = 0;
		int hunpoid = 0;
		KindSelecter kindSelecter = XMLTemplateService.getKindSelecter(kind);
		
		int poolLimit = XMLTemplateService.getSysBasicConfig().getPoolLimits().get(userStats.getNextDisciple());
		if (userStats.getDiscipleLucky() + addLucky >= poolLimit && poolId != 1) {
			poolId = userStats.getNextDisciple();
		}
		
		List<Integer> randKinds = kindSelecter.getKinds();
		List<Integer> randRates = getRandRates(kindSelecter, poolId);
		
		
		int randIndex = Util.getRandResult(randRates);
		
		int randResult = randKinds.get(randIndex);
		int isHunpo = randResult/1000%100;//31是弟子，32是魂魄
		int discipleType = randResult%10;//1甲，2乙，3丙，4丁
		
		KindSelecter discipleSelecter = new KindSelecter();
		discipleSelecter = XMLTemplateService.getDiscipleSelecter(discipleType);
		
		List<Integer> discipleIds = discipleSelecter.getKinds();
		List<Integer> discipleRates = getRandRates(discipleSelecter, poolId);
		List<Integer> userHasDisciple = player.getDiscipleitems();
		
		if (isHunpo == 31) {//抽到的是弟子
			int index = Util.getRandResult(discipleRates);
			int discipleId = discipleIds.get(index);
			if (userHasDisciple.contains(discipleId)) {
				hunpoid = discipleId;
			}
			else {
				itemid = discipleId;
			}
		}
		else {//抽到的是魂魄
			ArrayList<Integer> hasAQuality = new ArrayList<Integer>();
			ArrayList<Integer> hasBQuality = new ArrayList<Integer>();
			ArrayList<Integer> hasCQuality = new ArrayList<Integer>();
			ArrayList<Integer> hasDQuality = new ArrayList<Integer>();
			for (int i = 0; i < userHasDisciple.size(); i++) {
				DiscipleXmlTemplate discipleInfo = XMLTemplateService.getDiscipleTemplate(userHasDisciple.get(i));
				switch (discipleInfo.getQuality()) {
					case 1:
						hasAQuality.add(userHasDisciple.get(i));
						break;
					case 2:
						hasBQuality.add(userHasDisciple.get(i));
						break;
					case 3:
						hasCQuality.add(userHasDisciple.get(i));
						break;
					case 4:
						hasDQuality.add(userHasDisciple.get(i));
						break;
				}
			}
			
			int secondSelectorId = 0;
			
			switch (discipleType) {
				case 1:
					if (hasAQuality.size() != 0) {
						int index = (int)(Math.random() * hasAQuality.size());
						hunpoid = hasAQuality.get(index);
					}
					else {
						secondSelectorId = 2;
					}
					break;
				case 2:
					if (hasBQuality.size() != 0) {
						int index = (int)(Math.random() * hasBQuality.size());
						hunpoid = hasBQuality.get(index);
					}
					else {
						secondSelectorId = 2;
					}
					break;
				case 3:
					if (hasCQuality.size() != 0) {
						int index = (int)(Math.random() * hasCQuality.size());
						hunpoid = hasCQuality.get(index);
					}
					else {
						secondSelectorId = 3;
					}
					break;
				case 4:
					if (hasDQuality.size() != 0) {
						int index = (int)(Math.random() * hasDQuality.size());
						hunpoid = hasDQuality.get(index);
					}
					else {
						secondSelectorId = 4;
					}
					break;
			}

			if (hunpoid == 0 && secondSelectorId != 0) {//此情况为没有该类型的弟子，所以生成不了对应的魂魄，需要转化成下级弟子
				KindSelecter discipleSelecter2 = XMLTemplateService.getDiscipleSelecter(secondSelectorId);
				List<Integer> discipleIds2 = discipleSelecter2.getKinds();
				List<Integer> discipleRates2 = getRandRates(discipleSelecter2, poolId);
				
				ArrayList<Integer> hasQualityDisciple = new ArrayList<Integer>();
				
				switch (secondSelectorId) {
					case 1:
						hasQualityDisciple = hasAQuality;
						break;
					case 2:
						hasQualityDisciple = hasBQuality;
						break;
					case 3:
						hasQualityDisciple = hasCQuality;
						break;
					case 4:
						hasQualityDisciple = hasDQuality;
						break;
				}
				List<Integer> diffIds = new ArrayList<Integer>();
				List<Integer> diffRates = new ArrayList<Integer>();
				
				if (hasQualityDisciple.size() == 0) {
					diffIds = discipleIds2;
					diffRates = discipleRates2;
				}
				else {
					for (int i = 0; i < discipleIds2.size(); i++) {
						if (!hasQualityDisciple.contains(discipleIds2.get(i))) {
							diffIds.add(discipleIds2.get(i));
							diffRates.add(discipleRates2.get(i));
						}
					}
				}
				
				if (diffIds.size() != 0) {
					int index = Util.getRandResult(diffRates);
					itemid = diffIds.get(index);
				}
				else {
					int index = Util.getRandResult(discipleRates2);
					hunpoid = discipleIds2.get(index);
					
				}
			}
		}
		
		DiscipleXmlTemplate discipleInfo = new DiscipleXmlTemplate();
		if (itemid != 0 && hunpoid == 0) {
			discipleInfo = XMLTemplateService.getDiscipleTemplate(itemid);
		}
		else if (hunpoid != 0 && itemid == 0) {
			discipleInfo = XMLTemplateService.getDiscipleTemplate(hunpoid);
		} 
		
		Disciple disciple=null;
		Soul soul=null;
		int count=1;
		if (itemid != 0) {
			disciple = DiscipleUtil.newDisciple(player, itemid, 1, 0);
			
			Skill skill = player.getSkills().get(disciple.getSkill1());
			OPUserSkillInfoRet.SkillInfo.Builder skillBuilder = SkillUtil.buildSkillBuilder(skill);
			builder.addSkillInfo(skillBuilder);
			
			OPUserDiscipleInfoRet.DiscipleInfo.Builder discipleBuilder = OPUserDiscipleInfoRet.DiscipleInfo.newBuilder();
			initDiscipleBuilder(disciple, discipleBuilder);
			builder.addDiscipleInfo(discipleBuilder);
			
			if (discipleInfo.getQuality() == 1) {
				ArrayList<Integer> broParams = new ArrayList<Integer>(1);
				broParams.add(itemid);
				BoardCastInfoHandler.addBoardCase(BoardCastInfoHandler.RECRUITDISCIPLE_TYPE, player.getName(), broParams);
			}
		}
		else {
			HashMap<Integer, Soul> userSouls = player.getSouls();
			soul = new Soul();
			count = XMLTemplateService.getSysBasicConfig().getSoulCounts().get(discipleInfo.getQuality());
			if (userSouls.containsKey(hunpoid)) {
				soul = userSouls.get(hunpoid);
				soul.setCount(soul.getCount() + count);
				player.updateOneSoul(soul);
			}
			else {
				soul.setItemid(hunpoid);
				soul.setPlayerid(playerId);
				soul.setCount(count);
				player.addOneSoul(soul);
			}
			
			OPUserSoulInfoRet.SoulInfo.Builder soulBuilder= OPUserSoulInfoRet.SoulInfo.newBuilder();
			soulBuilder.setId(soul.getId());
			soulBuilder.setItemid(soul.getItemid());
			soulBuilder.setCount(soul.getCount());
			builder.addSoulInfo(soulBuilder);
		}
		
		if (discipleType == 1 && isHunpo == 31 && userStats.getDiscipleLucky() + addLucky >= poolLimit) {
			userStats.setDiscipleLucky(0);
			int nextId = userStats.getNextDisciple() + 1;
			if (nextId >= XMLTemplateService.getSysBasicConfig().getPoolLimits().size()) {
				nextId = XMLTemplateService.getSysBasicConfig().getPoolLimits().size() - 1;
			}
			userStats.setNextDisciple(nextId);
		}
		else if (userStats.getDiscipleLucky() + addLucky < poolLimit) {
			userStats.setDiscipleLucky(userStats.getDiscipleLucky() + addLucky);
		} 
	//log{1 本次奖池 , 2 下次高级奖池 , 3 抽取前运气值 , 4 抽取后运气值 , 5 魂魄0/弟子1 , 6 品质 , 7 id , 8 数量 }
		if(itemid!=0)//最终抽到的是弟子
		{
			int disciple_quality=XMLTemplateService.getDiscipleTemplate(itemid).getQuality();
			logger_recruitDisciple.info("Maeket Recruit Disciple Info: {}, {} , {} , {} , {}, {} , {} , {}",new Object[]{ poolId , userStats.getNextDisciple() , his_lucky , userStats.getDiscipleLucky() , 1 , disciple_quality, disciple.getItemid() , count});
		}
		else///最终抽到的是魂魄
		{
			int soul_quality=XMLTemplateService.getDiscipleTemplate(hunpoid).getQuality();
			logger_recruitDisciple.info("Maeket Recruit Disciple Info: {}, {} , {} , {} , {}, {} , {} , {}",new Object[]{ poolId , userStats.getNextDisciple() , his_lucky , userStats.getDiscipleLucky() , 0 , soul_quality , soul.getItemid() , count});
		}
		player.setUserStats(userStats);
	}
	
	private static void initDiscipleBuilder(Disciple disciple, OPUserDiscipleInfoRet.DiscipleInfo.Builder discipleBuilder) {
		discipleBuilder.setId(disciple.getId());
		discipleBuilder.setItemid(disciple.getItemid());
		discipleBuilder.setLevel(disciple.getLevel());
		discipleBuilder.setExp(disciple.getExp());
		discipleBuilder.setAttack(disciple.getAttackToInt());
		discipleBuilder.setDefence(disciple.getDefenceToInt());
		discipleBuilder.setHealth(disciple.getHealthToInt());
		discipleBuilder.setRp(disciple.getRpToInt());
		discipleBuilder.setUpgradelevel(disciple.getUpgradelevel());
		discipleBuilder.setPotentiality(disciple.getPotentiality());
		discipleBuilder.setSkill1(disciple.getSkill1());
		discipleBuilder.setBattleid(disciple.getBattleid());
	}
	
	public static List<Integer> getRandRates(KindSelecter selecter,int poolId){
		List<Integer> randRates = new ArrayList<Integer>();
		switch (poolId) {
			case 1:
				randRates = selecter.getRate1();
				break;
			case 2:
				randRates = selecter.getRate2();
				break;
			case 3:
				randRates = selecter.getRate3();
				break;
			case 4:
				randRates = selecter.getRate4();
				break;
			case 5:
				randRates = selecter.getRate5();
				break;
			case 6:
				randRates = selecter.getRate6();
				break;
		}
		return randRates;
	}
    /***********************************************抽取弟子**********************************************/
}
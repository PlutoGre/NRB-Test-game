package com.oldmountain.dzm.packethandler;


import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.ConcurrentHashMap;

import net.sf.json.JSONObject;

import org.apache.mina.common.IoSession;
import org.hibernate.HibernateException;
import org.hibernate.SQLQuery;
import org.hibernate.Session;
import org.hibernate.Transaction;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.google.protobuf.InvalidProtocolBufferException;
import com.oldmountain.dzm.DZM;
import com.oldmountain.dzm.configEntity.SkillBookNpcStats;
import com.oldmountain.dzm.entity.Battle;
import com.oldmountain.dzm.entity.BattleArrayObject;
import com.oldmountain.dzm.entity.Challenge;
import com.oldmountain.dzm.entity.Disciple;
import com.oldmountain.dzm.entity.DiscipleXmlTemplate;
import com.oldmountain.dzm.entity.Equip;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.entity.Skill;
import com.oldmountain.dzm.entity.SkillXmlTemplate;
import com.oldmountain.dzm.entity.Title;
import com.oldmountain.dzm.message.OP;
import com.oldmountain.dzm.message.UserBattleArray;
import com.oldmountain.dzm.message.ErrorReporter.OPErrorReporter;
import com.oldmountain.dzm.message.ShowUserBattle.OPShowUserBattleArray;
import com.oldmountain.dzm.message.ShowUserBattle.OPShowUserBattleArrayRet;
import com.oldmountain.dzm.message.TitleInfo.OPTitleInfoRet;
import com.oldmountain.dzm.message.UserBattleArray.OPUserBattleArrayRet;
import com.oldmountain.dzm.message.UserDiscipleInfo.OPUserDiscipleInfoRet;
import com.oldmountain.dzm.message.UserEquipInfo.OPUserEquipInfoRet;
import com.oldmountain.dzm.message.UserSkillInfo.OPUserSkillInfoRet;
import com.oldmountain.dzm.net.ObjectAccessor;
import com.oldmountain.dzm.net.packet.Packet;
import com.oldmountain.dzm.util.BattleUtil;
import com.oldmountain.dzm.util.DiscipleUtil;
import com.oldmountain.dzm.util.EquipUtil;
import com.oldmountain.dzm.util.SkillUtil;
import com.oldmountain.dzm.util.TimeUtil;
import com.oldmountain.dzm.util.TitleUtil;
import com.oldmountain.dzm.util.Util;
import com.oldmountain.dzm.util.XMLTemplateService;

public class ShowUserBattleArrayHandler {
	private static final ConcurrentHashMap<Integer,OPShowUserBattleArrayRet.Builder> OPShowUserBattleArrayRetMap = new ConcurrentHashMap<Integer,OPShowUserBattleArrayRet.Builder>(500);
	private static final Logger log = LoggerFactory.getLogger(ShowUserBattleArrayHandler.class);
	private static int nextFreshTime=0;
	public static void showBattleArray(Packet packet,IoSession session){
		OPShowUserBattleArray params=null;
		OPShowUserBattleArrayRet.Builder builder=OPShowUserBattleArrayRet.newBuilder();
		try {
			params = OPShowUserBattleArray.parseFrom(packet.data.array());
		} catch (InvalidProtocolBufferException e) {
			e.printStackTrace();
			return;
		}
		int playerId = (Integer)session.getAttribute("playerid");
		Player player = ObjectAccessor.getOnlineUser(playerId);
		int id=player.getId();
		long current_time = Util.getServerMillTime();
		int version = params.getVersion();
		int relationType = params.getRelationType();
		int relationId = params.getRelationId();
		int isNpc = params.getIsNpc();
		//判断是否跨天,如跨天则清除缓存
		checkTimeToClearMap(Util.getServerTime());
		
		if(OPShowUserBattleArrayRetMap.containsKey(relationId)){
			builder=OPShowUserBattleArrayRetMap.get(relationId);
		}
		else
		{
			boolean toCache=false;
			Player enemy=ObjectAccessor.getOnlineUser(relationId);
			if(enemy==null){
				enemy=new Player();
				enemy.setId(relationId);
			}
			log.info("show user battle array handler received: {}, {}, {}", new Object[]{id , version , relationType , relationId});
			BattleArrayObject battleArrayObject=null;
			try {
				switch (relationType) {
				case 1:
					//血战 
					battleArrayObject=BattleUtil.getUserBattleArrayObject(enemy);
					buildeByBattleArrayObject(builder, battleArrayObject, enemy);
					builder.setPlayerLevel(enemy.getLevel());
					builder.setPlayerName(enemy.getName());
					builder.setTitleNum(Math.round(enemy.getTitle().getTotalAmount()*(1+enemy.getTitle().getTotalRate())));
					toCache=true;
					break;
				case 2:
					//论剑
					Challenge enemyChallenge = ChallengeHandler.getChallenge(relationId);
					if(enemyChallenge.getRank()<=20){
						toCache=true;
					}
					if(enemyChallenge.getIsnpc()==1)
					{
						battleArrayObject=getNpcBattleArrayObjectById(enemyChallenge);
						builderNpcBattleArray(builder, battleArrayObject);
						builder.setTitleNum(0);
					}
					else
					{
						battleArrayObject=BattleUtil.getUserBattleArrayObject(enemy);
						buildeByBattleArrayObject(builder, battleArrayObject, enemy);
						builder.setTitleNum(Math.round(enemy.getTitle().getTotalAmount()*(1+enemy.getTitle().getTotalRate())));
					}
					builder.setPlayerName(enemyChallenge.getName());
					builder.setPlayerLevel(enemyChallenge.getLevel());
					break;
				case 3:
					//残章
					if(isNpc==1)
					{
						SkillBookNpcStats _npcStats=player.getSkillBookNpcStats();
						int lastRandomIndex=_npcStats.getLastRandomIndex();
						JSONObject npc=(JSONObject)_npcStats.getRandomNpcArr().get(lastRandomIndex);
						builder.setPlayerName("");
						builder.setPlayerLevel(npc.getInt("l"));
						battleArrayObject=_npcStats.getLastNpcBattle();
						builderNpcBattleArray(builder, battleArrayObject);
						builder.setTitleNum(0);
					}
					else
					{
						battleArrayObject=BattleUtil.getUserBattleArrayObject(enemy);
						buildeByBattleArrayObject(builder, battleArrayObject, enemy);
						builder.setPlayerLevel(enemy.getLevel());
						builder.setPlayerName(enemy.getName());
						builder.setTitleNum(Math.round(enemy.getTitle().getTotalAmount()*(1+enemy.getTitle().getTotalRate())));
					}
					break;
					
				default:
					break;
				}
			} catch (Exception e) {
				OPErrorReporter.Builder errorBuilder = OPErrorReporter.newBuilder();
				errorBuilder.setErrorid(OP.code.OPCODE_SHOW_USER_BATTLERET_S_VALUE);
				errorBuilder.setErrormsg("relationId doesn't exist");
				player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errorBuilder);
				log.info("Error Reporter: show user battle array handler send: {}, {}, {}", new Object[]{id , relationType , relationId});
				return ;
			}
			if(toCache && !OPShowUserBattleArrayRetMap.contains(relationId)){
					OPShowUserBattleArrayRetMap.put(relationId, builder);
			}
		}
		player.setLastVerifyTime(Util.getServerTime());	
		player.sendPacket(OP.code.OPCODE_SHOW_USER_BATTLERET_S_VALUE, builder);
		long current_time_2 = Util.getServerMillTime();
		long taken_time=current_time_2-current_time;
			
		log.info("change equip handler ret send: {}, {}, {}", new Object[]{id ,taken_time});
	}
	public static BattleArrayObject getNpcBattleArrayObjectById(Challenge enemyChallenge){
		BattleArrayObject enemyBattleArray = new BattleArrayObject();
		int careerId = enemyChallenge.getPlayerid() / 100000;
		enemyBattleArray = XMLTemplateService.getCareerTemplate(careerId).getBattleInfo();
		return enemyBattleArray;
	}
	
	private static boolean isChallengeNPC(int enemyId){
		boolean isNPC=false;
		Challenge enemyChallenge = ChallengeHandler.getChallenge(enemyId);
		if(enemyChallenge!=null&&enemyChallenge.getIsnpc() == 1){
			isNPC=true;
		}
		return isNPC;
	}
	private static void builderNpcBattleArray(OPShowUserBattleArrayRet.Builder builder,BattleArrayObject battleArrayObject){
		int serverTime=Util.getServerTime();
		OPUserBattleArrayRet.Builder battleBuilder=UserBattleArray.OPUserBattleArrayRet.newBuilder();
		OPUserDiscipleInfoRet.Builder userDiscipleBuilder=OPUserDiscipleInfoRet.newBuilder();
		OPUserSkillInfoRet.Builder userSkillInfoBuilder=OPUserSkillInfoRet.newBuilder();
		ArrayList<Battle> battles=battleArrayObject.getBattlearrays();
		for(int i=0 ; i<battles.size() ; i++){
			Battle battle=battles.get(i);
			battle.setId(i);
			OPUserBattleArrayRet.BattleArrayInfo.Builder battleArrayInfoBuilder=OPUserBattleArrayRet.BattleArrayInfo.newBuilder();
			BattleUtil.buildBattleBuilder(battleArrayInfoBuilder, battle, i);
			battleBuilder.addBattle(battleArrayInfoBuilder);
			
			OPUserDiscipleInfoRet.DiscipleInfo.Builder discipleInfo=OPUserDiscipleInfoRet.DiscipleInfo.newBuilder();
			DiscipleXmlTemplate discipleXmlTemplate=XMLTemplateService.getDiscipleTemplate(battle.getDiscipleid());
			discipleInfo.setId(battle.getDiscipleid());
			discipleInfo.setItemid(battle.getItemid());
			int level=battle.getLevel();
			if(level>150){
				level=150;
			}
			discipleInfo.setLevel(level);
			discipleInfo.setExp(0);
			discipleInfo.setHealth(battle.getHealth());
			discipleInfo.setAttack(battle.getAttack());
			discipleInfo.setDefence(battle.getDefence());
			discipleInfo.setRp(battle.getRp());
			discipleInfo.setUpgradelevel(0);
			discipleInfo.setPotentiality(0);
			discipleInfo.setSkill1(discipleXmlTemplate.getDefaultSkill());
			discipleInfo.setBattleid(i);
			userDiscipleBuilder.addDisciple(discipleInfo);
			
			OPUserSkillInfoRet.SkillInfo.Builder	skillInfo=OPUserSkillInfoRet.SkillInfo.newBuilder();
			SkillXmlTemplate skillXmlTemplate=XMLTemplateService.getSkillTemplate(battle.getDefaultskillitem());
			skillInfo.setId(battle.getDefaultskillitem());
			skillInfo.setIsborn(1);
			skillInfo.setItemid(battle.getDefaultskillitem());
			skillInfo.setLevel(1);
			skillInfo.setConsume(0);
			skillInfo.setQuantity(skillXmlTemplate.getQuantity());
			userSkillInfoBuilder.addSkill(skillInfo);
			
			
		}
		
		battleBuilder.setServertime(serverTime);
		userDiscipleBuilder.setServertime(serverTime);
		userSkillInfoBuilder.setServertime(serverTime);
		
		builder.setUserBattleArray(battleBuilder);
		builder.setUserDisciple(userDiscipleBuilder);
		builder.setUserSkill(userSkillInfoBuilder);
	}
		private static void buildeByBattleArrayObject(OPShowUserBattleArrayRet.Builder builder , BattleArrayObject battleArrayObject , Player enemy){
		OPUserBattleArrayRet.Builder battleBuilder=UserBattleArray.OPUserBattleArrayRet.newBuilder();
		OPUserDiscipleInfoRet.Builder userDiscipleBuilder=OPUserDiscipleInfoRet.newBuilder();
		OPUserEquipInfoRet.Builder userEquipInfoBuilder=OPUserEquipInfoRet.newBuilder();
		OPUserSkillInfoRet.Builder userSkillInfoBuilder=OPUserSkillInfoRet.newBuilder();
		OPTitleInfoRet.Builder titleInfoBuilder=OPTitleInfoRet.newBuilder();
		
		int serverTime=Util.getServerTime();
		int enemyId=battleArrayObject.getPlayerid();
		List<Battle> battles=battleArrayObject.getBattlearrays();
		ArrayList<Integer> skillIds=new ArrayList<Integer>();
		ArrayList<Integer> equipIds=new ArrayList<Integer>();
		ArrayList<Integer> discipleIds=new ArrayList<Integer>();
		List<Title> titleList = DZM.getEntityManager().limitQuery("from Title where playerid = ?", 0, 1, enemyId);
		if (titleList.size() == 1) {
			Title title = titleList.get(0);
			enemy.setTitle(title);
		}
		for(int i=0 ; i<battles.size() ; i++){
			Battle battle=battles.get(i);
			if(battle.getDiscipleid()==0){
				continue;
			}
			OPUserBattleArrayRet.BattleArrayInfo.Builder battleArrayInfoBuilder=OPUserBattleArrayRet.BattleArrayInfo.newBuilder();
			BattleUtil.buildBattleBuilder(battleArrayInfoBuilder, battle, i);
			battleBuilder.addBattle(battleArrayInfoBuilder);
			
			discipleIds.add(battle.getDiscipleid());
			skillIds.add(battle.getDefaultskill());
			if(battle.getSkill1()!=0){
				skillIds.add(battle.getSkill1());
			}
			if(battle.getSkill2()!=0){
				skillIds.add(battle.getSkill2());
			}
			if(battle.getEquip1()!=0){
				equipIds.add(battle.getEquip1());
			}
			if(battle.getEquip2()!=0){
				equipIds.add(battle.getEquip2());
			}
			if(battle.getEquip3()!=0){
				equipIds.add(battle.getEquip3());
			}
		}
		ArrayList<Disciple> disciples=getDisciples(discipleIds);
		ArrayList<Equip> equips=getEquips(equipIds);
		ArrayList<Skill> skills=getSkills(skillIds);
		for(Disciple disciple:disciples){
			userDiscipleBuilder.addDisciple(DiscipleUtil.buildDiscipleBuilder(disciple));
		}
		for(Skill skill:skills){
			userSkillInfoBuilder.addSkill(SkillUtil.buildSkillBuilder(skill));
		}
		for(Equip equip:equips){
			userEquipInfoBuilder.addEquip(EquipUtil.buildReturn(equip));
		}
		
		titleInfoBuilder=TitleUtil.buildTitleBuilder(enemy);
		
		battleBuilder.setServertime(serverTime);
		userDiscipleBuilder.setServertime(serverTime);
		userSkillInfoBuilder.setServertime(serverTime);
		userEquipInfoBuilder.setServertime(serverTime);
		titleInfoBuilder.setServertime(serverTime);
		
		builder.setUserBattleArray(battleBuilder);
		builder.setUserDisciple(userDiscipleBuilder);
		builder.setUserSkill(userSkillInfoBuilder);
		builder.setUserEquip(userEquipInfoBuilder);
		builder.setUserTitle(titleInfoBuilder);
	}
	
	public static ArrayList<Skill> getSkills(ArrayList<Integer> ids){
		ArrayList<Skill> _list=null;
        Session session=DZM.getEntityManager().getSessionFactory();
        Transaction tx = session.beginTransaction( ) ;
		try
        {
			String sql = "select * from skill where id in (:ids)";
			SQLQuery query = session.createSQLQuery( sql ) ;
			query.setParameterList("ids", ids);
	        query.addEntity(Skill.class);
	        _list=(ArrayList<Skill>) query.list();
	        session.clear();
	        tx.commit() ;
        }catch (HibernateException e) {
        	if (tx!=null)
            {
                tx.rollback();
            }
            throw e;
		}
		return _list;
	}
	
	public static ArrayList<Equip> getEquips(ArrayList<Integer> ids){
		ArrayList<Equip> _list=null;
		Session session=DZM.getEntityManager().getSessionFactory();
		Transaction tx = session.beginTransaction( ) ;
		try
		{
			String sql = "select * from equip where id in (:ids)";
			SQLQuery query = session.createSQLQuery( sql ) ;
			query.setParameterList("ids", ids);
			query.addEntity(Equip.class);
			_list=(ArrayList<Equip>) query.list();
			session.clear();
			tx.commit() ;
		}catch (HibernateException e) {
			if (tx!=null)
			{
				tx.rollback();
			}
			throw e;
		}
		return _list;
	}
	
	public static ArrayList<Disciple> getDisciples(ArrayList<Integer> ids){
		ArrayList<Disciple> _list=null;
		Session session=DZM.getEntityManager().getSessionFactory();
		Transaction tx = session.beginTransaction( ) ;
		try
		{
			String sql = "select * from disciple where id in (:ids)";
			SQLQuery query = session.createSQLQuery( sql ) ;
			query.setParameterList("ids", ids);
			query.addEntity(Disciple.class);
			_list=(ArrayList<Disciple>) query.list();
			session.clear();
			tx.commit() ;
		}catch (HibernateException e) {
			if (tx!=null)
			{
				tx.rollback();
			}
			throw e;
		}
		return _list;
	}
	
	public static void checkTimeToClearMap(int current_time){
		if(nextFreshTime<=current_time){
			nextFreshTime=current_time+6*60*60;
			OPShowUserBattleArrayRetMap.clear();
		}
	}
}


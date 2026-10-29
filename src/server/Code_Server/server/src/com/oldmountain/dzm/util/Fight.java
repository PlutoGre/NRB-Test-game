package com.oldmountain.dzm.util;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Random;

import com.oldmountain.dzm.configEntity.WorldBossConfig;
import com.oldmountain.dzm.entity.Actions;
import com.oldmountain.dzm.entity.Battle;
import com.oldmountain.dzm.entity.BattleArrayObject;
import com.oldmountain.dzm.entity.Characters;
import com.oldmountain.dzm.entity.Damage;
import com.oldmountain.dzm.entity.FinalRound;
import com.oldmountain.dzm.entity.Round;
import com.oldmountain.dzm.entity.SkillXmlTemplate;
import com.oldmountain.dzm.message.ErrorReporter.OPErrorReporter;
import com.oldmountain.dzm.message.UserBattle.OPUserBattleRet;
import com.oldmountain.dzm.net.ObjectAccessor;


public class Fight {
	private Round round1;//第一回合
	private Round round2;//第二回合
	private FinalRound finalRound;//最终回合
	private int result;//战斗的结果
	private OPUserBattleRet.Team.Builder teamBuilder;//战斗的气势和姓名
	
	public Fight(){
		this.round1 = this.round2 = new Round();
		this.finalRound = new FinalRound();
		this.result = 0;
		teamBuilder = OPUserBattleRet.Team.newBuilder();
	}
	
	public Round getRound1(){
		return round1;
	}
	
	public void setRound1(Round round){
		this.round1 = round;
	}
	
	public Round getRound2(){
		return round2;
	}
	
	public void setRound2(Round round){
		this.round2 = round;
	}
	
	public FinalRound getFinalRound(){
		return finalRound;
	}
	
	public void setFInalRound(FinalRound finalRound){
		this.finalRound = finalRound;
	}
	
	public int getResult(){
		return result;
	}
	
	public void setResult(int result){
		this.result = result;
	}
	
	public OPUserBattleRet.Team.Builder getTeamBuilder(){
		return teamBuilder;
	}
	
	public void setTeamBuilder(OPUserBattleRet.Team.Builder teamBuilder){
		this.teamBuilder = teamBuilder;
	}
	
	public void doFight(BattleArrayObject orginUserBattleInfo,BattleArrayObject orginEnemyBattleInfo,ArrayList<Float>enemySkills){
		OPUserBattleRet.Team.Builder teamBuilder = OPUserBattleRet.Team.newBuilder();
		teamBuilder.setName1("");
		teamBuilder.setName2("");
		teamBuilder.setTeam1Archievements(orginUserBattleInfo.getMomentum());
		teamBuilder.setTeam2Archievements(orginEnemyBattleInfo.getMomentum());
		this.setTeamBuilder(teamBuilder);
		
		int selfMomentum = orginUserBattleInfo.getMomentum();
		int enemyMomentum = orginEnemyBattleInfo.getMomentum();
		
		int firstHand = 0;
		
		if (selfMomentum < enemyMomentum) {
			firstHand = 1;
		}
		
		ArrayList<Battle> orginUserArray = orginUserBattleInfo.getBattlearrays();
		ArrayList<Battle> orginEnemyArray = orginEnemyBattleInfo.getBattlearrays();
		
		ArrayList<Battle> userArray = new ArrayList<Battle>();
		ArrayList<Battle> enemyArray = new ArrayList<Battle>();
		
		HashMap<Integer, Integer> userSkillRates = new HashMap<Integer, Integer>();
		HashMap<Integer, ArrayList<ArrayList<Integer>>> userSkill = new HashMap<Integer, ArrayList<ArrayList<Integer>>>();
		HashMap<Integer, Integer> enemySkillRates = new HashMap<Integer, Integer>();
		HashMap<Integer, ArrayList<ArrayList<Integer>>> enemySkill = new HashMap<Integer, ArrayList<ArrayList<Integer>>>();
		
		
		
		boolean gotto = true;
		/*******************第一轮 交战 begin ***************************/		
		int userLeftCount = orginUserArray.size();
		int enemyLeftCount = orginEnemyArray.size();
		
		ArrayList<Characters> characters = new ArrayList<Characters>();
		Characters character = new Characters();
		Round round1 = new Round();
		ArrayList<Actions> actions = new ArrayList<Actions>();
		Actions action = new Actions();
		ArrayList<Damage> damages = new ArrayList<Damage>();
		Damage damage = new Damage();
		round1.setGotTO(gotto);
		int position = 0;
		for (int i = 0; i < userLeftCount; i++) {
			ArrayList<Integer> skillIds = new ArrayList<Integer>();
			ArrayList<Integer> skillTypes = new ArrayList<Integer>();
			ArrayList<Integer> skillQuantity = new ArrayList<Integer>();
			ArrayList<Integer> skillRates = new ArrayList<Integer>();
			ArrayList<ArrayList<Integer>> skills = new ArrayList<ArrayList<Integer>>();
			int rate = 0;
			Battle battle = orginUserArray.get(i).clone();
			if (battle == null) {
				ObjectAccessor.delBattleArray(orginUserBattleInfo.getPlayerid());
				OPErrorReporter.Builder errBuilder = OPErrorReporter.newBuilder();
				errBuilder.setErrorid(3);
				errBuilder.setErrormsg("battle error");
				return ;
			}
			if (battle.getDiscipleid() == 0) {
				continue;
			}
			character = new Characters();
			userArray.add(battle);
			character.setItemid(battle.getItemid());
			character.setLife(battle.getHealth());
			character.setPower(battle.getRp());
			character.setPosition(position * 2);
			character.setLevel(battle.getLevel());
			characters.add(character);
			
			if (battle.getDefaultskilltype() == 1 || battle.getDefaultskilltype() == 2) {
				SkillXmlTemplate skillInfo = XMLTemplateService.getSkillTemplate(battle.getDefaultskillitem());
				rate += Math.round(skillInfo.getRate() * 100);
				skillIds.add(skillInfo.getId());
				skillTypes.add(skillInfo.getType());
				skillQuantity.add((int)battle.getDefaultskillquantity());
				skillRates.add(rate);
				
			}
			else {
				action = new Actions();
				action.setType(1);
				action.setSkillId(battle.getDefaultskillitem());
				action.setPosition(position * 2);
				actions.add(action);
			}
			
			if (battle.getSkill1type() == 1 || battle.getSkill1type() == 2) {
				SkillXmlTemplate skillInfo = XMLTemplateService.getSkillTemplate(battle.getSkill1item());
				rate += Math.round(skillInfo.getRate() * 100);
				skillIds.add(skillInfo.getId());
				skillTypes.add(skillInfo.getType());
				skillQuantity.add((int)battle.getSkill1quantity());
				skillRates.add(rate);
			}
			else if (battle.getSkill1type() != 0){
				action = new Actions();
				action.setType(1);
				action.setSkillId(battle.getSkill1item());
				action.setPosition(position * 2);
				actions.add(action);
			}
			
			if (battle.getSkill2type() == 1 || battle.getSkill2type() == 2) {
				SkillXmlTemplate skillInfo = XMLTemplateService.getSkillTemplate(battle.getSkill2item());
				rate += Math.round(skillInfo.getRate() * 100);
				skillIds.add(skillInfo.getId());
				skillTypes.add(skillInfo.getType());
				skillQuantity.add((int)battle.getSkill2quantity());
				skillRates.add(rate);
			}
			else if (battle.getSkill2type() != 0) {
				action = new Actions();
				action.setType(1);
				action.setSkillId(battle.getSkill2item());
				action.setPosition(position * 2);
				actions.add(action);
			}
			
			userSkillRates.put(battle.getItemid(), rate);
			skills.add(skillIds);
			skills.add(skillTypes);
			skills.add(skillQuantity);
			skills.add(skillRates);
			userSkill.put(battle.getItemid(), skills);
			
			position ++;
		}
		position = 0;
		for (int i = 0; i < enemyLeftCount;i++) {
			ArrayList<Integer> skillIds = new ArrayList<Integer>();
			ArrayList<Integer> skillTypes = new ArrayList<Integer>();
			ArrayList<Integer> skillQuantity = new ArrayList<Integer>();
			ArrayList<Integer> skillRates = new ArrayList<Integer>();
			ArrayList<ArrayList<Integer>> skills = new ArrayList<ArrayList<Integer>>();
			int rate = 0;
			Battle battle = orginEnemyArray.get(i).clone();
			if (battle.getDiscipleid() == 0) {
				continue;
			}
			character = new Characters();
			enemyArray.add(battle);
			character.setItemid(battle.getItemid());
			character.setLife(battle.getHealth());
			character.setPower(battle.getRp());
			character.setPosition(position * 2 +1);
			character.setLevel(battle.getLevel());
			characters.add(character);
			
			if (battle.getDefaultskilltype() == 1 || battle.getDefaultskilltype() == 2) {
				SkillXmlTemplate skillInfo = XMLTemplateService.getSkillTemplate(battle.getDefaultskillitem());
				if (enemySkills != null) {
					rate += Math.round(skillInfo.getRate() * 100 * enemySkills.get(i));
				}
				else {
					rate += Math.round(skillInfo.getRate() * 100);
				}
				skillIds.add(skillInfo.getId());
				skillTypes.add(skillInfo.getType());
				skillQuantity.add((int)battle.getDefaultskillquantity());
				skillRates.add(rate);
			}
			else {
				action = new Actions();
				action.setType(1);
				action.setSkillId(battle.getDefaultskillitem());
				action.setPosition(position * 2 + 1);
				actions.add(action);
			}
			
			if (battle.getSkill1type() == 1 || battle.getSkill1type() == 2) {
				SkillXmlTemplate skillInfo = XMLTemplateService.getSkillTemplate(battle.getSkill1item());
				if (enemySkills != null) {
					rate += Math.round(skillInfo.getRate() * 100 * enemySkills.get(i));
				}
				else {
					rate += Math.round(skillInfo.getRate() * 100);
				}
				skillIds.add(skillInfo.getId());
				skillTypes.add(skillInfo.getType());
				skillQuantity.add((int)battle.getSkill1quantity());
				skillRates.add(rate);
			}
			else if (battle.getSkill1type() != 0){
				action = new Actions();
				action.setType(1);
				action.setSkillId(battle.getSkill1item());
				action.setPosition(position * 2 + 1);
				actions.add(action);
			}
			
			if (battle.getSkill2type() == 1 || battle.getSkill2type() == 2) {
				SkillXmlTemplate skillInfo = XMLTemplateService.getSkillTemplate(battle.getSkill2item());
				if (enemySkills != null) {
					rate += Math.round(skillInfo.getRate() * 100 * enemySkills.get(i));
				}
				else {
					rate += Math.round(skillInfo.getRate() * 100);
				}
				skillIds.add(skillInfo.getId());
				skillTypes.add(skillInfo.getType());
				skillQuantity.add((int)battle.getSkill2quantity());
				skillRates.add(rate);
			}
			else if (battle.getSkill2type() != 0){
				action = new Actions();
				action.setType(1);
				action.setSkillId(battle.getSkill2item());
				action.setPosition(position * 2 + 1);
				actions.add(action);
			}
			
			enemySkillRates.put(battle.getItemid(), rate);
			
			skills.add(skillIds);
			skills.add(skillTypes);
			skills.add(skillQuantity);
			skills.add(skillRates);
			enemySkill.put(battle.getItemid(), skills);
			
			position ++;
		}
		
		userLeftCount = userArray.size();
		enemyLeftCount = enemyArray.size();
		
		int minCount = Math.min(userArray.size(), enemyArray.size());
		Random random = new Random();
		round1.setCharacters(characters);
		for (int i = 0; i < 11; i++) {
			if (userLeftCount <= 0 || enemyLeftCount <= 0) {
				gotto = false;
				if (userLeftCount <= 0) {
					this.setResult(5);
				}
				else {
					this.setResult(0);
				}
				break;
			}
			for (int j = 0; j < minCount; j++) {
				int rand = random.nextInt(100) + 1;
				action = new Actions();
				damage = new Damage();
				damages = new ArrayList<Damage>();
				Battle userBattle = userArray.get(j);
				Battle enemyBattle = enemyArray.get(j);
				if (userBattle.getHealth() <= 0 || enemyBattle.getHealth() <= 0) {//一方已死亡，直接跳过
					continue;
				}
				if ((i + j) % 2 == firstHand) {	
					action.setPosition(j * 2);
					if (rand > userSkillRates.get(userBattle.getItemid())) {
						action.setType(0);
						damage.setPosition(j * 2 + 1);
						int damageNum = userBattle.getAttack() - enemyBattle.getDefence();
						damageNum = damageNum < 1 ? 1 : damageNum;
						damage.setHarmValue(damageNum);
						damages.add(damage);
						action.setDamages(damages);
						actions.add(action);
						int leftHealth = enemyBattle.getHealth() - damageNum < 0 ? 0 : enemyBattle.getHealth() - damageNum;
						enemyBattle.setHealth(leftHealth);
						if (leftHealth <= 0) {
							enemyLeftCount --;
						}
					}
					else {
						action.setType(1);
						int index = 0;
						ArrayList<ArrayList<Integer>> randSkill = userSkill.get(userBattle.getItemid());
						rand = random.nextInt(userSkillRates.get(userBattle.getItemid()));
						ArrayList<Integer> rateArrayList = randSkill.get(3);
						for (int k = 0; k < rateArrayList.size(); k++) {
							if (rand <= rateArrayList.get(k)) {
								index = k;
								break;
							}
						}
						action.setSkillId(randSkill.get(0).get(index));
						if (randSkill.get(1).get(index) == 1) {//单体攻击技能
							int damageNum = userBattle.getAttack() + (userBattle.getRp()/100) * randSkill.get(2).get(index) - enemyBattle.getDefence();
							damageNum = damageNum < 1 ? 1 : damageNum;
							damage.setPosition(j * 2 + 1);
							damage.setHarmValue(damageNum);
							damages.add(damage);
							action.setDamages(damages);
							actions.add(action);
							int leftHealth = enemyBattle.getHealth() - damageNum < 0 ? 0 : enemyBattle.getHealth() - damageNum;
							enemyBattle.setHealth(leftHealth);
							if (leftHealth <= 0) {
								enemyLeftCount --;
							}
						}
						else {//群体攻击
							int attack = userBattle.getAttack() + (userBattle.getRp()/100) * randSkill.get(2).get(index);
							for (int k = 0; k < enemyArray.size(); k++) {
								if (enemyArray.get(k).getHealth() <= 0) {
									continue;
								}
								int distence = Math.abs(j - k);
								int damageNum = this.getDistenceDamage(attack, distence) - enemyArray.get(k).getDefence();
								damage = new Damage();
								damageNum = damageNum < 1 ? 1 : damageNum;
								damage.setPosition(k * 2 + 1);
								damage.setHarmValue(damageNum);
								damages.add(damage);
								int leftHealth = enemyArray.get(k).getHealth() - damageNum < 0 ? 0 : enemyArray.get(k).getHealth() - damageNum;
								enemyArray.get(k).setHealth(leftHealth);
								if (leftHealth <= 0) {
									enemyLeftCount --;
								}
							}
							action.setDamages(damages);
							actions.add(action);
						}
						
					}
					
				}
				else {
					action.setPosition(j * 2 + 1);
					
					if (rand > enemySkillRates.get(enemyBattle.getItemid())) {
						action.setType(0);
						damage.setPosition(j * 2);
						int damageNum = enemyBattle.getAttack() - userBattle.getDefence();
						damageNum = damageNum < 1 ? 1 : damageNum;
						damage.setHarmValue(damageNum);
						damages.add(damage);
						action.setDamages(damages);
						actions.add(action);
						int leftHealth = userBattle.getHealth() - damageNum < 0 ? 0 : userBattle.getHealth() - damageNum;
						userBattle.setHealth(leftHealth);
						if (leftHealth <= 0) {
							userLeftCount --;
						}
					}
					else {
						action.setType(1);
						int index = 0;
						
						ArrayList<ArrayList<Integer>> randSkill = enemySkill.get(enemyBattle.getItemid());
						rand = random.nextInt(enemySkillRates.get(enemyBattle.getItemid()));
						ArrayList<Integer> rateArrayList = randSkill.get(3);
						for (int k = 0; k < rateArrayList.size(); k++) {
							if (rand <= rateArrayList.get(k)) {
								index = k;
								break;
							}
						}
						action.setSkillId(randSkill.get(0).get(index));
						
						if (randSkill.get(1).get(index) == 1) {//单体攻击技能
							int damageNum = enemyBattle.getAttack() + (enemyBattle.getRp()/100) * randSkill.get(2).get(index) - userBattle.getDefence();
							damageNum = damageNum < 1 ? 1 : damageNum;
							damage.setPosition(j * 2);
							damage.setHarmValue(damageNum);
							damages.add(damage);
							action.setDamages(damages);
							actions.add(action);
							int leftHealth = userBattle.getHealth() - damageNum < 0 ? 0 : userBattle.getHealth() - damageNum;
							userBattle.setHealth(leftHealth);
							if (leftHealth <= 0) {
								userLeftCount --;
							}
						}
						else {//群体攻击
							int attack = enemyBattle.getAttack() + (enemyBattle.getRp()/100) * randSkill.get(2).get(index);
							for (int k = 0; k < userArray.size(); k++) {
								if (userArray.get(k).getHealth() <= 0) {
									continue;
								}
								int distence = Math.abs(j - k);
								int damageNum = this.getDistenceDamage(attack, distence) - userArray.get(k).getDefence();
								damage = new Damage();
								damageNum = damageNum < 1 ? 1 : damageNum;
								damage.setPosition(k * 2);
								damage.setHarmValue(damageNum);
								damages.add(damage);
								int leftHealth = userArray.get(k).getHealth() - damageNum < 0 ? 0 : userArray.get(k).getHealth() - damageNum;
								userArray.get(k).setHealth(leftHealth);
								if (leftHealth <= 0) {
									userLeftCount --;
								}
							}
							action.setDamages(damages);
							actions.add(action);
						}
					}
				}
			}
		}
		round1.setActions(actions);
		
		this.setRound1(round1);
		/**********************第一轮end*************************/
		if (!gotto) {//如果第一轮已经完胜或者完败
			return ;
		}
		/***********************第二轮 begin****************************/		
		characters = new ArrayList<Characters>();
		character = new Characters();
		Round round2 = new Round();
		actions = new ArrayList<Actions>();
		action = new Actions();
		damages = new ArrayList<Damage>();
		damage = new Damage();
		round2.setGotTO(gotto);
		
		for (int i = 0; i < userArray.size(); ) {
			Battle battle = userArray.get(i);
			if (battle.getHealth() == 0) {
				userArray.remove(i);
				continue;
			}
			character = new Characters();
			character.setItemid(battle.getItemid());
			character.setLife(battle.getHealth());
			character.setPower(battle.getRp());
			character.setPosition(i * 2);
			character.setLevel(battle.getLevel());
			characters.add(character);
			i++;
		}
		for (int i = 0; i < enemyArray.size();) {
			Battle battle = enemyArray.get(i);
			if (battle.getHealth() == 0) {
				enemyArray.remove(i);
				continue;
			}
			character = new Characters();
			character.setItemid(battle.getItemid());
			character.setLife(battle.getHealth());
			character.setPower(battle.getRp());
			character.setPosition(i * 2 +1);
			character.setLevel(battle.getLevel());
			characters.add(character);
			i++;
		}
		round2.setCharacters(characters);
		userLeftCount = userArray.size();
		enemyLeftCount = enemyArray.size();
		
		minCount = Math.min(userArray.size(), enemyArray.size());
		
		for (int i = 0; i < 11; i++) {
			if (userLeftCount <= 0 || enemyLeftCount <= 0) {
				gotto = false;
				if (userLeftCount <= 0) {
					this.setResult(4);
				}
				else {
					this.setResult(1);
				}
				break;
			}
			for (int j = 0; j < minCount; j++) {
				int rand = random.nextInt(100) + 1;
				action = new Actions();
				damage = new Damage();
				damages = new ArrayList<Damage>();
				Battle userBattle = userArray.get(j);
				Battle enemyBattle = enemyArray.get(j);
				if (userBattle.getHealth() <= 0 || enemyBattle.getHealth() <= 0) {//一方已死亡，直接跳过
					continue;
				}
				if ((i + j) % 2 != firstHand) {	
					action.setPosition(j * 2);
					if (rand > userSkillRates.get(userBattle.getItemid())) {
						action.setType(0);
						damage.setPosition(j * 2 + 1);
						int damageNum = userBattle.getAttack() - enemyBattle.getDefence();
						damageNum = damageNum < 1 ? 1 : damageNum;
						damage.setHarmValue(damageNum);
						damages.add(damage);
						action.setDamages(damages);
						actions.add(action);
						int leftHealth = enemyBattle.getHealth() - damageNum < 0 ? 0 : enemyBattle.getHealth() - damageNum;
						enemyBattle.setHealth(leftHealth);
						if (leftHealth <= 0) {
							enemyLeftCount --;
						}
					}
					else {
						action.setType(1);
						int index = 0;
						ArrayList<ArrayList<Integer>> randSkill = userSkill.get(userBattle.getItemid());
						rand = random.nextInt(userSkillRates.get(userBattle.getItemid()));
						ArrayList<Integer> rateArrayList = randSkill.get(3);
						for (int k = 0; k < rateArrayList.size(); k++) {
							if (rand <= rateArrayList.get(k)) {
								index = k;
								break;
							}
						}
						action.setSkillId(randSkill.get(0).get(index));
						if (randSkill.get(1).get(index) == 1) {//单体攻击技能
							int damageNum = userBattle.getAttack() + (userBattle.getRp()/100) * randSkill.get(2).get(index) - enemyBattle.getDefence();
							damageNum = damageNum < 1 ? 1 : damageNum;
							damage.setPosition(j * 2 + 1);
							damage.setHarmValue(damageNum);
							damages.add(damage);
							action.setDamages(damages);
							actions.add(action);
							int leftHealth = enemyBattle.getHealth() - damageNum < 0 ? 0 : enemyBattle.getHealth() - damageNum;
							enemyBattle.setHealth(leftHealth);
							if (leftHealth <= 0) {
								enemyLeftCount --;
							}
						}
						else {//群体攻击
							int attack = userBattle.getAttack() + (userBattle.getRp()/100) * randSkill.get(2).get(index);
							for (int k = 0; k < enemyArray.size(); k++) {
								if (enemyArray.get(k).getHealth() <= 0) {
									continue;
								}
								int distence = Math.abs(j - k);
								int damageNum = this.getDistenceDamage(attack, distence) - enemyArray.get(k).getDefence();
								damage = new Damage();
								damageNum = damageNum < 1 ? 1 : damageNum;
								damage.setPosition(k * 2 + 1);
								damage.setHarmValue(damageNum);
								damages.add(damage);
								int leftHealth = enemyArray.get(k).getHealth() - damageNum < 0 ? 0 : enemyArray.get(k).getHealth() - damageNum;
								enemyArray.get(k).setHealth(leftHealth);
								if (leftHealth <= 0) {
									enemyLeftCount --;
								}
							}
							action.setDamages(damages);
							actions.add(action);
						}
						
					}
					
				}
				else {
					action.setPosition(j * 2 + 1);
					
					if (rand > enemySkillRates.get(enemyBattle.getItemid())) {
						action.setType(0);
						damage.setPosition(j * 2);
						int damageNum = enemyBattle.getAttack() - userBattle.getDefence();
						damageNum = damageNum < 1 ? 1 : damageNum;
						damage.setHarmValue(damageNum);
						damages.add(damage);
						action.setDamages(damages);
						actions.add(action);
						int leftHealth = userBattle.getHealth() - damageNum < 0 ? 0 : userBattle.getHealth() - damageNum;
						userBattle.setHealth(leftHealth);
						if (leftHealth <= 0) {
							userLeftCount --;
						}
					}
					else {
						action.setType(1);
						int index = 0;
						
						ArrayList<ArrayList<Integer>> randSkill = enemySkill.get(enemyBattle.getItemid());
						rand = random.nextInt(enemySkillRates.get(enemyBattle.getItemid()));
						ArrayList<Integer> rateArrayList = randSkill.get(3);
						for (int k = 0; k < rateArrayList.size(); k++) {
							if (rand <= rateArrayList.get(k)) {
								index = k;
								break;
							}
						}
						action.setSkillId(randSkill.get(0).get(index));
						
						if (randSkill.get(1).get(index) == 1) {//单体攻击技能
							int damageNum = enemyBattle.getAttack() + (enemyBattle.getRp()/100) * randSkill.get(2).get(index) - userBattle.getDefence();
							damageNum = damageNum < 1 ? 1 : damageNum;
							damage.setPosition(j * 2);
							damage.setHarmValue(damageNum);
							damages.add(damage);
							action.setDamages(damages);
							actions.add(action);
							int leftHealth = userBattle.getHealth() - damageNum < 0 ? 0 : userBattle.getHealth() - damageNum;
							userBattle.setHealth(leftHealth);
							if (leftHealth <= 0) {
								userLeftCount --;
							}
						}
						else {//群体攻击
							int attack = enemyBattle.getAttack() + (enemyBattle.getRp()/100) * randSkill.get(2).get(index);
							for (int k = 0; k < userArray.size(); k++) {
								if (userArray.get(k).getHealth() <= 0) {
									continue;
								}
								int distence = Math.abs(j - k);
								int damageNum = this.getDistenceDamage(attack, distence) - userArray.get(k).getDefence();
								damage = new Damage();
								damageNum = damageNum < 1 ? 1 : damageNum;
								damage.setPosition(k * 2);
								damage.setHarmValue(damageNum);
								damages.add(damage);
								int leftHealth = userArray.get(k).getHealth() - damageNum < 0 ? 0 : userArray.get(k).getHealth() - damageNum;
								userArray.get(k).setHealth(leftHealth);
								if (leftHealth <= 0) {
									userLeftCount --;
								}
							}
							action.setDamages(damages);
							actions.add(action);
						}
					}
				}
			}
			
		}
		round2.setActions(actions);
		this.setRound2(round2);
		/***********************第二轮 end******************************/
		if (!gotto) {//如果第二轮已经胜利或者失败
			return ;
		}
		/**********************第三轮 begin*****************************/
		FinalRound finalRound = new FinalRound();
		
		characters = new ArrayList<Characters>();
		int userTotalHealth  = 0;
		int userTotalRp = 0;
		int enemyTotalHealth = 0;
		int enemyTotalRp = 0;
		finalRound.setGotto(gotto);
		for (int i = 0; i < userArray.size(); ) {
			Battle battle = userArray.get(i);
			if (battle.getHealth() == 0) {
				userArray.remove(i);
				continue;
			}
			character = new Characters();
			character.setItemid(battle.getItemid());
			character.setLife(battle.getHealth());
			character.setPower(battle.getRp());
			character.setPosition(i * 2);
			character.setLevel(battle.getLevel());
			characters.add(character);
			userTotalHealth += battle.getHealth();
			userTotalRp += battle.getRp();
			i++;
		}
		for (int i = 0; i < enemyArray.size();) {
			Battle battle = enemyArray.get(i);
			if (battle.getHealth() == 0) {
				enemyArray.remove(i);
				continue;
			}
			character = new Characters();
			character.setItemid(battle.getItemid());
			character.setLife(battle.getHealth());
			character.setPower(battle.getRp());
			character.setPosition(i * 2 +1);
			character.setLevel(battle.getLevel());
			characters.add(character);
			enemyTotalHealth += battle.getHealth();
			enemyTotalRp += battle.getRp();
			i++;
		}
		
		finalRound.setCharacters(characters);
		
		if ((userTotalHealth + userTotalRp) > (enemyTotalHealth + enemyTotalRp)) {
			finalRound.setWin(true);
			this.setResult(2);
		}
		else if((userTotalHealth + userTotalRp) < (enemyTotalHealth + enemyTotalRp)) {
			finalRound.setWin(false);
			this.setResult(3);
		}
		else {
			int a = random.nextInt(2);
			if (a == 0) {
				finalRound.setWin(true);
				this.setResult(2);
			}
			else {
				finalRound.setWin(false);
				this.setResult(3);
			}
		}
		this.setFInalRound(finalRound);
		/**********************第三轮 end*******************************/
	}
	
	public int getDistenceDamage(int orginDamage,int distence){
		int damage = 0;
		switch (distence) {
			case 0:
				damage = orginDamage * 1;
				break;
			case 1:
				damage = (int) Math.round(orginDamage * 0.67);
				break;
			case 2:
				damage = (int) Math.round(orginDamage * 0.53);
				break;
			case 3:
				damage = (int) Math.round(orginDamage * 0.4);
				break;
			case 4:
				damage = (int) Math.round(orginDamage * 0.3);
				break;
			case 5:
				damage = (int) Math.round(orginDamage * 0.2);
				break;
			case 6:
				damage = (int) Math.round(orginDamage * 0.13);
				break;
			case 7:
				damage = (int) Math.round(orginDamage * 0.08);
				break;
		}
		
		return damage;
	}
	
	public int worldBossFight(BattleArrayObject userBattleArray,float addQuantity){
		WorldBossConfig bossConfig = XMLTemplateService.getWorldBossConfig();
		ArrayList<Battle> orginUserArray = userBattleArray.getBattlearrays();
		ArrayList<Battle> userArray = new ArrayList<Battle>();
		int totalDamage = 0;
		ArrayList<Characters> characters = new ArrayList<Characters>();
		Characters character = new Characters();
		
		HashMap<Integer, Integer> userSkillRates = new HashMap<Integer, Integer>();
		HashMap<Integer, ArrayList<ArrayList<Integer>>> userSkill = new HashMap<Integer, ArrayList<ArrayList<Integer>>>();
		
		ArrayList<Actions> actions = new ArrayList<Actions>();
		Actions action = new Actions();
		ArrayList<Damage> damages = new ArrayList<Damage>();
		Damage damage = new Damage();
		
		Round round1 = new Round();
		round1.setGotTO(true);
		
		int position = 0;
		for (int i = 0; i < orginUserArray.size(); i++) {
			ArrayList<Integer> skillIds = new ArrayList<Integer>();
			ArrayList<Integer> skillTypes = new ArrayList<Integer>();
			ArrayList<Integer> skillQuantity = new ArrayList<Integer>();
			ArrayList<Integer> skillRates = new ArrayList<Integer>();
			ArrayList<ArrayList<Integer>> skills = new ArrayList<ArrayList<Integer>>();
			int rate = 0;
			Battle battle = orginUserArray.get(i).clone();
			if (battle.getDiscipleid() == 0) {
				continue;
			}
			battle.setAttack((int)(battle.getAttack() * (1 + addQuantity)));
			character = new Characters();
			userArray.add(battle);
			character.setItemid(battle.getItemid());
			character.setLife(battle.getHealth());
			character.setPower(battle.getRp());
			character.setPosition(position * 2);
			character.setLevel(battle.getLevel());
			characters.add(character);
			
			if (battle.getDefaultskilltype() == 1 || battle.getDefaultskilltype() == 2) {
				SkillXmlTemplate skillInfo = XMLTemplateService.getSkillTemplate(battle.getDefaultskillitem());
				rate += Math.round(skillInfo.getRate() * 100);
				skillIds.add(skillInfo.getId());
				skillTypes.add(skillInfo.getType());
				skillQuantity.add((int)battle.getDefaultskillquantity());
				skillRates.add(rate);
				
			}
			else {
				action = new Actions();
				action.setType(1);
				action.setSkillId(battle.getDefaultskillitem());
				action.setPosition(position * 2);
				actions.add(action);
			}
			
			if (battle.getSkill1type() == 1 || battle.getSkill1type() == 2) {
				SkillXmlTemplate skillInfo = XMLTemplateService.getSkillTemplate(battle.getSkill1item());
				rate += Math.round(skillInfo.getRate() * 100);
				skillIds.add(skillInfo.getId());
				skillTypes.add(skillInfo.getType());
				skillQuantity.add((int)battle.getSkill1quantity());
				skillRates.add(rate);
			}
			else if (battle.getSkill1type() != 0){
				action = new Actions();
				action.setType(1);
				action.setSkillId(battle.getSkill1item());
				action.setPosition(position * 2);
				actions.add(action);
			}
			
			if (battle.getSkill2type() == 1 || battle.getSkill2type() == 2) {
				SkillXmlTemplate skillInfo = XMLTemplateService.getSkillTemplate(battle.getSkill2item());
				rate += Math.round(skillInfo.getRate() * 100);
				skillIds.add(skillInfo.getId());
				skillTypes.add(skillInfo.getType());
				skillQuantity.add((int)battle.getSkill2quantity());
				skillRates.add(rate);
			}
			else if (battle.getSkill2type() != 0) {
				action = new Actions();
				action.setType(1);
				action.setSkillId(battle.getSkill2item());
				action.setPosition(position * 2);
				actions.add(action);
			}
			
			userSkillRates.put(battle.getItemid(), rate);
			skills.add(skillIds);
			skills.add(skillTypes);
			skills.add(skillQuantity);
			skills.add(skillRates);
			userSkill.put(battle.getItemid(), skills);
			
			position ++;
		}
		
		int battleCount = userArray.size();
		
		int npcItemId = bossConfig.getNpcItemId();
		for (int i = 0; i < battleCount; i++) {
			character = new Characters();
			character.setItemid(npcItemId);
			character.setLife(-1);
			character.setPower(0);
			character.setPosition(i * 2 + 1);
			character.setLevel(999);
			characters.add(character);
		}
		
		Random random = new Random();
		round1.setCharacters(characters);
		
		for (int i = 0; i < battleCount; i++) {
			action = new Actions();
			damage = new Damage();
			damages = new ArrayList<Damage>();
			Battle userBattle = userArray.get(i);
			action.setPosition(i*2);
			if (userSkillRates.get(userBattle.getItemid()) > 0) {
				action.setType(1);
				int index = 0;
				ArrayList<ArrayList<Integer>> randSkill = userSkill.get(userBattle.getItemid());
				int rand = random.nextInt(userSkillRates.get(userBattle.getItemid()));
				ArrayList<Integer> rateArrayList = randSkill.get(3);
				for (int k = 0; k < rateArrayList.size(); k++) {
					if (rand <= rateArrayList.get(k)) {
						index = k;
						break;
					}
				}
				action.setSkillId(randSkill.get(0).get(index));
				
				if (randSkill.get(1).get(index) == 1) {//单体攻击技能
					int damageNum = userBattle.getAttack() + (userBattle.getRp()/100) * randSkill.get(2).get(index);
					damageNum = damageNum < 1 ? 1 : damageNum;
					damage.setPosition(i * 2 + 1);
					damage.setHarmValue(damageNum);
					damages.add(damage);
					action.setDamages(damages);
					actions.add(action);
					
					totalDamage += damageNum;
				}
				else {//群体攻击
					int attack = userBattle.getAttack() + (userBattle.getRp()/100) * randSkill.get(2).get(index);
					for (int k = 0; k < battleCount; k++) {
						int distence = Math.abs(i - k);
						int damageNum = this.getDistenceDamage(attack, distence);
						damage = new Damage();
						damageNum = damageNum < 1 ? 1 : damageNum;
						damage.setPosition(k * 2 + 1);
						damage.setHarmValue(damageNum);
						damages.add(damage);
						
						totalDamage += damageNum;
					}
					action.setDamages(damages);
					actions.add(action);
				}
			}
			else {
				action.setType(0);
				damage.setPosition(i * 2 + 1);
				int damageNum = userBattle.getAttack();
				damageNum = damageNum < 1 ? 1 : damageNum;
				damage.setHarmValue(damageNum);
				damages.add(damage);
				action.setDamages(damages);
				actions.add(action);
			}
			
		}
		
		round1.setActions(actions);
		this.setRound1(round1);
		
		return totalDamage;
	}
}
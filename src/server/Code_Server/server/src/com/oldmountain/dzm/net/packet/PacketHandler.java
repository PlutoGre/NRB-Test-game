package com.oldmountain.dzm.net.packet;

import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;

import org.dom4j.Branch;

import com.oldmountain.dzm.DZM;
import com.oldmountain.dzm.message.OP;
import com.oldmountain.dzm.message.OPLeague;
import com.oldmountain.dzm.message.LeagueStructExt.OPLeaguaShopBuy;
import com.oldmountain.dzm.packethandler.*;

public class PacketHandler extends Thread{ 
	static BlockingQueue<Packet> inqueue =  new LinkedBlockingQueue<Packet>(); 
	
	public static void RecvPacket( Packet packet )
	{
		inqueue.add(packet);
	}
	 
	public static void Update() throws InterruptedException
	{
		while( !inqueue.isEmpty() )
		{
			Packet packet = inqueue.poll();
			PacketHandler.handle(packet);
		}
	}
	
	/**
	 * 
	 * @param packet
	 */
	public static void handle(Packet packet)
    {
        //TODO:非法请求判断
		switch (packet.opcode)
        {
			//登陆
			case OP.code.OPCODE_PLAYER_LOGIN_C_VALUE:
				LoginHandler.login(packet, packet.session);
				break;
			//增加体力
			case OP.code.OPCODE_ADD_POWER_C_VALUE:
				AddPowerHandler.addPower(packet, packet.session);
				break;
			//获取用户基础信息接口
			case OP.code.OPCODE_GET_USER_BASIC_INFO_C_VALUE:
				UserBasicInfoHandler.getUserBasicInfo(packet, packet.session);
				break;
			//获取用户弟子列表信息
			case OP.code.OPCODE_GET_USER_DISCIPLE_INFO_C_VALUE:
				UserDiscipleInfoHandler.getUserDiscipleInfo(packet, packet.session);
				break;
			//获取用户装备列表信息
			case OP.code.OPCODE_GET_USER_EQUIP_INFO_C_VALUE:
				UserEquipInfoHandler.getUserEquipInfo(packet, packet.session);
				break;
			//获取用户技能列表信息
			case OP.code.OPCODE_GET_USER_SKILL_INFO_C_VALUE:
				UserSkillInfoHandler.getUserSkillInfo(packet, packet.session);
				break;
			//获取用户江湖信息
			case OP.code.OPCODE_GET_USER_CAREER_INFO_C_VALUE:
				UserCareerInfoHandler.getUserCareerInfo(packet, packet.session);
				break;
			//获取包裹道具信息
			case OP.code.OPCODE_GET_USER_TOOL_INFO_C_VALUE:
				UserToolInfoHandler.getUserToolInfo(packet, packet.session);
				break;
			//使用道具	
			case OP.code.OPCODE_USE_TOOL_C_VALUE:
				UseToolHandler.useTool(packet, packet.session);
				break;
			//获取用户魂魄信息
			case OP.code.OPCODE_GET_USER_SOUL_INFO_C_VALUE:
				UserSoulInfoHandler.getUserSoulInfo(packet, packet.session);
				break;
			//获取充值列表信息
			case OP.code.OPCODE_GET_SHOP_ITEM_LIST_C_VALUE:
				GetShopListHandler.getShopList(packet, packet.session);
				break;
			//充值漏单检查
			case OP.code.OPCODE_GET_SHOP_PURCHASE_CHECK_C_VALUE:
				ShopPurchaseCheckHandler.shopPurchaseCheck(packet, packet.session);
				break;
            //留言接口
            case OP.code.OPCODE_SEND_FRIEND_MESSAGE_C_VALUE:
                SendFriendMessageHandler.SendFriendMessage(packet,packet.session);
                break;
            //处理传书信息接口
            case OP.code.OPCODE_DISPOSE_USER_SYSMSG_INFO_C_VALUE:
                DisposeSysMsgInfoHandler.DisposeSysMsgInfo(packet,packet.session);
                break;
            //处理恩怨信息接口
            case OP.code.OPCODE_DISPOSE_USER_FRIENDS_INFO_C_VALUE:
                DisposeFriendsInfoHandler.DisposeFriendInfo(packet, packet.session);
                break;
            //返回传书列表
            case OP.code.OPCODE_GET_USER_SYSMSG_INFO_C_VALUE:
                UserSysMsgInfoHandler.getSysMsgInfo(packet, packet.session);
                break;
            //返回恩怨列表
            case OP.code.OPCODE_GET_USER_FRIENDS_INFO_C_VALUE:
                UserFriendsInfoHandler.getFriendsInfo(packet, packet.session);
                break;
            //进入聊天界面    
            case OP.code.OPCODE_ENTER_CHAT_PAGE_C_VALUE:
            	ChatSendMsgInfoHandler.enterPage(packet, packet.session);
            	break;
            //退出聊天界面
            case OP.code.OPCODE_QUIT_CHAT_PAGE_C_VALUE:
            	ChatSendMsgInfoHandler.leavePage(packet, packet.session);
            	break;
            //发送聊天信息
            case OP.code.OPCODE_CHAT_SEND_MSG_C_VALUE:
                ChatSendMsgInfoHandler.sendChatInfo(packet, packet.session);
                break;
            //广播炫耀列表
            case OP.code.OPCODE_GET_BOARDCAST_INFO_C_VALUE:
                BoardCastInfoHandler.getBoardCast(packet, packet.session);
                break;
            //更新设置面板
            case OP.code.OPCODE_CHANGE_SETTING_C_VALUE:
                ChangeSettingHandler.changeSetting(packet, packet.session);
                break;
            //传功接口
            case OP.code.OPCODE_SWALLOW_DISCIPLE_C_VALUE:
            	SwallowDiscipleHandler.swallowDisciple(packet, packet.session);
                break;
            //培养接口
            case OP.code.OPCODE_TRAIN_DISCIPLE_C_VALUE:
            	TrainDiscipleHandler.trainDisciple(packet, packet.session);
                break;
            //培养确认接口
            case OP.code.OPCODE_DEAL_TRAIN_STATUS_C_VALUE:
            	DealTrainStatusHandler.dealTrainStatus(packet, packet.session);
                break;
            //获取连续登陆奖励信息
            case OP.code.OPCODE_GET_CONTINUE_LOGIN_C_VALUE:
                ContinueLoginHandler.getContinueLogin(packet, packet.session);
                break;
            //领取连续登陆奖励
            case OP.code.OPCODE_REWARD_CONTINUE_LOGIN_C_VALUE:
            	ContinueLoginHandler.rewardContinueLogin(packet, packet.session);
                break;
            //技能升级
            case OP.code.OPCODE_UPGRADE_SKILL_C_VALUE:
            	UpgradeSkillHandler.upgradeSkill(packet, packet.session);
            	break;
            //获取玩家的阵容信息
            case OP.code.OPCODE_GET_USER_BATTLE_ARRAY_INFO_C_VALUE:
            	UserBattleArrayHandler.getUserBattleArray(packet, packet.session);
            	break;
            //弟子突破包
            case OP.code.OPCODE_UPGRADE_DISCIPLE_C_VALUE:
            	UpgradeDiscipleHandler.upgradeDisciple(packet, packet.session);
                break;
            //招募弟子
            case OP.code.OPCODE_RECRUIT_DISCIPLE_C_VALUE:
            	RecruitDiscipleHandler.recruitDisciple(packet, packet.session);
                break;
            //出售装备
            case OP.code.OPCODE_SELL_EQUIP_C_VALUE:
            	SellEquipHandler.sellEquip(packet, packet.session);
                break;
            //获取奇遇面板展现列表
            case OP.code.OPCODE_GET_ADVENTURE_INFO_C_VALUE:
                AdventureInfoHandler.getAdventureInfo(packet, packet.session);
                break;
            //获取吃叫花鸡活动信息
            case OP.code.OPCODE_GET_ADVENTURE_POWERINFO_C_VALUE:
                AddPowerHandler.getAdventurePowerStats(packet, packet.session);
                break;
            //获取无量洞信息
            case OP.code.OPCODE_GET_ADVENTURE_BOUNDLESSHOLEINFO_C_VALUE:
                GetAdventureBoundlessHoleHandler.getBoundlessHole(packet, packet.session);
                break;
            //累加无量洞参拜次数
            case OP.code.OPCODE_ADD_ADVENTURE_BOUNDLESSHOLEINFO_C_VALUE:
                AddBoundlessHoleHandler.addBoundlessHole(packet, packet.session);
                break;
            //升级装备
            case OP.code.OPCODE_UPGRADE_EQUIP_C_VALUE:
            	UpgradeEquipHandler.doUpgradeEquip(packet, packet.session);
                break;
            //获取血战到底活动数据信息
            case OP.code.OPCODE_GET_FIGHTEND_INFO_C_VALUE:
                FightEndInfoHandler.getFightEndInfo(packet, packet.session);
                break;
            //血战到底请求战斗
            case OP.code.OPCODE_FIGHTEND_BATTLE_C_VALUE:
                FightEndBattleHandler.doFightBattleInfo(packet, packet.session);
                break;
            //图鉴接口
            case OP.code.OPCODE_HANDBOOK_INFO_C_VALUE:
            	HandbookInfoHandler.getHandbookInfo(packet, packet.session);
                break;
            //请求指点信息
            case OP.code.OPCODE_GET_ADVENTURE_TEACHERINFO_C_VALUE:
                GetAdventureTeacherInfoHandler.getInfo(packet,packet.session);
                break;
            //指点获取经验
            case OP.code.OPCODE_ADD_ADVENTURE_TEACHEXP_C_VALUE:
                AddAdventureTeachExpHandler.addExp(packet,packet.session);
                break;
            //血战到底添加昨日属性加成
            case OP.code.OPCODE_FIGHTEND_ADD_YEST_ATTR_INFO_C_VALUE:
                FightEndAddYestAttrHandler.addAttr(packet, packet.session);
                break;
            //血战到底临时属性加成
            case OP.code.OPCODE_FIGHTEND_ADD_TEMP_ATTR_INFO_C_VALUE:
                FightEndTempAddStepAttrHandler.addAttr(packet, packet.session);
                break;
            //血战到底领取奖励
            case OP.code.OPCODE_GET_FIGHTEND_REWARD_INFO_C_VALUE:
                FightEndGetRewardHandler.gerReward(packet, packet.session);
                break;
            //血战到底排行榜
            case OP.code.OPCODE_GET_FIGHTEND_RANK_INFO_C_VALUE:
                FightEndRankInfoHandler.getFightEndInfo(packet, packet.session);
                break;
            //更换阵容技能
            case OP.code.OPCODE_CHANGE_SKILL_C_VALUE:
            	ChangeSkillHandler.changeSkill(packet, packet.session);
                break;
             //更换阵容装备
            case OP.code.OPCODE_CHANGE_EQUIP_C_VALUE:
            	ChangeEquipHandler.changeEquip(packet, packet.session);
                break;
            //获取玩家的集市信息
            case OP.code.OPCODE_GET_USER_MARKET_INFO_C_VALUE:
            	UserMarketInfoHandler.getUserMarketInfo(packet, packet.session);
            	break;
            //集市招募弟子
            case OP.code.OPCODE_MARKET_RECRUIT_DISCIPLE_C_VALUE:
            	MarketRecruitDiscipleHandler.recruitDisciple(packet, packet.session);
            	break;
            //战斗接口
            case OP.code.OPCODE_USER_BATTLE_C_VALUE:
            	UserBattleHandler.getBattleResult(packet, packet.session);
            	break;
            //弟子上阵
            case OP.code.OPCODE_CHANGE_DISCIPLE_C_VALUE:
            	ChangeDiscipleHandler.changeDiscipleHandler(packet, packet.session);
            	break;
            //获取掌门决信息
            case OP.code.OPCODE_GET_TEAM_BUFF_INFO_C_VALUE:
                TeamBuffInfoHandler.getTeamBuffInfo(packet, packet.session);
                break;
            //升级掌门决信息
            case OP.code.OPCODE_UPGRADE_TEAM_BUFF_INFO_C_VALUE:
                TeamBuffInfoHandler.upgradeTeamBuffInfo(packet, packet.session);
                break;
            //购买道具
            case OP.code.OPCODE_BUY_TOOLS_C_VALUE:
                BuyToolHandler.buyTool(packet, packet.session);
                break;
            //升级领取VIP奖励信息
            case OP.code.OPCODE_ADVENTURE_LEVEL_UP_REWARD_C_VALUE:
            	AdventureLevelUpRewardInfoHandler.getInfo(packet, packet.session);
            	break;
            //升级领取VIP奖励
            case OP.code.OPCODE_ADVENTURE_LEVEL_UP_GET_REWARD_C_VALUE:
            	AdventureLevelUpGetRewardInfoHandler.getInfo(packet, packet.session);
                break;
            //阵容换位置
            case OP.code.OPCODE_CHANGE_BATTLE_POSITION_C_VALUE:
            	ChangeBattlePositionHandler.changeBattlePosition(packet, packet.session);
            	break;
            //充值关卡技术计数
            case OP.code.OPCODE_RESET_CAREER_COUNT_C_VALUE:
            	UserCareerInfoHandler.resetCount(packet, packet.session);
            	break;
            //充值接口
            case OP.code.OPCODE_RECHARGE_C_VALUE:
            	RechargeHandler.recharge(packet, packet.session);
            	break;
            //获取用户气势接口
            case OP.code.OPCODE_GET_TITLE_INFO_C_VALUE:
            	TitleInfoHandler.getTitleInfo(packet, packet.session);
            	break;
            //获取用户论剑信息
            case OP.code.OPCODE_USER_ARENA_INFO_C_VALUE:
            	ChallengeHandler.getUserChallengeInfo(packet, packet.session);
            	break;
            //获取论剑列表
            case OP.code.OPCODE_GET_ARENA_OPPONENT_LIST_C_VALUE:
            	ChallengeHandler.getEnemyList(packet, packet.session);
            	break;
            //刷新积分
            case OP.code.OPCODE_REFRESH_ARENA_C_VALUE:
            	ChallengeHandler.refreshScore(packet, packet.session);
            	break;
            //积分兑换培养单
            case OP.code.OPCODE_GET_ARENA_REWARD_C_VALUE:
            	ChallengeHandler.getReward(packet, packet.session);
            	break;
            //挑战玩家
            case OP.code.OPCODE_CHALLENGE_OPPONENT_C_VALUE:
            	ChallengeHandler.doChallenge(packet, packet.session);
            	break;
            case OP.code.OPCODE_INSTRUCTION_C_VALUE:
            	InstructionHandler.doInstruction(packet, packet.session);
            	break;
            //GM指令
            case OP.code.OPCODE_UPDATE_USERINFO_BY_GM_C_VALUE:
            	if (DZM.canUseGmTools == 1) {
            		UpdateUserInfoByGMhandler.updateUserInfo(packet, packet.session);
				}
            	break;
            //江湖吃鸡
            case OP.code.OPCODE_ADDPOWER_CAREER_C_VALUE:
                AddPowerHandler.addPowerCareer(packet,packet.session);
                break;
            case OP.code.OPCODE_ADD_ARENA_NUM_C_VALUE:
                AddArenaNumHandler.addArenaNum(packet,packet.session);
                break;
            case OP.code.OPCODE_CHANE_NAME_C_VALUE:
                ChangeNameHandler.changeName(packet,packet.session);
                break;
            case OP.code.OPCODE_HEARTBEAT_C_VALUE:
            	HeartBeatHandler.doHeartBeat(packet, packet.session);
            	break;
            case OP.code.OPCODE_BACK_STAGE_FETCH_USER_INFO_C_VALUE:
            	BackStageFetchUserInfoHandler.doFetchUserInfo(packet, packet.session);
            	break;
            case OP.code.OPCODE_BACK_STAGE_UPDATE_USER_INFO_C_VALUE:
            	BackStageUpdateUserInfoHandler.doUpdateUserInfo(packet, packet.session);
            	break;
            case OP.code.OPCODE_ADVENTURE_CONTINUELOGIN_GET_INFO_C_VALUE:
                AdventureContinueLoginHandler.getInfo(packet,packet.session);
                break;
            case OP.code.OPCODE_ADVENTURE_CONTINUELOGIN_REWARD_C_VALUE:
                AdventureContinueLoginHandler.getReward(packet,packet.session);
                break;
            case OP.code.OPCODE_BACK_STAGE_PUSH_REWARD_INFO_C_VALUE:
                BackStagePushRewardInfoHandler.doPushReward(packet, packet.session);
                break;
                //cdkey奖励领取
            case OP.code.OPCODE_CDKEY_REWARD_C_VALUE:
            	CdkeyRewardHandler.getReward(packet,packet.session);
            	break;
            case OP.code.OPCODE_ADD_VITALITY_C_VALUE:
            	AddVitalityHandler.addVitality(packet, packet.session);
            	break;
            //残章信息获取接口
            case OP.code.OPCODE_GET_SKILL_BOOK_INFO_C_VALUE:
                SkillBookInfoHandler.getSkillBookInfo(packet,packet.session);
                break;
            //残章玩家列表
            case OP.code.OPCODE_GET_SKILL_BOOK_LOOT_INFO_C_VALUE:
                GetSkillBookLootHandler.getSkillBookLoot(packet,packet.session);
                break;
            //抢夺残章
            case OP.code.OPCODE_SNATCH_SKILL_BOOK_INFO_C_VALUE:
                SnatchSkillBookHandler.snatchSkillBook(packet,packet.session);
                break;
            //拼合残章
            case OP.code.OPCODE_PIECE_SKILL_BOOK_C_VALUE:
                PieceSkillBookHandler.pieceSkillBook(packet,packet.session);
                break;
			//查询用户运气值
            case OP.code.OPCODE_BACK_STAGE_FETCH_LUCKY_C_VALUE:
            	BackStageFetchLuckyHandler.doFetchLucky(packet, packet.session);
            	break;
            //记录传书阅读时间
            case OP.code.OPCODE_RECORDREADMSGTIME_C_VALUE:
                UserSysMsgInfoHandler.recordReadMsgTime(packet,packet.session);
                break;
            //特殊引导购买物品
            case OP.code.OPCODE_GUIDE_BUY_TOOL_C_VALUE:
            	GuideBuyToolHandler.buyTool(packet,packet.session);
            	break;
            //特殊引导记录
            case OP.code.OPCODE_GUIDE_RECORD_C_VALUE:
            	GuideRecordHandler.recordType(packet,packet.session);
            	break;
            //添加邀请码
            case OP.code.OPCODE_ADD_INVITEKEY_C_VALUE:
            	UserInvitedHandler.addInvitedKey(packet, packet.session);
            	break;
            //获取用户的邀请状态
            case OP.code.OPCODE_USER_INVITESTATUS_C_VALUE:
            	UserInvitedHandler.getUserInviteStatus(packet, packet.session);
            	break;
            //领取邀请奖励
            case OP.code.OPCODE_GET_INVITE_REWARD_C_VALUE:
            	UserInvitedHandler.getInviteReward(packet, packet.session);
            	break;
            //充值返利
            case OP.code.OPCODE_CHARGE_REBATE_C_VALUE:
            	ChargeRebateHandler.getChargeRebateInfo(packet, packet.session);
            	break;
            //财神到
            case OP.code.OPCODE_FORTUNE_COME_C_VALUE:
            	FortuneComeHandler.getFortuneComeInfo(packet, packet.session);
            	break;
            //挑战世界boss
            case OP.code.OPCODE_WORLDBOSS_FIGHT_C_VALUE:
            	WorldBossHandler.WorldBossFight(packet, packet.session);
            	break;
            //刷新世界boss信息
            case OP.code.OPCODE_WORLDBOSS_REFRESH_C_VALUE:
            	WorldBossHandler.refreshWorldBoss(packet, packet.session);
            	break;
            //获取世界boss信息
            case OP.code.OPCODE_GET_WORLD_BOSS_INFO_C_VALUE:
            	WorldBossHandler.getWorldBossInfo(packet, packet.session);
            	break;
            //查看阵容
            case OP.code.OPCODE_SHOW_USER_BATTLE_C_VALUE:
            	ShowUserBattleArrayHandler.showBattleArray(packet, packet.session);
            	break;
            //激活码
            case OP.code.OPCODE_ACTIVE_CODE_C_VALUE:
                ActiveCodeHandler.validateActiveCode(packet, packet.session);
                break;
             ///连闯
            case OP.code.OPCODE_QUICK_CAREER_BATTLE_C_VALUE:
                QuickCareerBattleHandle.quickCareer(packet, packet.session);
                break;
            case OP.code.OPCODE_DEEP_WATER_EXPLORE_C_VALUE:
            	DeepWaterExploreHandler.getInfo(packet, packet.session);
            	break;
            //宝石升级
            case OP.code.OPCODE_UPGRADE_STONEINFO_C_VALUE:
                EquipDiamondHandler.UpgradeStone(packet,packet.session);
                break;
            //移除宝石
            case OP.code.OPCODE_REMOVE_STONEINFO_C_VALUE:
                EquipDiamondHandler.RemoveStone(packet, packet.session);
                break;
            //镶嵌宝石
            case OP.code.OPCODE_INLAID_STONEINFO_C_VALUE:
                EquipDiamondHandler.InlaidStone(packet,packet.session);
                break;
            //装备打孔
            case OP.code.OPCODE_PUNCH_STONEINFO_C_VALUE:
                EquipDiamondHandler.PunchEquipStone(packet,packet.session);
                break;
            //获取用户联盟信息
            case OPLeague.code.OPCODE_GET_USERLEAGUAINFO_C_VALUE:
            	LeaguaHandler.getUserLeaguaInfo(packet, packet.session);
            	break;
            //创建联盟
            case OPLeague.code.OPCODE_CREATE_LEAGUA_C_VALUE:
            	LeaguaHandler.createLeagua(packet, packet.session);
            	break;
            //申请加入联盟
            case OPLeague.code.OPCODE_APPLY_LEAGUA_C_VALUE:
            	LeaguaHandler.applyLeagua(packet, packet.session);
            	break;
            //处理联盟申请
            case OPLeague.code.OPCODE_DEAL_APPLY_LEAGUA_C_VALUE:
            	LeaguaHandler.dealApply(packet, packet.session);
            	break;
            //获取联盟申请列表
            case OPLeague.code.OPCODE_GET_LEAGUA_APPLYINFO_C_VALUE:
            	LeaguaHandler.getLeaguaApplyInfo(packet, packet.session);
            	break;
            //获取本联盟图腾信息
            case OPLeague.code.OPCODE_GET_TOTEMINFO_C_VALUE:
            	LeaguaHandler.getTotemInfo(packet, packet.session);
            	break;
            //获取能攻打图腾的联盟信息列表
            case OPLeague.code.OPCODE_GET_ROB_LIST_VALUE:
            	LeaguaHandler.getLeaguaRobList(packet, packet.session);
            	break;
            //攻击他会图腾
            case OPLeague.code.OPCODE_ROB_LEAGUE_VALUE:
            	LeaguaHandler.robLeagua(packet, packet.session);
            	break;
            //图腾修复
            case OPLeague.code.OPCODE_REPAIR_TOTEM_C_VALUE:
            	LeaguaHandler.repairTotem(packet, packet.session);
            	break;
            //获取公会徽章列表
            case OPLeague.code.OPCODE_GET_LEAGUAMEDAL_C_VALUE:
            	LeaguaHandler.getLeaguaMedalInfo(packet, packet.session);
            	break;
            //领取徽章
            case OPLeague.code.OPCODE_RECEIVE_LEAGUAMEDAL_C_VALUE:
            	LeaguaHandler.receiveMedal(packet, packet.session);
            	break;
            //更换联盟当前徽章
            case OPLeague.code.OPCODE_CHANGE_LEAGUAMEDAL_VALUE:
            	LeaguaHandler.changeActiveMedal(packet, packet.session);
            	break;
            //获取成员列表
            case OPLeague.code.OPCODE_GET_LEAGUAMEMBER_C_VALUE:
            	LeaguaHandler.getLeaguaMembersList(packet, packet.session);
            	break;
            //退出联盟
            case OPLeague.code.OPCODE_QUIT_LEAGUA_C_VALUE:
            	LeaguaHandler.quitLeagua(packet, packet.session);
            	break;
            //踢出公会
            case OPLeague.code.OPCODE_FIRE_LEAGUA_MEMBER_C_VALUE:
            	LeaguaHandler.fireMember(packet, packet.session);
            	break;
            //转让公会
            case OPLeague.code.OPCODE_TRANSFER_LEAGUAOWNER_C_VALUE:
            	LeaguaHandler.transferLeaguaOwner(packet, packet.session);
            	break;
            //解散公会
            case OPLeague.code.OPCODE_DISBAND_LEAGUA_C_VALUE:
            	LeaguaHandler.disbandLeagua(packet, packet.session);
            	break;
            //提升成员等级
            case OPLeague.code.OPCODE_UPGRADE_LEAGUA_MEMBER_C_VALUE:
            	LeaguaHandler.upgradeMemberGrade(packet, packet.session);
            	break;
            //获取联盟建筑信息
            case OPLeague.code.OPCODE_GET_LEAGUABUILDING_C_VALUE:
            	LeaguaHandler.getLeaguaBuildingInfo(packet, packet.session);
            	break;
            //联盟建筑捐献升级
            case OPLeague.code.OPCODE_DONATE_BUILDING_C_VALUE:
            	LeaguaHandler.donateLeaguaBuilding(packet, packet.session);
            	break;
            //捐献联盟资金
            case OPLeague.code.OPCODE_DONATE_FOUNDS_C_VALUE:
            	LeaguaHandler.donateLeaguaFunds(packet, packet.session);
            	break;
            //获取联盟商店列表
            case OPLeague.code.OPCODE_GET_LEAGUASHOPINFO_C_VALUE:
            	LeaguaHandler.getLeaguaShopInfo(packet, packet.session);
            	break;
            //购买物品
            case OPLeague.code.OPCODE_LEAGUASHOPBUY_C_VALUE:
            	LeaguaHandler.leaguaShopBuy(packet, packet.session);
            	break;
            //修改联盟公告	
            case OPLeague.code.OPCODE_REFRESH_LEAGUA_BROADCAST_C_VALUE:
            	LeaguaHandler.refreshBroadcast(packet,packet.session);
            	break;
            //清空联盟商店购买cd
            case OPLeague.code.OPCODE_CLEAR_SHOPBUY_CD_VALUE:
            	LeaguaHandler.clearShopBuyCd(packet, packet.session);
            	break;
            //领取帮贡
            case OPLeague.code.OPCODE_RECEIVE_TOTEMCONTRIBUTION_C_VALUE:
            	LeaguaHandler.receiveTotemContribution(packet, packet.session);
            	break;
            //获取图腾修复需要的资金
            case OPLeague.code.OPCODE_TOTEM_REPAIR_COST_C_VALUE:
            	LeaguaHandler.getTotemRepairCost(packet, packet.session);
            	break;
            //获取女神回馈界面信息
            case OP.code.OPCODE_GODDESS_REBATE_INFO_C_VALUE:
            	GoddessRebateHandler.getGoddessRebateInfo(packet, packet.session);
            	break;
            //女神反馈活动,献花	
            case OP.code.OPCODE_GODDESS_GIVEFLOWER_C_VALUE:
            	GoddessRebateHandler.giveFlower(packet, packet.session);
            	break;
			default:
				break;
		}
	}
}

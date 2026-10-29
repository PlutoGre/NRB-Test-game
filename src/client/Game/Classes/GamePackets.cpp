
#include "stdafx.h"

#include "GamePackets.h"
#include "OP.pb.h"

#ifndef _REGISISTER_FACTORIES_
#define _REGISISTER_FACTORIES_

AUTO_REGISITER_PACKET_FACTORY(OPLogin,OPCODE_PLAYER_LOGIN_C);
AUTO_REGISITER_PACKET_FACTORY(OPLoginRet,OPCODE_PLAYER_LOGINRET_S);

AUTO_REGISITER_PACKET_FACTORY(OPUserBasicInfo,OPCODE_GET_USER_BASIC_INFO_C);
AUTO_REGISITER_PACKET_FACTORY(OPUserBasicInfoRet,OPCODE_GET_USER_BASIC_INFORET_S);

AUTO_REGISITER_PACKET_FACTORY(OPUserMarketInfo,OPCODE_GET_USER_MARKET_INFO_C);
AUTO_REGISITER_PACKET_FACTORY(OPUserMarketInfoRet,OPCODE_GET_USER_MARKET_INFORET_S);

AUTO_REGISITER_PACKET_FACTORY(OPUserDiscipleInfo,OPCODE_GET_USER_DISCIPLE_INFO_C);
AUTO_REGISITER_PACKET_FACTORY(OPUserDiscipleInfoRet,OPCODE_GET_USER_DISCIPLE_INFORET_S);

AUTO_REGISITER_PACKET_FACTORY(OPUserEquipleInfo,OPCODE_GET_USER_EQUIP_INFO_C);
AUTO_REGISITER_PACKET_FACTORY(OPUserEquipInfoRet,OPCODE_GET_USER_EQUIP_INFORET_S);

AUTO_REGISITER_PACKET_FACTORY(OPMarketRecruitDisciple,OPCODE_MARKET_RECRUIT_DISCIPLE_C);
AUTO_REGISITER_PACKET_FACTORY(OPMarketRecruitDiscipleRet,OPCODE_MARKET_RECRUIT_DISCIPLERET_S);

AUTO_REGISITER_PACKET_FACTORY(OPUpdateCareer,OPCODE_UPDATE_CAREER_C);
AUTO_REGISITER_PACKET_FACTORY(OPUpdateCareerRet,OPCODE_UPDATE_CAREERRET_S);

AUTO_REGISITER_PACKET_FACTORY(OPUserCareerInfo,OPCODE_GET_USER_CAREER_INFO_C);
AUTO_REGISITER_PACKET_FACTORY(OPUserCareerInfoRet,OPCODE_GET_USER_CAREER_INFORET_S);

AUTO_REGISITER_PACKET_FACTORY(OPQuickCareerBattle, OPCODE_QUICK_CAREER_BATTLE_C);
AUTO_REGISITER_PACKET_FACTORY(OPQuickCareerBattleRet, OPCODE_QUICK_CAREER_BATTLERET_S);

AUTO_REGISITER_PACKET_FACTORY(OPShopList,OPCODE_GET_SHOP_ITEM_LIST_C);
AUTO_REGISITER_PACKET_FACTORY(OPShopListRet,OPCODE_GET_SHOP_ITEM_LISTRET_S);

AUTO_REGISITER_PACKET_FACTORY(OPShopPurchaseCheck,OPCODE_GET_SHOP_PURCHASE_CHECK_C);
AUTO_REGISITER_PACKET_FACTORY(OPShopPurchaseCheckRet,OPCODE_GET_SHOP_PURCHASE_RETURN_S);

AUTO_REGISITER_PACKET_FACTORY(OPSendMessage,OPCODE_SEND_FRIEND_MESSAGE_C);
AUTO_REGISITER_PACKET_FACTORY(OPSendMessageRet,OPCODE_SEND_FRIEND_MESSAGERET_S);

AUTO_REGISITER_PACKET_FACTORY(OPDisposeSysMsg,OPCODE_DISPOSE_USER_SYSMSG_INFO_C);
AUTO_REGISITER_PACKET_FACTORY(OPDisposeSysMsgRet,OPCODE_DISPOSE_USER_SYSMSG_INFORET_S);

AUTO_REGISITER_PACKET_FACTORY(OPDisposeFriendsInfo,OPCODE_DISPOSE_USER_FRIENDS_INFO_C);
AUTO_REGISITER_PACKET_FACTORY(OPDisposeFriendsInfoRet,OPCODE_DISPOSE_USER_FRIENDS_INFORET_S);

AUTO_REGISITER_PACKET_FACTORY(OPUserFriendsInfo,OPCODE_GET_USER_FRIENDS_INFO_C);
AUTO_REGISITER_PACKET_FACTORY(OPUserFriendsInfoRet,OPCODE_GET_USER_FRIENDS_INFORET_S);

AUTO_REGISITER_PACKET_FACTORY(OPUserSysMsgInfo,OPCODE_GET_USER_SYSMSG_INFO_C);
AUTO_REGISITER_PACKET_FACTORY(OPUserSysMsgInfoRet,OPCODE_GET_USER_SYSMSG_INFORET_S);

AUTO_REGISITER_PACKET_FACTORY(OPChatSendMsgInfo,OPCODE_CHAT_SEND_MSG_C);
AUTO_REGISITER_PACKET_FACTORY(OPChatSendMsgInfoRet,OPCODE_CHAT_SEND_MSGRET_S);

AUTO_REGISITER_PACKET_FACTORY(OPUserToolInfo,OPCODE_GET_USER_TOOL_INFO_C);
AUTO_REGISITER_PACKET_FACTORY(OPUserToolInfoRet,OPCODE_GET_USER_TOOL_INFORET_S);

AUTO_REGISITER_PACKET_FACTORY(OPUserSoulInfo,OPCODE_GET_USER_SOUL_INFO_C);
AUTO_REGISITER_PACKET_FACTORY(OPUserSoulInfoRet,OPCODE_GET_USER_SOUL_INFORET_S);

AUTO_REGISITER_PACKET_FACTORY(OPTrainDisciple,OPCODE_TRAIN_DISCIPLE_C);
AUTO_REGISITER_PACKET_FACTORY(OPTrainDiscipleRet,OPCODE_TRAIN_DISCIPLERET_S);

AUTO_REGISITER_PACKET_FACTORY(OPDealTrainStatus,OPCODE_DEAL_TRAIN_STATUS_C);
AUTO_REGISITER_PACKET_FACTORY(OPDealTrainStatusRet,OPCODE_DEAL_TRAIN_STATUSRET_S);

AUTO_REGISITER_PACKET_FACTORY(OPSwallowDisciple,OPCODE_SWALLOW_DISCIPLE_C);
AUTO_REGISITER_PACKET_FACTORY(OPSwallowDiscipleRet,OPCODE_SWALLOW_DISCIPLERET_S);

AUTO_REGISITER_PACKET_FACTORY(OPUseTool,OPCODE_USE_TOOL_C);
AUTO_REGISITER_PACKET_FACTORY(OPUseToolRet,OPCODE_USE_TOOLRET_S);

AUTO_REGISITER_PACKET_FACTORY(OPBoardCastInfo,OPCODE_GET_BOARDCAST_INFO_C);
AUTO_REGISITER_PACKET_FACTORY(OPBoardCastInfoRet,OPCODE_GET_BOARDCAST_INFORET_S);

AUTO_REGISITER_PACKET_FACTORY(OPChangeSetting,OPCODE_CHANGE_SETTING_C);
AUTO_REGISITER_PACKET_FACTORY(OPChangeSettingRet,OPCODE_CHANGE_SETTINGRET_S);

AUTO_REGISITER_PACKET_FACTORY(OPContinueLogin,OPCODE_GET_CONTINUE_LOGIN_C);
AUTO_REGISITER_PACKET_FACTORY(OPContinueLoginRet,OPCODE_GET_CONTINUE_LOGINRET_S);

AUTO_REGISITER_PACKET_FACTORY(OPRewardContinueLogin,OPCODE_REWARD_CONTINUE_LOGIN_C);
AUTO_REGISITER_PACKET_FACTORY(OPRewardContinueLoginRet,OPCODE_REWARD_CONTINUE_LOGINRET_S);

AUTO_REGISITER_PACKET_FACTORY(OPUserSkillInfo, OPCODE_GET_USER_SKILL_INFO_C);
AUTO_REGISITER_PACKET_FACTORY(OPUserSkillInfoRet, OPCODE_GET_USER_SKILL_INFORET_S);

AUTO_REGISITER_PACKET_FACTORY(OPUpgradeEquip,OPCODE_UPGRADE_EQUIP_C);
AUTO_REGISITER_PACKET_FACTORY(OPUpgradeEquipRet,OPCODE_UPGRADE_EQUIPRET_S);

AUTO_REGISITER_PACKET_FACTORY(OPUpgradeDisciple,OPCODE_UPGRADE_DISCIPLE_C);
AUTO_REGISITER_PACKET_FACTORY(OPUpgradeDiscipleRet,OPCODE_UPGRADE_DISCIPLERET_S);

AUTO_REGISITER_PACKET_FACTORY(OPUpgradeSkill,OPCODE_UPGRADE_SKILL_C);
AUTO_REGISITER_PACKET_FACTORY(OPUpgradeSkillRet,OPCODE_UPGRADE_SKILLRET_S);

AUTO_REGISITER_PACKET_FACTORY(OPAdventureInfo,OPCODE_GET_ADVENTURE_INFO_C);
AUTO_REGISITER_PACKET_FACTORY(OPAdventureInfoRet,OPCODE_GET_ADVENTURE_INFORET_S);

AUTO_REGISITER_PACKET_FACTORY(OPRecruitDisciple,OPCODE_RECRUIT_DISCIPLE_C);
AUTO_REGISITER_PACKET_FACTORY(OPRecruitDiscipleRet,OPCODE_RECRUIT_DISCIPLERET_S);

AUTO_REGISITER_PACKET_FACTORY(OPSellEquip,OPCODE_SELL_EQUIP_C);
AUTO_REGISITER_PACKET_FACTORY(OPSellEquipRet, OPCODE_SELL_EQUIPRET_S);

AUTO_REGISITER_PACKET_FACTORY(OPAddPower,OPCODE_ADD_POWER_C);
AUTO_REGISITER_PACKET_FACTORY(OPAddPowerRet,OPCODE_ADD_POWERRET_S);

AUTO_REGISITER_PACKET_FACTORY(OPAddVitality,OPCODE_ADD_VITALITY_C);
AUTO_REGISITER_PACKET_FACTORY(OPAddVitalityRet,OPCODE_ADD_VITALITYRET_S);

AUTO_REGISITER_PACKET_FACTORY(OPGetAdventurePower,OPCODE_GET_ADVENTURE_POWERINFO_C);
AUTO_REGISITER_PACKET_FACTORY(OPGetAdventurePowerRet,OPCODE_GET_ADVENTURE_POWERINFORET_S);

AUTO_REGISITER_PACKET_FACTORY(OPGetBoundlessHole,OPCODE_GET_ADVENTURE_BOUNDLESSHOLEINFO_C);
AUTO_REGISITER_PACKET_FACTORY(OPGetBoundlessHoleRet,OPCODE_GET_ADVENTURE_BOUNDLESSHOLEINFORET_S);

AUTO_REGISITER_PACKET_FACTORY(OPAddBoundlessHole,OPCODE_ADD_ADVENTURE_BOUNDLESSHOLEINFO_C);
AUTO_REGISITER_PACKET_FACTORY(OPAddBoundlessHoleRet,OPCODE_ADD_ADVENTURE_BOUNDLESSHOLEINFORET_S);

AUTO_REGISITER_PACKET_FACTORY(OPUpgradeRole, OPCODE_UPGRADE_ROLE_C);
AUTO_REGISITER_PACKET_FACTORY(OPUpgradeRoleRet, OPCODE_UPGRADE_ROLERET_S);

AUTO_REGISITER_PACKET_FACTORY(OPGetTeacherInfo,OPCODE_GET_ADVENTURE_TEACHERINFO_C);
AUTO_REGISITER_PACKET_FACTORY(OPGetTeacherInfoRet,OPCODE_GET_ADVENTURE_TEACHERINFORET_S);

AUTO_REGISITER_PACKET_FACTORY(OPFightEndBattle, OPCODE_FIGHTEND_BATTLE_C);
AUTO_REGISITER_PACKET_FACTORY(OPFightEndBattleRet, OPCODE_FIGHTEND_BATTLERET_S);

AUTO_REGISITER_PACKET_FACTORY(OPFightEndInfo, OPCODE_GET_FIGHTEND_INFO_C);
AUTO_REGISITER_PACKET_FACTORY(OPFightEndInfoRet, OPCODE_GET_FIGHTEND_INFORET_S);

AUTO_REGISITER_PACKET_FACTORY(OPHandbookInfo, OPCODE_HANDBOOK_INFO_C);
AUTO_REGISITER_PACKET_FACTORY(OPHandbookInfoRet, OPCODE_HANDBOOK_INFORET_S);

AUTO_REGISITER_PACKET_FACTORY(OPAddTeachExp, OPCODE_ADD_ADVENTURE_TEACHEXP_C);
AUTO_REGISITER_PACKET_FACTORY(OPAddTeachExpRet, OPCODE_ADD_ADVENTURE_TEACHEXPRET_S);


AUTO_REGISITER_PACKET_FACTORY(OPUserBattleArray, OPCODE_GET_USER_BATTLE_ARRAY_INFO_C);
AUTO_REGISITER_PACKET_FACTORY(OPUserBattleArrayRet, OPCODE_GET_USER_BATTLE_ARRAY_INFORET_S);
AUTO_REGISITER_PACKET_FACTORY(OPFightEndAddYestAttr, OPCODE_FIGHTEND_ADD_YEST_ATTR_INFO_C);
AUTO_REGISITER_PACKET_FACTORY(OPFightEndAddYestAttrRet, OPCODE_FIGHTEND_ADD_YEST_ATTR_INFORET_S);

AUTO_REGISITER_PACKET_FACTORY(OPFightEndTempAddStepAttr, OPCODE_FIGHTEND_ADD_TEMP_ATTR_INFO_C);
AUTO_REGISITER_PACKET_FACTORY(OPFightEndTempAddStepAttrRet, OPCODE_FIGHTEND_ADD_TEMP_ATTR_INFORET_S);

AUTO_REGISITER_PACKET_FACTORY(OPFightEndGetReward, OPCODE_GET_FIGHTEND_REWARD_INFO_C);
AUTO_REGISITER_PACKET_FACTORY(OPFightEndGetRewardRet, OPCODE_GET_FIGHTEND_REWARD_INFORET_S);

AUTO_REGISITER_PACKET_FACTORY(OPFightEndRankInfo, OPCODE_GET_FIGHTEND_RANK_INFO_C);
AUTO_REGISITER_PACKET_FACTORY(OPFightEndRankInfoRet, OPCODE_GET_FIGHTEND_RANK_INFORET_S);

AUTO_REGISITER_PACKET_FACTORY(OPChangeSkill, OPCODE_CHANGE_SKILL_C);
AUTO_REGISITER_PACKET_FACTORY(OPChangeSkillRet, OPCODE_CHANGE_SKILLRET_S);

AUTO_REGISITER_PACKET_FACTORY(OPChangeEquip, OPCODE_CHANGE_EQUIP_C);
AUTO_REGISITER_PACKET_FACTORY(OPChangeEquipRet, OPCODE_CHANGE_EQUIPRET_S);

AUTO_REGISITER_PACKET_FACTORY(OPUserBattle, OPCODE_USER_BATTLE_C);
AUTO_REGISITER_PACKET_FACTORY(OPUserBattleRet, OPCODE_USER_BATTLERET_S);

AUTO_REGISITER_PACKET_FACTORY(OPBuyTool, OPCODE_BUY_TOOLS_C);
AUTO_REGISITER_PACKET_FACTORY(OPBuyToolRet, OPCODE_BUY_TOOLSRET_S);

AUTO_REGISITER_PACKET_FACTORY(OPBuyPackage, OPCODE_BUY_PACKAGE_C);
AUTO_REGISITER_PACKET_FACTORY(OPBuyPackageRet, OPCODE_BUY_PACKAGERET_S);

AUTO_REGISITER_PACKET_FACTORY(OPGetWorldBossInfo, OPCODE_GET_WORLD_BOSS_INFO_C);
AUTO_REGISITER_PACKET_FACTORY(OPGetWorldBossInfoRet, OPCODE_GET_WORLD_BOSS_INFORET_S);

AUTO_REGISITER_PACKET_FACTORY(OPGetSkillBookInfo, OPCODE_GET_SKILL_BOOK_INFO_C);
AUTO_REGISITER_PACKET_FACTORY(OPGetSkillBookInfoRet, OPCODE_GET_SKILL_BOOK_INFORET_S);

AUTO_REGISITER_PACKET_FACTORY(OPPieceSkillBook, OPCODE_PIECE_SKILL_BOOK_C);
AUTO_REGISITER_PACKET_FACTORY(OPPieceSkillBookRet, OPCODE_PIECE_SKILL_BOOKRET_S);

AUTO_REGISITER_PACKET_FACTORY(OPSkillBookLoot, OPCODE_GET_SKILL_BOOK_LOOT_INFO_C);
AUTO_REGISITER_PACKET_FACTORY(OPSkillBookLootRet, OPCODE_GET_SKILL_BOOK_LOOT_INFORET_S);

AUTO_REGISITER_PACKET_FACTORY(OPInstruction, OPCODE_INSTRUCTION_C);
AUTO_REGISITER_PACKET_FACTORY(OPInstructionRet, OPCODE_INSTRUCTIONRET_S);

AUTO_REGISITER_PACKET_FACTORY(OPSnatchSkillBook, OPCODE_SNATCH_SKILL_BOOK_INFO_C);
AUTO_REGISITER_PACKET_FACTORY(OPSnatchSkillBookRet, OPCODE_SNATCH_SKILL_BOOK_INFORET_S);

AUTO_REGISITER_PACKET_FACTORY(OPTeamBuffInfo, OPCODE_GET_TEAM_BUFF_INFO_C);
AUTO_REGISITER_PACKET_FACTORY(OPTeamBuffInfoRet, OPCODE_GET_TEAM_BUFF_INFORET_S);

AUTO_REGISITER_PACKET_FACTORY(OPUpgradeTeamBuffInfo, OPCODE_UPGRADE_TEAM_BUFF_INFO_C);
AUTO_REGISITER_PACKET_FACTORY(OPUpgradeTeamBuffInfoRet, OPCODE_UPGRADE_TEAM_BUFF_INFORET_S);

AUTO_REGISITER_PACKET_FACTORY(OPChangeDisciple, OPCODE_CHANGE_DISCIPLE_C);
AUTO_REGISITER_PACKET_FACTORY(OPChangeDiscipleRet, OPCODE_CHANGE_DISCIPLERET_S);


AUTO_REGISITER_PACKET_FACTORY(OPErrorReporter, OPCODE_ERROR_RET_S);
AUTO_REGISITER_PACKET_FACTORY(OPServerDisConnectRet, OPCODE_SERVER_DISCONNECT_S);

AUTO_REGISITER_PACKET_FACTORY(OPAdventureLevelUpReward, OPCODE_ADVENTURE_LEVEL_UP_REWARD_C);
AUTO_REGISITER_PACKET_FACTORY(OPAdventureLevelUpRewardRet, OPCODE_ADVENTURE_LEVEL_UP_REWARDRET_S);

AUTO_REGISITER_PACKET_FACTORY(OPAdventureLevelUpGetReward, OPCODE_ADVENTURE_LEVEL_UP_GET_REWARD_C);
AUTO_REGISITER_PACKET_FACTORY(OPAdventureLevelUpGetRewardRet, OPCODE_ADVENTURE_LEVEL_UP_GET_REWARDRET_S);

AUTO_REGISITER_PACKET_FACTORY(OPChangeBattlePosition, OPCODE_CHANGE_BATTLE_POSITION_C);
AUTO_REGISITER_PACKET_FACTORY(OPChangeBattlePositionRet, OPCODE_CHANGE_BATTLE_POSITIONRET_S);

AUTO_REGISITER_PACKET_FACTORY(OPTitleInfo, OPCODE_GET_TITLE_INFO_C);
AUTO_REGISITER_PACKET_FACTORY(OPTitleInfoRet, OPCODE_GET_TITLE_INFORET_S);
AUTO_REGISITER_PACKET_FACTORY(OPResetCareerCount, OPCODE_RESET_CAREER_COUNT_C);
AUTO_REGISITER_PACKET_FACTORY(OPResetCareerCountRet, OPCODE_RESET_CAREER_COUNTRET_S);

AUTO_REGISITER_PACKET_FACTORY(OPUserArenaInfo, OPCODE_USER_ARENA_INFO_C);
AUTO_REGISITER_PACKET_FACTORY(OPUserArenaInfoRet, OPCODE_USER_ARENA_INFORET_S);
AUTO_REGISITER_PACKET_FACTORY(OPGetArenaReward, OPCODE_GET_ARENA_REWARD_C);
AUTO_REGISITER_PACKET_FACTORY(OPGetArenaRewardRet, OPCODE_GET_ARENA_REWARDRET_S);
AUTO_REGISITER_PACKET_FACTORY(OPGetArenaOpponentList, OPCODE_GET_ARENA_OPPONENT_LIST_C);
AUTO_REGISITER_PACKET_FACTORY(OPGetArenaOpponentListRet, OPCODE_GET_ARENA_OPPONENT_LISTRET_S);
AUTO_REGISITER_PACKET_FACTORY(OPChallengeOpponent, OPCODE_CHALLENGE_OPPONENT_C);
AUTO_REGISITER_PACKET_FACTORY(OPChallengeOpponentRet, OPCODE_CHALLENGE_OPPONENTRET_S);
AUTO_REGISITER_PACKET_FACTORY(OPRefreshArenaScore, OPCODE_REFRESH_ARENA_C);
AUTO_REGISITER_PACKET_FACTORY(OPRefreshArenaScoreRet, OPCODE_REFRESH_ARENARET_S);
AUTO_REGISITER_PACKET_FACTORY(OPUserUpgrade, OPCODE_USER_UPGRADE_S);
AUTO_REGISITER_PACKET_FACTORY(OPUpdateUserInfoByGM, OPCODE_UPDATE_USERINFO_BY_GM_C);
AUTO_REGISITER_PACKET_FACTORY(OPUpdateUserInfoByGMRet, OPCODE_UPDATE_USERINFO_BY_GM_RET_S);
AUTO_REGISITER_PACKET_FACTORY(OPAddPowerCareer, OPCODE_ADDPOWER_CAREER_C);
AUTO_REGISITER_PACKET_FACTORY(OPAddPowerCareerRet, OPCODE_ADDPOWER_CAREERRET_S);
AUTO_REGISITER_PACKET_FACTORY(OPAddArenaNum, OPCODE_ADD_ARENA_NUM_C);
AUTO_REGISITER_PACKET_FACTORY(OPAddArenaNumRet, OPCODE_ADD_ARENA_NUMRET_S);

AUTO_REGISITER_PACKET_FACTORY(OPChangeName, OPCODE_CHANE_NAME_C);
AUTO_REGISITER_PACKET_FACTORY(OPChangeNameRet, OPCODE_CHANE_NAMERET_S);

AUTO_REGISITER_PACKET_FACTORY(OPHeartbeat, OPCODE_HEARTBEAT_C);
AUTO_REGISITER_PACKET_FACTORY(OPHeartbeatRet, OPCODE_HEARTBEATRET_S);

AUTO_REGISITER_PACKET_FACTORY(OPGetAdventureContinueLogin, OPCODE_ADVENTURE_CONTINUELOGIN_GET_INFO_C);
AUTO_REGISITER_PACKET_FACTORY(OPGetAdventureContinueLoginRet, OPCODE_ADVENTURE_CONTINUELOGIN_GET_INFORET_S);

AUTO_REGISITER_PACKET_FACTORY(OPRewardAdventureContinueLogin, OPCODE_ADVENTURE_CONTINUELOGIN_REWARD_C);
AUTO_REGISITER_PACKET_FACTORY(OPRewardAdventureContinueLoginRet, OPCODE_ADVENTURE_CONTINUELOGIN_REWARDRET_S);

AUTO_REGISITER_PACKET_FACTORY(OPRecordReadMsgTime, OPCODE_RECORDREADMSGTIME_C);
AUTO_REGISITER_PACKET_FACTORY(OPRecordReadMsgTimeRet, OPCODE_RECORDREADMSGTIMERET_S);

AUTO_REGISITER_PACKET_FACTORY(OPCdkeyReward, OPCODE_CDKEY_REWARD_C);
AUTO_REGISITER_PACKET_FACTORY(OPCdkeyRewardRet, OPCODE_CDKEY_REWARDRET_S);

AUTO_REGISITER_PACKET_FACTORY(OPGuideRecord, OPCODE_GUIDE_RECORD_C);
AUTO_REGISITER_PACKET_FACTORY(OPGuideRecordRet, OPCODE_GUIDE_RECORDRET_S);

AUTO_REGISITER_PACKET_FACTORY(OPGuideBuyTool, OPCODE_GUIDE_BUY_TOOL_C);
AUTO_REGISITER_PACKET_FACTORY(OPGuideBuyToolRet, OPCODE_GUIDE_BUY_TOOLRET_S);

AUTO_REGISITER_PACKET_FACTORY(OPAddInviteKey, OPCODE_ADD_INVITEKEY_C);
AUTO_REGISITER_PACKET_FACTORY(OPAddInviteKeyRet, OPCODE_ADD_INVITEKEYRET_S);

AUTO_REGISITER_PACKET_FACTORY(OPUserInviteStatus, OPCODE_USER_INVITESTATUS_C);
AUTO_REGISITER_PACKET_FACTORY(OPUserInviteStatusRet, OPCODE_USER_INVITESTATUSRET_S);

AUTO_REGISITER_PACKET_FACTORY(OPGetInviteReward, OPCODE_GET_INVITE_REWARD_C);
AUTO_REGISITER_PACKET_FACTORY(OPGetInviteRewardRet, OPCODE_GET_INVITE_REWARDRET_S);

AUTO_REGISITER_PACKET_FACTORY(OPWorldBossFight, OPCODE_WORLDBOSS_FIGHT_C);
AUTO_REGISITER_PACKET_FACTORY(OPWorldBossFightRet, OPCODE_WORLDBOSS_FIGHTRET_S);

AUTO_REGISITER_PACKET_FACTORY(OPWorldBossRefresh, OPCODE_WORLDBOSS_REFRESH_C);
AUTO_REGISITER_PACKET_FACTORY(OPWorldBossRefreshRet, OPCODE_WORLDBOSS_REFRESHRET_S);

AUTO_REGISITER_PACKET_FACTORY(OPChargeRebate, OPCODE_CHARGE_REBATE_C);
AUTO_REGISITER_PACKET_FACTORY(OPChargeRebateRet, OPCODE_CHARGE_REBATERET_S);

AUTO_REGISITER_PACKET_FACTORY(OPFortuneCome, OPCODE_FORTUNE_COME_C);
AUTO_REGISITER_PACKET_FACTORY(OPFortuneComeRet, OPCODE_FORTUNE_COMERET_S);

AUTO_REGISITER_PACKET_FACTORY(OPShowUserBattleArray, OPCODE_SHOW_USER_BATTLE_C);
AUTO_REGISITER_PACKET_FACTORY(OPShowUserBattleArrayRet, OPCODE_SHOW_USER_BATTLERET_S);

AUTO_REGISITER_PACKET_FACTORY(OPUpdateTitle, OPCODE_UPDATE_TITLE_S);

AUTO_REGISITER_PACKET_FACTORY(OPActiveCode, OPCODE_ACTIVE_CODE_C);
AUTO_REGISITER_PACKET_FACTORY(OPActiveCodeRet, OPCODE_ACTIVE_CODERET_S);

AUTO_REGISITER_PACKET_FACTORY(OPEnterChatPage, OPCODE_ENTER_CHAT_PAGE_C);
AUTO_REGISITER_PACKET_FACTORY(OPEnterChatPageRet, OPCODE_ENTER_CHAT_PAGERET_S);

AUTO_REGISITER_PACKET_FACTORY(OPQuitChatPage, OPCODE_QUIT_CHAT_PAGE_C);
AUTO_REGISITER_PACKET_FACTORY(OPQuitChatPageRet, OPCODE_QUIT_CHAT_PAGERET_S);

AUTO_REGISITER_PACKET_FACTORY(OPUpgradeStoneInfo, OPCODE_UPGRADE_STONEINFO_C);
AUTO_REGISITER_PACKET_FACTORY(OPUpgradeStoneInfoRet, OPCODE_UPGRADE_STONEINFORET_S);

AUTO_REGISITER_PACKET_FACTORY(OPInlaidStoneInfo, OPCODE_INLAID_STONEINFO_C);
AUTO_REGISITER_PACKET_FACTORY(OPInlaidStoneInfoRet, OPCODE_INLAID_STONEINFORET_S);

AUTO_REGISITER_PACKET_FACTORY(OPRemoveStoneInfo, OPCODE_REMOVE_STONEINFO_C);
AUTO_REGISITER_PACKET_FACTORY(OPRemoveStoneInfoRet, OPCODE_REMOVE_STONEINFORET_S);

AUTO_REGISITER_PACKET_FACTORY(OPPunchStoneInfo, OPCODE_PUNCH_STONEINFO_C);
AUTO_REGISITER_PACKET_FACTORY(OPPunchStoneInfoRet, OPCODE_PUNCH_STONEINFORET_S);

AUTO_REGISITER_PACKET_FACTORY(OPChatPushRet, OPCODE_CHAT_PUSHRET_S);

// AUTO_REGISITER_PACKET_FACTORY(AddFriend, );
// AUTO_REGISITER_PACKET_FACTORY(AddPower, );
// AUTO_REGISITER_PACKET_FACTORY(AddVitality, );
// AUTO_REGISITER_PACKET_FACTORY(BuyPackage, );
// AUTO_REGISITER_PACKET_FACTORY(BuyTools, );
// AUTO_REGISITER_PACKET_FACTORY(ChallengePlayer, );
// AUTO_REGISITER_PACKET_FACTORY(ChangeBattlePosition, );
// AUTO_REGISITER_PACKET_FACTORY(ChangeDisciple, );
// AUTO_REGISITER_PACKET_FACTORY(ChangeEquip, );
// AUTO_REGISITER_PACKET_FACTORY(ChangeSetting, );
// AUTO_REGISITER_PACKET_FACTORY(ChangeSkill, );
// AUTO_REGISITER_PACKET_FACTORY(Chat, );
// AUTO_REGISITER_PACKET_FACTORY(DailyLogin, );
// AUTO_REGISITER_PACKET_FACTORY(ExchangeGift, );
// AUTO_REGISITER_PACKET_FACTORY(ExchangeIntegral, );
// AUTO_REGISITER_PACKET_FACTORY(Login, );
// AUTO_REGISITER_PACKET_FACTORY(MarketRecruitDisciple, );
// AUTO_REGISITER_PACKET_FACTORY(OP, );
// AUTO_REGISITER_PACKET_FACTORY(PlayerLogout, );
// AUTO_REGISITER_PACKET_FACTORY(RecruitDisciple, );
// AUTO_REGISITER_PACKET_FACTORY(RefineEquip, );
// AUTO_REGISITER_PACKET_FACTORY(SellEquip, );
// AUTO_REGISITER_PACKET_FACTORY(SendFriendMessage, );
// AUTO_REGISITER_PACKET_FACTORY(ShakeLantern, );
// AUTO_REGISITER_PACKET_FACTORY(SnatchFragment, );
// AUTO_REGISITER_PACKET_FACTORY(SwallowDisciple, );
// AUTO_REGISITER_PACKET_FACTORY(TrainDisciple, );
// AUTO_REGISITER_PACKET_FACTORY(UpdateCareer, );
// AUTO_REGISITER_PACKET_FACTORY(UpgradeDisciple, );
// AUTO_REGISITER_PACKET_FACTORY(UpgradeEquip, );
// AUTO_REGISITER_PACKET_FACTORY(UpgradeSkill, );
// AUTO_REGISITER_PACKET_FACTORY(UpgradeZhangMenJue, );
// AUTO_REGISITER_PACKET_FACTORY(UserBasicInfo, );
// AUTO_REGISITER_PACKET_FACTORY(UserBattle, );
// AUTO_REGISITER_PACKET_FACTORY(UserBattleArray, );
// AUTO_REGISITER_PACKET_FACTORY(UserDiscipleInfo, );
// AUTO_REGISITER_PACKET_FACTORY(UserEquipInfo, );
// AUTO_REGISITER_PACKET_FACTORY(UserMainInfo, );
// AUTO_REGISITER_PACKET_FACTORY(UserSkillInfo, );
// AUTO_REGISITER_PACKET_FACTORY(UseTool, );

void _gamePacketsCreatePackets()
{

	{
		OPLogin *msg = new OPLogin;
		msg->set_pwd("pswd");
		msg->set_version(1);
		msg->set_puid("dylan");
		PacketManager::Get()->_buildDefaultMessage(OPCODE_PLAYER_LOGIN_C,msg);
	}

	{
		OPUserBasicInfo* msg = new OPUserBasicInfo;
		msg->set_version(1);
		PacketManager::Get()->_buildDefaultMessage(OPCODE_GET_USER_BASIC_INFO_C,msg);
	}
	{
		OPUserDiscipleInfo* msg = new OPUserDiscipleInfo;
		msg->set_version(1);
		PacketManager::Get()->_buildDefaultMessage(OPCODE_GET_USER_DISCIPLE_INFO_C,msg);
	}
	{
		OPUserEquipleInfo* msg = new OPUserEquipleInfo;
		msg->set_version(1);
		PacketManager::Get()->_buildDefaultMessage(OPCODE_GET_USER_EQUIP_INFO_C,msg);
	}
	{
		OPMarketRecruitDisciple* msg = new OPMarketRecruitDisciple;
		msg->set_type(1);
		PacketManager::Get()->_buildDefaultMessage(OPCODE_MARKET_RECRUIT_DISCIPLE_C,msg);
	}
	{
		OPUpdateCareer* msg = new OPUpdateCareer;
		msg->set_id(1);
		PacketManager::Get()->_buildDefaultMessage(OPCODE_UPDATE_CAREER_C,msg);
	}
	{
		OPShopList* msg = new OPShopList;
		msg->set_version(1);
		PacketManager::Get()->_buildDefaultMessage(OPCODE_GET_SHOP_ITEM_LIST_C,msg);
	}
	{
		OPShopPurchaseCheck* msg = new OPShopPurchaseCheck;
		msg->add_cooorderserial("serial1");
		msg->add_cooorderserial("serial2");
		msg->set_version(1);
		PacketManager::Get()->_buildDefaultMessage(OPCODE_GET_SHOP_PURCHASE_CHECK_C ,msg);
	}
	{
		OPSendMessage* msg = new OPSendMessage;
		msg->set_version(1);
		msg->set_uid(2325);
		msg->set_msg("test");
		PacketManager::Get()->_buildDefaultMessage(OPCODE_SEND_FRIEND_MESSAGE_C ,msg);
	}
	{
		OPUserFriendsInfo* info = new OPUserFriendsInfo;
		info->set_version(1);
		PacketManager::Get()->_buildDefaultMessage(OPCODE_GET_USER_FRIENDS_INFO_C,info);
	}
	{
		OPDisposeFriendsInfo* info=new OPDisposeFriendsInfo;
		info->set_version(1);
		info->set_id(2);
		info->set_oprtype(3);
		PacketManager::Get()->_buildDefaultMessage(OPCODE_DISPOSE_USER_FRIENDS_INFO_C,info);
	}
	{
		OPDisposeSysMsg* info=new OPDisposeSysMsg;
		info->set_version(1);
		info->set_id(3);
		info->set_oprtype(2);
		PacketManager::Get()->_buildDefaultMessage(OPCODE_DISPOSE_USER_SYSMSG_INFO_C,info);
	}
	{
		OPUserSysMsgInfo* info=new OPUserSysMsgInfo;
		info->set_version(1);
		PacketManager::Get()->_buildDefaultMessage(OPCODE_GET_USER_SYSMSG_INFO_C,info);
	}
	{
		OPChatSendMsgInfo* info=new OPChatSendMsgInfo;
		info->set_version(1);
		info->set_msg("xxxxdsss");
		PacketManager::Get()->_buildDefaultMessage(OPCODE_CHAT_SEND_MSG_C,info);
	}
	{
		OPBoardCastInfo* info=new OPBoardCastInfo;
		info->set_version(1);
		PacketManager::Get()->_buildDefaultMessage(OPCODE_GET_BOARDCAST_INFO_C,info);
	}
	{
		OPChangeSetting* info=new OPChangeSetting;
		info->set_version(1);
		info->set_type("Music");
		info->set_oprret(1);
		PacketManager::Get()->_buildDefaultMessage(OPCODE_CHANGE_SETTING_C,info);
	}
	{
		OPUseTool* msg  = new OPUseTool;
		msg->set_id(1);
		msg->set_num(1);
		PacketManager::Get()->_buildDefaultMessage(OPCODE_USE_TOOL_C, msg);
	}
	{
		OPUserToolInfo* msg  = new OPUserToolInfo;
		msg->set_version(1);
		PacketManager::Get()->_buildDefaultMessage(OPCODE_GET_USER_TOOL_INFO_C, msg);
	}
	{
		OPUserSoulInfo* msg  = new OPUserSoulInfo;
		msg->set_version(1);
		PacketManager::Get()->_buildDefaultMessage(OPCODE_GET_USER_SOUL_INFO_C, msg);
	}
	{
		OPTrainDisciple* msg  = new OPTrainDisciple;
		msg->set_id(3);
		msg->set_type(2);
		PacketManager::Get()->_buildDefaultMessage(OPCODE_TRAIN_DISCIPLE_C, msg);
	}
	{
		OPDealTrainStatus* msg  = new OPDealTrainStatus;
		msg->set_id(3);
		msg->set_type(1);
		PacketManager::Get()->_buildDefaultMessage(OPCODE_DEAL_TRAIN_STATUS_C, msg);
	}
	{
		OPSwallowDisciple* msg  = new OPSwallowDisciple;
		msg->set_id(3);
		msg->set_swallowedid(1);
		msg->set_pelletid(2);
		PacketManager::Get()->_buildDefaultMessage(OPCODE_SWALLOW_DISCIPLE_C, msg);
	}
	{
		OPContinueLogin* msg  = new OPContinueLogin;
		msg->set_version(1);
		PacketManager::Get()->_buildDefaultMessage(OPCODE_GET_CONTINUE_LOGIN_C, msg);
	}
	{
		OPRewardContinueLogin* msg  = new OPRewardContinueLogin;
		msg->set_version(1);
		PacketManager::Get()->_buildDefaultMessage(OPCODE_REWARD_CONTINUE_LOGIN_C, msg);
	}
	{
		OPUserSkillInfo* msg = new OPUserSkillInfo;
		msg->set_version(1);
		PacketManager::Get()->_buildDefaultMessage(OPCODE_GET_USER_SKILL_INFO_C,msg);
	}
	{
		OPUpgradeEquip * msg = new OPUpgradeEquip;
		msg->set_id(1);
		PacketManager::Get()->_buildDefaultMessage(OPCODE_UPGRADE_EQUIP_C,msg);
	}
	{
		OPUpgradeDisciple* msg = new OPUpgradeDisciple;
		msg->set_id(1);
		PacketManager::Get()->_buildDefaultMessage(OPCODE_UPGRADE_DISCIPLE_C,msg);
	}
	{
		OPUpgradeSkill* msg = new OPUpgradeSkill;
		msg->set_id(1);
		msg->clear_kid();
		msg->add_kid(2);
		PacketManager::Get()->_buildDefaultMessage(OPCODE_UPGRADE_SKILL_C,msg);
	}
	{
		OPAdventureInfo* msg = new OPAdventureInfo;
		msg->set_version(1);
		PacketManager::Get()->_buildDefaultMessage(OPCODE_GET_ADVENTURE_INFO_C,msg);
	}
	{
		OPRecruitDisciple* msg = new OPRecruitDisciple;
		msg->set_id(1);
		PacketManager::Get()->_buildDefaultMessage(OPCODE_RECRUIT_DISCIPLE_C,msg);
	}
	{
		OPAddPower* msg = new OPAddPower;
		msg->set_typeid_(1);
		PacketManager::Get()->_buildDefaultMessage(OPCODE_ADD_POWER_C,msg);
	}
	{
		OPGetAdventurePower* msg = new OPGetAdventurePower;
		msg->set_version(1);
		PacketManager::Get()->_buildDefaultMessage(OPCODE_GET_ADVENTURE_POWERINFO_C,msg);
	}
	{
		OPGetBoundlessHole* msg = new OPGetBoundlessHole;
		msg->set_version(1);
		PacketManager::Get()->_buildDefaultMessage(OPCODE_GET_ADVENTURE_BOUNDLESSHOLEINFO_C,msg);
	}
	{
		OPAddBoundlessHole* msg = new OPAddBoundlessHole;
		msg->set_version(1);
		PacketManager::Get()->_buildDefaultMessage(OPCODE_ADD_ADVENTURE_BOUNDLESSHOLEINFO_C,msg);
	}
	{
		OPUpgradeRole* msg = new OPUpgradeRole;
		msg->set_version(1);
		PacketManager::Get()->_buildDefaultMessage(OPCODE_UPGRADE_ROLE_C, msg);
	}
	{
		OPFightEndBattle* msg = new OPFightEndBattle;
		msg->set_version(1);
		msg->set_id(1);
		PacketManager::Get()->_buildDefaultMessage(OPCODE_FIGHTEND_BATTLE_C, msg);
	}
	{
		OPFightEndInfo* msg = new OPFightEndInfo;
		msg->set_version(1);
		PacketManager::Get()->_buildDefaultMessage(OPCODE_GET_FIGHTEND_INFO_C, msg);
	}
	{
		OPFightEndAddYestAttr* msg = new OPFightEndAddYestAttr;
		msg->set_version(1);
		msg->set_id(2);
		PacketManager::Get()->_buildDefaultMessage(OPCODE_FIGHTEND_ADD_YEST_ATTR_INFO_C, msg);
	}
	{
		OPFightEndTempAddStepAttr* msg = new OPFightEndTempAddStepAttr;
		msg->set_version(1);
		msg->set_id(1);
		PacketManager::Get()->_buildDefaultMessage(OPCODE_FIGHTEND_ADD_TEMP_ATTR_INFO_C, msg);
	}
	{
		OPFightEndGetReward* msg = new OPFightEndGetReward;
		msg->set_version(1);
		PacketManager::Get()->_buildDefaultMessage(OPCODE_GET_FIGHTEND_REWARD_INFO_C, msg);
	}
	{
		OPFightEndRankInfo* msg = new OPFightEndRankInfo;
		msg->set_version(1);
		msg->set_index(1);
		PacketManager::Get()->_buildDefaultMessage(OPCODE_GET_FIGHTEND_RANK_INFO_C, msg);
	}
	{
		OPUserBattleArray* msg = new OPUserBattleArray;
		msg->set_version(1);
		PacketManager::Get()->_buildDefaultMessage(OPCODE_GET_USER_BATTLE_ARRAY_INFO_C, msg);
	}
	{
		OPChangeSkill* msg = new OPChangeSkill;
		msg->set_newskillid(1);
		msg->set_battleposition(1);
		msg->set_skillposition(2);
		PacketManager::Get()->_buildDefaultMessage(OPCODE_CHANGE_SKILL_C, msg);
	}
	{
		OPChangeEquip* msg = new OPChangeEquip;
		msg->set_id(1);
		msg->set_battleposition(1);
		PacketManager::Get()->_buildDefaultMessage(OPCODE_CHANGE_EQUIP_C, msg);
	}
	{
		OPGetTeacherInfo* msg = new OPGetTeacherInfo;
		msg->set_version(1);
		msg->set_adventureitemid(1);
		PacketManager::Get()->_buildDefaultMessage(OPCODE_GET_ADVENTURE_TEACHERINFO_C, msg);
	}
	{
		OPAddTeachExp* msg = new OPAddTeachExp;
		msg->set_version(1);
		msg->set_type(1);
		msg->set_adventureid(1);
		PacketManager::Get()->_buildDefaultMessage(OPCODE_ADD_ADVENTURE_TEACHEXP_C, msg);
	}
	{
		OPHandbookInfo* msg = new OPHandbookInfo;
		msg->set_version(1);
		PacketManager::Get()->_buildDefaultMessage(OPCODE_HANDBOOK_INFO_C, msg);
	}
	{
		/*
		OPGetWorldBossInfo* msg = new OPGetWorldBossInfo;
		msg->set_version(1);
		msg->set_bossid(1);
		PacketManager::Get()->_buildDefaultMessage(OPCODE_GET_WORLD_BOSS_INFO_C, msg);
		*/
	}
	{
		OPGetSkillBookInfo* msg = new OPGetSkillBookInfo;
		msg->set_version(1);
		msg->set_skillid(1);
		PacketManager::Get()->_buildDefaultMessage(OPCODE_GET_SKILL_BOOK_INFO_C, msg);
	}
	{
		OPPieceSkillBook* msg = new OPPieceSkillBook;
		msg->set_version(1);
		msg->set_skillid(1);
		PacketManager::Get()->_buildDefaultMessage(OPCODE_PIECE_SKILL_BOOK_C, msg);
	}
	{
		OPSkillBookLoot* msg = new OPSkillBookLoot;
		msg->set_version(1);
		msg->set_skillbookid(101);
		PacketManager::Get()->_buildDefaultMessage(OPCODE_GET_SKILL_BOOK_LOOT_INFO_C, msg);
	}
	{
		OPSnatchSkillBook* msg = new OPSnatchSkillBook;
		msg->set_version(1);
		msg->set_playerid(1);
		msg->set_bookid(101);
		PacketManager::Get()->_buildDefaultMessage(OPCODE_SNATCH_SKILL_BOOK_INFO_C, msg);
	}
	{
		OPAdventureLevelUpReward* msg = new OPAdventureLevelUpReward;
		msg->set_version(1);
		PacketManager::Get()->_buildDefaultMessage(OPCODE_ADVENTURE_LEVEL_UP_REWARD_C,msg);
	}
	{
		OPAdventureLevelUpGetReward* msg = new OPAdventureLevelUpGetReward;
		msg->set_version(1);
		msg->set_rewardid(1);
		PacketManager::Get()->_buildDefaultMessage(OPCODE_ADVENTURE_LEVEL_UP_GET_REWARD_C,msg);
	}
	{
		OPTeamBuffInfo* msg = new OPTeamBuffInfo;
		msg->set_version(1);
		PacketManager::Get()->_buildDefaultMessage(OPCODE_GET_TEAM_BUFF_INFO_C, msg);
	}
	{
		OPUpgradeTeamBuffInfo* msg = new OPUpgradeTeamBuffInfo;
		msg->set_version(1);
		msg->set_teamid(1);
		for(int i=0;i<20;++i)
		{
			OPUpgradeTeamBuffInfo_CousumeItem* memberList=msg->add_memberlist();
			memberList->set_memberid(i);
		}
		for(int i=0;i<20;++i)
		{
			OPUpgradeTeamBuffInfo_CousumeItem* memberList=msg->add_soullist();
			memberList->set_memberid(i);
			memberList->set_count(rand()%100+1);
		}
		PacketManager::Get()->_buildDefaultMessage(OPCODE_UPGRADE_TEAM_BUFF_INFO_C, msg);
	}
	{
		OPBuyTool* msg = new OPBuyTool;
		msg->set_itemid(3501004);
		msg->set_count(1);
		PacketManager::Get()->_buildDefaultMessage(OPCODE_BUY_TOOLS_C, msg);
	}
	{
		OPResetCareerCount* msg = new OPResetCareerCount;
		msg->set_version(1);
		msg->set_id(1002);
		PacketManager::Get()->_buildDefaultMessage(OPCODE_RESET_CAREER_COUNT_C, msg);
	}
	{
		OPUpdateUserInfoByGM* msg = new OPUpdateUserInfoByGM;
		msg->set_version(1);
		msg->set_msg("/add gold 1000");
		PacketManager::Get()->_buildDefaultMessage(OPCODE_UPDATE_USERINFO_BY_GM_C, msg);
	}
	{
		OPAddPowerCareer* msg = new OPAddPowerCareer;
		msg->set_version(1);
		msg->set_chapterid(1);
		PacketManager::Get()->_buildDefaultMessage(OPCODE_ADDPOWER_CAREER_C, msg);
	}
	{
		OPAddArenaNum* msgAddArena = new OPAddArenaNum;
		msgAddArena->set_typeid_(1);
		PacketManager::Get()->_buildDefaultMessage(OPCODE_ADD_ARENA_NUM_C,msgAddArena);
	}
	{
		OPChangeName* msgChangeName = new OPChangeName;
		msgChangeName->set_name("aaaaat");
		PacketManager::Get()->_buildDefaultMessage(OPCODE_CHANE_NAME_C,msgChangeName);
	}
	{
		OPGetAdventureContinueLogin* msg = new OPGetAdventureContinueLogin;
		msg->set_version(1);
		PacketManager::Get()->_buildDefaultMessage(OPCODE_ADVENTURE_CONTINUELOGIN_GET_INFO_C,msg);
	}
	{
		OPRewardAdventureContinueLogin* msg = new OPRewardAdventureContinueLogin;
		msg->set_version(1);
		msg->set_id(1);
		PacketManager::Get()->_buildDefaultMessage(OPCODE_ADVENTURE_CONTINUELOGIN_REWARD_C,msg);
	}
	{
		OPCdkeyReward* msg = new OPCdkeyReward;
		msg->set_version(1);
		msg->set_cdkey("121314141");
		PacketManager::Get()->_buildDefaultMessage(OPCODE_CDKEY_REWARD_C,msg);
	}
	{
		OPRecordReadMsgTime* msg = new OPRecordReadMsgTime;
		msg->set_version(1);
		PacketManager::Get()->_buildDefaultMessage(OPCODE_RECORDREADMSGTIME_C,msg);
	}
	{
		OPGuideBuyTool* msg = new OPGuideBuyTool;
		msg->set_version(1);
		msg->set_typeid_(1);
		PacketManager::Get()->_buildDefaultMessage(OPCODE_GUIDE_BUY_TOOL_C,msg);
	}
	{
		OPGuideRecord* msg = new OPGuideRecord;
		msg->set_version(1);
		msg->set_type("GBuyEquip");
		PacketManager::Get()->_buildDefaultMessage(OPCODE_GUIDE_RECORD_C,msg);
	}
	{
		OPAddInviteKey* msg = new OPAddInviteKey;
		msg->set_key("OPAddInviteKey");
		PacketManager::Get()->_buildDefaultMessage(OPCODE_ADD_INVITEKEY_C,msg);
	}
	{
		OPAddInviteKey* msg = new OPAddInviteKey;
		msg->set_key("OPAddInviteKey");
		PacketManager::Get()->_buildDefaultMessage(OPCODE_ADD_INVITEKEY_C,msg);
	}
	{
		OPUserInviteStatus* msg = new OPUserInviteStatus;
		msg->set_version(1);
		PacketManager::Get()->_buildDefaultMessage(OPCODE_USER_INVITESTATUS_C,msg);
	}
	{
		OPGetInviteReward* msg = new OPGetInviteReward;
		msg->set_type(1);
		PacketManager::Get()->_buildDefaultMessage(OPCODE_GET_INVITE_REWARD_C,msg);
	}
	{
		OPShowUserBattleArray* msg = new OPShowUserBattleArray;
		msg->set_version(1);
		msg->set_isnpc(0);
		msg->set_relationid(1);
		msg->set_relationtype(1);
		PacketManager::Get()->_buildDefaultMessage(OPCODE_SHOW_USER_BATTLE_C,msg);
	}
	{
		OPActiveCode* msg = new OPActiveCode;
		msg->set_uid("1234");
		msg->set_activecode("111112222233333");
		PacketManager::Get()->_buildDefaultMessage(OPCODE_ACTIVE_CODE_C,msg);
	}
	{
		OPPunchStoneInfo* msg = new OPPunchStoneInfo;
		msg->set_version(1);
		msg->set_equipid(3);
		msg->set_position(2);
		PacketManager::Get()->_buildDefaultMessage(OPCODE_PUNCH_STONEINFO_C,msg);
	}
	{
		OPUpgradeStoneInfo* msg = new OPUpgradeStoneInfo;
		msg->set_version(1);
		msg->set_stoneid(5001011);
		PacketManager::Get()->_buildDefaultMessage(OPCODE_UPGRADE_STONEINFO_C,msg);
	}
	{
		OPInlaidStoneInfo* msg = new OPInlaidStoneInfo;
		msg->set_version(1);
		msg->set_equipid(1);
		msg->set_stoneid(5001011);
		msg->set_position(2);
		PacketManager::Get()->_buildDefaultMessage(OPCODE_INLAID_STONEINFO_C,msg);
	}
	{
		OPRemoveStoneInfo* msg = new OPRemoveStoneInfo;
		msg->set_version(1);
		msg->set_equipid(1);
		msg->set_position(1);
		PacketManager::Get()->_buildDefaultMessage(OPCODE_REMOVE_STONEINFO_C,msg);
	}
}



#endif//_REGISISTER_FACTORIES_
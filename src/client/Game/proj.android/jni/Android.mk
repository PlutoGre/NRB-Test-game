LOCAL_PATH := $(call my-dir)

MY_LOCAL_PATH:=$(LOCAL_PATH)

include $(LOCAL_PATH)/prebuild/Android.mk

LOCAL_PATH := ${MY_LOCAL_PATH}

include $(CLEAR_VARS)

LOCAL_MODULE    := DreamOnePiece

LOCAL_CPP_EXTENSION := .cc .cpp

STLPORT_FORCE_REBUILD := true

ALOCAL_SRC_FILES := 

LOCAL_SRC_FILES :=GameMain.cpp \
ScreenShotJni.cpp \
BulletinBoardPage.cpp \
		../../Platform/GamePlatform.cpp \
		../../Classes/AboutPage.cpp \
		../../Classes/ActiveCodePage.cpp \
		../../Classes/AddPowerPage.cpp \
		../../Classes/AdventureBase.cpp \
		../../Classes/AdventureContinuePage.cpp \
		../../Classes/AdventureInvitePage.cpp \
		../../Classes/AdventurePage.cpp \
		../../Classes/AES.cpp \
		../../Classes/AnnouncementPage.cpp \
		../../Classes/AppDelegate.cpp \
		../../Classes/Arena.cpp \
		../../Classes/ArenaBuyChallengePage.cpp \
		../../Classes/ArenaPage.cpp \
		../../Classes/ArenaRewardAniPage.cpp \
		../../Classes/ArenaRewardPage.cpp \
		../../Classes/AsyncSocket.cpp \
		../../Classes/AsyncSprite.cpp \
		../../Classes/BattleArrayInfo.cpp \
		../../Classes/BlackBoard.cpp \
		../../Classes/BoardCastLabel.cpp \
		../../Classes/BoundlessHolePage.cpp \
		../../Classes/buyingCheck.cpp \
		../../Classes/BuyPropInfoPage.cpp \
		../../Classes/BuyPropPopupPage.cpp \
		../../Classes/BuyPropsCountPage.cpp \
		../../Classes/BuyPropsPage.cpp \
		../../Classes/BuyStoneCountPage.cpp \
		../../Classes/BuyStonePopupPage.cpp \
		../../Classes/BuyToolAlertPage.cpp \
		../../Classes/CareerConfigManager.cpp \
		../../Classes/CareerPage.cpp \
		../../Classes/CareerQuickResultPage.cpp \
		../../Classes/CareerStoryPage.cpp \
		../../Classes/CCBContainer.cpp \
		../../Classes/CCBLuaContainer.cpp \
		../../Classes/CCBManager.cpp \
		../../Classes/CCBScriptContainer.cpp \
		../../Classes/CDKeyInvitePage.cpp \
		../../Classes/CDKeyMsgPage.cpp \
		../../Classes/ChangeNamePage.cpp \
		../../Classes/CharacterConsts.cpp \
		../../Classes/ChargeRebatePage.cpp \
		../../Classes/ChatPage.cpp \
		../../Classes/Concurrency.cpp \
		../../Classes/ContinueLoginPage.cpp \
		../../Classes/CurlDownload.cpp \
		../../Classes/DataTableManager.cpp \
		../../Classes/Disciple.cpp \
		../../Classes/DiscipleCardPage.cpp \
		../../Classes/DiscipleChangePage.cpp \
		../../Classes/DiscipleInfoPage.cpp \
		../../Classes/DisciplePage.cpp \
		../../Classes/DiscipleRecruitPage.cpp \
		../../Classes/DiscipleUpgradePage.cpp \
		../../Classes/DiscipleHandInfoPage.cpp \
		../../Classes/DiscipleViewTeamInfoPage.cpp \
		../../Classes/EmbattlePage.cpp \
		../../Classes/Equip.cpp \
		../../Classes/EquipCardPage.cpp \
		../../Classes/EquipChangePage.cpp \
		../../Classes/EquipEnhanceAniPage.cpp \
		../../Classes/EquipEnhancePage.cpp \
		../../Classes/EquipEnhanceResultPage.cpp \
		../../Classes/EquipInfoPage.cpp \
		../../Classes/EquipPage.cpp \
		../../Classes/EquipQuickEnhancePage.cpp \
		../../Classes/EquipSellPage.cpp \
		../../Classes/EquipSurprisePage.cpp \
		../../Classes/EquipHandInfoPage.cpp \
		../../Classes/FightCardSprite.cpp \
		../../Classes/FightConfigManager.cpp \
		../../Classes/FightEndPage.cpp \
		../../Classes/FightEndPopupPage.cpp \
		../../Classes/FightEndRankPage.cpp \
		../../Classes/FightGuidePage.cpp \
		../../Classes/FightPage.cpp \
		../../Classes/FightPage_finalRound.cpp \
		../../Classes/FightPage_resultState.cpp \
		../../Classes/FightPage_resultState_FightEnd.cpp \
		../../Classes/FightPage_roundState.cpp \
		../../Classes/FightTasks.cpp \
		../../Classes/FightTasksSkill.cpp \
		../../Classes/FortuneComePage.cpp \
		../../Classes/FortuneComeReward.cpp \
		../../Classes/FriendsDetailPopupPage.cpp \
		../../Classes/FriendsPage.cpp \
		../../Classes/GameConsts.cpp \
		../../Classes/Gamelua.cpp \
		../../Classes/GameMaths.cpp \
		../../Classes/GameMessages.cpp \
		../../Classes/GameNotification.cpp \
		../../Classes/GamePackets.cpp \
		../../Classes/GamePrecedure.cpp \
		../../Classes/GetPropManager.cpp \
		../../Classes/GiftPreviewPage.cpp \
		../../Classes/GoodsPopupPage.cpp \
		../../Classes/GuideBuyEquipPage.cpp \
		../../Classes/GuideBuyExpPage.cpp \
		../../Classes/GuideRechargePage.cpp \
		../../Classes/HandbookPage.cpp \
		../../Classes/HelpPage.cpp \
		../../Classes/inifile.cpp \
		../../Classes/InstructionManager.cpp \
		../../Classes/IoSocket.cpp \
		../../Classes/Language.cpp \
		../../Classes/LeaveMsgPage.cpp \
		../../Classes/LevelUpRewardPage.cpp \
		../../Classes/LoadingFrame.cpp \
		../../Classes/LoadingScene.cpp \
		../../Classes/LoadingUnit.cpp \
		../../Classes/LoginPacket.cpp \
		../../Classes/MainFrame.cpp \
		../../Classes/MainPage.cpp \
		../../Classes/MarketPage.cpp \
		../../Classes/MemberPage.cpp \
		../../Classes/MessageBoxPage.cpp \
		../../Classes/MessageManager.cpp \
		../../Classes/NewActivePromptPage.cpp \
		../../Classes/NewActivePromptToolPage.cpp \
		../../Classes/NumberChangeAction.cpp \
		../../Classes/ObtainSkillBookPage.cpp \
		../../Classes/PackageLogic.cpp \
		../../Classes/PackagePage.cpp \
		../../Classes/PacketBase.cpp \
		../../Classes/PacketManager.cpp \
		../../Classes/PackPreviewPage.cpp \
		../../Classes/pb.cpp \
		../../Classes/PlayerInfPage.cpp \
		../../Classes/PlayerUpgradePage.cpp \
		../../Classes/Popup1stPayTipPage.cpp \
		../../Classes/PopupManager.cpp \
		../../Classes/PopupNotEnoughPage.cpp \
		../../Classes/PopupPage.cpp \
		../../Classes/PopUpSurprisePage.cpp \
		../../Classes/PropInfoPage.cpp \
		../../Classes/PropUpPage.cpp \
		../../Classes/RechargePage.cpp \
		../../Classes/ResManager.cpp \
		../../Classes/RestrictedWord.cpp \
		../../Classes/ScriptContentBase.cpp \
		../../Classes/ScriptMathToLua.cpp \
		../../Classes/ScrollNumCard.cpp \
		../../Classes/ScrollNumCardTask.cpp \
		../../Classes/ServerDateManager.cpp \
		../../Classes/SettingsPage.cpp \
		../../Classes/SeverConsts.cpp \
		../../Classes/Skill.cpp \
		../../Classes/SkillBookLootPage.cpp \
		../../Classes/SkillBookPage.cpp \
		../../Classes/SkillBookSurprisePage.cpp \
		../../Classes/SkillCardPage.cpp \
		../../Classes/SkillChangePage.cpp \
		../../Classes/SkillConsts.cpp \
		../../Classes/SkillInfoPage.cpp \
		../../Classes/SkillPage.cpp \
		../../Classes/SkillSurprisePage.cpp \
		../../Classes/SkillUpChoosePage.cpp \
		../../Classes/SkillUpgradePage.cpp \
		../../Classes/SkillUpgradeResultPage.cpp \
		../../Classes/SocketBase.cpp \
		../../Classes/SoulInfoPage.cpp \
		../../Classes/SoundManager.cpp \
		../../Classes/SpriteShader.cpp \
		../../Classes/stdafx.cpp \
		../../Classes/StoneInlaidPage.cpp \
		../../Classes/StoneUpgradePage.cpp \
		../../Classes/StringConverter.cpp \
		../../Classes/SysMsgPage.cpp \
		../../Classes/SkillHandInfoPage.cpp \
		../../Classes/SkillViewTeamInfoPage.cpp \
		../../Classes/TableReader.cpp \
		../../Classes/TeacherTeachPage.cpp \
		../../Classes/TeacherTeachResultPage.cpp \
		../../Classes/TeamBuffPage.cpp \
		../../Classes/TeamBuffUpgradePage.cpp \
		../../Classes/TeamPage.cpp \
		../../Classes/ThreadSocket.cpp \
		../../Classes/TimeCalculator.cpp \
		../../Classes/Title.cpp \
		../../Classes/TitleCardPage.cpp \
		../../Classes/TitleGetNewPage.cpp \
		../../Classes/TitleInfoPage.cpp \
		../../Classes/TitleLVUpPage.cpp \
		../../Classes/TitlePage.cpp \
		../../Classes/TrainingBolusChoosePage.cpp \
		../../Classes/TrainPage.cpp \
		../../Classes/TrainResultPage.cpp \
		../../Classes/TranExpChooseMemberPage.cpp \
		../../Classes/TranExpPreview.cpp \
		../../Classes/TranExpResult.cpp \
		../../Classes/TransferEXPPage.cpp \
		../../Classes/ViewListTeamPage.cpp \
		../../Classes/ViewTeamPage.cpp \
		../../Classes/VIPPrivilegePage.cpp \
		../../Classes/ViewTeamEquipInfoPage.cpp \
		../../Classes/waitingManager.cpp \
		../../Classes/WishPage.cpp \
		../../Classes/WorldBossFight.cpp \
		../../Classes/WorldBossFightEnd.cpp \
		../../Classes/WorldBossGiftPreviewPage.cpp \
		../../Classes/WorldBossJoinBattle.cpp \
		../../Classes/WorldBossPage.cpp \
		../../Classes/WorldBossRankPage.cpp \
		../../Classes/ActivityPopPage.cpp \
		../../Protobuf/ActiveCode.pb.cc \
		../../Protobuf/AddArenaNum.pb.cc \
		../../Protobuf/AddBoundlessHole.pb.cc \
		../../Protobuf/AddFriend.pb.cc \
		../../Protobuf/AddInviteKey.pb.cc \
		../../Protobuf/AddPower.pb.cc \
		../../Protobuf/AddPowerForCareer.pb.cc \
		../../Protobuf/AddTeachExp.pb.cc \
		../../Protobuf/AddVitality.pb.cc \
		../../Protobuf/AdventureInfo.pb.cc \
		../../Protobuf/AdventureLevelUpGetReward.pb.cc \
		../../Protobuf/AdventureLevelUpReward.pb.cc \
		../../Protobuf/AdventureTeacherInfo.pb.cc \
		../../Protobuf/BoardCastInfo.pb.cc \
		../../Protobuf/BuyPackage.pb.cc \
		../../Protobuf/BuyTools.pb.cc \
		../../Protobuf/CdkeyReward.pb.cc \
		../../Protobuf/ChallengeOpponent.pb.cc \
		../../Protobuf/ChallengePlayer.pb.cc \
		../../Protobuf/ChangeBattlePosition.pb.cc \
		../../Protobuf/ChangeDisciple.pb.cc \
		../../Protobuf/ChangeEquip.pb.cc \
		../../Protobuf/ChangeName.pb.cc \
		../../Protobuf/ChangeSetting.pb.cc \
		../../Protobuf/ChangeSkill.pb.cc \
		../../Protobuf/ChargeRebate.pb.cc \
		../../Protobuf/Chat.pb.cc \
		../../Protobuf/ChatPush.pb.cc \
		../../Protobuf/ChatSendMsgInfo.pb.cc \
		../../Protobuf/ContinueLogin.pb.cc \
		../../Protobuf/DailyLogin.pb.cc \
		../../Protobuf/DealTrainStatus.pb.cc \
		../../Protobuf/DisposeFriendsInfo.pb.cc \
		../../Protobuf/DisposeSysMsgInfo.pb.cc \
		../../Protobuf/ErrorReporter.pb.cc \
		../../Protobuf/ExchangeGift.pb.cc \
		../../Protobuf/ExchangeIntegral.pb.cc \
		../../Protobuf/FightEndAddYestAttr.pb.cc \
		../../Protobuf/FightEndBattle.pb.cc \
		../../Protobuf/FightEndGetReward.pb.cc \
		../../Protobuf/FightEndInfo.pb.cc \
		../../Protobuf/FightEndRankInfo.pb.cc \
		../../Protobuf/FightEndTempAddStepAttr.pb.cc \
		../../Protobuf/FortuneCome.pb.cc \
		../../Protobuf/GetAdventureBoundlessHole.pb.cc \
		../../Protobuf/GetAdventureContinueLogin.pb.cc \
		../../Protobuf/GetAdventurePowerInfo.pb.cc \
		../../Protobuf/GetArenaOpponentList.pb.cc \
		../../Protobuf/GetArenaReward.pb.cc \
		../../Protobuf/GetInviteReward.pb.cc \
		../../Protobuf/GetSkillBookLoot.pb.cc \
		../../Protobuf/GuideBuyTool.pb.cc \
		../../Protobuf/GuideRecord.pb.cc \
		../../Protobuf/HandbookInfo.pb.cc \
		../../Protobuf/Heartbeat.pb.cc \
		../../Protobuf/InlaidStoneInfo.pb.cc \
		../../Protobuf/Instruction.pb.cc \
		../../Protobuf/Login.pb.cc \
		../../Protobuf/MarketRecruitDisciple.pb.cc \
		../../Protobuf/OP.pb.cc \
		../../Protobuf/PieceSkillBook.pb.cc \
		../../Protobuf/playerInfo.pb.cc \
		../../Protobuf/PlayerLogout.pb.cc \
		../../Protobuf/protobufTest.pb.cc \
		../../Protobuf/PunchStoneInfo.pb.cc \
		../../Protobuf/QuickCareerBattle.pb.cc \
		../../Protobuf/RecordReadMsgTime.pb.cc \
		../../Protobuf/RecruitDisciple.pb.cc \
		../../Protobuf/RefineEquip.pb.cc \
		../../Protobuf/RefreshArenaScore.pb.cc \
		../../Protobuf/RemoveStoneInfo.pb.cc \
		../../Protobuf/ResetCareerCount.pb.cc \
		../../Protobuf/RewardAdventureContinueLogin.pb.cc \
		../../Protobuf/RewardContinueLogin.pb.cc \
		../../Protobuf/SellEquip.pb.cc \
		../../Protobuf/SendFriendMessage.pb.cc \
		../../Protobuf/ServerDisconnect.pb.cc \
		../../Protobuf/ShakeLantern.pb.cc \
		../../Protobuf/ShopList.pb.cc \
		../../Protobuf/ShopPurchase.pb.cc \
		../../Protobuf/ShowUserBattle.pb.cc \
		../../Protobuf/SkillBookInfo.pb.cc \
		../../Protobuf/SnatchFragment.pb.cc \
		../../Protobuf/SnatchSkillBook.pb.cc \
		../../Protobuf/SwallowDisciple.pb.cc \
		../../Protobuf/TeamBuffInfo.pb.cc \
		../../Protobuf/TitleInfo.pb.cc \
		../../Protobuf/TrainDisciple.pb.cc \
		../../Protobuf/UpdateCareer.pb.cc \
		../../Protobuf/UpdateTitle.pb.cc \
		../../Protobuf/UpdateUserInfoByGM.pb.cc \
		../../Protobuf/UpgradeDisciple.pb.cc \
		../../Protobuf/UpgradeEquip.pb.cc \
		../../Protobuf/UpgradeRole.pb.cc \
		../../Protobuf/UpgradeSkill.pb.cc \
		../../Protobuf/UpgradeStoneInfo.pb.cc \
		../../Protobuf/UpgradeTeamBuffInfo.pb.cc \
		../../Protobuf/UpgradeZhangMenJue.pb.cc \
		../../Protobuf/UserArenaInfo.pb.cc \
		../../Protobuf/UserBasicInfo.pb.cc \
		../../Protobuf/UserBattle.pb.cc \
		../../Protobuf/UserBattleArray.pb.cc \
		../../Protobuf/UserCareerInfo.pb.cc \
		../../Protobuf/UserChatInfo.pb.cc \
		../../Protobuf/UserDiscipleInfo.pb.cc \
		../../Protobuf/UserEquipInfo.pb.cc \
		../../Protobuf/UserFriendsInfo.pb.cc \
		../../Protobuf/UserInviteStatus.pb.cc \
		../../Protobuf/UserMainInfo.pb.cc \
		../../Protobuf/UserMarketInfo.pb.cc \
		../../Protobuf/UserRewardInfo.pb.cc \
		../../Protobuf/UserSkillInfo.pb.cc \
		../../Protobuf/UserSoulInfo.pb.cc \
		../../Protobuf/UserSysMsgInfo.pb.cc \
		../../Protobuf/UserToolInfo.pb.cc \
		../../Protobuf/UserUpgrade.pb.cc \
		../../Protobuf/UseTool.pb.cc \
		../../Protobuf/WorldBossFight.pb.cc \
		../../Protobuf/WorldBossInfo.pb.cc \
		../../Protobuf/WorldBossRefresh.pb.cc \



#mark1

$(info "这里可以使用Eclipse C/C++ Build的Environment环境变量")
$(info "ProjDirPath: " $(PWD))
$(info "LOCAL_PATH: " $(LOCAL_PATH))

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../Classes
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../Platform
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../Protobuf
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../CocosDenshion/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../LibOS/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../Lib91/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../extensions
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../tinyxml
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../protobuf-2.5.0rc1/src
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../jsoncpp-src-0.5.0/include


LOCAL_WHOLE_STATIC_LIBRARIES := cocos2dx_static
LOCAL_WHOLE_STATIC_LIBRARIES += cocosdenshion_static
LOCAL_WHOLE_STATIC_LIBRARIES += cocos_extension_static
LOCAL_WHOLE_STATIC_LIBRARIES += protobuf_static
LOCAL_WHOLE_STATIC_LIBRARIES += jsoncpp_static
LOCAL_WHOLE_STATIC_LIBRARIES += tinyxml_static
LOCAL_WHOLE_STATIC_LIBRARIES += OS_static
LOCAL_WHOLE_STATIC_LIBRARIES += lp91_static
LOCAL_WHOLE_STATIC_LIBRARIES += cocos_lua_static

#与Game.so无关，不需要
#LOCAL_SHARED_LIBRARIES := udid

#这个不能用，早期arm7硬件不保险支持
#LOCAL_ARM_NEON := true

#默认是thumb16位指令，设arm mode意思编译为32位指令
#LOCAL_ARM_MODE := arm

LOCAL_CFLAGS += -DANDROID
LOCAL_CPPFLAGS += -DANDROID

include $(BUILD_SHARED_LIBRARY)


$(call import-module,cocos2dx)
$(call import-module,CocosDenshion/android)
$(call import-module,extensions)
$(call import-module,protobuf-2.5.0rc1/proj.android)
$(call import-module,jsoncpp-src-0.5.0/makefiles)
$(call import-module,tinyxml/proj.android)
$(call import-module,libOS/android)
$(call import-module,91Lib/android)
$(call import-module,scripting/lua/proj.android)


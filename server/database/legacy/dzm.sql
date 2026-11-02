/*
SQLyog 企业版 - MySQL GUI v8.14 
MySQL - 5.5.32 : Database - dzm
*********************************************************************
*/

/*!40101 SET NAMES utf8 */;

/*!40101 SET SQL_MODE=''*/;

/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;
CREATE DATABASE /*!32312 IF NOT EXISTS*/`dzm` /*!40100 DEFAULT CHARACTER SET utf8 COLLATE utf8_bin */;

USE `dzm`;

/*Table structure for table `activecode` */

DROP TABLE IF EXISTS `activecode`;

CREATE TABLE `activecode` (
  `active_code` varchar(50) COLLATE utf8_bin NOT NULL,
  `create_time` int(11) NOT NULL,
  `used_time` int(11) NOT NULL DEFAULT '0',
  `playerid` varchar(50) COLLATE utf8_bin NOT NULL DEFAULT '',
  `status` int(10) NOT NULL DEFAULT '0',
  PRIMARY KEY (`active_code`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;

/*Table structure for table `adventurefightrank` */

DROP TABLE IF EXISTS `adventurefightrank`;

CREATE TABLE `adventurefightrank` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `playerId` int(10) NOT NULL,
  `playerName` varchar(50) COLLATE utf8_bin NOT NULL,
  `playerLevel` smallint(6) DEFAULT NULL,
  `barrierCount` tinyint(4) NOT NULL,
  `passBarrier` smallint(6) DEFAULT NULL,
  `obtainStar` int(11) DEFAULT '0',
  `continueInRankAmount` smallint(6) DEFAULT NULL,
  `rankTime` int(11) NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;

/*Table structure for table `adventurefightstats` */

DROP TABLE IF EXISTS `adventurefightstats`;

CREATE TABLE `adventurefightstats` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `playerId` int(11) NOT NULL,
  `barrierCount` tinyint(11) NOT NULL,
  `battlePanelType` tinyint(4) NOT NULL,
  `battleAmount` tinyint(4) NOT NULL,
  `yestBarrierLV` smallint(6) NOT NULL,
  `yestObtainStar` int(11) NOT NULL,
  `maxBarrierLV` smallint(6) NOT NULL,
  `maxObtainStar` smallint(6) NOT NULL,
  `currBarrierLV` int(11) NOT NULL DEFAULT '0',
  `currObtainStar` int(11) NOT NULL DEFAULT '0',
  `currSurplusStar` int(11) DEFAULT NULL,
  `extraRewardDetailInfo` varchar(255) CHARACTER SET utf8 DEFAULT NULL,
  `extraRewardConf` varchar(255) CHARACTER SET utf8 DEFAULT NULL,
  `obtainStarDetail` varchar(255) CHARACTER SET utf8 DEFAULT NULL,
  `proAdditionInfo` varchar(255) CHARACTER SET utf8 DEFAULT NULL,
  `additionSelDetail` varchar(255) CHARACTER SET utf8 DEFAULT NULL,
  `teamConfInfo` text CHARACTER SET utf8,
  `lastUpdateTime` int(11) NOT NULL,
  `lastOprTime` int(11) NOT NULL,
  `nextResetTime` int(11) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;

/*Table structure for table `bag` */

DROP TABLE IF EXISTS `bag`;

CREATE TABLE `bag` (
  `id` int(10) NOT NULL AUTO_INCREMENT,
  `playerid` int(10) NOT NULL,
  `itemid` int(10) NOT NULL,
  `count` int(10) NOT NULL,
  `source` int(10) NOT NULL,
  `create_time` int(10) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;

/*Table structure for table `battle` */

DROP TABLE IF EXISTS `battle`;

CREATE TABLE `battle` (
  `id` int(10) NOT NULL AUTO_INCREMENT,
  `playerid` int(10) NOT NULL,
  `discipleid` int(10) DEFAULT NULL,
  `itemid` int(10) DEFAULT NULL,
  `level` int(10) DEFAULT NULL,
  `attack` int(10) DEFAULT NULL,
  `defence` int(10) DEFAULT NULL,
  `health` int(10) DEFAULT NULL,
  `rp` int(10) DEFAULT NULL,
  `defaultSkill` int(10) DEFAULT NULL,
  `defaultSkillItem` int(10) DEFAULT NULL,
  `defaultSkillType` int(10) DEFAULT NULL,
  `defaultSkillQuantity` float DEFAULT NULL,
  `skill1` int(10) DEFAULT NULL,
  `skill1item` int(10) DEFAULT NULL,
  `skill1Type` int(10) DEFAULT NULL,
  `skill1Quantity` float DEFAULT NULL,
  `skill2` int(10) DEFAULT NULL,
  `skill2item` int(10) DEFAULT NULL,
  `skill2Type` int(10) DEFAULT NULL,
  `skill2Quantity` float DEFAULT NULL,
  `equip1` int(10) DEFAULT NULL,
  `equip1item` int(10) DEFAULT NULL,
  `equip2` int(10) DEFAULT NULL,
  `equip2item` int(10) DEFAULT NULL,
  `equip3` int(10) DEFAULT NULL,
  `equip3item` int(10) DEFAULT NULL,
  `fates` varchar(60) COLLATE utf8_bin DEFAULT NULL,
  `fatesstatus` varchar(20) COLLATE utf8_bin DEFAULT NULL,
  PRIMARY KEY (`id`),
  KEY `index_playerId` (`playerid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;

/*Table structure for table `career` */

DROP TABLE IF EXISTS `career`;

CREATE TABLE `career` (
  `playerid` int(10) NOT NULL,
  `cchapter` int(10) NOT NULL,
  `cpoint` int(10) NOT NULL,
  `chapter_add_power` text COLLATE utf8_bin,
  `point_evaluate` text COLLATE utf8_bin,
  `point_count` text COLLATE utf8_bin,
  `check_time` int(10) NOT NULL,
  PRIMARY KEY (`playerid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;

/*Table structure for table `cdkey` */

DROP TABLE IF EXISTS `cdkey`;

CREATE TABLE `cdkey` (
  `cd_key` varchar(50) COLLATE utf8_bin NOT NULL,
  `create_time` int(11) NOT NULL,
  `used_time` int(11) NOT NULL DEFAULT '0',
  `playerid` int(11) NOT NULL DEFAULT '0',
  `status` int(10) NOT NULL DEFAULT '0',
  PRIMARY KEY (`cd_key`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;

/*Table structure for table `challenge` */

DROP TABLE IF EXISTS `challenge`;

CREATE TABLE `challenge` (
  `playerid` int(10) NOT NULL,
  `level` int(10) NOT NULL,
  `name` varchar(50) COLLATE utf8_bin NOT NULL,
  `isnpc` tinyint(4) NOT NULL,
  `rank` int(10) NOT NULL,
  `highest_rank` int(10) NOT NULL,
  `score` int(10) NOT NULL,
  `hasgotreward` varchar(200) COLLATE utf8_bin NOT NULL,
  `lastsettletime` int(10) NOT NULL,
  `lastrefreshtime` int(10) NOT NULL,
  PRIMARY KEY (`playerid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;

/*Table structure for table `disciple` */

DROP TABLE IF EXISTS `disciple`;

CREATE TABLE `disciple` (
  `id` int(10) NOT NULL AUTO_INCREMENT,
  `playerid` int(10) NOT NULL,
  `itemid` int(10) NOT NULL,
  `level` int(10) NOT NULL,
  `exp` int(10) NOT NULL,
  `health` float NOT NULL,
  `attack` float NOT NULL,
  `defence` float NOT NULL,
  `rp` float NOT NULL,
  `upgradelevel` int(10) NOT NULL,
  `potentiality` int(10) NOT NULL,
  `skill1` int(10) NOT NULL,
  `battleid` int(10) NOT NULL,
  `expire_time` int(10) NOT NULL,
  PRIMARY KEY (`id`),
  KEY `index_player` (`playerid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;

/*Table structure for table `equip` */

DROP TABLE IF EXISTS `equip`;

CREATE TABLE `equip` (
  `id` int(10) NOT NULL AUTO_INCREMENT,
  `playerid` int(10) NOT NULL,
  `itemid` int(10) NOT NULL,
  `type` int(10) NOT NULL,
  `level` int(10) NOT NULL,
  `refinexp` int(10) NOT NULL,
  `refinelevel` int(10) NOT NULL,
  `attack` int(10) NOT NULL,
  `defence` int(10) NOT NULL,
  `health` int(10) NOT NULL,
  `battleid` int(10) NOT NULL,
  `expire_time` int(10) NOT NULL,
  `diamondinfo` varchar(2047) COLLATE utf8_bin DEFAULT '{}',
  PRIMARY KEY (`id`),
  KEY `index_playerid` (`playerid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;

/*Table structure for table `handbook` */

DROP TABLE IF EXISTS `handbook`;

CREATE TABLE `handbook` (
  `playerid` int(10) NOT NULL,
  `discipleInfo` varchar(2047) COLLATE utf8_bin DEFAULT NULL,
  `equipInfo` varchar(2047) COLLATE utf8_bin DEFAULT NULL,
  `skillInfo` varchar(2047) COLLATE utf8_bin DEFAULT NULL,
  PRIMARY KEY (`playerid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;

/*Table structure for table `invitestatus` */

DROP TABLE IF EXISTS `invitestatus`;

CREATE TABLE `invitestatus` (
  `playerid` int(10) NOT NULL,
  `invitestatus` varchar(200) COLLATE utf8_bin NOT NULL DEFAULT '{}',
  `rewardstatus` varchar(200) COLLATE utf8_bin NOT NULL DEFAULT '{}',
  PRIMARY KEY (`playerid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;

/*Table structure for table `leagua` */

DROP TABLE IF EXISTS `leagua`;

CREATE TABLE `leagua` (
  `leaguaId` int(10) NOT NULL AUTO_INCREMENT,
  `leaguaName` varchar(20) COLLATE utf8_bin NOT NULL,
  `leaguaLevel` int(10) NOT NULL DEFAULT '1',
  `leaguaMaxMemberCount` int(10) NOT NULL,
  `leaguaCurMemberCount` int(10) NOT NULL DEFAULT '1',
  `leaguaFunds` bigint(64) NOT NULL DEFAULT '0',
  `ownerPlayerId` int(10) NOT NULL,
  `leaguaActiveMedal` int(10) NOT NULL DEFAULT '0',
  `leaguaBroadcast` varchar(150) COLLATE utf8_bin NOT NULL DEFAULT '',
  `leaguaTotemInfo` text COLLATE utf8_bin,
  `leaguaBuildingInfo` text COLLATE utf8_bin,
  `leaguaShopInfo` text COLLATE utf8_bin,
  `leaguaShopRefreshTime` int(10) NOT NULL,
  `leaguaWinRate` float NOT NULL DEFAULT '0',
  `createTime` int(10) NOT NULL,
  `broadcastRefreshTime` int(10) NOT NULL,
  `contributionCount` int(10) NOT NULL DEFAULT '0',
  `contributionTime` int(10) NOT NULL DEFAULT '0',
  `receiveMedalTime` int(10) NOT NULL DEFAULT '0',
  `transLeaderTime` int(10) NOT NULL DEFAULT '0',
  PRIMARY KEY (`leaguaId`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;

/*Table structure for table `leaguaapply` */

DROP TABLE IF EXISTS `leaguaapply`;

CREATE TABLE `leaguaapply` (
  `id` int(10) NOT NULL AUTO_INCREMENT,
  `playerId` int(10) NOT NULL,
  `playerName` varchar(20) COLLATE utf8_bin NOT NULL,
  `playerLevel` int(10) NOT NULL,
  `applyTime` int(10) NOT NULL,
  `applyLeaguaId` int(10) NOT NULL,
  PRIMARY KEY (`id`),
  KEY `index_leaguaid` (`applyLeaguaId`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;

/*Table structure for table `leaguamedal` */

DROP TABLE IF EXISTS `leaguamedal`;

CREATE TABLE `leaguamedal` (
  `id` int(10) NOT NULL AUTO_INCREMENT,
  `medalId` int(10) NOT NULL,
  `ownerLeaguaID` int(10) NOT NULL DEFAULT '0',
  `ownerLeaguaName` varchar(20) COLLATE utf8_bin NOT NULL DEFAULT '',
  `count` int(10) NOT NULL,
  PRIMARY KEY (`id`),
  KEY `index_leaguaId` (`ownerLeaguaID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;

/*Table structure for table `leaguamedalchangeinfo` */

DROP TABLE IF EXISTS `leaguamedalchangeinfo`;

CREATE TABLE `leaguamedalchangeinfo` (
  `id` int(10) NOT NULL AUTO_INCREMENT,
  `medalId` int(10) NOT NULL,
  `beforeOwnerLeaguaName` varchar(20) COLLATE utf8_bin NOT NULL DEFAULT '""',
  `afterOwnerLeaguaName` varchar(20) COLLATE utf8_bin NOT NULL,
  `changeTime` int(10) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;

/*Table structure for table `leaguamembers` */

DROP TABLE IF EXISTS `leaguamembers`;

CREATE TABLE `leaguamembers` (
  `playerId` int(10) NOT NULL,
  `playerName` varchar(20) COLLATE utf8_bin NOT NULL,
  `playerlevel` int(10) NOT NULL,
  `playerGrade` int(10) NOT NULL DEFAULT '0',
  `playerLeftContribution` int(10) NOT NULL DEFAULT '0',
  `playerTotalContribution` int(10) NOT NULL DEFAULT '0',
  `playerKillCount` int(10) NOT NULL DEFAULT '0',
  `guildId` int(10) NOT NULL,
  `shopPurchaseInfo` text COLLATE utf8_bin NOT NULL,
  `lastBuyTime` int(10) NOT NULL,
  `time` int(10) NOT NULL,
  `nextcanattacktime` int(10) NOT NULL,
  `leftattacktimes` int(10) NOT NULL,
  `leftrewardtimes` int(10) NOT NULL,
  `reccontributiontime` int(10) NOT NULL,
  `lastrefreshtime` int(10) NOT NULL,
  PRIMARY KEY (`playerId`),
  KEY `index_leaguaid` (`guildId`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;

/*Table structure for table `mapping` */

DROP TABLE IF EXISTS `mapping`;

CREATE TABLE `mapping` (
  `puid` varchar(50) COLLATE utf8_bin NOT NULL,
  `gameid` int(10) NOT NULL AUTO_INCREMENT,
  `name` varchar(50) COLLATE utf8_bin NOT NULL,
  `passwd` varchar(50) COLLATE utf8_bin DEFAULT NULL,
  UNIQUE KEY `gameid` (`gameid`),
  UNIQUE KEY `name` (`name`),
  KEY `index_puid` (`puid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;

/*Table structure for table `moment` */

DROP TABLE IF EXISTS `moment`;

CREATE TABLE `moment` (
  `playerid` int(10) NOT NULL,
  `careertypeid` int(10) NOT NULL,
  `careertypenum` int(10) NOT NULL,
  `viptypeid` int(10) NOT NULL,
  `viptypenum` float NOT NULL,
  `challengetypeid` int(10) NOT NULL,
  `challengetypenum` int(10) NOT NULL,
  `chiptypeid` int(10) NOT NULL,
  `chiptypenum` int(10) NOT NULL,
  `fighttypeid` int(10) NOT NULL,
  `fighttypenum` int(10) NOT NULL,
  `destroytypeid` int(10) NOT NULL,
  `destroytypenum` int(10) NOT NULL,
  `friendtypeid` int(10) NOT NULL,
  `friendtypenum` int(10) NOT NULL,
  `fightranktypeid` int(10) NOT NULL,
  `fightranktypenum` float NOT NULL,
  PRIMARY KEY (`playerid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;

/*Table structure for table `pay` */

DROP TABLE IF EXISTS `pay`;

CREATE TABLE `pay` (
  `CooOrderSerial` varchar(100) COLLATE utf8_bin NOT NULL,
  `ConsumeStreamId` varchar(100) COLLATE utf8_bin NOT NULL,
  `uid` varchar(50) COLLATE utf8_bin NOT NULL,
  `playerId` int(10) NOT NULL,
  `goodsId` int(10) NOT NULL,
  `goodsCount` int(10) NOT NULL,
  `goodsCost` int(10) NOT NULL,
  `amount` int(10) NOT NULL,
  `addnum` int(10) NOT NULL,
  `isFirstPay` smallint(6) NOT NULL,
  `request_time` varchar(50) COLLATE utf8_bin NOT NULL,
  `create_time` int(10) NOT NULL,
  PRIMARY KEY (`CooOrderSerial`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;

/*Table structure for table `player` */

DROP TABLE IF EXISTS `player`;

CREATE TABLE `player` (
  `id` int(10) NOT NULL,
  `puid` varchar(50) COLLATE utf8_bin NOT NULL,
  `password` varchar(50) COLLATE utf8_bin NOT NULL,
  `name` varchar(50) COLLATE utf8_bin NOT NULL,
  `level` int(10) NOT NULL,
  `exp` int(10) NOT NULL,
  `powerbytime` int(10) NOT NULL,
  `powerbychicken` int(10) NOT NULL,
  `poweraddtime` int(10) NOT NULL,
  `eatfoodtime` int(10) NOT NULL,
  `maxpower` int(10) NOT NULL,
  `todaychickennum` int(10) NOT NULL,
  `eatchickentime` int(10) NOT NULL,
  `vitalitybytime` int(10) NOT NULL,
  `vitalitybypellet` int(10) NOT NULL,
  `vitalityaddtime` int(10) NOT NULL,
  `maxvitality` int(10) NOT NULL,
  `todaypelletnum` int(10) NOT NULL,
  `eatpellettime` int(10) NOT NULL,
  `viplevel` int(10) NOT NULL,
  `rechargenum` int(10) NOT NULL,
  `silvercoins` bigint(10) NOT NULL,
  `sysgoldcoins` int(10) NOT NULL,
  `rechargegoldcoins` int(10) NOT NULL,
  `todayleavetimes` int(10) NOT NULL,
  `tentime` int(10) NOT NULL,
  `hundredtime` int(10) NOT NULL,
  `wanlitime` int(10) NOT NULL,
  `tutorialstep` int(10) NOT NULL,
  `registertime` int(10) NOT NULL,
  `origin` int(10) NOT NULL,
  `momentum` int(10) NOT NULL,
  `battleids` varchar(60) COLLATE utf8_bin NOT NULL,
  `setting` varchar(50) COLLATE utf8_bin NOT NULL,
  `firstgain` varchar(255) COLLATE utf8_bin NOT NULL DEFAULT '{}',
  `lastlogin` int(10) NOT NULL,
  `invitedid` int(10) NOT NULL DEFAULT '0',
  `instructionFinishTime` int(10) NOT NULL DEFAULT '0',
  `todaybuychickennum` int(10) NOT NULL DEFAULT '0',
  `todaybuypelletnum` int(10) NOT NULL DEFAULT '0',
  `leaguaid` int(10) NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  KEY `index_puid` (`puid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;

/*Table structure for table `playerfrom` */

DROP TABLE IF EXISTS `playerfrom`;

CREATE TABLE `playerfrom` (
  `puid` varchar(50) COLLATE utf8_bin NOT NULL,
  `playerid` int(10) NOT NULL,
  `platform` varchar(30) COLLATE utf8_bin DEFAULT NULL,
  `deviceid` varchar(100) COLLATE utf8_bin DEFAULT NULL,
  `createtime` int(10) NOT NULL,
  PRIMARY KEY (`puid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;

/*Table structure for table `relationship` */

DROP TABLE IF EXISTS `relationship`;

CREATE TABLE `relationship` (
  `playerid` int(10) NOT NULL,
  `friendids` text COLLATE utf8_bin,
  `enemyids` text COLLATE utf8_bin,
  `confirm` text COLLATE utf8_bin,
  PRIMARY KEY (`playerid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;

/*Table structure for table `rewards` */

DROP TABLE IF EXISTS `rewards`;

CREATE TABLE `rewards` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `playerId` int(11) NOT NULL,
  `rewardType` tinyint(4) NOT NULL,
  `rewardMsg` varchar(255) COLLATE utf8_bin DEFAULT NULL,
  `rewardInfo` varchar(50) COLLATE utf8_bin DEFAULT NULL,
  `rewardContent` varchar(255) CHARACTER SET utf8 DEFAULT NULL,
  `status` tinyint(4) NOT NULL,
  `addTime` int(11) NOT NULL,
  `rewardTime` int(11) DEFAULT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `UQ_rewards_id` (`id`),
  KEY `IDX_playerId` (`playerId`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;

/*Table structure for table `skill` */

DROP TABLE IF EXISTS `skill`;

CREATE TABLE `skill` (
  `id` int(10) NOT NULL AUTO_INCREMENT,
  `playerid` int(10) NOT NULL,
  `itemid` int(10) NOT NULL,
  `type` int(10) NOT NULL,
  `level` int(10) NOT NULL,
  `quantity` float NOT NULL,
  `consume` int(10) NOT NULL,
  `battleid` int(10) NOT NULL,
  `isdefaultskill` tinyint(4) NOT NULL,
  `expire_time` int(10) NOT NULL,
  PRIMARY KEY (`id`),
  KEY `index_playerid` (`playerid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;

/*Table structure for table `skillbook` */

DROP TABLE IF EXISTS `skillbook`;

CREATE TABLE `skillbook` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `playerId` int(11) NOT NULL,
  `skillId` int(11) NOT NULL,
  `pieceChance` int(11) NOT NULL DEFAULT '0',
  `skillBookDetail` varchar(255) COLLATE utf8_bin NOT NULL DEFAULT '{}',
  PRIMARY KEY (`id`),
  UNIQUE KEY `UQ_SkillBook_id` (`id`),
  KEY `IDX_playerId` (`playerId`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin CHECKSUM=1 DELAY_KEY_WRITE=1 ROW_FORMAT=DYNAMIC;

/*Table structure for table `soul` */

DROP TABLE IF EXISTS `soul`;

CREATE TABLE `soul` (
  `id` int(10) NOT NULL AUTO_INCREMENT,
  `itemid` int(10) NOT NULL,
  `playerid` int(10) NOT NULL,
  `count` int(10) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;

/*Table structure for table `stats` */

DROP TABLE IF EXISTS `stats`;

CREATE TABLE `stats` (
  `playerid` int(10) NOT NULL,
  `firstbaitime` int(10) NOT NULL DEFAULT '0',
  `firstwantime` int(10) NOT NULL DEFAULT '0',
  `buygifts` varchar(300) COLLATE utf8_bin NOT NULL,
  `discipleLucky` int(10) NOT NULL,
  `nextDisciple` int(10) NOT NULL,
  `logincount` int(10) NOT NULL,
  `getreward` int(10) NOT NULL,
  `loginreward` varchar(50) COLLATE utf8_bin NOT NULL,
  `levelupreward` varchar(50) COLLATE utf8_bin DEFAULT '',
  `visitRewardCount` int(5) NOT NULL DEFAULT '0',
  `visitTimes` int(10) NOT NULL DEFAULT '0',
  `lastVisitTime` int(10) NOT NULL,
  `hasVisited` int(2) NOT NULL DEFAULT '0',
  `luckOne` int(10) NOT NULL,
  `luckTwo` int(10) NOT NULL,
  `treasureWorth` int(10) NOT NULL,
  `ameatfood` tinyint(4) NOT NULL,
  `pmeatfood` tinyint(4) NOT NULL,
  `challengetimes` int(10) NOT NULL,
  `luckPool` float NOT NULL,
  `adContinueInfo` varchar(100) COLLATE utf8_bin DEFAULT '{}',
  `cdkeyTypes` varchar(500) COLLATE utf8_bin DEFAULT NULL,
  `sysmsgTime` int(10) DEFAULT '0',
  `pieceskilltimes` int(10) NOT NULL DEFAULT '0',
  `getRebate` int(10) DEFAULT '0',
  `chargeHeap` int(10) DEFAULT '0',
  `chargeTerm` int(10) DEFAULT '0',
  `fortuneComeTimes` int(10) DEFAULT '0',
  `giveSoulTerm` int(10) DEFAULT '0',
  `giveSoulHeap2` int(10) DEFAULT '0',
  `giveSoulHeap3` int(10) DEFAULT '0',
  `giveSoulDifferTimes2` int(10) DEFAULT '0',
  `giveSoulDifferTimes3` int(10) DEFAULT '0',
  `fightEndTimes` int(10) DEFAULT '0',
  `nextQuickCareerTime` int(10) DEFAULT '0',
  `luckyStarTermid` int(10) DEFAULT '0',
  `luckyStarDropTimes1` int(10) DEFAULT '0',
  `luckyStarDropTimes2` int(10) DEFAULT '0',
  `luckyStayHeap1` int(10) DEFAULT '0',
  `luckyStayHeap2` int(10) DEFAULT '0',
  `challengeActTermId` int(10) DEFAULT '0',
  `highestRankInAct` int(10) DEFAULT '0',
  `deepWaterLastTimes` varchar(100) COLLATE utf8_bin DEFAULT '{}',
  `deepWaterHeap` varchar(100) COLLATE utf8_bin DEFAULT '{}',
  `deepWaterTermid` int(10) DEFAULT '0',
  `dwNextRefreshTime` int(10) DEFAULT '0',
  PRIMARY KEY (`playerid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;

/*Table structure for table `sysmsginfo` */

DROP TABLE IF EXISTS `sysmsginfo`;

CREATE TABLE `sysmsginfo` (
  `id` int(11) NOT NULL,
  `playerId` int(11) NOT NULL,
  `msgType` tinyint(4) NOT NULL,
  `msgInfo` varchar(255) COLLATE utf8_bin NOT NULL DEFAULT '{}',
  `msgContent` text COLLATE utf8_bin,
  `status` tinyint(4) NOT NULL DEFAULT '0',
  `addTime` int(11) NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `UQ_sysmsginfo_id` (`id`),
  KEY `IDX_playerId` (`playerId`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin CHECKSUM=1 DELAY_KEY_WRITE=1 ROW_FORMAT=DYNAMIC;

/*Table structure for table `teambuffer` */

DROP TABLE IF EXISTS `teambuffer`;

CREATE TABLE `teambuffer` (
  `playerid` int(10) NOT NULL,
  `healthbufferlevel` int(10) NOT NULL,
  `healthbufferexp` int(10) NOT NULL,
  `attackbufferlevel` int(10) NOT NULL,
  `attackbufferexp` int(10) NOT NULL,
  `defencebufferlevel` int(10) NOT NULL,
  `defencebufferexp` int(10) NOT NULL,
  `rpbufferlevel` int(10) NOT NULL,
  `rpbufferexp` int(10) NOT NULL,
  PRIMARY KEY (`playerid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;

/*Table structure for table `title` */

DROP TABLE IF EXISTS `title`;

CREATE TABLE `title` (
  `playerid` int(10) NOT NULL,
  `careertypeid` int(10) NOT NULL,
  `careertypenum` int(10) NOT NULL,
  `viptypeid` int(10) NOT NULL,
  `viptypenum` float NOT NULL,
  `challengetypeid` int(10) NOT NULL,
  `challengetypenum` int(10) NOT NULL,
  `chiptypeid` int(10) NOT NULL,
  `chiptypenum` int(10) NOT NULL,
  `fighttypeid` int(10) NOT NULL,
  `fighttypenum` int(10) NOT NULL,
  `battletypeid` int(10) NOT NULL,
  `battletypenum` int(10) NOT NULL,
  `friendtypeid` int(10) NOT NULL,
  `friendtypenum` int(10) NOT NULL,
  `fightrankid` int(10) NOT NULL,
  `fightranknum` float NOT NULL,
  `fightranktime` int(10) NOT NULL,
  `battlerankid` int(10) NOT NULL,
  `battleranknum` float NOT NULL,
  `battleranktime` int(10) NOT NULL,
  PRIMARY KEY (`playerid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;

/*Table structure for table `worldboss` */

DROP TABLE IF EXISTS `worldboss`;

CREATE TABLE `worldboss` (
  `id` int(10) NOT NULL AUTO_INCREMENT,
  `totalHealth` bigint(20) NOT NULL,
  `leftHealth` bigint(20) NOT NULL,
  `level` int(10) NOT NULL,
  `killedTime` int(10) NOT NULL,
  `lastRefreshTime` int(10) NOT NULL,
  `killerId` int(10) NOT NULL,
  `killerName` varchar(50) COLLATE utf8_bin NOT NULL,
  `killerLevel` int(10) NOT NULL,
  `killerDamage` int(10) NOT NULL,
  `date` int(10) NOT NULL,
  PRIMARY KEY (`id`),
  KEY `key_date` (`date`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;

/*Table structure for table `worldbossrank` */

DROP TABLE IF EXISTS `worldbossrank`;

CREATE TABLE `worldbossrank` (
  `id` int(10) NOT NULL AUTO_INCREMENT,
  `playerid` int(10) NOT NULL,
  `rank` int(10) NOT NULL,
  `level` int(10) NOT NULL,
  `name` varchar(50) COLLATE utf8_bin NOT NULL,
  `damage` int(10) NOT NULL,
  `ranktime` int(10) NOT NULL,
  PRIMARY KEY (`id`),
  KEY `key_time` (`ranktime`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;

/*Table structure for table `activityinfo` */

DROP TABLE IF EXISTS `activityinfo`;

CREATE TABLE `activityinfo` (
  `playerid` int(10) NOT NULL,
  `activityinfo` text COLLATE utf8_bin,
  `laseRefreshTime` int(10) NOT NULL,
  PRIMARY KEY (`playerid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;

/*Table structure for table `wpayopr201304` */

DROP TABLE IF EXISTS `wpayopr201304`;

CREATE TABLE `wpayopr201304` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `playerId` int(11) NOT NULL,
  `propId` int(11) NOT NULL,
  `payType` smallint(6) NOT NULL,
  `buyNum` int(11) NOT NULL,
  `originalCash` int(11) NOT NULL,
  `consumeCash` int(11) NOT NULL,
  `surplusCash` int(11) NOT NULL,
  `oprTime` int(11) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;

/*Table structure for table `wpayopr201305` */

DROP TABLE IF EXISTS `wpayopr201305`;

CREATE TABLE `wpayopr201305` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `playerId` int(11) NOT NULL,
  `propId` int(11) NOT NULL,
  `payType` smallint(6) NOT NULL,
  `buyNum` int(11) NOT NULL,
  `originalCash` int(11) NOT NULL,
  `consumeCash` int(11) NOT NULL,
  `surplusCash` int(11) NOT NULL,
  `oprTime` int(11) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;

/*Table structure for table `wpayopr201306` */

DROP TABLE IF EXISTS `wpayopr201306`;

CREATE TABLE `wpayopr201306` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `playerId` int(11) NOT NULL,
  `propId` int(11) NOT NULL,
  `payType` smallint(6) NOT NULL,
  `buyNum` int(11) NOT NULL,
  `originalCash` int(11) NOT NULL,
  `consumeCash` int(11) NOT NULL,
  `surplusCash` int(11) NOT NULL,
  `oprTime` int(11) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;

/*Table structure for table `wpayopr201307` */

DROP TABLE IF EXISTS `wpayopr201307`;

CREATE TABLE `wpayopr201307` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `playerId` int(11) NOT NULL,
  `propId` int(11) NOT NULL,
  `payType` smallint(6) NOT NULL,
  `buyNum` int(11) NOT NULL,
  `originalCash` int(11) NOT NULL,
  `consumeCash` int(11) NOT NULL,
  `surplusCash` int(11) NOT NULL,
  `oprTime` int(11) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;

/*Table structure for table `wpayopr201308` */

DROP TABLE IF EXISTS `wpayopr201308`;

CREATE TABLE `wpayopr201308` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `playerId` int(11) NOT NULL,
  `propId` int(11) NOT NULL,
  `payType` smallint(6) NOT NULL,
  `buyNum` int(11) NOT NULL,
  `originalCash` int(11) NOT NULL,
  `consumeCash` int(11) NOT NULL,
  `surplusCash` int(11) NOT NULL,
  `oprTime` int(11) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;

/*Table structure for table `wpayopr201309` */

DROP TABLE IF EXISTS `wpayopr201309`;

CREATE TABLE `wpayopr201309` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `playerId` int(11) NOT NULL,
  `propId` int(11) NOT NULL,
  `payType` smallint(6) NOT NULL,
  `buyNum` int(11) NOT NULL,
  `originalCash` int(11) NOT NULL,
  `consumeCash` int(11) NOT NULL,
  `surplusCash` int(11) NOT NULL,
  `oprTime` int(11) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;

/*Table structure for table `wpayopr201310` */

DROP TABLE IF EXISTS `wpayopr201310`;

CREATE TABLE `wpayopr201310` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `playerId` int(11) NOT NULL,
  `propId` int(11) NOT NULL,
  `payType` smallint(6) NOT NULL,
  `buyNum` int(11) NOT NULL,
  `originalCash` int(11) NOT NULL,
  `consumeCash` int(11) NOT NULL,
  `surplusCash` int(11) NOT NULL,
  `oprTime` int(11) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;

/*Table structure for table `wpayopr201311` */

DROP TABLE IF EXISTS `wpayopr201311`;

CREATE TABLE `wpayopr201311` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `playerId` int(11) NOT NULL,
  `propId` int(11) NOT NULL,
  `payType` smallint(6) NOT NULL,
  `buyNum` int(11) NOT NULL,
  `originalCash` int(11) NOT NULL,
  `consumeCash` int(11) NOT NULL,
  `surplusCash` int(11) NOT NULL,
  `oprTime` int(11) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;

/*Table structure for table `wpayopr201312` */

DROP TABLE IF EXISTS `wpayopr201312`;

CREATE TABLE `wpayopr201312` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `playerId` int(11) NOT NULL,
  `propId` int(11) NOT NULL,
  `payType` smallint(6) NOT NULL,
  `buyNum` int(11) NOT NULL,
  `originalCash` int(11) NOT NULL,
  `consumeCash` int(11) NOT NULL,
  `surplusCash` int(11) NOT NULL,
  `oprTime` int(11) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

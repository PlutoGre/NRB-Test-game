# Host: localhost  (Version: 5.5.40)
# Date: 2015-09-22 21:50:49
# Generator: MySQL-Front 5.3  (Build 4.120)

/*!40101 SET NAMES utf8 */;

#
# Structure for table "activecode"
#

DROP TABLE IF EXISTS `activecode`;
CREATE TABLE `activecode` (
  `active_code` varchar(50) COLLATE utf8_bin NOT NULL,
  `create_time` int(11) NOT NULL,
  `used_time` int(11) NOT NULL DEFAULT '0',
  `playerid` varchar(50) COLLATE utf8_bin NOT NULL DEFAULT '',
  `status` int(10) NOT NULL DEFAULT '0',
  PRIMARY KEY (`active_code`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;

#
# Data for table "activecode"
#


#
# Structure for table "activityinfo"
#

DROP TABLE IF EXISTS `activityinfo`;
CREATE TABLE `activityinfo` (
  `playerid` int(10) NOT NULL,
  `activityinfo` text COLLATE utf8_bin,
  `laseRefreshTime` int(10) NOT NULL,
  PRIMARY KEY (`playerid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;

#
# Data for table "activityinfo"
#

INSERT INTO `activityinfo` VALUES (1,'{}',0);

#
# Structure for table "adventurefightrank"
#

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

#
# Data for table "adventurefightrank"
#


#
# Structure for table "adventurefightstats"
#

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

#
# Data for table "adventurefightstats"
#


#
# Structure for table "bag"
#

DROP TABLE IF EXISTS `bag`;
CREATE TABLE `bag` (
  `id` int(10) NOT NULL AUTO_INCREMENT,
  `playerid` int(10) NOT NULL,
  `itemid` int(10) NOT NULL,
  `count` int(10) NOT NULL,
  `source` int(10) NOT NULL,
  `create_time` int(10) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=5 DEFAULT CHARSET=utf8 COLLATE=utf8_bin;

#
# Data for table "bag"
#

INSERT INTO `bag` VALUES (1,1,3503001,1,0,1442926100),(2,1,3001002,1,0,1442926444),(3,1,2501001,1,0,1442927337),(4,1,2503001,1,0,1442927337);

#
# Structure for table "battle"
#

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
) ENGINE=InnoDB AUTO_INCREMENT=4 DEFAULT CHARSET=utf8 COLLATE=utf8_bin;

#
# Data for table "battle"
#

INSERT INTO `battle` VALUES (1,1,1,65,6,160,75,210,210,1,25,2,7.93,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'43,213,49,335,390,499','0,0,0,0,0,0'),(2,1,2,84,2,171,62,189,116,2,27,1,28.06,0,0,0,0,0,0,0,0,2,0,0,0,0,0,'454,326,408,183,249','0,0,0,0,0'),(3,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'','');

#
# Structure for table "career"
#

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

#
# Data for table "career"
#

INSERT INTO `career` VALUES (1,2,201,'{}','{\"101\":3,\"102\":3,\"103\":3,\"104\":3,\"105\":3,\"106\":3}','{\"103\":1,\"104\":1,\"105\":1,\"106\":1}',1442926099);

#
# Structure for table "cdkey"
#

DROP TABLE IF EXISTS `cdkey`;
CREATE TABLE `cdkey` (
  `cd_key` varchar(50) COLLATE utf8_bin NOT NULL,
  `create_time` int(11) NOT NULL,
  `used_time` int(11) NOT NULL DEFAULT '0',
  `playerid` int(11) NOT NULL DEFAULT '0',
  `status` int(10) NOT NULL DEFAULT '0',
  PRIMARY KEY (`cd_key`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;

#
# Data for table "cdkey"
#


#
# Structure for table "challenge"
#

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

#
# Data for table "challenge"
#

INSERT INTO `challenge` VALUES (10100000,1,'丁斯布塔',1,108,1,0,'',0,0),(10200000,1,'乌卡亚纳',1,107,1,0,'',0,0),(10300000,1,'乔丁萨基',1,106,1,0,'',0,0),(10400000,1,'亚布修帕洛',1,105,1,0,'',0,0),(10500000,1,'修拉波塔',1,104,1,0,'',0,0),(10600000,2,'克丁卡凯菲',1,103,1,0,'',0,0),(20100000,2,'凯乔拉菲诺',1,102,1,0,'',0,0),(20200000,2,'利塔迪基斯',1,101,1,0,'',0,0),(20300000,2,'卡修达玛菲',1,100,1,0,'',0,0),(20400000,2,'古乌雷玛拉',1,99,1,0,'',0,0),(20500000,3,'史亚卡修洛',1,98,1,0,'',0,0),(20600000,3,'基克玛萨',1,97,1,0,'',0,0),(20700000,3,'塔基帕洛曼',1,96,1,0,'',0,0),(30100000,3,'塔费尼洛',1,95,1,0,'',0,0),(30200000,3,'奥曼希亚',1,94,1,0,'',0,0),(30300000,3,'奥波克诺',1,93,1,0,'',0,0),(30400000,4,'尼修布沙',1,92,1,0,'',0,0),(30500000,4,'尼卡塔拉',1,91,1,0,'',0,0),(30600000,4,'巴布洛琳',1,90,1,0,'',0,0),(30700000,4,'巴萨迪雷帕',1,89,1,0,'',0,0),(30800000,4,'布基拉巴玛',1,88,1,0,'',0,0),(30900000,5,'布尼达帝',1,87,1,0,'',0,0),(40100000,5,'帕乔克汉纳',1,86,1,0,'',0,0),(40200000,5,'帕斯纳沙',1,85,1,0,'',0,0),(40300000,5,'帕杰克洛',1,84,1,0,'',0,0),(40400000,6,'帝尼拉曼',1,83,1,0,'',0,0),(40500000,6,'帝杰乔希',1,82,1,0,'',0,0),(40600000,6,'拉塔乌卡',1,81,1,0,'',0,0),(40700000,6,'拉达沙贝纳',1,80,1,0,'',0,0),(40800000,6,'斯克希玛泰',1,79,1,0,'',0,0),(40900000,7,'斯古曼修',1,78,1,0,'',0,0),(41000000,7,'曼丁尼诺',1,77,1,0,'',0,0),(50100000,7,'曼希纳基贝',1,76,1,0,'',0,0),(50200000,7,'曼杰利雷',1,75,1,0,'',0,0),(50300000,7,'杰乌亚拉欧',1,74,1,0,'',0,0),(50400000,7,'杰奥布利',1,73,1,0,'',0,0),(50500000,7,'欧亚索乔',1,72,1,0,'',0,0),(50600000,7,'欧尼索丁',1,71,1,0,'',0,0),(50700000,7,'欧琳尼特',1,70,1,0,'',0,0),(50800000,8,'汉玛巴泰',1,69,1,0,'',0,0),(50900000,8,'汉琳莫修',1,68,1,0,'',0,0),(51000000,8,'汉雷拉迪斯',1,67,1,0,'',0,0),(60100000,8,'沙修利迪丁',1,66,1,0,'',0,0),(60200000,8,'沙泰特曼',1,65,1,0,'',0,0),(60300000,8,'沙玛诺曼索',1,64,1,0,'',0,0),(60400000,8,'波乔达莫巴',1,63,1,0,'',0,0),(60500000,9,'波卡古洛',1,62,1,0,'',0,0),(60600000,9,'波布贝修',1,61,1,0,'',0,0),(60700000,9,'波洛亚曼',1,60,1,0,'',0,0),(60800000,9,'泰丁玛欧',1,59,1,0,'',0,0),(60900000,9,'泰基特古',1,58,1,0,'',0,0),(61000000,9,'泰特波修',1,57,1,0,'',0,0),(61100000,10,'泰萨古亚洛',1,56,1,0,'',0,0),(61200000,10,'洛乔巴贝拉',1,55,1,0,'',0,0),(70100000,10,'洛卡帝尼杰',1,54,1,0,'',0,0),(70200000,10,'洛塔基索',1,53,1,0,'',0,0),(70300000,10,'洛欧贝迪',1,52,1,0,'',0,0),(70400000,10,'洛欧史帕',1,51,1,0,'',0,0),(70500000,11,'特乌利乔',1,50,1,0,'',0,0),(70600000,11,'特卡尼利古',1,49,1,0,'',0,0),(70700000,11,'特希洛尼拉',1,48,1,0,'',0,0),(70800000,11,'特莫巴尼贝',1,47,1,0,'',0,0),(70900000,11,'特费莫杰琳',1,46,1,0,'',0,0),(71000000,12,'玛克贝纳',1,45,1,0,'',0,0),(71100000,12,'玛布纳欧',1,44,1,0,'',0,0),(71200000,13,'玛斯汉泰纳',1,43,1,0,'',0,0),(80100000,13,'玛索杰特',1,42,1,0,'',0,0),(80200000,13,'琳史特诺菲',1,41,1,0,'',0,0),(80300000,13,'琳洛诺莫欧',1,40,1,0,'',0,0),(80400000,13,'索利凯波',1,39,1,0,'',0,0),(80500000,14,'索尼萨巴',1,38,1,0,'',0,0),(80600000,14,'纳琳莫基',1,37,1,0,'',0,0),(80700000,14,'纳霍斯帕沙',1,36,1,0,'',0,0),(80800000,14,'艾亚奥达',1,35,1,0,'',0,0),(80900000,14,'艾塔史修索',1,34,1,0,'',0,0),(81000000,14,'莫克萨洛',1,33,1,0,'',0,0),(81100000,14,'莫达玛巴',1,32,1,0,'',0,0),(81200000,15,'菲利曼玛',1,31,1,0,'',0,0),(81300000,15,'菲基拉雷贝',1,30,1,0,'',0,0),(90100000,15,'菲泰乌希修',1,29,1,0,'',0,0),(90200000,15,'萨乌利修',1,28,1,0,'',0,0),(90300000,15,'萨希沙利',1,27,1,0,'',0,0),(90400000,15,'萨欧克巴',1,26,1,0,'',0,0),(90500000,15,'萨汉布奥',1,25,1,0,'',0,0),(90600000,17,'诺利纳丁',1,24,1,0,'',0,0),(90700000,17,'诺波雷希',1,23,1,0,'',0,0),(90800000,17,'贝亚曼费汉',1,22,1,0,'',0,0),(90900000,17,'费丁古乌',1,21,1,0,'',0,0),(91000000,17,'达贝布雷',1,20,1,0,'',0,0),(91100000,17,'雷欧亚布',1,19,1,0,'',0,0),(91200000,18,'霍泰琳洛',1,18,1,0,'',0,0),(91300000,18,'乔杰纳克',1,17,1,0,'',0,0),(91400000,19,'亚拉贝乔曼',1,16,1,0,'',0,0),(100100000,19,'克杰霍特',1,15,1,0,'',0,0),(100200000,19,'诺索莫凯',1,14,1,0,'',0,0),(100300000,19,'贝奥帝乔史',1,13,1,0,'',0,0),(100400000,19,'费利萨洛曼',1,12,1,0,'',0,0),(100500000,19,'霍玛斯泰',1,11,1,0,'',0,0),(100600000,19,'乔艾纳洛帕',1,10,1,0,'',0,0),(100700000,20,'亚雷沙汉',1,9,1,0,'',0,0),(100800000,20,'贝琳塔欧',1,8,1,0,'',0,0),(100900000,20,'迪史乔菲',1,7,1,0,'',0,0),(101000000,20,'雷卡古泰洛',1,6,1,0,'',0,0),(101100000,20,'诺琳特古巴',1,5,1,0,'',0,0),(101200000,20,'迪布艾修',1,4,1,0,'',0,0),(101300000,21,'费利雷希波',1,3,1,0,'',0,0),(101400000,21,'达诺泰萨曼',1,2,1,0,'',0,0),(101500000,22,'贝克索玛诺',1,1,1,0,'',0,0);

#
# Structure for table "disciple"
#

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
) ENGINE=InnoDB AUTO_INCREMENT=4 DEFAULT CHARSET=utf8 COLLATE=utf8_bin;

#
# Data for table "disciple"
#

INSERT INTO `disciple` VALUES (1,1,65,6,751,210,160,75,209.5,0,75,1,1,0),(2,1,84,2,183,188.5,150,61.5,116.2,0,14,2,2,0),(3,1,118,1,0,94,49,29,48,0,0,3,0,0);

#
# Structure for table "equip"
#

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
) ENGINE=InnoDB AUTO_INCREMENT=3 DEFAULT CHARSET=utf8 COLLATE=utf8_bin;

#
# Data for table "equip"
#

INSERT INTO `equip` VALUES (1,1,23,1,1,0,0,46,0,0,0,0,'{\"d\":[{\"c\":4,\"s\":1,\"i\":0},{\"c\":4,\"s\":1,\"i\":0}],\"a\":{\"b\":1,\"v\":\"26\"}}'),(2,1,32,1,1,0,0,21,0,0,2,0,'{\"d\":[{\"c\":4,\"s\":1,\"i\":0}],\"a\":{\"b\":1,\"v\":\"25\"}}');

#
# Structure for table "handbook"
#

DROP TABLE IF EXISTS `handbook`;
CREATE TABLE `handbook` (
  `playerid` int(10) NOT NULL,
  `discipleInfo` varchar(2047) COLLATE utf8_bin DEFAULT NULL,
  `equipInfo` varchar(2047) COLLATE utf8_bin DEFAULT NULL,
  `skillInfo` varchar(2047) COLLATE utf8_bin DEFAULT NULL,
  PRIMARY KEY (`playerid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;

#
# Data for table "handbook"
#

INSERT INTO `handbook` VALUES (1,'{\"160\":1,\"65\":0,\"84\":0,\"118\":0}','{\"23\":0,\"32\":0}','{\"25\":0,\"27\":0,\"69\":0}');

#
# Structure for table "invitestatus"
#

DROP TABLE IF EXISTS `invitestatus`;
CREATE TABLE `invitestatus` (
  `playerid` int(10) NOT NULL,
  `invitestatus` varchar(200) COLLATE utf8_bin NOT NULL DEFAULT '{}',
  `rewardstatus` varchar(200) COLLATE utf8_bin NOT NULL DEFAULT '{}',
  PRIMARY KEY (`playerid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;

#
# Data for table "invitestatus"
#


#
# Structure for table "leagua"
#

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

#
# Data for table "leagua"
#


#
# Structure for table "leaguaapply"
#

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

#
# Data for table "leaguaapply"
#


#
# Structure for table "leaguamedal"
#

DROP TABLE IF EXISTS `leaguamedal`;
CREATE TABLE `leaguamedal` (
  `id` int(10) NOT NULL AUTO_INCREMENT,
  `medalId` int(10) NOT NULL,
  `ownerLeaguaID` int(10) NOT NULL DEFAULT '0',
  `ownerLeaguaName` varchar(20) COLLATE utf8_bin NOT NULL DEFAULT '',
  `count` int(10) NOT NULL,
  PRIMARY KEY (`id`),
  KEY `index_leaguaId` (`ownerLeaguaID`)
) ENGINE=InnoDB AUTO_INCREMENT=10 DEFAULT CHARSET=utf8 COLLATE=utf8_bin;

#
# Data for table "leaguamedal"
#

INSERT INTO `leaguamedal` VALUES (1,3,0,'',1),(2,4,0,'',1),(3,5,0,'',1),(4,6,0,'',1),(5,7,0,'',1),(6,8,0,'',1),(7,9,0,'',1),(8,10,0,'',1),(9,11,0,'',1);

#
# Structure for table "leaguamedalchangeinfo"
#

DROP TABLE IF EXISTS `leaguamedalchangeinfo`;
CREATE TABLE `leaguamedalchangeinfo` (
  `id` int(10) NOT NULL AUTO_INCREMENT,
  `medalId` int(10) NOT NULL,
  `beforeOwnerLeaguaName` varchar(20) COLLATE utf8_bin NOT NULL DEFAULT '""',
  `afterOwnerLeaguaName` varchar(20) COLLATE utf8_bin NOT NULL,
  `changeTime` int(10) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;

#
# Data for table "leaguamedalchangeinfo"
#


#
# Structure for table "leaguamembers"
#

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

#
# Data for table "leaguamembers"
#


#
# Structure for table "mapping"
#

DROP TABLE IF EXISTS `mapping`;
CREATE TABLE `mapping` (
  `puid` varchar(50) COLLATE utf8_bin NOT NULL,
  `gameid` int(10) NOT NULL AUTO_INCREMENT,
  `name` varchar(50) COLLATE utf8_bin NOT NULL,
  `passwd` varchar(50) COLLATE utf8_bin DEFAULT NULL,
  UNIQUE KEY `gameid` (`gameid`),
  UNIQUE KEY `name` (`name`),
  KEY `index_puid` (`puid`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=utf8 COLLATE=utf8_bin;

#
# Data for table "mapping"
#

INSERT INTO `mapping` VALUES ('2111',1,'player2111','pswd');

#
# Structure for table "moment"
#

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

#
# Data for table "moment"
#


#
# Structure for table "pay"
#

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

#
# Data for table "pay"
#

INSERT INTO `pay` VALUES ('','','',0,0,0,0,0,0,0,'',0);

#
# Structure for table "player"
#

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

#
# Data for table "player"
#

INSERT INTO `player` VALUES (1,'2111','pswd','波沙尼史',2,183,26,0,1442926418,0,30,0,0,12,0,1442926427,12,0,0,3,0,3700,40,0,4,1442926613,1442926618,1442926210,1,1442926099,0,10,'1,2,3','{\"music\":1,\"sound\":1,\"chat\":1}','{}',1442926099,0,1442926210,0,0,0);

#
# Structure for table "playerfrom"
#

DROP TABLE IF EXISTS `playerfrom`;
CREATE TABLE `playerfrom` (
  `puid` varchar(50) COLLATE utf8_bin NOT NULL,
  `playerid` int(10) NOT NULL,
  `platform` varchar(30) COLLATE utf8_bin DEFAULT NULL,
  `deviceid` varchar(100) COLLATE utf8_bin DEFAULT NULL,
  `createtime` int(10) NOT NULL,
  PRIMARY KEY (`puid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;

#
# Data for table "playerfrom"
#

INSERT INTO `playerfrom` VALUES ('2111',1,'win32platforminfo_91','win32Device',1442926099);

#
# Structure for table "relationship"
#

DROP TABLE IF EXISTS `relationship`;
CREATE TABLE `relationship` (
  `playerid` int(10) NOT NULL,
  `friendids` text COLLATE utf8_bin,
  `enemyids` text COLLATE utf8_bin,
  `confirm` text COLLATE utf8_bin,
  PRIMARY KEY (`playerid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;

#
# Data for table "relationship"
#

INSERT INTO `relationship` VALUES (1,'{}','{}','{}');

#
# Structure for table "rewards"
#

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
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=utf8 COLLATE=utf8_bin;

#
# Data for table "rewards"
#

INSERT INTO `rewards` VALUES (1,1,7,'恭喜您在一个新游戏区创建了角色，如果您之前已经在其他区创建了角色，请在登录界面检查是否进入了正确的游戏区.','{}','[]',1,1442926099,1442926099);

#
# Structure for table "skill"
#

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
) ENGINE=InnoDB AUTO_INCREMENT=4 DEFAULT CHARSET=utf8 COLLATE=utf8_bin;

#
# Data for table "skill"
#

INSERT INTO `skill` VALUES (1,1,25,2,1,7.93,0,1,1,0),(2,1,27,1,1,28.06,0,2,1,0),(3,1,69,1,1,12.99,0,0,1,0);

#
# Structure for table "skillbook"
#

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

#
# Data for table "skillbook"
#


#
# Structure for table "soul"
#

DROP TABLE IF EXISTS `soul`;
CREATE TABLE `soul` (
  `id` int(10) NOT NULL AUTO_INCREMENT,
  `itemid` int(10) NOT NULL,
  `playerid` int(10) NOT NULL,
  `count` int(10) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=3 DEFAULT CHARSET=utf8 COLLATE=utf8_bin;

#
# Data for table "soul"
#

INSERT INTO `soul` VALUES (1,160,1,5),(2,84,1,9);

#
# Structure for table "stats"
#

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

#
# Data for table "stats"
#

INSERT INTO `stats` VALUES (1,0,0,'',100,3,1,0,'12001:2002:10','',0,0,1442926099,0,600,600,0,0,0,7,0,'{\"v\":1,\"c\":1,\"1\":1}',NULL,0,0,0,0,0,0,0,0,0,0,0,0,1442926101,0,0,0,0,0,0,0,'{}','{}',0,0);

#
# Structure for table "sysmsginfo"
#

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

#
# Data for table "sysmsginfo"
#


#
# Structure for table "teambuffer"
#

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

#
# Data for table "teambuffer"
#

INSERT INTO `teambuffer` VALUES (1,0,0,0,0,0,0,0,0);

#
# Structure for table "title"
#

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

#
# Data for table "title"
#

INSERT INTO `title` VALUES (1,1001,10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);

#
# Structure for table "worldboss"
#

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

#
# Data for table "worldboss"
#


#
# Structure for table "worldbossrank"
#

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

#
# Data for table "worldbossrank"
#


#
# Structure for table "wpayopr201304"
#

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

#
# Data for table "wpayopr201304"
#


#
# Structure for table "wpayopr201305"
#

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

#
# Data for table "wpayopr201305"
#


#
# Structure for table "wpayopr201306"
#

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

#
# Data for table "wpayopr201306"
#


#
# Structure for table "wpayopr201307"
#

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

#
# Data for table "wpayopr201307"
#


#
# Structure for table "wpayopr201308"
#

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

#
# Data for table "wpayopr201308"
#


#
# Structure for table "wpayopr201309"
#

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

#
# Data for table "wpayopr201309"
#


#
# Structure for table "wpayopr201310"
#

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

#
# Data for table "wpayopr201310"
#


#
# Structure for table "wpayopr201311"
#

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

#
# Data for table "wpayopr201311"
#


#
# Structure for table "wpayopr201312"
#

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

#
# Data for table "wpayopr201312"
#


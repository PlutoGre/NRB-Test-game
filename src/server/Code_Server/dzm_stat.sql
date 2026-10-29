/*
SQLyog 企业版 - MySQL GUI v8.14 
MySQL - 5.5.30 : Database - dzm_stat
*********************************************************************
*/

/*!40101 SET NAMES utf8 */;

/*!40101 SET SQL_MODE=''*/;

/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;
CREATE DATABASE /*!32312 IF NOT EXISTS*/`dzm_stat` /*!40100 DEFAULT CHARACTER SET utf8 */;

USE `dzm_stat`;

/*Table structure for table `stat_active_level` */

DROP TABLE IF EXISTS `stat_active_level`;

CREATE TABLE `stat_active_level` (
  `id` int(10) NOT NULL AUTO_INCREMENT,
  `serverid` int(10) DEFAULT '1',
  `date` varchar(20) COLLATE utf8_bin NOT NULL,
  `level` int(10) NOT NULL,
  `num` int(10) NOT NULL,
  `perCent` float NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=228 DEFAULT CHARSET=utf8 COLLATE=utf8_bin;

/*Table structure for table `stat_common_level` */

DROP TABLE IF EXISTS `stat_common_level`;

CREATE TABLE `stat_common_level` (
  `id` int(10) NOT NULL AUTO_INCREMENT,
  `serverid` int(10) DEFAULT '1',
  `date` varchar(20) COLLATE utf8_bin NOT NULL,
  `level` int(10) NOT NULL,
  `num` int(10) NOT NULL,
  `perCent` float NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=228 DEFAULT CHARSET=utf8 COLLATE=utf8_bin;

/*Table structure for table `stat_firstday_level` */

DROP TABLE IF EXISTS `stat_firstday_level`;

CREATE TABLE `stat_firstday_level` (
  `id` int(10) NOT NULL AUTO_INCREMENT,
  `serverid` int(10) DEFAULT '1',
  `date` varchar(20) COLLATE utf8_bin NOT NULL,
  `level` int(10) NOT NULL,
  `num` int(10) NOT NULL,
  `perCent` float NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=111 DEFAULT CHARSET=utf8 COLLATE=utf8_bin;

/*Table structure for table `stat_monthly_pay` */

DROP TABLE IF EXISTS `stat_monthly_pay`;

CREATE TABLE `stat_monthly_pay` (
  `month` varchar(10) NOT NULL,
  `serverid` int(10) NOT NULL,
  `monthActive` int(10) NOT NULL DEFAULT '0',
  `monthPay` int(10) NOT NULL DEFAULT '0',
  PRIMARY KEY (`month`,`serverid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

/*Table structure for table `stat_player_info` */

DROP TABLE IF EXISTS `stat_player_info`;

CREATE TABLE `stat_player_info` (
  `id` int(10) NOT NULL AUTO_INCREMENT,
  `serverid` int(10) NOT NULL,
  `userid` int(10) NOT NULL,
  `username` varchar(20) COLLATE utf8_bin NOT NULL,
  `lastlogin` int(10) NOT NULL,
  `registertime` int(10) NOT NULL,
  `userlevel` int(10) NOT NULL,
  `userviplevel` int(10) NOT NULL,
  `userxp` int(10) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=1839 DEFAULT CHARSET=utf8 COLLATE=utf8_bin;

/*Table structure for table `stat_player_left` */

DROP TABLE IF EXISTS `stat_player_left`;

CREATE TABLE `stat_player_left` (
  `id` int(10) NOT NULL AUTO_INCREMENT,
  `date` varchar(20) COLLATE utf8_bin NOT NULL,
  `serverid` int(10) DEFAULT '1',
  `registnum` int(10) NOT NULL,
  `activenum` int(10) NOT NULL,
  `leftnum1` float DEFAULT NULL,
  `leftnum2` float DEFAULT NULL,
  `leftnum3` float DEFAULT NULL,
  `leftnum4` float DEFAULT NULL,
  `leftnum5` float DEFAULT NULL,
  `leftnum6` float DEFAULT NULL,
  `leftnum7` float DEFAULT NULL,
  `leftnum8` float DEFAULT NULL,
  `leftnum9` float DEFAULT NULL,
  `leftnum10` float DEFAULT NULL,
  `leftnum11` float DEFAULT NULL,
  `leftnum12` float DEFAULT NULL,
  `leftnum13` float DEFAULT NULL,
  `leftnum14` float DEFAULT NULL,
  `leftnum30` float DEFAULT NULL,
  `activeRecharge` int(10) DEFAULT '0',
  `activeWeek` int(10) DEFAULT '0',
  `activeMonth` int(10) DEFAULT '0',
  `newPayer` int(10) DEFAULT '0',
  `logoutNum` int(10) DEFAULT '0',
  `stayin7days` float DEFAULT NULL,
  `stayin15days` float DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=9 DEFAULT CHARSET=utf8 COLLATE=utf8_bin;

/*Table structure for table `stat_player_recharge` */

DROP TABLE IF EXISTS `stat_player_recharge`;

CREATE TABLE `stat_player_recharge` (
  `id` int(10) NOT NULL AUTO_INCREMENT,
  `serverid` int(10) NOT NULL,
  `userid` int(10) NOT NULL,
  `isFirstPay` int(10) DEFAULT NULL,
  `CreateTime` varchar(20) COLLATE utf8_bin DEFAULT NULL,
  `GoodsId` int(10) DEFAULT NULL,
  `GoodsCount` int(10) DEFAULT NULL,
  `registertime` int(10) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;

/*Table structure for table `stat_recharge_param` */

DROP TABLE IF EXISTS `stat_recharge_param`;

CREATE TABLE `stat_recharge_param` (
  `id` int(10) NOT NULL,
  `amount` int(10) NOT NULL,
  `addNum` int(10) NOT NULL,
  `costMoney` int(10) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;

/*Table structure for table `stat_server_info` */

DROP TABLE IF EXISTS `stat_server_info`;

CREATE TABLE `stat_server_info` (
  `serverid` int(10) NOT NULL,
  `servername` varchar(20) COLLATE utf8_bin NOT NULL,
  `serverip` varchar(20) COLLATE utf8_bin NOT NULL,
  `serverport` int(10) DEFAULT NULL,
  `serverusername` varchar(20) COLLATE utf8_bin DEFAULT NULL,
  `serverpassword` varchar(20) COLLATE utf8_bin DEFAULT NULL,
  PRIMARY KEY (`serverid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;

/*Table structure for table `stat_use_gold` */

DROP TABLE IF EXISTS `stat_use_gold`;

CREATE TABLE `stat_use_gold` (
  `id` int(10) NOT NULL AUTO_INCREMENT,
  `serverid` int(10) NOT NULL,
  `time` int(10) NOT NULL,
  `userid` int(10) NOT NULL,
  `typeid` int(10) NOT NULL,
  `num` int(10) NOT NULL,
  `beforewaste` int(10) NOT NULL,
  `afterwaste` int(10) NOT NULL,
  `price` int(10) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=99071 DEFAULT CHARSET=utf8 COLLATE=utf8_bin;

/*Table structure for table `stat_use_gold_param` */

DROP TABLE IF EXISTS `stat_use_gold_param`;

CREATE TABLE `stat_use_gold_param` (
  `id` int(10) NOT NULL,
  `name` varchar(20) COLLATE utf8_bin NOT NULL,
  `price` int(10) NOT NULL,
  `describe` varchar(20) COLLATE utf8_bin NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;

/*Table structure for table `stat_user_info` */

DROP TABLE IF EXISTS `stat_user_info`;

CREATE TABLE `stat_user_info` (
  `name` varchar(20) COLLATE utf8_bin NOT NULL,
  `password` varchar(32) COLLATE utf8_bin NOT NULL,
  `stat` int(10) NOT NULL,
  `describe` varchar(20) COLLATE utf8_bin DEFAULT NULL,
  PRIMARY KEY (`name`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

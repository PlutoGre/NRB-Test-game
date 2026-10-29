use dzm;
set names utf8;
/*Table structure for table `activityinfo` */

DROP TABLE IF EXISTS `activityinfo`;

CREATE TABLE `activityinfo` (
  `playerid` int(10) NOT NULL,
  `activityinfo` text COLLATE utf8_bin,
  `laseRefreshTime` int(10) NOT NULL,
  PRIMARY KEY (`playerid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;
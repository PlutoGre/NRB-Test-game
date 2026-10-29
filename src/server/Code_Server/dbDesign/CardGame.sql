USE dzm
;

SET FOREIGN_KEY_CHECKS=0;



DROP TABLE IF EXISTS `AdventureFightRank` CASCADE
;
DROP TABLE IF EXISTS `AdventureFightStats` CASCADE
;
DROP TABLE IF EXISTS `rewards` CASCADE
;
DROP TABLE IF EXISTS `skillbook` CASCADE
;
DROP TABLE IF EXISTS `sysmsginfo` CASCADE
;
DROP TABLE IF EXISTS `wPayOprYYYYMM` CASCADE
;

CREATE TABLE `AdventureFightRank`
(
	`id` INTEGER NOT NULL AUTO_INCREMENT,
	`playerId` INTEGER NOT NULL,
	`playerName` VARCHAR(50) NOT NULL,
	`playerLevel` SMALLINT,
	`barrierCount` TINYINT NOT NULL,
	`passBarrier` SMALLINT,
	`obtainStar` INTEGER DEFAULT 0,
	`continueInRankAmount` SMALLINT,
	`rankTime` INTEGER NOT NULL DEFAULT 0,
	PRIMARY KEY (`id`),
	UNIQUE `UQ_AdventureFightRank_id`(`id`)

) 
;


CREATE TABLE `AdventureFightStats`
(
	`id` INTEGER NOT NULL AUTO_INCREMENT,
	`playerId` INTEGER NOT NULL,
	`barrierCount` TINYINT NOT NULL DEFAULT 5,
	`battlePanelType` TINYINT NOT NULL,
	`battleAmount` TINYINT NOT NULL,
	`yestBarrierLV` SMALLINT NOT NULL,
	`yestObtainStar` INTEGER NOT NULL,
	`maxBarrierLV` SMALLINT NOT NULL,
	`maxObtainStar` SMALLINT NOT NULL,
	`currBarrierLV` INTEGER NOT NULL DEFAULT 0,
	`currObtainStar` INTEGER NOT NULL DEFAULT 0,
	`currSurplusStar` INTEGER,
	`extraRewardDetailInfo` NVARCHAR(255),
	`extraRewardConf` NVARCHAR(255),
	`obtainStarDetail` NVARCHAR(255),
	`proAdditionInfo` NVARCHAR(255),
	`additionSelDetail` NVARCHAR(255),
	`teamConfInfo` TEXT,
	`lastUpdateTime` INTEGER NOT NULL,
	`lastOprTime` INTEGER NOT NULL,
	`nextResetTime` INTEGER NOT NULL,
	PRIMARY KEY (`id`),
	UNIQUE `UQ_AdventureFightStats_id`(`id`),
	INDEX `IDX_playerId` ( ASC)

) 
;


CREATE TABLE `rewards`
(
	`id` INTEGER NOT NULL AUTO_INCREMENT,
	`playerId` INTEGER NOT NULL,
	`rewardType` TINYINT NOT NULL,
	`rewardMsg` NVARCHAR(255),
	`rewardInfo` VARCHAR(50),
	`rewardContent` NVARCHAR(255),
	`status` TINYINT NOT NULL,
	`addTime` INTEGER NOT NULL,
	`rewardTime` INTEGER,
	PRIMARY KEY (`id`),
	UNIQUE `UQ_rewards_id`(`id`),
	INDEX `IDX_playerId` (`playerId` ASC)

) 
;


CREATE TABLE `skillbook`
(
	`id` INTEGER NOT NULL AUTO_INCREMENT,
	`playerId` INTEGER NOT NULL,
	`skillId` INTEGER NOT NULL,
	`pieceChance` INTEGER NOT NULL DEFAULT 0,
	`skillBookDetail` VARCHAR(255) NOT NULL DEFAULT '{}',
	PRIMARY KEY (`id`),
	UNIQUE `UQ_SkillBook_id`(`id`),
	INDEX `IDX_playerId` (`playerId` ASC)

) 
;


CREATE TABLE `sysmsginfo`
(
	`id` INTEGER NOT NULL,
	`playerId` INTEGER NOT NULL,
	`msgType` TINYINT NOT NULL,
	`msgInfo` VARCHAR(255) NOT NULL DEFAULT '{}',
	`msgContent` TEXT,
	`status` TINYINT NOT NULL DEFAULT 0,
	`addTime` INTEGER NOT NULL,
	PRIMARY KEY (`id`),
	UNIQUE `UQ_sysmsginfo_id`(`id`),
	INDEX `IDX_playerId` (`playerId` ASC)

) 
;


CREATE TABLE `wPayOprYYYYMM`
(
	`id` INTEGER NOT NULL AUTO_INCREMENT,
	`playerId` INTEGER NOT NULL,
	`propId` INTEGER NOT NULL,
	`payType` SMALLINT NOT NULL,
	`buyNum` INTEGER NOT NULL,
	`originalCash` INTEGER NOT NULL,
	`consumeCash` INTEGER NOT NULL,
	`surplusCash` INTEGER NOT NULL,
	`oprTime` INTEGER NOT NULL,
	PRIMARY KEY (`id`),
	UNIQUE `UQ_wBuyPropPay_id`(`id`),
	INDEX `IDX_playerId` (`playerId` ASC)

) 
;



SET FOREIGN_KEY_CHECKS=1;

<?php
class Control{
		
	public static function commPlayer(){
		include_once('../dataBase/Connection.php');
		$content = array();
		$content = Connection::getCommPlayer();
		include_once('commonPlayer.php');
	}

	public static function playerLeft(){
		include_once('../dataBase/Connection.php');
		$content = array();
		$content = Connection::getPlayerLeft();
		include_once('playerLeft.php');
	}

	public static function commonPay(){
		include_once('../dataBase/Connection.php');
		$content = array();
		$content = Connection::getCommonPay();
		include_once('commonPay.php');
	}

	public static function everydayPay(){
		include_once('../dataBase/Connection.php');
		$content = array();
		$content = Connection::getEverydayPay();
		include_once('everydayPay.php');
	}

	public static function commLevel(){
		include_once('../dataBase/Connection.php');
		$content = array();
		$content = Connection::getCommLevel();
		include_once('commonLevel.php');
	}

	public static function firstdayLevel(){
		include_once('../dataBase/Connection.php');
		$content = array();
		$content = Connection::getFirstdayLevel();
		include_once('firstdayLevel.php');
	}

	public static function topHundred(){
		include_once('../dataBase/Connection.php');
		$content = array();
		$content = Connection::getTopHundred();
		include_once('topHundred.php');
	}

	public static function searchUserInfo(){
		include_once('../dataBase/Connection.php');
		$content = array();
		$content = Connection::searchUserInfo();
		include_once('searchUserInfo.php');
	}

	public static function updateUserInfo(){
		include_once('../dataBase/Connection.php');
		$content = array();
		$content = Connection::updateUserInfo();
		include_once('updateUserInfo.php');
	}

	public static function searchUseGold($date){
		include_once('../dataBase/Connection.php');
		$content = array();
		$content = Connection::searchUseGold($date);
		include_once('searchUseGold.php');
	}

	public static function summaryUseGold($date){
		include_once('../dataBase/Connection.php');
		$content = array();
		$content = Connection::summaryUseGold($date);
		include_once('summaryUseGold.php');
	}

	public static function graphsEverydayRegist($date){
		include_once('../dataBase/Connection.php');
		$content = array();
		$content = Connection::graphsEverydayRegist($date);
		include_once('graphsEverydayRegist.php');
	}

	public static function graphsEverydayActive($length){
		include_once('../dataBase/Connection.php');
		$content = array();
		$content = Connection::graphsEverydayActive($length);
		include_once('graphsEverydayActive.php');
	}


}


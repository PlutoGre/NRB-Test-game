<?php 
class Connection{

	public static function commPlayer(){
		session_start();
		$serverid = $_SESSION["serverid"];
		$link=mysql_connect(DBConfig::$host,DBConfig::$user,DBConfig::$passwd);
		mysql_set_charset('utf8',$link);
		$db_selected=mysql_select_db(DBConfig::$db_name,$link);
		$result=mysql_query('select * from stat_player_left where serverid = $serverid');
		if(!$result){
			echo '错误'.mysql_error();
			exit;
		}
		$num_rows=mysql_num_rows($result);
		$totalResult = array();
		$i = 0;
		while($total = mysql_fetch_array($result)){
			$totalResult[$i] = $total[0] . " " . $total[1] . " " . $total[2] . " " . $total[3] . " " . "\n";
			$i = $i + 1;
		}
		return $totalResult;
	}
	
	public static function registAndActive($date){
		session_start();
		$serverid = $_SESSION["serverid"];
		$link=mysql_connect(DBConfig::$host,DBConfig::$user,DBConfig::$passwd);
		mysql_set_charset('utf8',$link);
		$db_selected=mysql_select_db(DBConfig::$db_name,$link);

		$sqlStr = "select stat_player_left.date ,registnum ,activenum"
			.",activeRecharge ,activenum - registnum ,activeWeek,activeMonth"
			.",COALESCE((select sum(stat_recharge_param.costMoney*stat_player_recharge.GoodsCount) from stat_player_recharge,stat_recharge_param where serverid = $serverid and stat_player_recharge.GoodsId = stat_recharge_param.id and FROM_UNIXTIME( CreateTime, '%Y%m%d' )  = stat_player_left.date ),0)"
			.",(select count(distinct userid) from stat_player_recharge where serverid = $serverid and FROM_UNIXTIME( CreateTime, '%Y%m%d' )  = stat_player_left.date )"
			.",newPayer"
			.",COALESCE((select count(distinct userid) from stat_player_recharge where serverid = $serverid and FROM_UNIXTIME( CreateTime, '%Y%m%d' )  = stat_player_left.date )/activenum,0)"
			.",(select count(1) from stat_player_recharge where serverid = $serverid and FROM_UNIXTIME( CreateTime, '%Y%m%d' )  = stat_player_left.date)"
			.",COALESCE((select sum(stat_recharge_param.costMoney*stat_player_recharge.GoodsCount)/count(distinct userid) from stat_player_recharge,stat_recharge_param where serverid = $serverid and stat_player_recharge.GoodsId = stat_recharge_param.id and FROM_UNIXTIME( CreateTime, '%Y%m%d' )  = stat_player_left.date )/activenum,'0')"
			.",COALESCE((select sum(stat_recharge_param.costMoney*stat_player_recharge.GoodsCount)/1 from stat_player_recharge,stat_recharge_param where serverid = $serverid and stat_player_recharge.GoodsId = stat_recharge_param.id and FROM_UNIXTIME( CreateTime, '%Y%m%d' ) = stat_player_left.date )/activenum,'0')"
			.",stayin7days,stayin15days"
			." from stat_player_left "
			." where serverid = $serverid ";
		if(strlen($date) == 8){
			$sqlStr .= " and stat_player_left.date >= $date";
		}else{
			$sqlStr .= " and 1=1";
		}
		$sqlStr .=" group by stat_player_left.date order by stat_player_left.date ";
		
		$result=mysql_query($sqlStr);

		if(!$result){
			echo '错误'.mysql_error();
			exit;
		}
		$num_rows=mysql_num_rows($result);
		$totalResult = array();
		$i = 0;
		while($total = mysql_fetch_array($result)){
			$totalResult[$i] = $total[0] . " " . $total[1] . " " . $total[2] . " " . $total[3] . " " . $total[4] . " " . $total[5] . " " . $total[6] . " " . $total[7] . " " . $total[8] . " " . $total[9] . " " . $total[10] . " " . $total[11] . " " . $total[12] . " " . $total[13] . " " . $total[14] . " " . $total[15] . " " . "\n";
			$i = $i + 1;
		}
		return $totalResult;
	}

	public static function playerLeft($date){
		session_start();
		$serverid = $_SESSION["serverid"];
		$link=mysql_connect(DBConfig::$host,DBConfig::$user,DBConfig::$passwd);
		mysql_set_charset('utf8',$link);
		$db_selected=mysql_select_db(DBConfig::$db_name,$link);
		$sqlStr = "select date,registnum,activenum,leftnum1,leftnum2,leftnum3,leftnum4,leftnum5,leftnum6,leftnum7,leftnum14,leftnum30,logoutNum from stat_player_left where serverid = $serverid ";
		if(strlen($date) == 8){
			$sqlStr .= " and date >= $date";
		}else{
		
		}
		$result=mysql_query($sqlStr);
		if(!$result){
			echo '错误'.mysql_error();
			exit;
		}
		$num_rows=mysql_num_rows($result);
		$sqlPlayerNumStr = "select distinct (userid) from stat_player_info";
		$playerNumResult=mysql_query($sqlPlayerNumStr);
		$playerNum=mysql_num_rows($playerNumResult);

		
		$totalResult = array();
		$i = 0;
		while($total = mysql_fetch_array($result)){

			$totalResult[$i] = $total[0] . " " . $total[1] . " " . $total[2] . " " 
			. number_format(trim($total[3]*100),2) . "% " 
			. number_format(trim($total[4]*100),2) . "% " 
			. number_format(trim($total[5]*100),2) . "% " 
			. number_format(trim($total[6]*100),2) . "% " 
			. number_format(trim($total[7]*100),2) . "% " 
			. number_format(trim($total[8]*100),2) . "% " 
			. number_format(trim($total[9]*100),2) . "% " 
			. number_format(trim($total[10]*100),2) . "% " 
			. number_format(trim($total[11]*100),2) . "% " 
			. $playerNum . " " 
			. $total[12] . "\n";
			$i = $i + 1;
		}
		return $totalResult;
	}

	public static function everydayPay(){
		session_start();
		$serverid = $_SESSION["serverid"];

		$link=mysql_connect(DBConfig::$host,DBConfig::$user,DBConfig::$passwd);
		mysql_set_charset('utf8',$link);
		$db_selected=mysql_select_db(DBConfig::$db_name,$link);
		$queryStr = " select userid,(select username from stat_player_info where stat_player_info.id = userid and stat_player_info.serverid = $serverid) "
				." , sum(stat_recharge_param.costMoney*stat_player_recharge.GoodsCount),count(1), FROM_UNIXTIME( CreateTime, '%Y%m%d' ) "
				." from stat_player_recharge,stat_recharge_param "
				." where stat_player_recharge.serverid = $serverid and stat_player_recharge.GoodsId = stat_recharge_param.id "
				." group by userid "
				." order by FROM_UNIXTIME( CreateTime, '%Y%m%d' ) ";
		$result=mysql_query($queryStr);
		if(!$result){
			echo '错误'.mysql_error();
			exit;
		}
		$num_rows=mysql_num_rows($result);
		$totalResult = array();
		$i = 0;
		while($total = mysql_fetch_array($result)){
			$totalResult[$i] = $total[0] . " " . $total[1] . " " . $total[2] . " " . $total[3] . " "  . $total[4] . " " . "\n";
			$i = $i + 1;
		}
		return $totalResult;
	}

	public static function commonPay(){
		session_start();
		$serverid = $_SESSION["serverid"];

		$link=mysql_connect(DBConfig::$host,DBConfig::$user,DBConfig::$passwd);
		mysql_set_charset('utf8',$link);
		$db_selected=mysql_select_db(DBConfig::$db_name,$link);
		$queryStr = "select FROM_UNIXTIME(CreateTime,'%Y%m%d'),sum(stat_recharge_param.costMoney*stat_player_recharge.GoodsCount),count(distinct userid),count(*),activenum,count(distinct userid)/activenum,sum(amount)/count(distinct userid),sum(amount)/activenum "
			." from stat_player_recharge,stat_player_left ,stat_recharge_param "
			." where FROM_UNIXTIME( CreateTime, '%Y%m%d' )  = stat_player_left.date "
			." and stat_player_recharge.serverid = $serverid and stat_player_left.serverid = $serverid and stat_player_recharge.GoodsId = stat_recharge_param.id "
			." group by FROM_UNIXTIME( CreateTime, '%Y%m%d' ) "
			." order by FROM_UNIXTIME( CreateTime, '%Y%m%d' ) ";
		$result=mysql_query($queryStr);
		if(!$result){
			echo '错误'.mysql_error();
			exit;
		}
		$num_rows=mysql_num_rows($result);
		$totalResult = array();
		$i = 0;
		while($total = mysql_fetch_array($result)){
			$totalResult[$i] = $total[0] . " " . $total[1] . " " . $total[2] . " " . $total[3] . " " . $total[4] . " " . $total[5] . " " . $total[6] . " " . $total[7] . " " . "\n";
			$i = $i + 1;
		}
		return $totalResult;
	}

	public static function commonLevel($date){
		session_start();
		$serverid = $_SESSION["serverid"];

		
		$link=mysql_connect(DBConfig::$host,DBConfig::$user,DBConfig::$passwd);
		mysql_set_charset('utf8',$link);
		$db_selected=mysql_select_db(DBConfig::$db_name,$link);
		$sqlStr = "select date,level,num,perCent from stat_common_level where serverid = $serverid ";
		if(strlen($date) == 8){
			$sqlStr .= " and date = $date";
		}else{
			$sqlStr .= " and date = " . date('Ymd',strtotime("-1 day"));
		}
		$sqlStr .= " order by level ";
//echo "commonLevelSql:" . $sqlStr;
		$result=mysql_query($sqlStr);
		if(!$result){
			echo '错误'.mysql_error();
			exit;
		}
		$num_rows=mysql_num_rows($result);
		$totalResult = array();
		$i = 0;
		while($total = mysql_fetch_array($result)){
			$totalResult[$i] = $total[0] . " " . $total[1] . " " . $total[2] . " " . number_format(trim($total[3]*100),2) . "% " . "\n";
			$i = $i + 1;
		}
		return $totalResult;
	}

	public static function activeLevel($date){
		session_start();
		$serverid = $_SESSION["serverid"];

		$link=mysql_connect(DBConfig::$host,DBConfig::$user,DBConfig::$passwd);
		mysql_set_charset('utf8',$link);
		$db_selected=mysql_select_db(DBConfig::$db_name,$link);
		$sqlStr = "select date,level,num,perCent from stat_active_level where serverid = $serverid ";
		if(strlen($date) == 8){
			$sqlStr .= " and date = $date";
		}else{
			$sqlStr .= " and date = " . date('Ymd',strtotime("-1 day"));
		}
		$sqlStr .= " order by level ";
		
		$result=mysql_query($sqlStr);
		if(!$result){
			echo '错误'.mysql_error();
			exit;
		}
		$num_rows=mysql_num_rows($result);
		$totalResult = array();
		$i = 0;
		while($total = mysql_fetch_array($result)){
			$totalResult[$i] = $total[0] . " " . $total[1] . " " . $total[2] . " " . number_format(trim($total[3]*100),2) . "% " . "\n";
			$i = $i + 1;
		}
		return $totalResult;
	}

	public static function firstdayLevel($date){
		session_start();
		$serverid = $_SESSION["serverid"];

		$link=mysql_connect(DBConfig::$host,DBConfig::$user,DBConfig::$passwd);
		mysql_set_charset('utf8',$link);
		$db_selected=mysql_select_db(DBConfig::$db_name,$link);
		$sqlStr = "select date,level,num,perCent from stat_firstday_level where serverid = $serverid ";
		if(strlen($date) == 8){
			$sqlStr .= " and date = $date";
		}else{
			$sqlStr .= " and date = " . date('Ymd',strtotime("-1 day"));
		}
		$sqlStr .= " order by level ";
		
		$result=mysql_query($sqlStr);
		if(!$result){
			echo '错误'.mysql_error();
			exit;
		}
		$num_rows=mysql_num_rows($result);
		$totalResult = array();
		$i = 0;
		while($total = mysql_fetch_array($result)){
			$totalResult[$i] = $total[0] . " " . $total[1] . " " . $total[2] . " " . $total[3]  . "\n";
			$i = $i + 1;
		}
		return $totalResult;
	}

	public static function registerPayer($date){
		session_start();
		$serverid = $_SESSION["serverid"];

		$link=mysql_connect(DBConfig::$host,DBConfig::$user,DBConfig::$passwd);
		mysql_set_charset('utf8',$link);
		$db_selected=mysql_select_db(DBConfig::$db_name,$link);

		$sqlStr = " select FROM_UNIXTIME(registertime,'%Y%m%d'),count(1) ,"
			." ifnull((select count(distinct userid) from stat_player_recharge where FROM_UNIXTIME(CreateTime,'%Y%m%d') = serverid = $serverid and FROM_UNIXTIME(registertime,'%Y%m%d')),0) as registerPayer, "
			." ifnull((select sum(stat_recharge_param.costMoney*stat_player_recharge.GoodsCount) from stat_player_recharge,stat_recharge_param where stat_player_recharge.serverid = $serverid and stat_player_recharge.GoodsId = stat_recharge_param.id and FROM_UNIXTIME(CreateTime,'%Y%m%d') = FROM_UNIXTIME(registertime,'%Y%m%d')),0) as amount "
			." from stat_player_info "
			." where serverid = $serverid "; 


		if(strlen($date) == 8){
			$sqlStr .= " and FROM_UNIXTIME(registertime,'%Y%m%d')  >= $date";
		}else{
			$sqlStr .= " ";
		}

		$sqlStr .= " group by FROM_UNIXTIME(registertime,'%Y%m%d') "
			. " order by FROM_UNIXTIME(registertime,'%Y%m%d') ";
		
		$result=mysql_query($sqlStr);
		if(!$result){
			echo '错误'.mysql_error();
			exit;
		}
		$num_rows=mysql_num_rows($result);
		$totalResult = array();
		$i = 0;
		while($total = mysql_fetch_array($result)){
			$totalResult[$i] = $total[0] . " " . $total[1] . " " . $total[2] . " " . $total[3] . " " . "\n";
			$i = $i + 1;
		}
		return $totalResult;
	}


	public static function vipDistribute(){
		session_start();
		$serverid = $_SESSION["serverid"];
		
		$link=mysql_connect(DBConfig::$host,DBConfig::$user,DBConfig::$passwd);
		mysql_set_charset('utf8',$link);
		$db_selected=mysql_select_db(DBConfig::$db_name,$link);
		
		$result=mysql_query("select userviplevel,count(1),count(1)/(select count(1) from stat_player_info where serverid = $serverid ) from stat_player_info where serverid = $serverid group by userviplevel");
		if(!$result){
			echo '错误'.mysql_error();
			exit;
		}
		$num_rows=mysql_num_rows($result);
		$totalResult = array();
		$i = 0;
		while($total = mysql_fetch_array($result)){
			$totalResult[$i] = $total[0] . " " . $total[1] . " " . number_format(trim($total[2]*100),2) . "% " . "\n";

			$i = $i + 1;
		}
		return $totalResult;
	}
	
	public static function monthlyPayInfo($date){
		session_start();
		$serverid = $_SESSION["serverid"];
	
		$link=mysql_connect(DBConfig::$host,DBConfig::$user,DBConfig::$passwd);
		mysql_set_charset('utf8',$link);
		$db_selected=mysql_select_db(DBConfig::$db_name,$link);
		
		$sqlStr = "select month,monthActive,monthPay from stat_monthly_pay where serverid = $serverid ";
		if(strlen($date) == 6){
			$sqlStr .= " and month = $date";
		}else{
			$sqlStr .= " and month = " . date('Ym');
		}
		
		$result=mysql_query($sqlStr);
		if(!$result){
			echo '错误'.mysql_error();
			exit;
		}
		$num_rows=mysql_num_rows($result);
		$totalResult = array();
		$i = 0;
		while($total = mysql_fetch_array($result)){
			$totalResult[$i] = $total[0] . " " . $total[1] . " " . $total[2] . " "  . " " . "\n";
			$i = $i + 1;
		}
		return $totalResult;
	}

	public static function vipDistributeGraphs(){
	/*
		session_start();
		$serverid = $_SESSION["serverid"];

		$link=mysql_connect(DBConfig::$host,DBConfig::$user,DBConfig::$passwd);
		mysql_set_charset('utf8',$link);
		$db_selected=mysql_select_db(DBConfig::$db_name,$link);
		$result=mysql_query('select viplevel,count(*),count(*)/(select count(*) from player) from player group by viplevel');
		if(!$result){
			echo '错误'.mysql_error();
			exit;
		}
		$num_rows=mysql_num_rows($result);
		$totalResult = array();
		$i = 0;
		while($total = mysql_fetch_array($result)){
			$totalResult[$i] = $total[0] . " " . $total[1] . " " . $total[2] . " " . "\n";
			$i = $i + 1;
		}
		return $totalResult;
		*/
		return null;
	}

	public static function topHundred(){
	/*
		session_start();
		$serverid = $_SESSION["serverid"];

		$link=mysql_connect(DBConfig::$host,DBConfig::$user,DBConfig::$passwd);
		mysql_set_charset('utf8',$link);
		$db_selected=mysql_select_db(DBConfig::$db_name,$link);
		$result=mysql_query('select name,puid,sysgoldcoins+rechargegoldcoins,level,exp from player order by exp DESC limit 100');
		if(!$result){
			echo '错误'.mysql_error();
			exit;
		}
		$num_rows=mysql_num_rows($result);
		$totalResult = array();
		$i = 0;
		while($total = mysql_fetch_array($result)){
echo $i . "\n";
echo $total[4] . "\n";
			$totalResult[$i] = $total[0] . " " . $total[1] . " " . $total[2] . " " . $total[3] . " " . $total[4] . " " . "\n";
			$i = $i + 1;
		}
		return $totalResult;
		*/
		return null;
	}

	public static function searchUserInfo(){
	/*
		session_start();
		$serverid = $_SESSION["serverid"];

		$link=mysql_connect(DBConfig::$host,DBConfig::$user,DBConfig::$passwd);
		mysql_set_charset('utf8',$link);
		$db_selected=mysql_select_db(DBConfig::$db_name,$link);
		$result=mysql_query('select name,puid,sysgoldcoins+rechargegoldcoins,level from player order by exp DESC limit 100');
		if(!$result){
			echo '错误'.mysql_error();
			exit;
		}
		$num_rows=mysql_num_rows($result);
		$totalResult = array();
		$i = 0;
		while($total = mysql_fetch_array($result)){
			$totalResult[$i] = $total[0] . " " . $total[1] . " " . $total[2] . " " . $total[3] . " " . "\n";
			$i = $i + 1;
		}
		return $totalResult;
		*/
		return null;
	}

	public static function updateUserInfo(){
	/*
		session_start();
		$serverid = $_SESSION["serverid"];

		$link=mysql_connect(DBConfig::$host,DBConfig::$user,DBConfig::$passwd);
		mysql_set_charset('utf8',$link);
		$db_selected=mysql_select_db(DBConfig::$db_name,$link);
		$result=mysql_query('select name,puid,sysgoldcoins+rechargegoldcoins,level from player order by exp DESC limit 100');
		if(!$result){
			echo '错误'.mysql_error();
			exit;
		}
		$num_rows=mysql_num_rows($result);
		$totalResult = array();
		$i = 0;
		while($total = mysql_fetch_array($result)){
			$totalResult[$i] = $total[0] . " " . $total[1] . " " . $total[2] . " " . $total[3] . " " . "\n";
			$i = $i + 1;
		}
		return $totalResult;
	*/
		return null;
	}

	public static function searchUseGold($date){
		session_start();
		$serverid = $_SESSION["serverid"];
	
		$link=mysql_connect(DBConfig::$host,DBConfig::$user,DBConfig::$passwd);
		
		mysql_set_charset('utf8',$link);
		$db_selected=mysql_select_db(DBConfig::$db_name,$link);
		$resultStr = "select stat_use_gold_param.id,FROM_UNIXTIME(time,'%Y%m%d-%H:%i:%s'),userid,(select username from stat_player_info where stat_player_info.userid = stat_use_gold.userid),stat_use_gold_param.name,num,stat_use_gold.price,beforewaste,afterwaste from stat_use_gold , stat_use_gold_param where stat_use_gold.serverid = $serverid and stat_use_gold.typeid = stat_use_gold_param.id ";
		
		if(strlen($date) == 8){
			$resultStr .= " and FROM_UNIXTIME(time,'%Y%m%d') = $date";
		}else{
			$resultStr .= " and FROM_UNIXTIME(time,'%Y%m%d') = " . date('Ymd',strtotime("-1 day"));
		}
		
		$result=mysql_query($resultStr); 

		if(!$result){
			echo '错误'.mysql_error();
			exit;
		}
		$num_rows=mysql_num_rows($result);
		$totalResult = array();
		$i = 0;
		while($total = mysql_fetch_array($result)){
			$totalResult[$i] = $total[1] . " " . $total[2] . " " . $total[3] . " " . $total[4] . " " . $total[5] . " " . $total[6] . " " . $total[7] . " " . $total[8] . " " . $total[9] . "\n";
			$i = $i + 1;
		}
		return $totalResult;
	}


	public static function summaryUseGold($date){
		session_start();
		$serverid = $_SESSION["serverid"];

		$link=mysql_connect(DBConfig::$host,DBConfig::$user,DBConfig::$passwd);
		mysql_set_charset('utf8',$link);
		$db_selected=mysql_select_db(DBConfig::$db_name,$link);
		
		$queryStr = "select FROM_UNIXTIME(time,'%Y%m%d'),stat_use_gold_param.id,stat_use_gold_param.name,stat_use_gold_param.price,sum(num),sum(stat_use_gold.price),sum(stat_use_gold.price)/(select sum(aaa.price) from stat_use_gold aaa where stat_use_gold.serverid = $serverid ";
		if(strlen($date) == 8){
			$queryStr .= " and FROM_UNIXTIME(aaa.time,'%Y%m%d') = $date ";
		}else{
			$queryStr .= " and FROM_UNIXTIME(aaa.time,'%Y%m%d') = " . date('Ymd',strtotime("-1 day"));
		}
		$queryStr .= ")*100 from stat_use_gold , stat_use_gold_param where stat_use_gold.serverid = $serverid and stat_use_gold.typeid = stat_use_gold_param.id ";
		if(strlen($date) == 8){
			$queryStr .= " and FROM_UNIXTIME(time,'%Y%m%d') = $date ";
		}else{
			$queryStr .= " and FROM_UNIXTIME(time,'%Y%m%d') = " . date('Ymd',strtotime("-1 day"));
		}
		$queryStr .= " group by stat_use_gold.typeid , FROM_UNIXTIME(time,'%Y%m%d')";
		$queryStr .= " order by sum(stat_use_gold.price) DESC ";
//echo "str:" . $queryStr;		
		$result=mysql_query($queryStr);
		if(!$result){
			echo '错误'.mysql_error();
			exit;
		}
		$num_rows=mysql_num_rows($result);
		$totalResult = array();
		$i = 0;
		while($total = mysql_fetch_array($result)){
			$totalResult[$i] = $total[0] . " " . $total[2] . " " . $total[3] . " " . $total[4] . " " . $total[5] . " " . number_format(trim($total[6]),2) . "% " . "\n";
			$i = $i + 1;
		}
		return $totalResult;
	}

	public static function graphsEverydayRegist(){
	/*
		session_start();
		$serverid = $_SESSION["serverid"];

		$link=mysql_connect(DBConfig::$host,DBConfig::$user,DBConfig::$passwd);
		mysql_set_charset('utf8',$link);
		$db_selected=mysql_select_db(DBConfig::$db_name,$link);
		$result=mysql_query("select FROM_UNIXTIME(registertime,'%Y%m%d'),count(*) from player group by FROM_UNIXTIME(registertime,'%Y%m%d')");
		if(!$result){
			echo '错误'.mysql_error();
			exit;
		}
		$num_rows=mysql_num_rows($result);
		$totalResult = array();
		$i = 0;
		while($total = mysql_fetch_array($result)){
			$totalResult[$i] = $total[0] . " " . $total[1] . "\n";
			$i = $i + 1;
		}
		return $totalResult;
	*/
		return null;
	}
	
	public static function graphsEverydayActive($length){
	/*
		session_start();
		$serverid = $_SESSION["serverid"];

		$link=mysql_connect(DBConfig::$host,DBConfig::$user,DBConfig::$passwd);
		mysql_set_charset('utf8',$link);
		$db_selected=mysql_select_db(DBConfig::$db_name,$link);
		
		$resultCount=mysql_query("select * from stat_player_left");
		$num_rows_count=mysql_num_rows($resultCount);
		$beginLength = $num_rows_count - $length;
		$resultStr = "select date ,activenum from stat_player_left order by date ";
		if($length>0&&$beginLength>0){
			$resultStr .= " limit $beginLength,$num_rows_count";
		}

		$result=mysql_query($resultStr);

		if(!$result){
			echo '错误'.mysql_error();
			exit;
		}
		$num_rows=mysql_num_rows($result);
		$totalResult = array();
		$i = 0;
		while($total = mysql_fetch_array($result)){
			$totalResult[$i] = $total[0] . " " . $total[1] . "\n";
			$i = $i + 1;
		}
		return $totalResult;
	*/
		return null;
	}
	
	public static function pushRewards(){
	/*
		session_start();
		$serverid = $_SESSION["serverid"];

		$link=mysql_connect(DBConfig::$host,DBConfig::$user,DBConfig::$passwd);
		mysql_set_charset('utf8',$link);
		$db_selected=mysql_select_db(DBConfig::$db_name,$link);
		
		$resultCount=mysql_query("select * from stat_player_left");
		$num_rows_count=mysql_num_rows($resultCount);
		$beginLength = $num_rows_count - $length;
		$resultStr = "select date ,activenum from stat_player_left order by date ";
		if($length>0&&$beginLength>0){
			$resultStr .= " limit $beginLength,$num_rows_count";
		}

		$result=mysql_query($resultStr);

		if(!$result){
			echo '错误'.mysql_error();
			exit;
		}
		$num_rows=mysql_num_rows($result);
		$totalResult = array();
		$i = 0;
		while($total = mysql_fetch_array($result)){
			$totalResult[$i] = $total[0] . " " . $total[1] . "\n";
			$i = $i + 1;
		}
		return $totalResult;
	*/
		return null;
	}
	
	public static function fetchLucky(){
		return null;
	}
	
	public static function fetchLogin(){
		return null;
	}
	
	public static function onlineUserCount($data){
		session_start();
		$serverid = $_SESSION["serverid"];
		
		$data_arr = json_decode($data,true);
		$endDate = date("Ymd",strtotime("-1day"));
		$startDate = date("Ymd",strtotime("-1day"));
		if ($data_arr && isset($data_arr['startDate']) && isset($data_arr['endDate'])){
			$startDate = $data_arr['startDate'];
			$endDate = $data_arr['endDate'];
		}
		$day = $startDate;
		$totalResult = array('startDate'=>$startDate,'endDate'=>$endDate);
		do {
			$content = array();
			$fileDir = "/data/backend_logs/$serverid/";
			$file = $fileDir . "OnlineCount.log.$day";
			$totalResult['date'][$day]['max'] = $totalResult['date'][$day]['min'] = 0;
			if (file_exists($file)) {
				$cmd = "cat ${file} | cut -d : -f 4 | cut -d , -f 1-2";
				exec($cmd, $content);
				
				foreach ( $content as $line ) {
					list($time,$count) = explode(',', $line);
					$arr = array('period'=>$time*1000,'count'=>$count);
					$totalResult["info"][] = $arr;
					$totalResult['date'][$day]['total'] += $count;
					$totalResult['date'][$day]['count'] ++;
					if ($count > $totalResult['date'][$day]['max']){
						$totalResult['date'][$day]['max'] = $count;
					}
					elseif ($count < $totalResult['date'][$day]['min']){
						$totalResult['date'][$day]['min'] = $count;
					}
				}
			}
			$day = date("Ymd",strtotime($day) + 24 * 3600);
		}
		while($day <= $endDate);
		
		return $totalResult;
	}
}



















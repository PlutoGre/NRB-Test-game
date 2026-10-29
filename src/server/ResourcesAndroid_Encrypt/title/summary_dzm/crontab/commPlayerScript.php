<?php

require_once(dirname(__dir__).'/common/common.php');
$needday = 1;
function affected(){
	if(($affected_rows=mysql_affected_rows())!= -1){
		printf("共操作了%d条记录",$affected_rows);		
	}else{
		printf("操作失败");

	}
}

$link=mysql_connect(DBConfig::$host,DBConfig::$user,DBConfig::$passwd);
mysql_set_charset('utf8',$link);
$db_selected=mysql_select_db(DBConfig::$db_name,$link);

$serverid = 1;

$queryserver = sprintf("select serverid,servername from stat_server_info");
$resultserver = mysql_query($queryserver,$link);

while($total = mysql_fetch_array($resultserver)){
	$serverid = $total[0];

	
	$file = "/home/luyungeng/apache-tomcat-7.0.37/webapps/ROOT/summary/crontab/log.txt";
	$arrayIndex = array();

	for($i=1;$i <= 14;$i++){
		$temp = $i + $needday;
		$arrayIndex[date('Ymd',strtotime("-$temp day")-$temp)] = 0;
	}
	$temp2 = 30 + $needday;
	$arrayIndex[date('Ymd',strtotime("-$temp2 day")-$temp2)] = 0;
	//查找出所有昨天登录过的用户，按照注册日期，降序排列查出前14个	
	$lastdayLogin = mysql_query("select count(1) ,FROM_UNIXTIME(registertime,'%Y%m%d') from stat_player_info where serverid = $serverid and FROM_UNIXTIME(registertime,'%Y%m%d')  < DATE_SUB(CURDATE(), INTERVAL ($needday) DAY) and FROM_UNIXTIME(lastlogin,'%Y%m%d') = DATE_SUB(CURDATE(), INTERVAL $needday DAY) group by FROM_UNIXTIME(registertime,'%Y%m%d') order by FROM_UNIXTIME(registertime,'%Y%m%d') DESC limit 14");
	while($total = mysql_fetch_array($lastdayLogin)){
		$arrayIndex[$total[1]] = $total[0];
	}
	
	//查找昨天登录的，30天前注册的用户	
	$lastdayLogin = mysql_query("select count(1) ,FROM_UNIXTIME(registertime,'%Y%m%d') from stat_player_info where serverid = $serverid and FROM_UNIXTIME(registertime,'%Y%m%d') = DATE_SUB(CURDATE(), INTERVAL ($needday+30) DAY) and FROM_UNIXTIME(lastlogin,'%Y%m%d') = DATE_SUB(CURDATE(), INTERVAL $needday DAY) group by FROM_UNIXTIME(registertime,'%Y%m%d') order by FROM_UNIXTIME(registertime,'%Y%m%d') DESC limit 1");
	while($total = mysql_fetch_array($lastdayLogin)){
		$arrayIndex[$total[1]] = $total[0];
	}
	
	//查找此前14天，每天注册的用户
	$totalRegist = mysql_query("select count(1) ,FROM_UNIXTIME(registertime,'%Y%m%d') from stat_player_info where serverid = $serverid and FROM_UNIXTIME(registertime,'%Y%m%d')  < DATE_SUB(CURDATE(), INTERVAL ($needday) DAY) group by FROM_UNIXTIME(registertime,'%Y%m%d') order by FROM_UNIXTIME(registertime,'%Y%m%d') DESC limit 14");
	while($total = mysql_fetch_array($totalRegist)){
		$arrayIndex[$total[1]] = $arrayIndex[$total[1]] / $total[0];
	}
	
	//查找30天前注册用户的数量	
	$totalRegist = mysql_query("select count(1) ,FROM_UNIXTIME(registertime,'%Y%m%d') from stat_player_info where serverid = $serverid and FROM_UNIXTIME(registertime,'%Y%m%d') = DATE_SUB(CURDATE(), INTERVAL ($needday+30) DAY) group by FROM_UNIXTIME(registertime,'%Y%m%d') order by FROM_UNIXTIME(registertime,'%Y%m%d') DESC limit 1");
	while($total = mysql_fetch_array($totalRegist)){
		$arrayIndex[$total[1]] = $arrayIndex[$total[1]] / $total[0];
	}
	
	//昨天注册的用户
	$lastdayRegist = mysql_query("select userid from stat_player_info where serverid = $serverid and FROM_UNIXTIME(registertime,'%Y%m%d') = DATE_SUB(CURDATE(), INTERVAL $needday DAY) ");
	$lastdayRegistNum = mysql_num_rows($lastdayRegist);

	//昨天登录的用户，日活跃
	$lastdayLogin = mysql_query("select userid from stat_player_info where serverid = $serverid and FROM_UNIXTIME(lastlogin,'%Y%m%d') = DATE_SUB(CURDATE(), INTERVAL $needday DAY)");
	$lastdayLoginNum = mysql_num_rows($lastdayLogin);
	
	//付费用户活跃数
	$activeRecharge = mysql_query("select stat_player_info.userid from stat_player_info,stat_player_recharge where stat_player_info.serverid = $serverid and stat_player_recharge.serverid = $serverid and FROM_UNIXTIME(lastlogin,'%Y%m%d') = DATE_SUB(CURDATE(), INTERVAL $needday DAY) and stat_player_recharge.userid = stat_player_info.userid");
	$activeRechargeNum = mysql_num_rows($activeRecharge);
	
	//此前一周活跃用户数
	$activeWeek	= mysql_query("select userid from stat_player_info where serverid = $serverid and FROM_UNIXTIME(lastlogin,'%Y%m%d') > DATE_SUB(CURDATE(), INTERVAL ($needday+7) DAY)");
	$activeWeekNum = mysql_num_rows($activeWeek);
	
	//此前一月活跃用户数
	$activeMonth	= mysql_query("select userid from stat_player_info where serverid = $serverid and FROM_UNIXTIME(lastlogin,'%Y%m%d') > DATE_SUB(CURDATE(), INTERVAL ($needday+30) DAY)");
	$activeMonthNum = mysql_num_rows($activeMonth);

	//新增付费用户人数
	$newPayer	= mysql_query("select userid from stat_player_recharge where serverid = $serverid and FROM_UNIXTIME( CreateTime, '%Y%m%d' ) = DATE_SUB(CURDATE(), INTERVAL $needday DAY) and isFirstPay = '1'");

	$newPayerNum = mysql_num_rows($newPayer);
	$temp = date('Ymd',strtotime("-$needday day") -$needday);
	$queryupdate = "update stat_player_left set registnum = $lastdayRegistNum,activenum = $lastdayLoginNum,activeRecharge = $activeRechargeNum,activeWeek = $activeWeekNum,activeMonth = $activeMonthNum,newPayer = $newPayerNum where serverid = $serverid and date = date($temp)";
	$resultinstert = mysql_query($queryupdate,$link);


	for($i=1;$i<=14;$i++){
		$tempDay = $i+$needday;
		$temp =date('Ymd',strtotime("-$tempDay day")-$tempDay); 
		$queryupdate = "update stat_player_left set leftnum${i} = $arrayIndex[$temp] where serverid = $serverid and date = date($temp)";
		$resultupdate = mysql_query($queryupdate,$link); 
	}
	$tempDay = 30+$needday;
	$temp =date('Ymd',strtotime("-$tempDay day")-$tempDay); 
	$queryupdate = "update stat_player_left set leftnum30 = $arrayIndex[$temp] where serverid = $serverid and date = date($temp)";
	$resultupdate = mysql_query($queryupdate,$link); 

	//7日内留存
	$stayin7days = mysql_query("select userid from stat_player_info where serverid = $serverid and FROM_UNIXTIME(registertime,'%Y%m%d') = DATE_SUB(CURDATE(), INTERVAL 7 DAY) and FROM_UNIXTIME(lastlogin,'%Y%m%d') > DATE_SUB(CURDATE(), INTERVAL 7 DAY)");
	$stayin7daysnum = mysql_num_rows($stayin7days);
	$temp = date('Ymd',strtotime("-7 day"));
	$queryupdate = "update stat_player_left set stayin7days = $stayin7daysnum where serverid = $serverid and date = date($temp)";
	$resultupdate = mysql_query($queryupdate,$link);
	
	//15日内留存
	$stayin15days = mysql_query("select userid from stat_player_info where serverid = $serverid and FROM_UNIXTIME(registertime,'%Y%m%d') = DATE_SUB(CURDATE(), INTERVAL 15 DAY) and FROM_UNIXTIME(lastlogin,'%Y%m%d') > DATE_SUB(CURDATE(), INTERVAL 15 DAY)");
	$stayin15daysnum = mysql_num_rows($stayin15days);
	$temp = date('Ymd',strtotime("-15 day"));
	$queryupdate = "update stat_player_left set stayin15days = $stayin15daysnum where serverid = $serverid and date = date($temp)";
	$resultupdate = mysql_query($queryupdate,$link);
	
	//月付费渗透率
	$month = date('Ym',strtotime("-1 day"));
	$monthFirstDay = $month . "01";
	$monthActive = mysql_query("select userid from stat_player_info where serverid = $serverid and FROM_UNIXTIME(lastlogin,'%Y%m%d') >= $monthFirstDay;");
	$monthActiveNum = mysql_num_rows($monthActive);
	
	$monthPayer = mysql_query("select sum(GoodsCount) from stat_player_recharge where serverid = $serverid and UNIX_TIMESTAMP(CreateTime) >= ".strtotime($monthFirstDay)." group by userid;");
	$monthPayerNum = mysql_num_rows($monthPayer);
	
	$queryupdate = "insert into stat_monthly_pay set month=$month,serverid=$serverid,monthActive=$monthActiveNum,monthPay=$monthPayerNum on dunplicate key update monthActive=$monthActiveNum,monthPay=$monthPayerNum";
	
	
	file_put_contents($file,$resultinstert);
	
}
?>









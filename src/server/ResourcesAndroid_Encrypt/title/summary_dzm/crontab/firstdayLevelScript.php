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
		
	$commLevelTotal = mysql_query("select userid from stat_player_info where serverid = $serverid and FROM_UNIXTIME(registertime,'%Y%m%d') = DATE_SUB(CURDATE(), INTERVAL $needday DAY)");
	$num_rows = mysql_num_rows($commLevelTotal);
	
	$commLevel = mysql_query("select count(1),userlevel from stat_player_info where serverid = $serverid and FROM_UNIXTIME(registertime,'%Y%m%d') = DATE_SUB(CURDATE(), INTERVAL $needday DAY) group by userlevel");

	while($total = mysql_fetch_array($commLevel)){
		$queryinsert = sprintf("insert into stat_firstday_level (serverid,date,level,num,perCent) values ('%d','%s','%d','%d','%f')", $serverid,date('Ymd',strtotime("-$needday day")),$total[1],$total[0],$total[0]/$num_rows);
		$resultinstert = mysql_query($queryinsert,$link);
	}
}
?>
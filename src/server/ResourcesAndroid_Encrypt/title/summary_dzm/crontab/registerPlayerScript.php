<?php
require_once(dirname(__dir__).'/common/common.php');
$needday = 1;		
$link=mysql_connect(DBConfig::$host,DBConfig::$user,DBConfig::$passwd);
mysql_set_charset('utf8',$link);
$db_selected=mysql_select_db(DBConfig::$db_name,$link);

$queryserver = sprintf("select serverid,servername from stat_server_info");
$resultserver = mysql_query($queryserver,$link);

while($total = mysql_fetch_array($resultserver)){
	$serverid = $total[0];
	
	$yesterday = date('Ymd',strtotime("-$needday day") -$needday );
	$fileDir = "/data/backend_logs/$serverid/";
	$file = $fileDir . "PlayerLogoutInfo.log.$yesterday";
	if (!file_exists($file)) {
		continue;
	}

	$cmd = "cat ${file}| grep 'info' |  awk -F' ' '{print $9,$10,$11,$12,$13,$14,$15}' | awk -F, '{print $1,$6,$1,$2,$3,$4,$5}' | sort -n -k1 -rk2 -k3 | awk '{if(!uid[$1]) {uid[$1]=1; print $0;}}'"; 
						
	$content = array();
	exec($cmd, $content);
	
	$newContent = array();
	$i = 0;
	foreach ( $content as $line ) {
		$newContent[$i] = $line . "\n";
		$i += 1;
		
		list($userid,$lastlogin,$username,$registertime,$userlevel,$userviplevel,$userxp) = explode('  ', $line);
		$query = sprintf("select serverid from  stat_player_info where serverid = '%d' and userid = '%d' ",$serverid,$userid);
		$result = mysql_query($query,$link);
		$num_rows=mysql_num_rows($result);
		if(mysql_fetch_array($result)){
		
			$sqlStr = "update stat_player_info set lastlogin = '%d',username = '%s',registertime = '%d',userlevel = '%d',userviplevel = '%d',userxp = '%d' where serverid = '%d' and userid = '%d'";
		}else{
		
			$sqlStr = "INSERT INTO stat_player_info (lastlogin,username,registertime,userlevel,userviplevel,userxp,serverid,userid) VALUES ('%d','%s','%d','%d','%d','%d','%d','%d')";
		}
		$queryinsert = sprintf($sqlStr,$lastlogin,$username,$registertime,$userlevel,$userviplevel,$userxp,$serverid,$userid);
		$resultinstert = mysql_query($queryinsert,$link);
	}
	$cmd = "cat ${file}| grep 'logout info:' |  awk -F' ' '{print $9,$10,$11,$12,$13,$14,$15}' | awk -F, '{print $1,$6,$1,$2,$3,$4,$5}' | sort -n -k1 -rk2 -k3 | awk '{if(!uid[$1]) {uid[$1]=1; print $0;}}'"; 
						
	$content = array();
	exec($cmd, $content);
	
	$newContent = array();
	$i = 0;
	foreach ( $content as $line ) {
		$newContent[$i] = $line . "\n";
		$i += 1;
	}
	$queryinsert = sprintf("insert into stat_player_left (serverid,date,logoutNum,registnum,activenum) values ('%d','%s','%d','%d','%d')",$serverid,date('Ymd',strtotime("-$needday day") -$needday),$i,0,0);
	$resultinstert = mysql_query($queryinsert,$link);
	

	$file1 = '/data/htdocs/summary/crontab/PlayerLogoutInfo.txt';
}
?>
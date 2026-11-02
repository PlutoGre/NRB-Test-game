<?php
require_once(dirname(__dir__).'/common/common.php');
$needday = 1;
$link=mysql_connect(DBConfig::$host,DBConfig::$user,DBConfig::$passwd);
mysql_set_charset('utf8',$link);
$db_selected=mysql_select_db(DBConfig::$db_name,$link);

$serverid = 1;

$queryserver = sprintf("select serverid,servername from stat_server_info");
$resultserver = mysql_query($queryserver,$link);

while($total = mysql_fetch_array($resultserver)){
	$serverid = $total[0];

	$yesterday = date('Ymd',strtotime("-$needday day"));
	$fileDir = "/data/backend_logs/$serverid/";
	$file = $fileDir . "GoldCost.log.$yesterday";

	if (!file_exists($file)) {
	echo "no such file:$file\n";
		continue;
	}

	$cmd = "cat ${file} | cut -d : -f 4 | cut -d , -f 1-7";
	$content = array();
	exec($cmd, $content);
	foreach ( $content as $line ) {
		list($uid,$userid,$typeid,$num,$beforewaste,$afterwaste,$price) = explode(',', $line);
		$queryinsert = sprintf("INSERT INTO stat_use_gold (serverid,time,userid,typeid,num,beforewaste,afterwaste,price) VALUES ('%d','%d','%d','%d','%d','%d','%d','%d')",$serverid,$uid,$userid,$typeid,$num,$beforewaste,$afterwaste,$price);
		$resultinstert = mysql_query($queryinsert,$link);
	}

}
?>
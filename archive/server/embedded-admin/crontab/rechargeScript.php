<?php
require_once(dirname(__dir__).'/common/common.php');
$needday =1;	
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
	$file = $fileDir . "RechargeInfo.log.$yesterday";
	if (!file_exists($file)) {
		continue;
	}

	$cmd = "cat ${file}| grep 'shopPurchaseCheckRet' |  awk -F' ' '{print $12,$13,$14,$15,$16,$17,$18}' | awk -F, '{print $1,$2,$3,$4,$5,$6,$7}'"; 

	$content = array();
	exec($cmd, $content);
	
	$newContent = array();
	$i = 0;
	foreach ( $content as $line ) {
		$newContent[$i] = $line . "\n";
		$i += 1;
		
		list($userid,$delay,$isFirstPay,$CreateTime,$GoodsId,$GoodsCount,$registertime) = explode('  ', $line);
		$queryinsert = sprintf("INSERT INTO stat_player_recharge (serverid,userid,isFirstPay,CreateTime,GoodsId,GoodsCount,registertime) VALUES ('%d','%d','%d','%d','%d','%d','%d')",$serverid,$userid,$isFirstPay,$CreateTime,$GoodsId,$GoodsCount,$registertime);
		$resultinstert = mysql_query($queryinsert,$link);
	}
	

	$file1 = '/data/htdocs/summary/crontab/RechargeInfo.txt';
	file_put_contents($file1,$newContent);
}
?>
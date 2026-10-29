<?php
	require_once(dirname(__dir__).'/common/common.php');
	
	$link=mysql_connect(DBConfig::$host,DBConfig::$user,DBConfig::$passwd);
	mysql_set_charset('utf8',$link);
	$db_selected=mysql_select_db(DBConfig::$db_name,$link);
	
	
	$today = date('Ymd');
	$fileDir = '/data/htdocs/summary/logs';
	$file = 'rechargeConfig.txt';

	$cmd = "cat rechargeConfig.txt | awk '{print $1,$2,$3,$4}'";
	
	$content = array();
	exec($cmd, $content);
	echo "******************************\n";
	foreach ( $content as $line ) {
		list($id,$amount,$addNum,$costMoney) = explode(' ', $line);
		echo "id:" . trim($id) . "\n";
		echo "amount:" . trim($amount) . "\n";
		echo "addNum:" . trim($addNum) . "\n";
		echo "costMoney:" . trim($costMoney) . "\n";
		echo "\n";
		$queryinsert = sprintf("INSERT INTO stat_recharge_param (id,amount,addNum,costMoney) VALUES ('%d','%d','%d','%d')",$id,$amount,$addNum,$costMoney);
		$resultinstert = mysql_query($queryinsert,$link);
	}
	
/*
	print_r($content);
	$file2 = '/data/htdocs/summary/logs/goldCost2.log';
	file_put_contents($file2,$content);
*/





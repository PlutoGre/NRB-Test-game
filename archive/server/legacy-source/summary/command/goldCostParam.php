<?php
	require_once(dirname(__dir__).'/common/common.php');
	
	$link=mysql_connect(DBConfig::$host,DBConfig::$user,DBConfig::$passwd);
	mysql_set_charset('utf8',$link);
	$db_selected=mysql_select_db(DBConfig::$db_name,$link);
	
	
	$today = date('Ymd');
	$fileDir = '/data/htdocs/summary/logs';
	$file = 'Tools.txt';

	$cmd = "cat Tools.txt | awk '{print $1,$2,$4}'";
	
	$content = array();
	exec($cmd, $content);
	echo "******************************\n";
	foreach ( $content as $line ) {
		list($id,$name,$price) = explode(' ', $line);
		echo "id:" . trim($id) . "\n";
		echo "name:" . trim($name) . "\n";
		echo "price:" . trim($price) . "\n";
		echo "\n";
		$queryinsert = sprintf("INSERT INTO stat_use_gold_param (id,name,price) VALUES ('%d','%s','%d')",$id,$name,$price);
		$resultinstert = mysql_query($queryinsert,$link);
	}
	
/*
	print_r($content);
	$file2 = '/data/htdocs/summary/logs/goldCost2.log';
	file_put_contents($file2,$content);
*/




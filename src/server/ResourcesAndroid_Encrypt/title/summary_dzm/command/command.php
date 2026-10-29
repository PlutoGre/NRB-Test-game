<?php
include_once('./firstCommand.php');
include_once('./secondCommand.php');
include_once('./UserInfo.php');
$today = date('Ymd');
$fileDir = '/home/logs/';
$file = '/home/logs/ChatInfo.log.20130415';

$cmd = "mkdir /home/logs/$today";
exec($cmd);

$cmd = "cat ${file}";
$content = array();
exec($cmd, $content);
$file2 = "/home/logs/$today/people.txt";
$content2 = "john";
file_put_contents($file2,$content);

FirstCommand::run();
UserInfo::run();


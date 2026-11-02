<?php
session_start();
$serverid = $_SESSION["serverid"];
if (!function_exists('json_decode')){
	exit('您的PHP不支持JSON，请升级您的PHP版本。');
}
header("Content-type: text/html; charset=utf-8");
require_once(dirname(__dir__).'/common/common.php');
$link=mysql_connect(DBConfig::$host,DBConfig::$user,DBConfig::$passwd);
mysql_set_charset('utf8',$link);
$db_selected=mysql_select_db(DBConfig::$db_name,$link);

if (!$_GET || !isset($_GET['name'])){
	echo 'error';
	return ;
}


$name = $_GET['name'];

require_once('../proto/message/pb_message.php');
require_once('../proto/pb_proto_BackStageFetchUserInfo.php');

$Result = array();//存放结果数组

$socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
session_start();
$connection = socket_connect($socket, $_SESSION['serverip'],$_SESSION['serverport']);

$a = 17222;
$opcode = 182;

$OPBackStageFetchUserInfo = new OPBackStageFetchUserInfo();
$OPBackStageFetchUserInfo->set_version(1);
$OPBackStageFetchUserInfo->set_name($name);

$data = $OPBackStageFetchUserInfo->SerializeToString();

$length = strlen($data);
$format = "nNNA".$length;

$t = pack($format,$a,$opcode,$length,$data);
socket_write($socket, $t);

$receiveData = socket_read($socket,1024*1024*4);
socket_close($socket);
$rt = array();

$format = "H*";
$rt = unpack($format,$receiveData);
$out = substr($rt[1], 20);

$OPBackStageFetchUserInfoRet = new OPBackStageFetchUserInfoRet();
$OPBackStageFetchUserInfoRet->parseFromString(hexDecode($out));

$Result['servertime'] = $OPBackStageFetchUserInfoRet->servertime();
$Result['userid'] = $OPBackStageFetchUserInfoRet->userid();
$Result['name'] = $OPBackStageFetchUserInfoRet->name();
$Result['registertime'] = $OPBackStageFetchUserInfoRet->registertime();
$Result['lastlogin'] = $OPBackStageFetchUserInfoRet->lastlogin();
$Result['level'] = $OPBackStageFetchUserInfoRet->level();
$Result['silvercoins'] = $OPBackStageFetchUserInfoRet->silvercoins();
$Result['goldcoins'] = $OPBackStageFetchUserInfoRet->goldcoins();

$id = $Result['userid'];

$data = array();

for ($i = 1; $i <= 10; $i++) {
	$info = array();
	$date = date("Ymd",strtotime("-$i days"));
	$info['date'] = $date;
	$info['status'] = 0;
	$file = "/data/backend_logs/$serverid/PlayerLogoutInfo.log.$date";
	if (!file_exists($file)){
		continue;
	}
	$cmd = "cat ${file}| grep 'info' |  grep 'info' |  awk -F' ' '{print $9}' | awk -F , '{print $1}'";
	$content = array();
	exec($cmd, $content);
	if (in_array($id, $content)){
		$info['status'] = 1;
	}
	$data[] = $info;
}
$Result['loginInfo'] = $data;

echo json_encode($Result);

//16进制编码转换为汉字
function hexDecode($s) {
	return preg_replace('/(\w{2})/e',"chr(hexdec('\\1'))",$s);
}
?>
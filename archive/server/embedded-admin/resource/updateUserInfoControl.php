<?php
header("Content-type: text/html; charset=utf-8");

if (!function_exists('json_decode')){
	exit('您的PHP不支持JSON，请升级您的PHP版本。');
}

/**
 * 应用服务器接收91服务器端发过来支付购买结果通知的接口DEMO
 * 当然这个DEMO只是个参考，具体的操作和业务逻辑处理开发者可以自由操作
 */
/*
 * 这里的MyAppId和MyAppKey是我们自己做测试的
* 开发者可以自己根据自己在dev.91.com平台上创建的具体应用信息进行修改
*/
$MyAppId = 102617;
$MyAppKey = '3e77d7dac206996edb4b15883a9f3f04dfa557b73e61fefd';

$Res = pay_result_notify_process($MyAppId,$MyAppKey);

//print_r($Res);

/**
 * 此函数就是接收91服务器那边传过来传后进行各种验证操作处理代码
 * @param int $MyAppId 应用Id
 * @param string $MyAppKey 应用Key
 * @return json 结果信息
 */

//汉字转换为16进制编码
function hexEncode($s) {
    return preg_replace('/(.)/es',"str_pad(dechex(ord('\\1')),2,'0',STR_PAD_LEFT)",$s);   
}

//16进制编码转换为汉字
function hexDecode($s) {
    return preg_replace('/(\w{2})/e',"chr(hexdec('\\1'))",$s);
}
 
function pay_result_notify_process($MyAppId,$MyAppKey){
	$date = date("Ymd");
	$file = "./recharge_log_".$date.".log";
	require_once('../proto/message/pb_message.php');
	require_once('../proto/pb_proto_BackStageUpdateUserInfo.php');
	
	$data = "AppId:".$_REQUEST['AppId']."\t Act:".$_REQUEST['Act']."\t ProductName:".$_REQUEST['ProductName']."\t ConsumeStreamId:".$_REQUEST['ConsumeStreamId']."\t CooOrderSerial:".$_REQUEST['CooOrderSerial']."\t Uin:".$_REQUEST['Uin']."\t GoodsId:".$_REQUEST['GoodsId']."\t GoodsInfo:".$_REQUEST['GoodsInfo']."\t GoodsCount:".$_REQUEST['GoodsCount']."\t OriginalMoney:".$_REQUEST['OriginalMoney']."\t OrderMoney:".$_REQUEST['OrderMoney']."\t Note:".$_REQUEST['Note']."\t PayStatus:".$_REQUEST['PayStatus']."\t CreateTime:".$_REQUEST['CreateTime']."\t Sign:".$_REQUEST['Sign']."\n";
	
//	error_log($data,3,$file);

	$Result = array();//存放结果数组

	if(empty($_GET)||!isset($_GET['field'])||!isset($_GET['value'])||!isset($_GET['name'])||!isset($_GET['value2'])){

		$Result['ErrorCode'] =  0;
		$Result['ErrorDesc'] =  urlencode('接收失败');
		$Res = json_encode($Result);
		return urldecode($Res);
	}

	$field 				= $_GET['field'];//
	$value	 			= $_GET['value'];//
	$name				= $_GET['name'];//用户掌门名
	$value2				= $_GET['value2'];

	$socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
	session_start();
	$connection = socket_connect($socket, $_SESSION['serverip'],$_SESSION["serverport"]);

	
	$a = 17222;
	
	$opcode = 184;
	
	$OPBackStageUpdateUserInfo = new OPBackStageUpdateUserInfo();
	$OPBackStageUpdateUserInfo->set_version(1);
	$OPBackStageUpdateUserInfo->set_name($name);
	$OPBackStageUpdateUserInfo->set_changeType($field);
	$OPBackStageUpdateUserInfo->set_changeNum1($value);
	$OPBackStageUpdateUserInfo->set_changeNum2($value2);
	
	$data = $OPBackStageUpdateUserInfo->SerializeToString();

	$length = strlen($data);
//	var_dump($length);
	$format = "nNNA".$length;

	$t = pack($format,$a,$opcode,$length,$data);
	socket_write($socket, $t);
	
	$receiveData = socket_read($socket,1024*1024*4);

	socket_close($socket);
	$rt = array();

	$format = "H*";
	$rt = unpack($format,$receiveData);
	$out = substr($rt[1], 20);

	$OPBackStageUpdateUserInfoRet = new OPBackStageUpdateUserInfoRet();
	$OPBackStageUpdateUserInfoRet->parseFromString(hexDecode($out));
/*
echo "servertime:" . $OPBackStageUpdateUserInfoRet->servertime() . "\n";
echo "userid:" . $OPBackStageUpdateUserInfoRet->userid() . "\n";
echo "name:" . $OPBackStageUpdateUserInfoRet->name() . "     \n";
echo "registertime:" . $OPBackStageUpdateUserInfoRet->registertime() . "\n";
echo "lastlogin:" . $OPBackStageUpdateUserInfoRet->lastlogin() . "\n";
echo "level:" . $OPBackStageUpdateUserInfoRet->level() . "\n";
echo "silvercoins:" . $OPBackStageUpdateUserInfoRet->silvercoins() . "\n";
echo "goldcoins:" . $OPBackStageUpdateUserInfoRet->goldcoins() . "\n";

*/
	$Result['servertime'] = $OPBackStageUpdateUserInfoRet->servertime();
	$Result['userid'] = $OPBackStageUpdateUserInfoRet->userid();
	$Result['name'] = $OPBackStageUpdateUserInfoRet->name();
	$Result['registertime'] = $OPBackStageUpdateUserInfoRet->registertime();
	$Result['lastlogin'] = $OPBackStageUpdateUserInfoRet->lastlogin();
	$Result['level'] = $OPBackStageUpdateUserInfoRet->level();
	$Result['silvercoins'] = $OPBackStageUpdateUserInfoRet->silvercoins();
	$Result['goldcoins'] = $OPBackStageUpdateUserInfoRet->goldcoins();
	
	echo json_encode($Result);
/*
	$data = "CooOrderSerial:".$CooOrderSerial." successed!. \n";
	error_log($data,3,$file);

	$Result['ErrorCode'] =  1;
	$Result['ErrorDesc'] =  urlencode('接收成功');
	$Res = json_encode($Result);
	return urldecode($Res);
*/

}



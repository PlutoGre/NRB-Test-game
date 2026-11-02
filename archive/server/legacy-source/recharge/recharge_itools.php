<?php
require_once('./sdk/itools/notify.class.php');
require_once('./config.php');
require_once('./message/pb_message.php');
require_once('./example/recharge/pb_proto_recharge.php');
date_default_timezone_set("Asia/Shanghai");
$date = date("Ymd");
$file = "./logs/itools/recharge_log_".$date.".log";

if (!isset($_POST['notify_data']) || !isset($_POST['sign'])) {
	$data = "params empty!data=" . $_POST['notify_data'] . ",sign:" . $_POST['sign'] . "\n";
	error_log($data,3,$file);
	die('fail');
}

//通知数据
$notify_data = $_POST['notify_data'];
//签名
$sign = $_POST['sign'];

$notify = new notify;

//RSA解密
$notify_data = $notify->decrypt($notify_data);

//验证签名
if ($notify->verify($notify_data, $sign)) {
	//逻辑处理, $notify_data: json数据(格式: {"order_id_com":"2013050900000712","user_id":"10010","amount":"0.10","account":"test001","order_id":"2013050900000713","result":"success"})
	$json = json_decode($notify_data, true);
	
	$order_id_com = $json['order_id_com'];
	$userId = "ITOUSR_" . $json['user_id'];
	$amount = $json['amount'];
	$account = $json['account'];
	$order_id = $json['order_id'];
	$result = $json['result'];
	
	$goodId = Config::getIdByAmount(intval($amount));
	
	$data = "AppId:5"."\t Act:1\t ProductName:海贼币\t ConsumeStreamId:".$order_id_com."\t CooOrderSerial:".$order_id."\t Uin:".$userId."\t GoodsId:".$goodId."\t GoodsInfo:"."\t GoodsCount:1\t OriginalMoney:".$amount."\t OrderMoney:".$amount."\t Note:"."\t PayStatus:".$result . "\t CreateTime:".date("Y-m-d H:i:s")."\t Sign:".$sign."\n";
	
	if ($goodId == 0){
		$err_msg = "goods id not exist.\t" . $data;
		error_log($err_msg,3,$file);
		die('fail');
	}
	
	if ($result != "success"){
		$err_msg = "result not right.\t" . $data;
		error_log($err_msg,3,$file);
		die('fail');
	}
	
	$ser_arr = explode("_", $order_id_com,2);
	$serverId = $ser_arr[0];
	$dbConfig = Config::getDBConfigByServerId($serverId);
	$javaConfig = Config::getJavaConfigByServerId($serverId);
	
	if (empty($dbConfig) || empty($javaConfig)){
		$data = "CooOrderSerial:".$order_id."\t server config:$serverId error!\n";
		error_log($data,3,$file);
		die(json_encode(array('status'=>'error')));
	}
		
	$db = mysql_connect($dbConfig['host'],$dbConfig['username'],$dbConfig['passwd']);
		
	mysql_select_db($dbConfig['dbName'],$db);
		
	$sql = "select * from pay where CooOrderSerial = '$order_id'";
		
	$query = mysql_query($sql,$db);
		
	$result = mysql_fetch_assoc($query);
	if (empty($result)){
		$socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
		$connection = socket_connect($socket, $javaConfig['host'],$javaConfig['port']);
			
		$a = 17222;
		$opcode = 995;
			
			
		$OPRecharge = new OPRecharge();
		$OPRecharge->set_ConsumeStreamId($order_id_com);
		$OPRecharge->set_CooOrderSerial($order_id);
		$OPRecharge->set_Uin($userId);
		$OPRecharge->set_GoodsId($goodId);
		$OPRecharge->set_GoodsCount(1);
		$OPRecharge->set_CreateTime(date("Y-m-d h:i:s"));
		$pdata = $OPRecharge->SerializeToString();
			
		$length = strlen($pdata);
		$format = "nNNA".$length;
		$t = pack($format,$a,$opcode,$length,$pdata);
		socket_write($socket, $t);
	}
	//记录日志
	$msg = "Pay success!\n" . $data;
	error_log($msg,3,$file);

	echo "success";

} else {
	$data = "check sign error!data=" . json_encode($notify_data) . ",sign:" . $sign ."\n";
	error_log($data,3,$file);
	echo "fail";
}
?>
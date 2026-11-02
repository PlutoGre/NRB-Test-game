<?php
date_default_timezone_set("Asia/Shanghai");
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

print_r($Res);

/**
 * 此函数就是接收91服务器那边传过来传后进行各种验证操作处理代码
 * @param int $MyAppId 应用Id
 * @param string $MyAppKey 应用Key
 * @return json 结果信息
 */
function pay_result_notify_process($MyAppId,$MyAppKey){
	
	$date = date("Ymd");
	$file = "./recharge_log_".$date.".log";
	require_once('./message/pb_message.php');
	require_once('./example/recharge/pb_proto_recharge.php');
	
	$data = "AppId:".$_REQUEST['AppId']."\t Act:".$_REQUEST['Act']."\t ProductName:".$_REQUEST['ProductName']."\t ConsumeStreamId:".$_REQUEST['ConsumeStreamId']."\t CooOrderSerial:".$_REQUEST['CooOrderSerial']."\t Uin:".$_REQUEST['Uin']."\t GoodsId:".$_REQUEST['GoodsId']."\t GoodsInfo:".$_REQUEST['GoodsInfo']."\t GoodsCount:".$_REQUEST['GoodsCount']."\t OriginalMoney:".$_REQUEST['OriginalMoney']."\t OrderMoney:".$_REQUEST['OrderMoney']."\t Note:".$_REQUEST['Note']."\t PayStatus:".$_REQUEST['PayStatus']."\t CreateTime:".$_REQUEST['CreateTime']."\t Sign:".$_REQUEST['Sign']."\n";
	
	error_log($data,3,$file);

	$Result = array();//存放结果数组

	if(empty($_GET)||!isset($_GET['AppId'])||!isset($_GET['Act'])||!isset($_GET['ProductName'])||!isset($_GET['ConsumeStreamId'])
			||!isset($_GET['CooOrderSerial'])||!isset($_GET['Uin'])||!isset($_GET['GoodsId'])||!isset($_GET['GoodsInfo'])||!isset($_GET['GoodsCount'])||!isset($_GET['OriginalMoney'])
			||!isset($_GET['OrderMoney'])||!isset($_GET['Note'])||!isset($_GET['PayStatus'])||!isset($_GET['CreateTime'])||!isset($_GET['Sign'])){

		$Result["ErrorCode"] =  "0";//注意这里的错误码一定要是字符串格式
		$Result["ErrorDesc"] =  urlencode("接收失败");
		$Res = json_encode($Result);
		return urldecode($Res);
	}

	$AppId 				= $_GET['AppId'];//应用ID
	$Act	 			= $_GET['Act'];//操作
	$ProductName		= $_GET['ProductName'];//应用名称
	$ConsumeStreamId	= $_GET['ConsumeStreamId'];//消费流水号
	$CooOrderSerial	 	= $_GET['CooOrderSerial'];//商户订单号
	$Uin			 	= $_GET['Uin'];//91帐号ID
	$GoodsId		 	= $_GET['GoodsId'];//商品ID
	$GoodsInfo		 	= $_GET['GoodsInfo'];//商品名称
	$GoodsCount		 	= $_GET['GoodsCount'];//商品数量
	$OriginalMoney	 	= $_GET['OriginalMoney'];//原始总价（格式：0.00）
	$OrderMoney		 	= $_GET['OrderMoney'];//实际总价（格式：0.00）
	$Note			 	= $_GET['Note'];//支付描述
	$PayStatus		 	= $_GET['PayStatus'];//支付状态：0=失败，1=成功
	$CreateTime		 	= $_GET['CreateTime'];//创建时间
	$Sign		 		= $_GET['Sign'];//91服务器直接传过来的sign


	//因为这个DEMO是接收验证支付购买结果的操作，所以如果此值不为1时就是无效操作
	if($Act != 1){
		$data = "CooOrderSerial:".$_REQUEST['CooOrderSerial']."\t act $Act invalid \n";
		error_log($data,3,$file);
		$Result['ErrorCode'] =  "3";
		$Result["ErrorDesc"] =  urlencode("Act无效");
		$Res = json_encode($Result);
		return urldecode($Res);
	}

	//如果传过来的应用ID开发者自己的应用ID不同，那说明这个应用ID无效
	if($MyAppId != $AppId){
		$data = "CooOrderSerial:".$_REQUEST['CooOrderSerial']."\t appid invalid \n";
		error_log($data,3,$file);
		$Result['ErrorCode'] =  "2";
		$Result["ErrorDesc"] =  urlencode("AppId无效");
		$Res = json_encode($Result);
		return urldecode($Res);
	}

	//按照API规范里的说明，把相应的数据进行拼接加密处理
	$sign_check = md5($MyAppId.$Act.$ProductName.$ConsumeStreamId.$CooOrderSerial.$Uin.$GoodsId.$GoodsInfo.$GoodsCount.$OriginalMoney.$OrderMoney.$Note.$PayStatus.$CreateTime.$MyAppKey);

	if($sign_check == $Sign){//当本地生成的加密sign跟传过来的sign一样时说明数据没问题

		/*
		 *
		* 开发者可以在此处进行订单号是否合法、商品是否正确等一些别的订单信息的验证处理
		* 相应的别的错误用不同的代码和相应说明信息，数字和信息开发者可以自定义（数字不能重复）
		* 如果所有的信息验证都没问题就可以做出验证成功后的相应逻辑操作
		* 不过最后一定要返回上面那样格式的json数据
		*
		*/
		$serverId = intval($Note);
		$dbConfig = getDBConfigByServerId($serverId);
		$javaConfig = getJavaConfigByServerId($serverId);
		
		if (empty($dbConfig) || empty($javaConfig)){
			$data = "CooOrderSerial:".$_REQUEST['CooOrderSerial']."\t server config:$sign_check error!\n";
			error_log($data,3,$file);
			$Result['ErrorCode'] =  "4";
			$Result['ErrorDesc'] =  urlencode('参数无效');
			$Res = json_encode($Result);
			return urldecode($Res);
		}
		
		$db = mysql_connect($dbConfig['host'],$dbConfig['username'],$dbConfig['passwd']);
		
		mysql_select_db($dbConfig['dbName'],$db);
		
		$sql = "select * from pay where CooOrderSerial = '$CooOrderSerial'";
		
		$query = mysql_query($sql,$db);
		
		$result = mysql_fetch_assoc($query);
		
		if (empty($result)){
			$socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
			$connection = socket_connect($socket, $javaConfig['host'],$javaConfig['port']);
			
			$a = 17222;
			$opcode = 995;
			
			
			$OPRecharge = new OPRecharge();
			$OPRecharge->set_ConsumeStreamId($ConsumeStreamId);
			$OPRecharge->set_CooOrderSerial($CooOrderSerial);
			$OPRecharge->set_Uin($Uin);
			$OPRecharge->set_GoodsId($GoodsId);
			$OPRecharge->set_GoodsCount($GoodsCount);
			$OPRecharge->set_CreateTime($CreateTime);
			
			$data = $OPRecharge->SerializeToString();
			
			$length = strlen($data);
			var_dump($length);
			$format = "nNNA".$length;
			$t = pack($format,$a,$opcode,$length,$data);
			socket_write($socket, $t);
		}
		
		$data = "CooOrderSerial:".$CooOrderSerial." successed!. \n";
		error_log($data,3,$file);

		$Result['ErrorCode'] =  "1";
		$Result['ErrorDesc'] =  urlencode('接收成功');
		$Res = json_encode($Result);
		return urldecode($Res);
	}else{
		$data = "CooOrderSerial:".$_REQUEST['CooOrderSerial']."\t sign:$sign_check error!\n";
		error_log($data,3,$file);
		$Result['ErrorCode'] =  "5";
		$Result['ErrorDesc'] =  urlencode('Sign无效');
		$Res = json_encode($Result);
		return urldecode($Res);
	}
}

function getDBConfigByServerId($serverId){
	$configArr = array();
	switch ($serverId){
		case 1:
			$configArr = array('host'=>'10.66.1.122:1026','username'=>'root','passwd'=>'youai123','dbName'=>'dzm_1');
			break;
		case 2:
			$configArr = array('host'=>'10.66.1.122:1027','username'=>'root','passwd'=>'youai123','dbName'=>'dzm_2');
			break;
		case 3:
			$configArr = array('host'=>'10.66.1.122:1047','username'=>'root','passwd'=>'yajy1234','dbName'=>'dzm');
			break;
		case 4:
			$configArr = array('host'=>'10.66.1.122:1046','username'=>'root','passwd'=>'youai123','dbName'=>'dzm');
			break;
	}
	return $configArr;
}

function getJavaConfigByServerId($serverId){
	$configArr = array();
	switch ($serverId){
		case 1:
			$configArr = array('host'=>'10.207.249.21','port'=>25524);
			break;
		case 2:
			$configArr = array('host'=>'10.207.137.110','port'=>25524);
			break;
		case 3:
			$configArr = array('host'=>'10.207.140.228','port'=>25524);
			break;
		case 4:
			$configArr = array('host'=>'10.207.141.49','port'=>25524);
			break;
	}
	return $configArr;
}
?>

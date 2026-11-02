<?php
/*
	pp充值平台：http://pay.25pp.com/
	以下为示例代码
*/
include('./sdk/Rsa.php');
include('./sdk/MyRsa.php');
require_once('./message/pb_message.php');
require_once('./example/recharge/pb_proto_recharge.php');
require_once('./config.php');
date_default_timezone_set("Asia/Shanghai");

testController::Appreturn();

class testController
{
	public function Appreturn()
	{
		$date = date("Ymd");
		$file = "./logs/tbt/recharge_log_".$date.".log";
		$gamekey = 'c1pWLjZG@TI5tCPnc1&WLYwF0Hg5C7Pn';//游戏验证key 由同步分配
		$gameid = 130729;
		//构造参数数组
		$paramArray = array(
		                    'source'=>'',
		                    'trade_no'=>'',
		                    'amount'=>'',
		                    'partner'=>'',
		                    'paydes'=>'',//SDK2.4新增字段，传支付说明信息
		                    'debug'=>0,//是否是测试模式
		                    'sign'=>'',
		              );
		             
		//参数赋值
		foreach($paramArray as $key =>$data){
		    if(isset($_GET[$key])){
		        $paramArray[$key] = $_GET[$key];
		    }
		}
		
		$data = "AppId:".$paramArray['partner']."\t Act:1\t ProductName:海贼币\t ConsumeStreamId:".$paramArray['trade_no']."\t CooOrderSerial:".$paramArray['source']."\t Uin:".$paramArray['paydes']."\t GoodsId:".$paramArray['paydes']."\t GoodsInfo:".$paramArray['paydes']."\t GoodsCount:1\t OriginalMoney:".$paramArray['amount']."\t OrderMoney:".$paramArray['amount']."\t Note:".$paramArray['paydes']."\t PayStatus:1"."\t CreateTime:".date("Y-m-d H:i:s")."\t Sign:".$paramArray['sign']."\n";
		error_log($data,3,$file);
		
		if ($paramArray['partner'] != $gameid){
			$data = "CooOrderSerial:".$paramArray['trade_no']."\t partner:".$paramArray['partner']." error!\n";
			error_log($data,3,$file);
			die('fail');
		}
		$chkres = self::chk($paramArray,$gamekey);
		if($chkres) {
			//验证通过
			//--------业务处理----------
			$order_id = $paramArray['trade_no'];
			$billno = $paramArray['trade_no'];
			$amount = intval($paramArray['amount']/100);
			$status = 1;
			$app_id = $paramArray['partner'];
			
			$paydes = $paramArray['paydes'];
			$desArr = explode("|", $paydes);
			if (count($desArr) != 2){
				$data = "CooOrderSerial:".$order_id."\t server paydes:$paydes error!\n";
				error_log($data,3,$file);
				die(json_encode(array('status'=>'error')));
			}
			$userArr = explode(":", $desArr[0]);
			if (count($userArr) != 2 || $userArr[0] != 'ID' || $userArr[1] == ''){
				$data = "CooOrderSerial:".$order_id."\t server paydes:$paydes error!\n";
				error_log($data,3,$file);
				die(json_encode(array('status'=>'error')));
			}
			$id = $userArr[1];
			$uuid = $id;
			$roleid = $id;
			
			$serverArr = explode(":", $desArr[1]);
			if (count($serverArr) != 2 || $serverArr[0] != 'SERVER' || $serverArr[1] == ''){
				$data = "CooOrderSerial:".$order_id."\t server paydes:$paydes error!\n";
				error_log($data,3,$file);
				die(json_encode(array('status'=>'error')));
			}
			$zone = intval($serverArr[1]);
			
			
			$dbConfig = Config::getDBConfigByServerId($zone);
			$javaConfig = Config::getJavaConfigByServerId($zone);
			$id = Config::getIdByAmount($amount);
			
			if (empty($dbConfig) || empty($javaConfig) || $id == 0){
				$data = "CooOrderSerial:".$order_id."\t server config:$zone error!\n";
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
				$OPRecharge->set_ConsumeStreamId($order_id);
				$OPRecharge->set_CooOrderSerial($billno);
				$OPRecharge->set_Uin($roleid);
				$OPRecharge->set_GoodsId($id);
				$OPRecharge->set_GoodsCount(1);
				$OPRecharge->set_CreateTime(date("Y-m-d h:i:s"));
					
				$data = $OPRecharge->SerializeToString();
					
				$length = strlen($data);
				//var_dump($length);
				$format = "nNNA".$length;
				$t = pack($format,$a,$opcode,$length,$data);
				socket_write($socket, $t);
			}
			
			
			echo json_encode(array('status'=>'success'));
		}else{
			echo json_encode(array('status'=>'error'));
		}

	}
	
	public function chk($paramArray,$gamekey)
	{
		if(isset($_GET['paydes']) && isset($_GET['debug'])){//SDK 2.4 版本以上(含2.4)paydes 需加入验证 
		    $sign = md5('source='.$paramArray['source'].'&trade_no='.$paramArray['trade_no'].'&amount='.$paramArray['amount'].'&partner='.$paramArray['partner'].'&paydes='.$paramArray['paydes'].'&debug='.$paramArray['debug'].'&key='.$gamekey);
		}else{
		    $sign = md5('source='.$paramArray['source'].'&trade_no='.$paramArray['trade_no'].'&amount='.$paramArray['amount'].'&partner='.$paramArray['partner'].'&key='.$gamekey);   
		}   
		if($sign==$paramArray['sign'])
		    return true;
		else
		    return false;
	}
}
?>

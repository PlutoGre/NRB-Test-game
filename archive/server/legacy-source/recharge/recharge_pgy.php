<?php
/*
	pp充值平台：http://pay.25pp.com/
	以下为示例代码
*/
include('./sdk/Rsa.php');
include('./sdk/MyRsa.php');
require_once('./message/pb_message.php');
require_once('./example/recharge/pb_proto_recharge.php');
date_default_timezone_set("Asia/Shanghai");

testController::Appreturn();

class testController
{
	public function Appreturn()
	{
		$date = date("Ymd");
		$file = "./recharge_log_pgy_".$date.".log";
		$notify_data = $_POST;
		
		$data = "AppId:".$notify_data['app_id']."\t Act:1\t ProductName:海贼币\t ConsumeStreamId:".$notify_data['order_id']."\t CooOrderSerial:".$notify_data['billno']."\t Uin:".$notify_data['roleid']."\t GoodsId:".$notify_data['amount']."\t GoodsInfo:".$notify_data['account']."\t GoodsCount:1\t OriginalMoney:".$notify_data['uuid']."\t OrderMoney:".$notify_data['uuid']."\t Note:".$notify_data['zone']."\t PayStatus:".$notify_data['status']."\t CreateTime:".date("Y-m-d H:i:s")."\t Sign:".$notify_data['sign']."\n";
		error_log($data,3,$file);
		
		if ($notify_data['status'] != 0){
			$data = "CooOrderSerial:".$notify_data['status']."\t status:".$notify_data['status']." error!\n";
			error_log($data,3,$file);
			die('fail');
		}
		$chkres = self::chk($notify_data);
		if($chkres) {
			//验证通过
			//--------业务处理----------
			$order_id = $notify_data['order_id'];
			$billno = $notify_data['billno'];
			$account = $notify_data['account'];
			$amount = $notify_data['amount'];
			$status = $notify_data['status'];
			$app_id = $notify_data['app_id'];
			$uuid = $notify_data['uuid'];
			$roleid = $notify_data['roleid'];
			$zone = $notify_data['zone'];
			
			$dbConfig = self::getDBConfigByServerId($zone);
			$javaConfig = self::getJavaConfigByServerId($zone);
			$id = self::getIdByAmount($amount);
			
			if (empty($dbConfig) || empty($javaConfig) || $id == 0){
				$data = "CooOrderSerial:".$order_id."\t server config:$zone error!\n";
				error_log($data,3,$file);
				die('fail');
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
				//$OPRecharge->set_Platform('PP');
					
				$data = $OPRecharge->SerializeToString();
					
				$length = strlen($data);
				var_dump($length);
				$format = "nNNA".$length;
				$t = pack($format,$a,$opcode,$length,$data);
				socket_write($socket, $t);
			}
			
			echo "success";
		}else{
			echo "fail";
		}

	}
	
	public function chk($notify_data)
	{
		$privatedata = $notify_data['sign'];
		error_log(date("Y-m-d h:i:s")." ".serialize($privatedata)."\r\n",3,'rsa.log');
		
		$privatebackdata = base64_decode($privatedata);
		error_log(date("Y-m-d h:i:s")."base64_decode ".serialize($privatebackdata)."\r\n",3,'rsa.log');
		$MyRsa = new MyRsa;
		$data = $MyRsa->publickey_decodeing($privatebackdata, MyRsa::public_key);
		error_log(date("Y-m-d h:i:s")."publickey_decodeing ".$data."\r\n",3,'rsa.log');
		
		$rs = json_decode($data,true);
		error_log(date("Y-m-d h:i:s")."rs ".serialize($rs)."\r\n",3,'rsa.log');
		if(empty($rs)||empty($notify_data))return false;
		if($rs["billno"] == $notify_data['billno']&&$rs["amount"] == $notify_data['amount']&&$rs["status"] == $notify_data['status']) {
			return true; 
		}else{
			return false;
		}
	}

	public static function getDBConfigByServerId($serverId){
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
			case 5:
				$configArr = array('host'=>'10.66.1.122:1051','username'=>'root','passwd'=>'youai875','dbName'=>'dzm');
				break;
			case 6:
				$configArr = array('host'=>'10.66.1.122:1052','username'=>'root','passwd'=>'youai6453','dbName'=>'dzm');
				break;
			case 7:
				$configArr = array('host'=>'10.66.1.122:1053','username'=>'root','passwd'=>'youai!@#','dbName'=>'dzm');
				break;
			case 8:
				$configArr = array('host'=>'10.66.1.122:1054','username'=>'root','passwd'=>'youai@#$','dbName'=>'dzm');
				break;
			case 9:
				$configArr = array('host'=>'10.66.1.122:1055','username'=>'root','passwd'=>'youai#$%','dbName'=>'dzm');
				break;
			case 10:
				$configArr = array('host'=>'10.66.1.122:1059','username'=>'root','passwd'=>'s3kslf35asfl','dbName'=>'dzm');
				break;
			case 11:
				$configArr = array('host'=>'10.66.1.122:1058','username'=>'root','passwd'=>'lk4nbl57xg','dbName'=>'dzm');
				break;
			case 12:
				$configArr = array('host'=>'10.66.1.122:1057','username'=>'root','passwd'=>'vl45a7ro1t9','dbName'=>'dzm');
				break;
			case 13:
				$configArr = array('host'=>'10.66.1.122:1074','username'=>'root','passwd'=>'ja3bv9s4lfga','dbName'=>'dzm');
				break;
			case 14:
				$configArr = array('host'=>'10.66.1.122:1073','username'=>'root','passwd'=>'hg48x01k2rq','dbName'=>'dzm');
				break;
			case 15:
				$configArr = array('host'=>'10.66.1.122:1072','username'=>'root','passwd'=>'qoe43b93m6','dbName'=>'dzm');
				break;
			case 16:
				$configArr = array('host'=>'10.66.1.122:1085','username'=>'root','passwd'=>'jh8sy5hg1l1z','dbName'=>'dzm');
				break;
			case 17:
				$configArr = array('host'=>'10.66.1.122:1084','username'=>'root','passwd'=>'al9kr4b27yyv','dbName'=>'dzm');
				break;
			case 18:
				$configArr = array('host'=>'10.66.1.122:1083','username'=>'root','passwd'=>'ah4f67bdec9','dbName'=>'dzm');
				break;
			case 19:
				$configArr = array('host'=>'10.66.1.122:1082','username'=>'root','passwd'=>'jw34g9ix54fk','dbName'=>'dzm');
				break;
			case 20:
				$configArr = array('host'=>'10.66.1.122:1081','username'=>'root','passwd'=>'A7H4br6mXo','dbName'=>'dzm');
				break;
		}
		return $configArr;
	}
	
	public static function getJavaConfigByServerId($serverId){
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
			case 5:
				$configArr = array('host'=>'10.207.136.72','port'=>25524);
				break;
			case 6:
				$configArr = array('host'=>'10.207.136.103','port'=>25524);
				break;
			case 7:
				$configArr = array('host'=>'10.207.136.16','port'=>25524);
				break;
			case 8:
				$configArr = array('host'=>'10.207.136.90','port'=>25524);
				break;
			case 9:
				$configArr = array('host'=>'10.207.255.10','port'=>25524);
				break;
			case 10:
				$configArr = array('host'=>'10.207.141.72','port'=>25524);
				break;
			case 11:
				$configArr = array('host'=>'10.207.141.64','port'=>25524);
				break;
			case 12:
				$configArr = array('host'=>'10.207.142.72','port'=>25524);
				break;
			case 13:
				$configArr = array('host'=>'10.207.144.243','port'=>25524);
				break;
			case 14:
				$configArr = array('host'=>'10.207.145.14','port'=>25524);
				break;
			case 15:
				$configArr = array('host'=>'10.207.142.73','port'=>25524);
				break;
			case 16:
				$configArr = array('host'=>'10.207.137.184','port'=>25524);
				break;
			case 17:
				$configArr = array('host'=>'10.207.146.117','port'=>25524);
				break;
			case 18:
				$configArr = array('host'=>'10.207.145.214','port'=>25524);
				break;
			case 19:
				$configArr = array('host'=>'10.207.145.147','port'=>25524);
				break;
			case 20:
				$configArr = array('host'=>'10.207.143.203','port'=>25524);
				break;
		}
		return $configArr;
	}
	
	public static function getIdByAmount($amount){
		$id = 0;
		switch ($amount){
			case 10:
				$id = 1;
				break;
			case 30:
				$id = 2;
				break;
			case 50:
				$id = 3;
				break;
			case 100:
				$id =4;
				break;
			case 200:
				$id = 5;
				break;
			case 500:
				$id = 6;
				break;
			case 1000:
				$id = 7;
				break;
			case 2000:
				$id = 8;
				break;
			case 5000:
				$id = 9;
				break;
		}
		return $id;
	}
}
?>

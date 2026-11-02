<?php
/**
 * 这个是调用Sdk里查询支付购买结果的DEMO
 * 
 */
require_once './sdk/Sdk.php';

$sdk = new Sdk();

//商户订单号

$CooOrderSerial = $_GET['CooOrderSerial'];

$Res = $sdk->query_pay_result($CooOrderSerial);

print_r($Res);

?>
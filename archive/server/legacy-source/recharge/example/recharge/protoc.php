<?php
require_once('../../parser/pb_parser.php');
$test = new PBParser();
$test->parse('./recharge.proto');

var_dump("File build success!");
?>
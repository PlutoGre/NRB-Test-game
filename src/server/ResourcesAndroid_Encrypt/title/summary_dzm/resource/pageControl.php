<?php
include_once('Control.php');

$id = $_GET['param'];
$type = $_GET['type'];
$data = $_GET['data'];

include_once('../dataBase/Connection.php');
include_once('../dataBase/dbConfig.php');
$content = array();
$content = Connection::$type($data);
include_once("$type.php");

<?php
class UserInfo{
	public static function run(){
		$fileName = 'UserInfo.log';
		$today = date('Ymd');
		$fileInputDir = '/home/logs';
		$fileInput = "$fileInputDir/$fileName";

		$fileOutputDir = "/home/logs/$today";
		$fileOutput = "$fileOutputDir/$fileName";
		
		$mkdirCmd = "mkdir $fileOutputDir";
		exec($mkdirCmd);

		$readFileCmd = "cat $fileInput | awk '{print $1,$2,$5,$6}'";
		$content = array();
		exec($readFileCmd,$content);

		$content2 = "hello \n world \n";
		foreach ($content as $line){

			$contentTotal .= $line."\n";
		}


		file_put_contents($fileOutput,$contentTotal);
	}
	public static function getData(){
		$fileName = 'UserInfo.log';
		$today = date('Ymd');
		$fileOutputDir = "/home/logs/$today";
		$fileOutput = "$fileOutputDir/$fileName";
		
		$content = array();
		$readFileCmd = "cat $fileOutput | awk '{print $1,$2,$3,$4}'";
		exec($readFileCmd , $content);
		return $content;
	}
}

<?php      

//设置PHPExcel类库的include path      
set_include_path('.'. PATH_SEPARATOR . 'i:\Zeal\PHP_LIBS' . PATH_SEPARATOR . get_include_path());      
     
/** 
* 以下是使用示例，对于以 //// 开头的行是不同的可选方式，请根据实际需要 
* 打开对应行的注释。 
* 如果使用 Excel5 ，输出的内容应该是GBK编码。 
*/     
require_once 'Classes/PHPExcel.php';
include 'Classes/PHPExcel/IOFactory.php';      
     
// uncomment      
////require_once 'PHPExcel/Writer/Excel5.php';       // 用于其他低版本xls      
// or      
////require_once 'PHPExcel/Writer/Excel2007.php'; // 用于 excel-2007 格式      
     
// 创建一个处理对象实例

//$objExcel = new PHPExcel();      
     
// 创建文件格式写入对象实例, uncomment      
////$objWriter = new PHPExcel_Writer_Excel5($objExcel);       // 用于其他版本格式      
// or      
////$objWriter = new PHPExcel_Writer_Excel2007($objExcel); // 用于 2007 格式      
//$objWriter->setOffice2003Compatibility(true);   

// 设置文档属性
//$tpl_mark=iconv("utf-8","gb2312",$tpl_mark);//将utf-8转换为gb2312
//$yourStr = mb_convert_encoding("gb2312", "UTF-8", $yourStr);
//echo date('H:i:s') . " Set properties\n";
$objPHPExcel = new PHPExcel();
$objPHPExcel->getProperties()->setCreator("Maarten Balliauw");
$objPHPExcel->getProperties()->setLastModifiedBy("Maarten Balliauw");
$objPHPExcel->getProperties()->setTitle("Office 2007 XLSX Test Document");
$objPHPExcel->getProperties()->setSubject("Office 2007 XLSX Test Document");
$objPHPExcel->getProperties()->setDescription("Test document for Office 2007 XLSX, generated using PHP classes.");
$objPHPExcel->getProperties()->setKeywords("office 2007 openxml php");
$objPHPExcel->getProperties()->setCategory("Test result file");


// 创建一个表，相当于销售数据
echo date('H:i:s') . " Add some data\n";
$objPHPExcel->setActiveSheetIndex(0);
		$link=mysql_connect(DBConfig::$host,DBConfig::$user,DBConfig::$passwd);
		mysql_set_charset('utf8',$link);
		$db_selected=mysql_select_db(DBConfig::$db_name,$link);
$sql="select * from player";
$rs=mysql_query($sql);
$i=0;
$k1="姓名";
$k2="性别";
$k3="年龄";
$k4="电话";
$k5="地址";
$k6="email";
$k7="QQ";
$k8="科室";
$k9="说明";
$k10="预约时间";
$k11="挂号时间";

/*-----------转码-----------*/
$k1=iconv("gb2312","utf-8",$k1);
$k2=iconv("gb2312","utf-8",$k2);
$k3=iconv("gb2312","utf-8",$k3);
$k4=iconv("gb2312","utf-8",$k4);
$k5=iconv("gb2312","utf-8",$k5);
$k6=iconv("gb2312","utf-8",$k6);
$k7=iconv("gb2312","utf-8",$k7);
$k8=iconv("gb2312","utf-8",$k8);
$k9=iconv("gb2312","utf-8",$k9);
$k10=iconv("gb2312","utf-8",$k10);
$k11=iconv("gb2312","utf-8",$k11);
/*---------------------栏目名称-----------------------*/
$objPHPExcel->getActiveSheet()->setCellValue('a1', "$k1");
$objPHPExcel->getActiveSheet()->setCellValue('b1', "$k2");
$objPHPExcel->getActiveSheet()->setCellValue('c1', "$k3");
$objPHPExcel->getActiveSheet()->setCellValue('d1', "$k4");
$objPHPExcel->getActiveSheet()->setCellValue('e1', "$k5");
$objPHPExcel->getActiveSheet()->setCellValue('f1', "$k6");
$objPHPExcel->getActiveSheet()->setCellValue('g1', "$k7");
$objPHPExcel->getActiveSheet()->setCellValue('h1', "$k8");
$objPHPExcel->getActiveSheet()->setCellValue('i1', "$k9");
$objPHPExcel->getActiveSheet()->setCellValue('j1', "$k10");
$objPHPExcel->getActiveSheet()->setCellValue('k1', "$k11");
while($arr=mysql_fetch_array($rs))
{
/*-----------------读取数据--------------------*/
$booking_name=$arr[id];
$booking_sex=$arr[puid];
$booking_age=$arr[password];
$booking_tel=$arr[name];
$booking_address=$arr[booking_address];
$booking_email=$arr[booking_email];
$booking_qq=$arr[booking_qq];
$booking_ke=$arr[booking_ke];
$booking_content=$arr[booking_content];
$booking_time=$arr[booking_time];
$booking_date=$arr[booking_date];
/*-----------转码-----------*/

$booking_name=iconv("gb2312","utf-8",$booking_name);
$booking_sex=iconv("gb2312","utf-8",$booking_sex);
$booking_age=iconv("gb2312","utf-8",$booking_age);
$booking_tel=iconv("gb2312","utf-8",$booking_tel);
$booking_address=iconv("gb2312","utf-8",$booking_address);
$booking_email=iconv("gb2312","utf-8",$booking_email);
$booking_qq=iconv("gb2312","utf-8",$booking_qq);
$booking_ke=iconv("gb2312","utf-8",$booking_ke);
$booking_content=iconv("gb2312","utf-8",$booking_content);
$booking_time=iconv("gb2312","utf-8",$booking_time);
$booking_date=iconv("gb2312","utf-8",$booking_date);
$u1=$i+2;


/*----------写入内容-------------*/
$booking_date=date("Y-m-d",$booking_date);
$objPHPExcel->getActiveSheet()->setCellValue('a'.$u1, "$booking_name");
$objPHPExcel->getActiveSheet()->setCellValue('b'.$u1, "$booking_sex");
$objPHPExcel->getActiveSheet()->setCellValue('c'.$u1, "$booking_age");
$objPHPExcel->getActiveSheet()->setCellValue('d'.$u1, "<$booking_tel>");
$objPHPExcel->getActiveSheet()->setCellValue('e'.$u1, "$booking_address");
$objPHPExcel->getActiveSheet()->setCellValue('f'.$u1, "$booking_email");
$objPHPExcel->getActiveSheet()->setCellValue('g'.$u1, "<$booking_qq>");
$objPHPExcel->getActiveSheet()->setCellValue('h'.$u1, "$booking_ke");
$objPHPExcel->getActiveSheet()->setCellValue('i'.$u1, "$booking_content");
$objPHPExcel->getActiveSheet()->setCellValue('j'.$u1, "$booking_time");
$objPHPExcel->getActiveSheet()->setCellValue('k'.$u1, "$booking_date");
$i++;
}


// 高置列的宽度
//echo date('H:i:s') . " Set column widths\n";
$objPHPExcel->getActiveSheet()->getColumnDimension('A')->setWidth(30);
$objPHPExcel->getActiveSheet()->getColumnDimension('B')->setWidth(12);


// 添加条件格式

// 设置字体
//echo date('H:i:s') . " Set fonts\n";
$objPHPExcel->getActiveSheet()->getStyle('A1')->getFont()->setBold(true);
$objPHPExcel->getActiveSheet()->getStyle('B1')->getFont()->setBold(true);
$objPHPExcel->getActiveSheet()->getStyle('A7')->getFont()->setBold(true);
$objPHPExcel->getActiveSheet()->getStyle('B7')->getFont()->setBold(true);


// 设置页眉和页脚。如果没有不同的标题奇数/即使是使用单头假定.
//echo date('H:i:s') . " Set header/footer\n";
$objPHPExcel->getActiveSheet()->getHeaderFooter()->setOddHeader('&L&BPersonal cash register&RPrinted on &D');
$objPHPExcel->getActiveSheet()->getHeaderFooter()->setOddFooter('&L&B' . $objPHPExcel->getProperties()->getTitle() . '&RPage &P of &N');


// 设置页方向和规模
//echo date('H:i:s') . " Set page orientation and size\n";
$objPHPExcel->getActiveSheet()->getPageSetup()->setOrientation(PHPExcel_Worksheet_PageSetup::ORIENTATION_PORTRAIT);
$objPHPExcel->getActiveSheet()->getPageSetup()->setPaperSize(PHPExcel_Worksheet_PageSetup::PAPERSIZE_A4);


// 重命名表
//echo date('H:i:s') . " Rename sheet\n";
$objPHPExcel->getActiveSheet()->setTitle('Invoice');


// 设置工作表的第一项指标资产负债表，因此Excel打开，这是第一个资产负债表
$objPHPExcel->setActiveSheetIndex(0);

  
// 保存Excel 2007文件
//echo date('H:i:s') . " Write to Excel2007 format\n";
$objWriter = PHPExcel_IOFactory::createWriter($objPHPExcel, 'Excel5');
$objWriter->save(str_replace('.php', '.xls', __FILE__));


// 回声记忆高峰使用
//echo date('H:i:s') . " Peak memory usage: " . (memory_get_peak_usage(true) / 1024 / 1024) . " MB\r\n";

// 回声做
//echo date('H:i:s') . " Done writing file.\r\n";
echo "<script>alert('挂号成功')</script>";

?>

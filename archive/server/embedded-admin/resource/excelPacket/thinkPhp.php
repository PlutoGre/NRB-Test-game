<?php
 header("charset=utf-8");

                   ini_set ('memory_limit', '128M');

                   $type=$_REQUEST['type'];

                   $search_type=$_REQUEST['search_type'];

                   $keyword=$_REQUEST['keyword'];

                   $sql_select="SELECT * FROM pp_view_planorder_prouduct WHERE type='".$type."' AND ".$search_type." LIKE '%".$keyword."%'";

                   $Model = new Model();

                   $list=$Model->query($sql_select);

                   //echo($sql_select);

 

                   error_reporting(E_ALL);

                   import("phpexcel.PHPExcel",dirname(__FILE__),".php");

                   $objPHPExcel = new PHPExcel();

                   // Set document properties

                   $objPHPExcel->getProperties()->setCreator("aigindustries")//创建者

                   ->setLastModifiedBy("aigindustries")//最后修改者

                   ->setTitle("aigindustries")//标题

                   ->setSubject("aigindustries")//主题

                   ->setDescription("aigindustries")//备注

                   ->setKeywords("aigindustries")//关键字

                   ->setCategory("aigindustries");//分类

                   // Add some data

                   $objPHPExcel->setActiveSheetIndex(0)

                   //设置表的名称标题

                   ->setCellValue('A1',"季度")

                   ->setCellValue('B1',"部门")

                   ->setCellValue('C1',"姓名")

                   ->setCellValue('D1',"备货性质")

                   ->setCellValue('E1',"组别")

                   ->setCellValue('F1',"类型")

                   ->setCellValue('G1',"品名")

                   ->setCellValue('H1',"规格")

                   ->setCellValue('I1',"材质")

                   ->setCellValue('J1',"包装")

                   ->setCellValue('K1',"季度第一个月订货量")

                   ->setCellValue('L1',"季度第二个月订货量")

                   ->setCellValue('M1',"季度第三个月订货量");

                   foreach($list as $k => $v)

                   {

                            $num=$k+2;

                            $objPHPExcel->setActiveSheetIndex(0)

                            //Excel的第A列，uid是你查出数组的键值，下面以此类推

                            ->setCellValue('A'.$num, $v['quarter'])

                            ->setCellValue('B'.$num, $v['userdept'])

                            ->setCellValue('C'.$num, $v['username'])

                            ->setCellValue('D'.$num, $v['prdfeature'])

                            ->setCellValue('E'.$num, $v['prdgroup'])

                            ->setCellValue('F'.$num, $v['prdkind'])

                            ->setCellValue('G'.$num, $v['prdname'])

                            ->setCellValue('H'.$num, $v['standard'])

                            ->setCellValue('I'.$num, $v['resource'])

                            ->setCellValue('J'.$num, $v['pack'])

                            ->setCellValue('K'.$num, $v['value1'])

                            ->setCellValue('L'.$num, $v['value2'])

                            ->setCellValue('M'.$num, $v['value3']);

                   }

                   // Rename worksheet

                   $objPHPExcel->getActiveSheet()->setTitle($type.'-该季度所有订货信息');

                   // Set active sheet index to the first sheet, so Excel opens this as the first sheet

                   $objPHPExcel->setActiveSheetIndex(0);

                   // Redirect output to a client’s web browser (Excel5)

                   header('Content-Type: application/vnd.ms-excel;charset=utf-8');

                   header('Content-Disposition: attachment;filename="'.$type.'-该季度所有订货信息.xls"');

                   header('Cache-Control: max-age=0');

                   $objWriter = PHPExcel_IOFactory::createWriter($objPHPExcel, 'Excel5');

                   $objWriter->save('php://output');

                   exit;
?>

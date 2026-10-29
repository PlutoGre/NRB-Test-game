<?php
include 'Classes/PHPExcel.php';
include 'Classes/PHPExcel/Writer/Excel2007.php';

class MyExcel {

    public static function makeExcel($title, $data, $excle_name) {
echo "in MyExcel\n";

	$cmd = "mkdir excel";
	exec($cmd);
	$cmd = "mkdir excel/" . date('Y-m-d');
	exec($cmd);

        $exclefile = 'excel/' . date('Y-m-d') . '/' . md5($excle_name) . '.xlsx';

        $Excel = new PHPExcel();
        $Excel->setActiveSheetIndex(0);
        $Excel->getSheet()->setTitle($title);

        $cell_one = $data[0];
        $j = 0;
        foreach ($cell_one as $k => $v) {
            $Excel->getSheet()->setCellValue(self::getCharByNunber($j) . '1', $k);
            $j++;
        }

echo "1\n";
        $x = 2;
        foreach ($data as $value) {
            $y = 0;
            foreach ($value as $k => $v) {
                $Excel->getSheet()->setCellValue(self::getCharByNunber($y) . $x, $v);
                $y++;
            }
            $x++;
        }
echo "2\n";

        $objwriter = new PHPExcel_Writer_Excel2007($Excel);
echo "3\n";
        $objwriter->save($exclefile);
echo "4\n";
//        TMDebugUtils::debugLog('make ' . $exclefile);
echo "out MyExcel\n";
        return $exclefile;
    }

    protected static function getCharByNunber($num) {
        $num = intval($num);
        $arr = array('A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',);
        return $arr[$num];
    }

}

//导出用户基本信息
function exportUserBaseInfo() {
    $excelTitle = '用户基本信息';
    $excelFileName = 'userbase';
    $data = array();
    // $rs = ....  这里rs是从db读取的数据
    for ($i = 0; $i < count($rs); $i++) {         $data[$i]['姓名'] = $rs[$i]->name;
        $data[$i]['年龄'] = intval($rs[$i]->age);
        $data[$i]['日期'] = $rs[$i]->date;
    }
    MyExcel::makeExcel($excelTitle, $data, $excelFileName);
echo "end\n";
}
exportUserBaseInfo();
/*
//导出新闻资讯信息
function exportUserBaseInfo() {
    $excelTitle = '新闻资讯信息';
    $excelFileName = 'news';
    $data = array();
    // $rs = ....  这里rs是从db读取的数据
    for ($i = 0; $i < count($rs); $i++) {         $data[$i]['标题'] = $rs[$i]->title;
        $data[$i]['作者'] = $rs[$i]->author;
        $data[$i]['日期'] = $rs[$i]->date;
        $data[$i]['浏览数'] = floatval($rs[$i]->view_count);
        $data[$i]['被评论数'] = floatval($rs[$i]->comment_count);
    }
    MyExcel::makeExcel($excelTitle, $data, $excelFileName);
}
*/
?>

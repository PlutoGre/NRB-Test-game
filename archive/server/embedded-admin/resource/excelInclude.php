<form id="sampleform" method="post" action="excelPacket/exportExcel.php">
<input type="hidden" id="" class="silvercoins" value='<?php echo json_encode($content); ?>' name="content"/>
<input type="hidden" id="" class="silvercoins" value="<?php echo $name; ?>" name="name"/>
<input type="hidden" id="" class="silvercoins" value="<?php echo $excelname; ?>" name="excelname"/>
<input type="submit" class="btnName btn btn-green big" value="导出excel表格"/>
</form>	


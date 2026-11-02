<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="utf-8" />
<title>Admin Template</title>
<meta name="description" content="Administry - Admin Template by www.865171.cn" />
<meta name="keywords" content="Admin,Template" />
<!-- Favicons --> 
<link rel="shortcut icon" type="image/png" HREF="img/favicons/favicon.png"/>
<link rel="icon" type="image/png" HREF="img/favicons/favicon.png"/>
<link rel="apple-touch-icon" HREF="img/favicons/apple.png" />
<!-- Main Stylesheet --> 
<link rel="stylesheet" href="css/style.css" type="text/css" />
<!-- Your Custom Stylesheet --> 
<link rel="stylesheet" href="css/custom.css" type="text/css" />
<!--swfobject - needed only if you require <video> tag support for older browsers -->
<script type="text/javascript" SRC="js/swfobject.js"></script>
<!-- jQuery with plugins -->
<script type="text/javascript" SRC="js/jquery-1.4.2.min.js"></script>
<!-- Could be loaded remotely from Google CDN : <script type="text/javascript" src="http://ajax.googleapis.com/ajax/libs/jquery/1.4.2/jquery.min.js"></script> -->
<script type="text/javascript" SRC="js/jquery.ui.core.min.js"></script>
<script type="text/javascript" SRC="js/jquery.ui.widget.min.js"></script>
<script type="text/javascript" SRC="js/jquery.ui.tabs.min.js"></script>
<!-- jQuery tooltips -->
<script type="text/javascript" SRC="js/jquery.tipTip.min.js"></script>
<!-- Superfish navigation -->
<script type="text/javascript" SRC="js/jquery.superfish.min.js"></script>
<script type="text/javascript" SRC="js/jquery.supersubs.min.js"></script>
<!-- jQuery form validation -->
<script type="text/javascript" SRC="js/jquery.validate_pack.js"></script>
<!-- jQuery popup box -->
<script type="text/javascript" SRC="js/jquery.nyroModal.pack.js"></script>
<!-- jQuery data tables -->
<script type="text/javascript" SRC="js/jquery.dataTables.min.js"></script>
<script type="text/javascript" SRC="js/raphael-min.js"></script>
<script type="text/javascript" SRC="js/morris.js"></script>
<script type="text/javascript" SRC="js/prettify.js"></script>
<script type="text/javascript" SRC="js/example.js"></script>

<link rel="stylesheet" href="css/example.css">
<link rel="stylesheet" href="css/prettify.css">
<link rel="stylesheet" href="css/morris.css">


<script type="text/javascript">
$(document).ready(function(){
	
	/* setup navigation, content boxes, etc... */
	Administry.setup();
	
	/* datatable */
	//$('#example').dataTable();
	
	/* expandable rows */
	Administry.expandableRows();

	var callback = function(Result) {
		
	}

	$(".wrapper .btnUser").click(
			function(){
				var startDate = $("#startDate").val();
				var endDate = $("#endDate").val();
				
				$.get("pageControl.php",{"type":"onlineUserCount","data":"{\"startDate\":"+startDate+",\"endDate\":"+endDate+"}"}, callback);  
			}
	);
});

</script>
</head>
<body>
	<?php include('menu.php'); ?>
	<!-- Page title -->
	<div id="pagetitle">
		<div class="wrapper">
			<h1>Love</h1>
		</div>
	</div>
	<!-- End of Page title -->

	
	<!-- Page content -->
	<div id="page">
		<!-- Wrapper -->
		<div class="wrapper">
				<!-- Left column/section -->
				<section class="column width6 first">					
					
					<h3>同时在线统计</h3>
					<?php $name="统计日期 最高在线用户数 最低在线用户数 平均用户在线数" ?>
					<?php $excelname="同时在线统计" ?>
					<?php include_once('./excelInclude.php');?>
					<p>
						<label class="required" >起始时间:</label>
						<input type="text" id="startDate" class="startDate small" value="<?php echo $content['startDate'];?>" name="startDate"/>
						<label class="required" >结束时间:</label>
						<input type="text" id="endDate" class="endDate small" value="<?php echo $content['endDate'];?>" name="startDate">
						<input type="button" class="btnUser btn btn-green big" value="search"/>
					</p>
					<table class="display stylized" id="example">
						<thead>
							<tr>
								<th>
								统计日期
								</th>
								<th>
								最高在线用户数
								</th>
								<th>
								最低在线用户数
								</th>
								<th>
								平均用户在线数	
								</th>
							</tr>
						</thead>
						<tbody>
					<?php foreach ( $content['date'] as $date => $data ) : ?>
							<tr class="gradeA">
								<td>
								<?php echo $date; ?>
								</td>
								<td>
								<?php echo $data['max']; ?>
								</td>
								<td>
								<?php echo $data['min']; ?>
								</td>
								<td>
								<?php echo round($data['total'] / $data['count']); ?>
								</td>
							</tr>
							<?php endforeach; ?>
						</tbody>
					</table>
					<p>实时在线统计</p>
					<div id="graph"></div>
					
					<script type="text/javascript">
					//var timestamp_data =[{"period":1369906558000,"count":" 0"},{"period":1369906858000,"count":" 1"},{"period":1369907158000,"count":" 0"},{"period":1369907458000,"count":" 2"},{"period":1369907758000,"count":" 3"},{"period":1369908058000,"count":" 0"},{"period":1369908358000,"count":" 3"},{"period":1369908658000,"count":" 4"},{"period":1369908958000,"count":" 3"},{"period":1369909258000,"count":" 2"},{"period":1369909558000,"count":" 1"},{"period":1369909858000,"count":" 3"},{"period":1369910158000,"count":" 3"},{"period":1369910458000,"count":" 4"},{"period":1369910758000,"count":" 4"},{"period":1369911058000,"count":" 5"},{"period":1369911358000,"count":" 4"},{"period":1369911658000,"count":" 2"},{"period":1369911958000,"count":" 3"},{"period":1369912258000,"count":" 3"},{"period":1369912558000,"count":" 1"},{"period":1369912858000,"count":" 1"},{"period":1369913158000,"count":" 3"},{"period":1369913458000,"count":" 2"},{"period":1369913758000,"count":" 1"},{"period":1369914058000,"count":" 1"},{"period":1369914358000,"count":" 1"},{"period":1369914658000,"count":" 1"},{"period":1369914958000,"count":" 1"},{"period":1369915258000,"count":" 1"},{"period":1369915558000,"count":" 1"}];
					var timestamp_data = <?php echo json_encode($content['info']);?>;
					/*
						var timestamp_data = [
						  {"period": 1349046000000, "licensed": 3407, "sorned": 660},
						  {"period": 1313103600000, "licensed": 3351, "sorned": 629},
						  {"period": 1299110400000, "licensed": 3269, "sorned": 618},
						  {"period": 1281222000000, "licensed": 3246, "sorned": 661},
						  {"period": 1273446000000, "licensed": 3257, "sorned": 667},
						  {"period": 1268524800000, "licensed": 3248, "sorned": 627},
						  {"period": 1263081600000, "licensed": 3171, "sorned": 660},
						  {"period": 1260403200000, "licensed": 3171, "sorned": 676},
						  {"period": 1254870000000, "licensed": 3201, "sorned": 656},
						  {"period": 1253833200000, "licensed": 3215, "sorned": 622}
						];
					*/
						Morris.Line({
						  element: 'graph',
						  data: timestamp_data,
						  xkey: 'period',
						  ykeys: ['count'],
						  labels: ['Online'],
						  dateFormat: function (x) { return new Date(x).toDateString(); }
						});
					</script>
					
					<div class="clear">&nbsp;</div>
				</section>
				<!-- End of Left column/section -->				
		</div>
		<!-- End of Wrapper -->
	</div>
	<!-- End of Page content -->
	
	<!-- Page footer -->
	<?php include('footer.php'); ?>
	<!-- End of Page footer -->

<!-- User interface javascript load -->
<script type="text/javascript" SRC="js/administry.js"></script>
</body>
</html>
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
<!-- Colour Schemes
Default colour scheme is blue. Uncomment prefered stylesheet to use it.
<link rel="stylesheet" href="css/brown.css" type="text/css" media="screen" />  
<link rel="stylesheet" href="css/gray.css" type="text/css" media="screen" />  
<link rel="stylesheet" href="css/green.css" type="text/css" media="screen" />
<link rel="stylesheet" href="css/pink.css" type="text/css" media="screen" />  
<link rel="stylesheet" href="css/red.css" type="text/css" media="screen" />
-->
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
<!-- Internet Explorer Fixes --> 
<!--[if IE]>
<link rel="stylesheet" type="text/css" media="all" href="css/ie.css"/>
<script src="js/html5.js"></script>
<![endif]-->
<!--Upgrade MSIE5.5-7 to be compatible with MSIE8: http://ie7-js.googlecode.com/svn/version/2.1(beta3)/IE8.js -->
<!--[if lt IE 8]>
<script src="js/IE8.js"></script>
<![endif]-->
<script type="text/javascript">
$(document).ready(function(){
	
	/* setup navigation, content boxes, etc... */
	Administry.setup();
	
	/* datatable */
	$('#example').dataTable();
	
	/* expandable rows */
	Administry.expandableRows();
});

</script>
</head>
<body>
	<?php include('menu.php'); ?>
	<!-- Page title -->
	<div id="pagetitle">
		<div class="wrapper">
			<h1>Love</h1>
			<!-- Quick search box -->
			<form action="pageControl.php" method="get">
				<input class="" type="text" id="" name="data" value="<?php echo $data ?>"/>
				<input class="" type="hidden" id="" name="type" value="playerLeft" />
				
			</form>
		</div>
	</div>
	<!-- End of Page title -->

	
	<!-- Page content -->
	<div id="page">
		<!-- Wrapper -->
		<div class="wrapper">
				<!-- Left column/section -->
				<section class="column width6 first">					
					
					<h3>留存率统计</h3>
					<?php $name="日期 注册用户数量 活跃用户数量 登出人数 1日留存 2日留存 3日留存 4日留存 5日留存 6日留存 7日留存 14日留存 30日留存" ?>
					<?php $excelname="留存率统计" ?>
					<?php include_once('./excelInclude.php');?>

					
					<table class="display stylized" id="example">
						<thead>
							<tr>
								<th>
						日期
								</th>
								<th>
						注册用户数量
								</th>
								<th>
						活跃用户数量
								</th>
								<th>
						登出人数	
								</th>
								<th>
						1日留存
								</th>
								<th>
						2日留存
								</th>
								<th>
						3日留存
								</th>
								<th>
						4日留存
								</th>
								<th>
						5日留存
								</th>
								<th>
						6日留存
								</th>
								<th>
						7日留存
								</th>
<!--
								<th>
						8日留存
								</th>
								<th>
						9日留存
								</th>
								<th>
						10日留存
								</th>
								<th>
						11日留存
								</th>
								<th>
						12日留存
								</th>
								<th>
						13日留存
								</th>
-->
								<th>
						14日留存
								</th>
								<th>
						30日留存
								</th>
							</tr>
						</thead>
						<tbody>
<?php $userNum = 0 ?>
<?php foreach ( $content as $line => $data ) : ?>
<?php list($date,$registnum,$activenum,$leftnum1,$leftnum2,$leftnum3,$leftnum4,$leftnum5,$leftnum6,$leftnum7,$leftnum14,$leftnum30,$userNumTemp,$logoutNum) = explode(' ',$data); ?>
<?php $userNum = $userNumTemp?>
							<tr class="gradeA">
								<td>
								<?php echo $date; ?>
								</td>
								<td>
								<?php echo $registnum; ?>
								</td>
								<td>
								<?php echo $activenum; ?>
								</td>
								<td>
								<?php echo $logoutNum; ?>
								</td>
								<td>
								<?php if(empty($leftnum1)) {echo "&nbsp";} else { echo number_format(trim($leftnum1),2) . "%";} ?>
								</td>
								<td>
								<?php if(empty($leftnum2)) {echo "&nbsp";} else { echo number_format(trim($leftnum2),2) . "%";} ?>
								</td>
								<td>
								<?php if(empty($leftnum3)) {echo "&nbsp";} else { echo number_format(trim($leftnum3),2) . "%";} ?>
								</td>
								<td>
								<?php if(empty($leftnum4)) {echo "&nbsp";} else { echo number_format(trim($leftnum4),2) . "%";} ?>
								</td>
								<td>
								<?php if(empty($leftnum5)) {echo "&nbsp";} else { echo number_format(trim($leftnum5),2) . "%";} ?>
								</td>
								<td>
								<?php if(empty($leftnum6)) {echo "&nbsp";} else { echo number_format(trim($leftnum6),2) . "%";} ?>
								</td>
								<td>
								<?php if(empty($leftnum7)) {echo "&nbsp";} else { echo number_format(trim($leftnum7),2) . "%";} ?>
								</td>
<!--
								<td>
								<?php if(empty($leftnum8)) {echo "&nbsp";} else { echo number_format(trim($leftnum8),2) . "%";} ?>
								</td>
								<td>
								<?php if(empty($leftnum9)) {echo "&nbsp";} else { echo number_format(trim($leftnum9),2) . "%";} ?>
								</td>
								<td>
								<?php if(empty($leftnum10)) {echo "&nbsp";} else { echo number_format(trim($leftnum10),2) . "%";} ?>
								</td>
								<td>
								<?php if(empty($leftnum11)) {echo "&nbsp";} else { echo number_format(trim($leftnum11),2) . "%";} ?>
								</td>
								<td>
								<?php if(empty($leftnum12)) {echo "&nbsp";} else { echo number_format(trim($leftnum12),2) . "%";} ?>
								</td>
								<td>
								<?php if(empty($leftnum13)) {echo "&nbsp";} else { echo number_format(trim($leftnum13),2) . "%";} ?>
								</td>
-->
								<td>
								<?php if(empty($leftnum14)) {echo "&nbsp";} else { echo number_format(trim($leftnum14),2) . "%";} ?>
								</td>
								<td>
								<?php if(empty($leftnum30)) {echo "&nbsp";} else { echo number_format(trim($leftnum30),2) . "%";} ?>
								</td>
							</tr>
<?php endforeach; ?>
					
						</tbody>
					</table>
					
					<div class="clear">&nbsp;</div>
					
				</section>
				<!-- End of Left column/section -->
				
				
				<!-- Right column/section -->
				<aside class="column width2">

					<div class="content-box">
						<header>
							<h3>Quick info</h3>
						</header>
						<section>
							<dl>
								<dt>总用户数：<?php echo $userNum; ?></dt>
							</dl>
						</section>
					</div>
				</aside>
				<!-- End of Right column/section -->
				
				
		</div>
		<!-- End of Wrapper -->
	</div>
	<!-- End of Page content -->
	
	<!-- Page footer -->
	<footer id="bottom">
		<div class="wrapper">
			<nav>
				<a href="#">Dashboard</a> &middot;
				<a href="#">Content</a> &middot;
				<a href="#">Reports</a> &middot;
				<a href="#">Users</a> &middot;
				<a href="#">Media</a> &middot;
				<a href="#">Events</a> &middot;
				<a href="#">Newsletter</a> &middot;
				<a href="#">Settings</a>
			</nav>
			<p>Copyright &copy; 2010</p>
		</div>
	</footer>
	<!-- End of Page footer -->
	
	<!-- Animated footer -->
	<footer id="animated">
		<ul>
			<li><a href="#">Dashboard</a></li>
			<li><a href="#">Content</a></li>
			<li><a href="#">Reports</a></li>
			<li><a href="#">Users</a></li>
			<li><a href="#">Media</a></li>
			<li><a href="#">Events</a></li>
			<li><a href="#">Newsletter</a></li>
			<li><a href="#">Settings</a></li>
		</ul>
	</footer>
	<!-- End of Animated footer -->
	
	<!-- Scroll to top link -->
	<a href="#" id="totop">^ scroll to top</a>

<!-- User interface javascript load -->
<script type="text/javascript" SRC="js/administry.js"></script>
</body>
</html>


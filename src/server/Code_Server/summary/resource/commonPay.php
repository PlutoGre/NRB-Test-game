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
				<input class="" type="text" id="q" name="q" />
				<input class="" type="hidden" id="q" name="type" value="searchUseGold" />
				
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
					
					<h3>付费基本数据统计</h3>
					
					<table class="display stylized" id="example">
						<thead>
							<tr>
								<th>
						日期	
								</th>
								<th>
						总金额	
								</th>
								<th>
						充值人数	
								</th>
								<th>
						充值次数	
								</th>
								<th>
						DAU	
								</th>
								<th>
						付费率	
								</th>
								<th>
						ARPPU	
								</th>
								<th>
						ARPU	
								</th>
							</tr>
						</thead>
						<tbody>
<?php foreach ( $content as $line => $data ) : ?>
<?php list($date,$amount,$player,$count,$activenum,$payCent,$ARPPU,$ARPU) = explode(' ',$data); ?>
							<tr class="gradeA">
								<td>
								<?php echo $date; ?>
								</td>
								<td>
								<?php echo $amount; ?>
								</td>
								<td>
								<?php echo $player; ?>
								</td>
								<td>
								<?php echo $count; ?>
								</td>
								<td>
								<?php echo $activenum; ?>
								</td>
								<td>
								<?php echo $payCent; ?>
								</td>
								<td>
								<?php echo $ARPPU; ?>
								</td>
								<td>
								<?php echo $ARPU; ?>
								</td>
							</tr>
<?php endforeach; ?>
						</tbody>
						<tfoot>
							<tr>
								<th>
						日期	
								</th>
								<th>
						总金额	
								</th>
								<th>
						充值人数	
								</th>
								<th>
						充值次数	
								</th>
								<th>
						DAU	
								</th>
								<th>
						付费率	
								</th>
								<th>
						ARPPU	
								</th>
								<th>
						ARPU	
								</th>
							</tr>
						</tfoot>
					</table>
					
					<div class="clear">&nbsp;</div>
					
				</section>
				<!-- End of Left column/section -->
				
		</div>
		<!-- End of Wrapper -->
	</div>
	<!-- End of Page content -->
	
<?php include('footer.php'); ?>
</body>
</html>



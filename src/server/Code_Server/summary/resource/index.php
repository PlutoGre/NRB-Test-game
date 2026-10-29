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
			<form action="" method="get"><input class="" type="text" id="q" name="q" /></form>
		</div>
	</div>
	<!-- End of Page title -->
	
	<!-- Page content -->
	<div id="page">
		<!-- Wrapper -->
		<div class="wrapper">
				<!-- Left column/section -->
				<section class="column width6 first">					
					
					<h3>有爱后台</h3>
					
					
					
				</section>
				<!-- End of Left column/section -->
				
		</div>
		<!-- End of Wrapper -->
	</div>
	<!-- End of Page content -->
	
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


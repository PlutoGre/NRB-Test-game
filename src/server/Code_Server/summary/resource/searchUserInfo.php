<?php include "security2.php"; ?>
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="utf-8" />
<title>Admin</title>
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
<!-- jQuery popup box -->
<script type="text/javascript" SRC="js/jquery.nyroModal.pack.js"></script>
<!-- jQuery form validation -->
<script type="text/javascript" SRC="js/jquery.validate_pack.js"></script>
<!-- Internet Explorer Fixes --> 
<!--[if IE]>
<link rel="stylesheet" type="text/css" media="all" href="css/ie.css"/>
<script src="js/html5.js"></script>
<![endif]-->
<!--Upgrade MSIE5.5-7 to be compatible with MSIE8: http://ie7-js.googlecode.com/svn/version/2.1(beta3)/IE8.js -->
<!--[if lt IE 8]>
<script src="js/IE8.js"></script>
<![endif]-->
<style type="text/css">  
input{ 
	font-size:11pt;  
	background-color:ffffe0;  
	border-style:none;  
	BORDER-TOP-STYLE: none; 
	BORDER-RIGHT-STYLE: none; 
	BORDER-LEFT-STYLE: none; 
	BORDER-BOTTOM-STYLE: solid;
}
</style>
<script type="text/javascript">
$(document).ready(function(){
	
	/* setup navigation, content boxes, etc... */
	Administry.setup();
	
	/* progress bar animations - setting initial values */
	Administry.progress("#progress1", 1, 6);
	var getDate = function(time){
		var date = new Date(time*1000);
		return date.toLocaleString();
	}
	
	var callback = function(Result) {
		var ResultJson = JSON.parse(Result);
		$(".wrapper .servertime").val(getDate(ResultJson['servertime']));
		$(".wrapper .name").val(ResultJson['name']);
		$(".wrapper .registertime").val(getDate(ResultJson['registertime']));
		$(".wrapper .lastlogin").val(getDate(ResultJson['lastlogin']));
		$(".wrapper .level").val(ResultJson['level']);
		$(".wrapper .silvercoins").val(ResultJson['silvercoins']);
		$(".wrapper .goldcoins").val(ResultJson['goldcoins']);
		alert("ok");
		
	}	
	
	$(".wrapper .btnUser").click(
		function(){
			var name = $(".name").val();
			$.get("searchUserInfoControl.php",{"name":name}, callback);  
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

					<h3>查询用户信息</h3>
					
					<form id="sampleform" method="post" action="updateUserInfoControl.php">

						<fieldset>

							<p>
								<label class="required" for="firstname">船长名:</label><br/>
								<input type="text" id="firstname" class="name half" value="" name="firstname"/>
								<input type="button" class="btnUser btn btn-green big" value="search"/>
							</p>
<!--
							<p>
								<label class="required" for="firstname">服务器时间:</label><br/>
								<input type="text" id="firstname" class="servertime half" value="" name="firstname"/>
							</p>
-->
							<p>
								<label class="required" for="firstname">注册时间:</label><br/>
								<input type="text" id="firstname" class="registertime half" value="" name="firstname"/>
							</p>
							<p>
								<label class="required" for="firstname">上次登录时间:</label><br/>
								<input type="text" id="firstname" class="lastlogin half" value="" name="firstname"/>
							</p>
							<p>
								<label class="required" for="firstname">等级:</label><br/>
								<input type="text" id="firstname" class="level half" value="" name="firstname"/>
							</p>
							<p>
								<label class="required" for="firstname">元宝:</label><br/>
								<input type="text" id="firstname" class="goldcoins half" value="" name="firstname"/>
							</p>
							<p>
								<label class="required" for="firstname">银币:</label><br/>
								<input type="text" id="firstname" class="silvercoins half" value="" name="firstname"/>
							</p>
							

						</fieldset>

					</form>
				
				</section>
				<!-- End of Left column/section -->
				
		</div>
		<!-- End of Wrapper -->
	</div>
	<!-- End of Page content -->

	<?php include('footer.php'); ?>
</body>
</html>







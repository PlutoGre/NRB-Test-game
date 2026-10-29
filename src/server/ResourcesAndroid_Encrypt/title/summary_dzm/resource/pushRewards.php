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
	var callback = function(Result) {

		alert("ok");
		
	}	
	
	$(".wrapper .btnUser").click(
		function(){
			var namesval = $(".wrapper .names").val();
			var rewardnameval = $(".wrapper .rewardname").val();
			var rewardcontentval = $(".wrapper .rewardcontent").val();
			var logintimeval = $(".wrapper .logintime").val();
			var levelStart = $(".wrapper .levelStart").val();
			var levelEnd = $(".wrapper .levelEnd").val();
			var rankStart = $(".wrapper .rankStart").val();
			var rankEnd = $(".wrapper .rankEnd").val();
			

			var arr = logintimeval.split("/");
 			var year = arr[0];
			var month = arr[1]-1;
 			var day = arr[2];
 			var timestamp = new Date(year,month,day,0,0).getTime()/1000; 

			$.get("pushRewardsControl.php",{"field":"compensate","rewardname":rewardnameval,"rewardcontent":rewardcontentval,"logintime":timestamp,"names":namesval,"levelStart":levelStart,"levelEnd":levelEnd,"rankStart":rankStart,"rankEnd":rankEnd}, callback);  
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

					<h3>发放礼包</h3>
					
					<form id="sampleform" method="post" action="pushRewards.php">

						<fieldset>

							<p>
								<label class="required" for="firstname">用户名字:</label><br/>
								<input type="text" id="firstname" class="names half" value="" name="firstname"/>
								<small>e.g. -1 or 海贼王的男人 or 海贼王的男人,海贼王的女人 可以发送全部用户，单个用户和部分用户，填写用户掌门名</small>
							</p>
							<p>
								<label class="required" for="firstname">名称:</label><br/>
								<input type="text" id="firstname" class="rewardname half" value="" name="firstname"/>
								<small>e.g. 女帝在给你发奖励喽</small>
							</p>
							<p>
								<label class="required" for="firstname">礼包:</label><br/>
								<input type="text" id="firstname" class="rewardcontent half" value="" name="firstname"/>
								<small>e.g. 11001:1001:500000,61001:3002001:2, 第一个字段是类型，第二个字段是具体奖励，第三个字段是数量</small>
							</p>
							<p>
								<label class="required" for="firstname">时间:</label><br/>
								<input type="text" id="firstname" class="logintime half" value="" name="firstname"/>
								<small>e.g. 2012/01/01 ,在此时间后登陆的用户可获得礼包，如果要全部发放，可以大量提前时间</small>
							</p>
							<p>
								<label class="required" for="firstname">等级区间:</label><br/>
								<input type="text" id="firstname" class="levelStart small" value="" name="firstname"/>
								-
								<input type="text" id="firstname" class="levelEnd small" value="" name="firstname">
								<small>e.g. 用户的等级区间</small>
							</p>
							<p>
								<label class="required" for="firstname">挑战名次区间:</label><br/>
								<input type="text" id="firstname" class="rankStart small" value="" name="firstname"/>
								-
								<input type="text" id="firstname" class="rankEnd small" value="" name="firstname">
								<small>e.g. 用户的排名区间</small>
							</p>
							<p>
								<input type="button" class="btnUser btn btn-green big" value="submit"/>
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







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
		$(".content-box .servertime").val(ResultJson['servertime']);
		$(".content-box .userid").val(ResultJson['userid']);
		$(".content-box .name").val(ResultJson['name']);
		$(".content-box .registertime").val(getDate(ResultJson['registertime']));
		$(".content-box .lastlogin").val(getDate(ResultJson['lastlogin']));
		$(".content-box .level").val(ResultJson['level']);
		$(".content-box .silvercoins").val(ResultJson['silvercoins']);
		$(".content-box .goldcoins").val(ResultJson['goldcoins']);
		alert("ok");
		
	}	

	$(".wrapper .btnName").click(
		function(){
			var val = 0;
			var val2 = 0;
			var name = $(".name").val();
			$.get("updateUserInfoControl.php",{"field":"gold","value":val,"name":name,"value2":val2}, callback);  
		}
	);
	
	$(".wrapper .btnCash").click(
		function(){
			var val = $(".wrapper .cash").val();
			var val2 = 0;
			var name = $(".name").val();
			$.get("updateUserInfoControl.php",{"field":"gold","value":val,"name":name,"value2":val2}, callback);  
		}
	);
	
	$(".wrapper .btnRecharge").click(
		function(){
			var val = $(".wrapper .recharge").val();
			var val2 = 0;
			var name = $(".name").val();
			$.get("updateUserInfoControl.php",{"field":"recharge","value":val,"name":name,"value2":val2}, callback);  
		}
	);
	
	$(".wrapper .btnSilver").click(
		function(){
			var val = $(".wrapper .silver").val();
			var val2 = 0;
			var name = $(".name").val();
			$.get("updateUserInfoControl.php",{"field":"silver","value":val,"name":name,"value2":val2}, callback);  
		}
	);
	
	$(".wrapper .btnExp").click(
		function(){
			var val = $(".wrapper .exp").val();
			var val2 = 0;
			var name = $(".name").val();
			$.get("updateUserInfoControl.php",{"field":"exp","value":val,"name":name,"value2":val2}, callback);  
		}
	);
	
	$(".wrapper .btnMember").click(
		function(){
			var val = $(".wrapper .member").val();
			var val2 = $(".wrapper .member2").val();
			var name = $(".name").val();
			$.get("updateUserInfoControl.php",{"field":"member","value":val,"name":name,"value2":val2}, callback);  
		}
	);
	
	$(".wrapper .btnTool").click(
		function(){
			var val = $(".wrapper .tool").val();
			var val2 = $(".wrapper .tool2").val();
			var name = $(".name").val();
			$.get("updateUserInfoControl.php",{"field":"tool","value":val,"name":name,"value2":val2}, callback);  
		}
	);
	
	$(".wrapper .btnHero").click(
		function(){
			var val = $(".wrapper .hero").val();
			var val2 = 0;
			var name = $(".name").val();
			$.get("updateUserInfoControl.php",{"field":"hero","value":val,"name":name,"value2":val2}, callback);  
		}
	);
	
	$(".wrapper .btnSoul").click(
		function(){
			var val = $(".wrapper .soul").val();
			var val2 = $(".wrapper .soul2").val();
			var name = $(".name").val();
			$.get("updateUserInfoControl.php",{"field":"soul","value":val,"name":name,"value2":val2}, callback);  
		}
	);
	
	$(".wrapper .btnSkillbook").click(
		function(){
			var val = $(".wrapper .skillbook").val();
			var val2 = $(".wrapper .skillbook2").val();
			var name = $(".name").val();
			$.get("updateUserInfoControl.php",{"field":"skillbook","value":val,"name":name,"value2":val2}, callback);  
		}
	);
	
	$(".wrapper .btnEquip").click(
		function(){
			var val = $(".wrapper .equip").val();
			var val2 = $(".wrapper .equip2").val();
			var name = $(".name").val();
			$.get("updateUserInfoControl.php",{"field":"equip","value":val,"name":name,"value2":val2}, callback);  
		}
	);
	
	$(".wrapper .btnPower").click(
		function(){
			var val = $(".wrapper .power").val();
			var val2 = 0;
			var name = $(".name").val();
			$.get("updateUserInfoControl.php",{"field":"power","value":val,"name":name,"value2":val2}, callback);  
		}
	);
	
	$(".wrapper .btnVitality").click(
		function(){
			var val = $(".wrapper .vitality").val();
			var val2 = 0;
			var name = $(".name").val();
			$.get("updateUserInfoControl.php",{"field":"vitality","value":val,"name":name,"value2":val2}, callback);  
		}
	);

	$(".wrapper .btnClearCache").click(
			function(){
				var val = $(".wrapper .userid").val();
				var val2 = 0;
				var name = $(".name").val();
				$.get("updateUserInfoControl.php",{"field":"clearCache","value":val,"name":name,"value2":val2}, callback);  
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

					<h3>修改用户信息</h3>
					
					<form id="sampleform" method="post" action="updateUserInfoControl.php">

						<fieldset>

							<p>
								<label class="required" for="firstname">船长名:</label><br/>
								<input type="text" id="firstname" class="name half" value="" name="firstname"/>
								<input type="button" class="btnName btn btn-green big" value="查询"/>
								<small>输入用户的船长名</small>
							</p>
							<p>
								<label class="required" for="firstname">元宝:</label><br/>
								<input type="text" id="firstname" class="cash half" value="" name="firstname"/>
								<input type="button" class="btnCash btn btn-green big" value="add"/>
								<small>输入增加的元宝</small>
							</p>
							<p>
								<label class="required" for="firstname">充值:</label><br/>
								<input type="text" id="firstname" class="recharge half" value="" name="firstname"/>
								<input type="button" class="btnRecharge btn btn-green big" value="add"/>
								<small>输入充值的金额</small>
							</p>
							<p>
								<label class="required" for="firstname">银币:</label><br/>
								<input type="text" id="firstname" class="silver half" value="" name="firstname"/>
								<input type="button" class="btnSilver btn btn-green big" value="add"/>
								<small>输入增加的银币</small>
							</p>
							<p>
								<label class="required" for="firstname">经验:</label><br/>
								<input type="text" id="firstname" class="exp half" value="" name="firstname"/>
								<input type="button" class="btnExp btn btn-green big" value="add"/>
								<small>输入增加的经验</small>
							</p>
							<p>
								<label class="required" for="firstname">弟子经验:</label><br/>
								<input type="text" id="firstname" class="member half" value="" name="firstname"/>
								<input type="text" id="firstname" class="member2 half" value="" name="firstname"/>
								<input type="button" class="btnMember btn btn-green big" value="add"/>
								<small>输入上阵弟子的序号和增加的经验,如第一个弟子为1</small>
							</p>
							<p>
								<label class="required" for="firstname">道具:</label><br/>
								<input type="text" id="firstname" class="tool half" value="" name="firstname"/>
								<input type="text" id="firstname" class="tool2 half" value="" name="firstname"/>
								<input type="button" class="btnTool btn btn-green big" value="add"/>
								<small>输入道具的ID和增加的数量</small>
							</p>
							<p>
								<label class="required" for="firstname">英雄:</label><br/>
								<input type="text" id="firstname" class="hero half" value="" name="firstname"/>
								<input type="button" class="btnHero btn btn-green big" value="add"/>
								<small>输入增加的影响ID</small>
							</p>
							<p>
								<label class="required" for="firstname">影子:</label><br/>
								<input type="text" id="firstname" class="soul half" value="" name="firstname"/>
								<input type="text" id="firstname" class="soul2 half" value="" name="firstname"/>
								<input type="button" class="btnSoul btn btn-green big" value="add"/>
								<small>输入增加的影子ID以及数量</small>
							</p>
							<p>
								<label class="required" for="firstname">技能:</label><br/>
								<input type="text" id="firstname" class="skillbook half" value="" name="firstname"/>
								<input type="text" id="firstname" class="skillbook2 half" value="" name="firstname"/>
								<input type="button" class="btnSkillbook btn btn-green big" value="add"/>
								<small>输入增加的技能ID以及数量</small>
							</p>
							<p>
								<label class="required" for="firstname">装备:</label><br/>
								<input type="text" id="firstname" class="equip half" value="" name="firstname"/>
								<input type="text" id="firstname" class="equip2 half" value="" name="firstname"/>
								<input type="button" class="btnEquip btn btn-green big" value="add"/>
								<small>输入增加的装备ID以及数量</small>
							</p>
							<p>
								<label class="required" for="firstname">体力:</label><br/>
								<input type="text" id="firstname" class="power half" value="" name="firstname"/>
								<input type="button" class="btnPower btn btn-green big" value="add"/>
								<small>输入增加的体力ID</small>
							</p>
							<p>
								<label class="required" for="firstname">精力:</label><br/>
								<input type="text" id="firstname" class="vitality half" value="" name="firstname"/>
								<input type="button" class="btnVitality btn btn-green big" value="add"/>
								<small>输入增加的精力ID</small>
							</p>
							<p>
								<label class="required" for="firstname">用户id:</label><br/>
								<input type="text" id="firstname" class="userid half" value="" name="firstname"/>
								<input type="button" class="btnClearCache btn btn-green big" value="clear"/>
								<small>输入需要清除缓存的用户ID</small>
							</p>

						</fieldset>

					</form>
				
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
<!--
								<dt>时间：</dt>
								<dd><input type="text" id="firstname" class="servertime" value="" name="firstname"/></dd>
-->
								<dt>用户ID：</dt>
								<dd><input type="text" id="firstname" class="userid" value="" name="firstname"/></dd>
								<dt>用户名：</dt>
								<dd><input type="text" id="firstname" class="name" value="" name="firstname"/></dd>
								<dt>注册时间：</dt>
								<dd><input type="text" id="firstname" class="registertime" value="" name="firstname"/></dd>
								<dt>上次登录时间：</dt>
								<dd><input type="text" id="firstname" class="lastlogin" value="" name="firstname"/></dd>
								<dt>等级：</dt>
								<dd><input type="text" id="firstname" class="level" value="" name="firstname"/></dd>
								<dt>元宝：</dt>
								<dd><input type="text" id="firstname" class="goldcoins" value="" name="firstname"/></dd>
								<dt>银币：</dt>
								<dd><input type="text" id="firstname" class="silvercoins" value="" name="firstname"/></dd>
					
								<form id="sampleform" method="post" action="excelPacket/testSuc.php">
									<input type="text" id="" class="silvercoins" value="world" name="hello"/></dd>
									<input type="submit" class="btnName btn btn-green big" value="download"/>
								</form>	
							</dl>
						</section>
					</div>
				</aside>
				<!-- End of Right column/section -->
				
		</div>
		<!-- End of Wrapper -->
	</div>
	<!-- End of Page content -->

	<?php include('footer.php'); ?>
</body>
</html>









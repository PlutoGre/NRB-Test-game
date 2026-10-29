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
		
		$(".thisLuckPool").val(ResultJson['thisLuckPool']);
		$(".nextLuckPool").val(ResultJson['nextLuckPool']);
		$(".luckOne").val(ResultJson['luckOne']);
		$(".luckTwo").val(ResultJson['luckTwo']);
		$(".treasureWorth").val(ResultJson['treasureWorth']);
		$(".thisDisciple").val(ResultJson['thisDisciple']);
		$(".nextDisciple").val(ResultJson['nextDisciple']);
		$(".discipleLucky").val(ResultJson['discipleLucky']);
		
		alert("ok");
		
	}	

	$(".wrapper .btnName").click(
		function(){
			var val = 0;
			var val2 = 0;
			var name = $(".name").val();
			$.get("fetchLuckyControl.php",{"field":"gold","value":val,"name":name,"value2":val2}, callback);  
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

					<h3>查看用户幸运值</h3>
					<!--
					<?php $name="当前所在 用户ID 金币数 等级 经验" ?>
					<?php include_once('./excelInclude.php');?>
		-->
					<fieldset>
						<p>
							<label class="required" for="firstname">船长名:</label><br/>
							<input type="text" id="firstname" class="name half" value="" name="firstname"/>
							<input type="button" class="btnName btn btn-green big" value="查询"/>
							<small>输入用户的船长名</small>
						</p>
					</fieldset>
							
					<table class="display stylized" id="example">
						<thead>
							<tr>
								<th>
						类型
								</th>
								<th>
						当前所在奖池
								</th>
								<th>
						下一个切换奖池
								</th>
								<th>
						当前已累计运气值
								</th>
								<th>
						当前已累计运气值2
								</th>
								<th>
						当前已抽取额度
								</th>
							</tr>
						</thead>
						
						<tbody>
							<tr class="gradeA">
								<td>
								开宝箱	
								</td>
								<td>
								<input type="text" class="thisLuckPool" style="BORDER-TOP-STYLE: none; BORDER-RIGHT-STYLE: none; BORDER-LEFT-STYLE: none; BORDER-BOTTOM-STYLE: none;width:50px"> 
								</td>
								<td>
								<input type="text" class="nextLuckPool" style="BORDER-TOP-STYLE: none; BORDER-RIGHT-STYLE: none; BORDER-LEFT-STYLE: none; BORDER-BOTTOM-STYLE: none;width:50px"> 
								</td>
								<td>
								<input type="text" class="luckOne" style="BORDER-TOP-STYLE: none; BORDER-RIGHT-STYLE: none; BORDER-LEFT-STYLE: none; BORDER-BOTTOM-STYLE: none;width:50px"> 
								</td>
								<td>
								<input type="text" class="luckTwo" style="BORDER-TOP-STYLE: none; BORDER-RIGHT-STYLE: none; BORDER-LEFT-STYLE: none; BORDER-BOTTOM-STYLE: none;width:50px"> 
								</td>
								<td>
								<input type="text" class="treasureWorth" style="BORDER-TOP-STYLE: none; BORDER-RIGHT-STYLE: none; BORDER-LEFT-STYLE: none; BORDER-BOTTOM-STYLE: none;width:50px"> 
								</td>
							</tr>
							<tr class="gradeA">
								<td>
								抽取弟子	
								</td>
								<td>
								<input type="text" class="thisDisciple" style="BORDER-TOP-STYLE: none; BORDER-RIGHT-STYLE: none; BORDER-LEFT-STYLE: none; BORDER-BOTTOM-STYLE: none;width:50px"> 
								</td>
								<td>
								<input type="text" class="nextDisciple" style="BORDER-TOP-STYLE: none; BORDER-RIGHT-STYLE: none; BORDER-LEFT-STYLE: none; BORDER-BOTTOM-STYLE: none;width:50px"> 
								</td>
								<td>
								<input type="text" class="discipleLucky" style="BORDER-TOP-STYLE: none; BORDER-RIGHT-STYLE: none; BORDER-LEFT-STYLE: none; BORDER-BOTTOM-STYLE: none;width:50px"> 
								</td>
								<td>
								<input type="text" class="" style="BORDER-TOP-STYLE: none; BORDER-RIGHT-STYLE: none; BORDER-LEFT-STYLE: none; BORDER-BOTTOM-STYLE: none;width:50px"> 
								</td>
								<td>
								<input type="text" class="" style="BORDER-TOP-STYLE: none; BORDER-RIGHT-STYLE: none; BORDER-LEFT-STYLE: none; BORDER-BOTTOM-STYLE: none;width:50px"> 
								</td>
							</tr>
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











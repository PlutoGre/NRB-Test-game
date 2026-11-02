<?php
	session_start();
	$_SESSION["user"];#增加用户名变数
	$_SESSION["password"];#增加密码变数
	$_SESSION["serverid"];#服务器ID
	$_SESSION["servername"];#服务器名称
	$_SESSION["serverip"];#服务器IP地址
	$_SESSION["serverport"];#服务器端口
	$_SESSION["tmlast"];#增加时间变数
	if($user==""){#判断是否是第一次登陆
//		$error="choose your name and input the password please!";
	}
	
	$user1 = $_POST["user1"];
	$password1 = md5($_POST["password1"]);
	$serverid = $_POST["serverid"];
	
	
	$tmlast=date("u"); #记录登陆时间
	require_once(dirname(__dir__).'/common/common.php');
	//查找server信息
	$link=mysql_connect(DBConfig::$host,DBConfig::$user,DBConfig::$passwd);
	mysql_set_charset('utf8',$link);
	$db_selected=mysql_select_db(DBConfig::$db_name,$link);
	$sqlStr = "select serverid,servername,serverip,serverport,serverusername,serverpassword from stat_server_info where 1=1 ";
	if($serverid){
		$sqlStr .= " and serverid = $serverid ";
	}
	$sqlStr .= " order by serverid ";
	
	$result=mysql_query($sqlStr);
	if(!$result){
		echo '错误'.mysql_error();
		exit;
	}
	$num_rows=mysql_num_rows($result);
	$servercontent = array();
	$i = 0;
	while($total = mysql_fetch_array($result)){
		$servercontent[$i] = $total[0] . " " . $total[1] . " " . $total[2] . "\n";
		$serverid = $total[0];
		$servername = $total[1];
		$serverip = $total[2];
		$serverpost = $total[3];
		$i = $i + 1;
	}
	if($user1){
		$user=trim($user1); #记录用户名（引用user1变量是为什么？请读者自己思考。）
		$password=trim($password1); #记录密码
		$_SESSION["user"] = $user;
		$_SESSION["password"] = $password;
		$_SESSION["serverid"] = $serverid;
		$_SESSION["servername"] = $servername;
		$_SESSION["serverip"] = $serverip;
		$_SESSION["serverport"] = $serverpost;
	}

	if($user1&&$password1){
		if($password1==888){ #判断登陆密码是否是默认密码888结束 php 程式
			$sid="phpsessid=".session_id(); #保存当前session的id号
			$warning="your password is still the default password888,please change it.";
			header("location:changepassword.php?$sid&warning=$warning"); #传递警告参数warning到changepassword.php 页面
			exit(); #立刻结束 php 程式
		}
		if(strtolower($user)=="root"){ #判断登陆用户是否是超级用户，可以自行扩充用户
			$filename="backend_index.php";
		}
		else{
			if(!$filename) #判断进入登陆页面的上一页是否是受保护页面
			$filename="index.php";
		}

		$sid="phpsessid=".session_id(); #保存当前session的id号
		header("location:$filename?$sid"); 
		#登陆成功进入指定页面，传递当前session的id号，防止用户不使用 cookie 而读不到 session 值
		exit(); #立刻结束 php 程式
	}
?>
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
<!-- jQuery form validation -->
<script type="text/javascript" SRC="js/jquery.validate_pack.js"></script>
<!-- jQuery popup box -->
<script type="text/javascript" SRC="js/jquery.nyroModal.pack.js"></script>
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
	
	// validate signup form on keyup and submit
	var validator = $("#loginform").validate({
		rules: {
			username: "required",
			password: "required"
		},
		messages: {
			username: "Enter your username",
			password: "Provide your password"
		},
		// the errorPlacement has to take the layout into account
		errorPlacement: function(error, element) {
			error.insertAfter(element.parent().find('label:first'));
		},
		// set new class to error-labels to indicate valid fields
		success: function(label) {
			// set &nbsp; as text for IE
			label.html("&nbsp;").addClass("ok");
		}
	});

});
</script>
</head>
<body>
	<!-- Header -->
	<header id="top">
		<div class="wrapper-login">
			<!-- Title/Logo - can use text instead of image -->
			<div id="title"><img SRC="img/logo.png" alt="Administry" /><!--<span>Administry</span> demo--></div>
			<!-- Main navigation -->
			<nav id="menu">
				<ul class="sf-menu">
					<li class="current"><a href="login.php">Login</a></li>
					<li><a href="register.php">Register</a></li>
			</ul>
			</nav>
			<!-- End of Main navigation -->
			<!-- Aside links -->
			<aside><b>English</b> &middot; <a href="#">Spanish</a> &middot; <a href="#">German</a></aside>
			<!-- End of Aside links -->
		</div>
	</header>
	<!-- End of Header -->
	<!-- Page title -->
	<div id="pagetitle">
		<div class="wrapper-login"></div>
	</div>
	<!-- End of Page title -->
	
	<!-- Page content -->
	<div id="page">
		<!-- Wrapper -->
		<div class="wrapper-login">
				<!-- Login form -->
				<section class="full">					
					
					<h3>Login</h3>
					
					<div class="box box-info">Type anything to log in</div>
					<?php echo"$error"; #显示登陆提示?>
					<form action="<?php echo $_SERVER['PHP_SELF'];#提交到当前页?>" method=post>
					
						<p>
							<label class="required" for="username">Username:</label><br/>
							<input type="text" id="username" class="full" value="" name="user1"/>
						</p>
						
						<p>
							<label class="required" for="password">Password:</label><br/>
							<input type="password" id="password" class="full" value="" name="password1"/>
						</p>
						<p>
							<select size="1" id="select" name="serverid">
								<?php foreach ( $servercontent as $line => $data ) : ?>
								<?php list($serverid,$servername,$serverip) = explode(' ',$data); ?>
								<option value ="<?php echo $serverid ?>"><?php echo $servername; ?></option>
								<?php endforeach; ?>
							</select>						
						</p>
						<p>
							<input type="submit" class="btn btn-green big" value="Login"/>
						</p>
						<div class="clear">&nbsp;</div>

					</form>
					
					<form id="emailform" style="display:none" method="post" action="#">
						<div class="box">
							<p id="emailinput">
								<label for="email">Email:</label><br/>
								<input type="text" id="email" class="full" value="" name="email"/>
							</p>
							<p>
								<input type="submit" class="btn" value="Send"/>
							</p>
						</div>
					</form>
					
				</section>
				<!-- End of login form -->
				
		</div>
		<!-- End of Wrapper -->
	</div>
	<!-- End of Page content -->
	
	<!-- Page footer -->
	<footer id="bottom">
		<div class="wrapper-login">
			<p>Copyright &copy; 2010 <b></p>
		</div>
	</footer>
	<!-- End of Page footer -->

<!-- User interface javascript load -->
<script type="text/javascript" SRC="js/administry.js"></script>
</body>
</html>



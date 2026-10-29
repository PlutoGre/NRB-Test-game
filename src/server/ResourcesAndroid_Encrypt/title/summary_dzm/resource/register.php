<?php
		require_once(dirname(__dir__).'/common/common.php');
	$user1 = $_POST["user1"];
	$password1 = md5($_POST["password1"]);
	
echo "user:" . $user1;
echo "password:" . $password1;
	
    $link=mysql_connect(DBConfig::$host,DBConfig::$user,DBConfig::$passwd);
    mysql_set_charset('utf8',$link);
    $db_selected=mysql_select_db(DBConfig::$db_name,$link);
		
    $result=mysql_query("select name from stat_user_info  where name='$user1'");

    $num_rows=mysql_num_rows($result);

echo "@@" . $num_rows . "@@";
    if(!$num_rows){ #判断是否找到密码匹配的用户

	if($user1){
        $queryinsert = sprintf("insert into stat_user_info (name,password,stat) values ('%s','%s','%d')",$user1,$password1,0);
        $resultinstert = mysql_query($queryinsert,$link);
		
		session_start();
		$user=trim($user1);
		$password=trim($password1);
		$_SESSION["user"] = $user;
		$_SESSION["password"] = $password;
		
		$filename="index.php";
		$sid="phpsessid=".session_id(); #保存当前session的id号
		header("location:$filename?$sid"); #登陆成功进入指定页面，传递当前session的id号，防止用户不使用 cookie 而读不到 session 值
		exit(); #立刻结束 php 程式
	}else{
	
	echo "no user";
	}

	}else{
		$error="user already exist , please login!";
		$filename="login.php";
		header("location:$filename"); 
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
					<li><a href="login.php">Login</a></li>
					<li class="current"><a href="register.php">Register</a></li>
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
					
					<h3>Register</h3>
					
					<div class="box box-info">Type anything to log in</div>

					<form id="loginform" method="post" action="<?php echo $_SERVER['PHP_SELF'];#提交到当前页?>">

						<p>
							<label class="required" for="username">Username:</label><br/>
							<input type="text" id="username" class="full" value="" name="user1"/>
						</p>
						
						<p>
							<label class="required" for="password">Password:</label><br/>
							<input type="password" id="password" class="full" value="" name="password1"/>
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
			<p>Copyright &copy; 2013 <b></p>
		</div>
	</footer>
	<!-- End of Page footer -->

<!-- User interface javascript load -->
<script type="text/javascript" SRC="js/administry.js"></script>
</body>
</html>



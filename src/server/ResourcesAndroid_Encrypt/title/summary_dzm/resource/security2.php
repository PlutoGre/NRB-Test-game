<?php
	require_once(dirname(__dir__).'/common/common.php');
	session_start();

        if($filename=="")
                $filename=$php_self; #记录当前页面路径
        if($durtime=="")
                $durtime=300; #设置 session “失效”时间
        $currtime=date("u");
        if(($currtime-$tmlast)>$durtime){ #判断 session是否“失效”
                //session_destroy();
                $error=urlencode("seesion expired.login again please!");
                header("location:login2.php?filename=$filename&error=$error&user=$user"); #跳到重新登陆页
                exit();
        }
        else{
                $tmlast=$currtime; # session 没“失效”则更新最后“登陆”时间
        }


		$link=mysql_connect(DBConfig::$host,DBConfig::$user,DBConfig::$passwd);
		mysql_set_charset('utf8',$link);
		$db_selected=mysql_select_db(DBConfig::$db_name,$link);

		$userTmp = $_SESSION["user"];
		$passwordTmp = $_SESSION["password"];
		$serverid = $_SESSION["serverid"];
		$servername = $_SESSION["servername"];
		$serverip = $_SESSION["serverip"];


		$result=mysql_query("select name from stat_user_info  where name='$userTmp' and password='$passwordTmp' and stat = 1");
		if(!$result){
			echo '错误'.mysql_error();
			exit;
		}
		$num_rows=mysql_num_rows($result);

        if(!$num_rows){ #判断是否找到密码匹配的用户
              $error=urlencode("password is wrong or noprivilege user.");

              header("location:login.php?filename=$filename&error=$error&user=$user"); #跳到密码错误登陆页
        }else{

        }





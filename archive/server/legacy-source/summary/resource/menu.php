<?php include "security2.php"; ?>
	<!-- Header -->
	<header id="top">
		<div class="wrapper">
			<!-- Title/Logo - can use text instead of image -->
			<div id="title"><img SRC="img/logo.png" alt="Administry" /><!--<span>Administry</span> demo--></div>
			<!-- Top navigation -->
			<div id="topnav">
				<a href="#"><img class="avatar" height="40" width="40" SRC="img/head.jpg" alt="" /></a>
				Logged in as <b><?php echo $_SESSION["user"]?></b>
				<span>|</span> <a href="logout.php">Logout</a><br />
				<?php echo $_SESSION["servername"]; ?>
			</div>
			<!-- End of Top navigation -->
			<!-- Main navigation -->
			<nav id="menu">
				<ul class="sf-menu">
					<li>
						<a HREF="#">用户信息</a>
						<ul>
							<li>
								<a HREF="pageControl.php?type=searchUserInfo">用户基本信息</a>
							</li>
							<li>
								<a HREF="pageControl.php?type=searchUseGold">用户每日付费与储值</a>
							</li>
							<li>
								<a HREF="pageControl.php?type=updateUserInfo">修改用户信息</a>
							</li>
						</ul>
					</li>
					
					<li>
						<a HREF="#">用户数据统计</a>
						<ul>
							<li>
								<a HREF="pageControl.php?type=registAndActive">注册用户与活跃用户</a>
							</li>
							<li>
								<a HREF="pageControl.php?type=playerLeft">用户留存率</a>
							</li>
							<li>
								<a HREF="pageControl.php?type=vipDistribute">用户VIP分布</a>
							</li>
							<li>
								<a HREF="pageControl.php?type=monthlyPayInfo">月付费渗透率</a>
							</li>
							<li>
								<a HREF="pageControl.php?type=onlineUserCount">同时在线用户数</a>
							</li>
						</ul>
					</li>
					
					<li>
						<a HREF="#">新用户统计</a>
						<ul>
							<li>
								<a HREF="pageControl.php?type=firstdayLevel">新注册用户等级分布</a>
							</li>
							<li>
								<a HREF="pageControl.php?type=registerPayer">新用户储值</a>
							</li>
						</ul>
					</li>
					
					<li>
						<a HREF="#">用户等级分布</a>
						<ul>
							<li>
								<a HREF="pageControl.php?type=commonLevel">全局等级分布</a>
							</li>
							<li>
								<a HREF="pageControl.php?type=activeLevel">活跃用户等级分布</a>
							</li>
						</ul>
					</li>
					
					<li>
						<a HREF="#">消费详细</a>
						<ul>
							<li>
								<a HREF="pageControl.php?type=summaryUseGold">每日道具详细</a>
							</li>
						</ul>
					</li>
					<li>
						<a HREF="#">其它</a>
						<ul>
							<li>
								<a HREF="pageControl.php?type=everydayPay">每日支付记录</a>
							</li>
							<li>
								<a HREF="pageControl.php?type=pushRewards">发放礼包</a>
							</li>
							<li>
								<a HREF="pageControl.php?type=fetchLucky">查看运气值</a>
							</li>
							<li>
								<a HREF="pageControl.php?type=fetchLogin">查看10日登陆</a>
							</li>
						</ul>
					</li>
				</ul>
			</nav>
			<!-- End of Main navigation -->
			<!-- Aside links -->
		</div>
	</header>
	<!-- End of Header -->




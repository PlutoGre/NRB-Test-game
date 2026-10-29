/**
 * 
 */
package com.youai.dreamonepiece.platform.uc;

import java.util.HashMap;
import java.util.Map;

import android.app.Activity;
import android.graphics.BitmapFactory;
import android.util.Log;
import cn.uc.gamesdk.UCCallbackListener;
import cn.uc.gamesdk.UCCallbackListenerNullException;
import cn.uc.gamesdk.UCGameSDK;
import cn.uc.gamesdk.UCGameSDKStatusCode;
import cn.uc.gamesdk.UCLogLevel;
import cn.uc.gamesdk.UCOrientation;
import cn.uc.gamesdk.info.FeatureSwitch;
import cn.uc.gamesdk.info.GameParamInfo;
import cn.uc.gamesdk.info.OrderInfo;
import cn.uc.gamesdk.info.PaymentInfo;

import com.youai.DeviceUtil;
import com.youai.IGameActivity;
import com.youai.IPlatformLoginAndPay;
import com.youai.PlatformAndGameInfo;
import com.youai.PlatformAndGameInfo.GameInfo;
import com.youai.PlatformAndGameInfo.LoginInfo;
import com.youai.PlatformAndGameInfo.PayInfo;
import com.youai.PlatformAndGameInfo.ShareInfo;
import com.youai.PlatformAndGameInfo.VersionInfo;
import com.youai.Util;
import com.youai.dreamonepiece.GameInterface.IGameAppStateCallback;
import com.youai.dreamonepiece.GameInterface.IGameUpdateStateCallback;
import com.youai.dreamonepiece.GameInterface.IPlatformSDKStateCallback;
import com.youai.dreamonepiece.platform.nd91.R;

/**
 * @author zhengxin
 * 2013-06-17
 *
 */
public class PlatformUCLoginAndPay implements IPlatformLoginAndPay {
	
	private static final String TAG = PlatformUCLoginAndPay.class.getSimpleName();
	private static PlatformUCLoginAndPay sInstance = null;
	//
	private IGameActivity 				mGameActivity;
	private IPlatformSDKStateCallback	mCallback1;
	private IGameUpdateStateCallback	mCallback2;
	private IGameAppStateCallback		mCallback3;
	//
	public Activity 				game_ctx = null;
	public GameInfo 				game_info = null;
	public LoginInfo 				login_info = null;
	public VersionInfo 				version_info = null;
	public PayInfo 					pay_info = null;
	
	private boolean 				mIsInited = false;
	private boolean 				mIsVersionUpdateCheckReturned = false;
	private boolean					mIsLogined = false;	//自己维护登录成功与否的状态
	//
	private final int 					mUCServerId = 1986;	//支付与uc对应的serverId
	//
	public static PlatformUCLoginAndPay getInstance() {
		if (sInstance == null) {
			sInstance = new PlatformUCLoginAndPay();
		}
		return sInstance;
	}
	
	private PlatformUCLoginAndPay() {
		//
	}

	@Override
	public void init(IGameActivity game_activity, GameInfo game_info) {
		//
		this.mGameActivity = game_activity;
		this.game_ctx = game_activity.getActivity();
		this.game_info = game_info;
		//
		this.game_info.debug_mode = PlatformAndGameInfo.enDebugMode_Release;
		//
		//this.game_info.cp_id = 27385;
		//this.game_info.app_id = 515092;//gameid
		//this.game_info.app_key = "c6570bd69c858f779a649906f7ea4698";
		//
		{
			// 监听用户注销登录消息
			// 九游社区-多账号管理-退出当前账号功能执行时会触发此监听
			
			try {
				UCGameSDK.defaultSDK().setLogoutNotifyListener(
						new UCCallbackListener<String>() {
							@Override
							public void callback(int statuscode, String msg) {
								// TODO 此处需要游戏客户端注销当前已经登录的游戏角色信息
								String s = "游戏接收到用户退出通知。" + msg;
								Log.e("UCGameSDK", s);
								
								mIsLogined = false;
								
							}
						});
			} catch (UCCallbackListenerNullException e) {
				// 处理异常
			}
			
			try {
				GameParamInfo gpi = new GameParamInfo();// 下面的值仅供参考
				gpi.setCpId(this.game_info.cp_id);
				gpi.setGameId(game_info.app_id);
				gpi.setServerId(mUCServerId);
				//gpi.setChannelId(2); // 渠道号统一处理，已不需设置，此参数已废弃，服务端此参数请设置值为2

				// 在九游社区设置显示查询充值历史和显示切换账号按钮，
				// 在不设置的情况下，默认情况情况下，生产环境显示查询充值历史记录按钮，不显示切换账户按钮
				// 测试环境设置无效
				gpi.setFeatureSwitch(new FeatureSwitch(true, true));

				// 设置SDK登录界面为竖屏
				UCGameSDK.defaultSDK().setOrientation(UCOrientation.PORTRAIT);

				UCGameSDK.defaultSDK().initSDK(this.game_ctx,
						UCLogLevel.DEBUG, false, gpi,
						new UCCallbackListener<String>() {
							@Override
							public void callback(int code, String msg) {
								
								Log.e("UCGameSDK", "UCGameSDK初始化接口返回数据 msg:" + msg
										+ ",code:" + code + ",debug:"
										+ true + "\n");
								switch (code) {
								// 初始化成功,可以执行后续的登录充值操作
								case UCGameSDKStatusCode.SUCCESS:
									
									PlatformUCLoginAndPay.sInstance.notifyInitComplete();
									break;

								// 初始化失败
								case UCGameSDKStatusCode.INIT_FAIL:
									// 调用sdk初始化接口
									
								default:
									break;
								}
							}
						});
			} catch (UCCallbackListenerNullException e) {
				e.printStackTrace();
			} catch (Exception e) {
				e.printStackTrace();
			}
		}
		//
	}

	
	public void notifyInitComplete() {
		
		this.mIsInited = true;
		//
		mCallback1.notifyInitPlatformSDKComplete();
		//
	}

	@Override
	public void unInit() {
		
		UCGameSDK.defaultSDK().exitSDK();
		
	}

	@Override
	public GameInfo getGameInfo() {
		
		return this.game_info;
	}

	@Override
	public void callLogin() {
		
		if (mIsLogined)
			return;
		
		final PlatformUCLoginAndPay me = this;

		try {
			// 登录接口回调，从这里可以获取登录结果
			UCCallbackListener<String> loginCallbackListener = new UCCallbackListener<String>() {
				@Override
				public void callback(int code, String msg) {
					Log.e("UCGameSDK", "UCGameSdk登录接口返回数据:code=" + code
							+ ",msg=" + msg);
					//
					// 登录成功。此时可以获取sid。并使用sid进行游戏的登录逻辑。
					// 客户端无法直接获取UCID
					if (code == UCGameSDKStatusCode.SUCCESS) {
						LoginInfo login_info = new LoginInfo();
						//获取sid。（注：ucid需要使用sid作为身份标识去SDK的服务器获取）
						//UCGameSDK.defaultSDK().getSid();
						//
						login_info.login_session = UCGameSDK.defaultSDK().getSid();
						//
						me.notifyLoginResult(login_info);
						//
						msg = "";
					}

					// 登录失败。应该先执行初始化成功后再进行登录调用。
					if (code == UCGameSDKStatusCode.NO_INIT) {
						// 没有初始化就进行登录调用，需要游戏调用SDK初始化方法
						msg = "NO_INIT," + msg;
					}

					// 登录退出。该回调会在登录界面退出时执行。
					if (code == UCGameSDKStatusCode.LOGIN_EXIT) {
						// 登录界面关闭，游戏需判断此时是否已登录成功进行相应操作
						msg = "LOGIN_EXIT," + msg;
					}
					//
					//Toast.makeText(me.game_ctx, msg, Toast.LENGTH_SHORT).show();
					//
					mCallback3.showWaitingViewImp(false, -1, "请重新登录");
				}
			};
			//
			UCGameSDK.defaultSDK().login(this.game_ctx,
					loginCallbackListener);
			mCallback3.showWaitingViewImp(true, -1, "正在登录");
			//
		} catch (UCCallbackListenerNullException e) {
			e.printStackTrace();
		}
		
	}

	@Override
	public void notifyLoginResult(LoginInfo login_result) {
		
		this.login_info = login_result;
		//
		String serverUrl = "";
		if (this.game_info.debug_mode == PlatformAndGameInfo.enDebugMode_Debug) {
			serverUrl = "http://sdk.test4.g.uc.cn/ss";
		}else {
			serverUrl = "http://sdk.g.uc.cn/ss";
		}
		/*
		 * HttpPost不能在主线程运行
		 * */
		final PlatformUCLoginAndPay me = this;
		final String svrUrl = serverUrl;
		new Thread("UCLoginHttpPostThread")
		{
			@Override
			public void run()
			{
				try
				{
					Map<String,Object> params = new HashMap<String,Object>();
					params.put("id", System.currentTimeMillis());//当前系统时间
					params.put("service", "ucid.user.sidInfo");
					
					UCGame game = new UCGame();
					game.setCpId(me.game_info.cp_id);//cpid是在游戏接入时由UC平台分配，同时分配相对应cpId的apiKey
					game.setGameId(me.game_info.app_id);//gameid是在游戏接入时由UC平台分配
					//game.setChannelId(channelId);//channelid是在游戏接入时由UC平台分配
					//serverid是在游戏接入时由UC平台分配，
					//若存在多个serverid情况，则根据用户选择进入的某一个游戏区而确定。
					//若在调用该接口时未能获取到具体哪一个serverid，则此时默认serverid=0
					game.setServerId(me.mUCServerId);
					
					params.put("game", game);
					
					Map data = new HashMap();
					data.put("sid", me.login_info.login_session);//在uc sdk登录成功时，游戏客户端通过uc sdk的api获取到sid，再游戏客户端由传到游戏服务器
					params.put("data", data);
					
					params.put("encrypt", "md5");
					/*
						签名规则=cpId+签名内容+apiKey
						假定cpId=109,apiKey=202cb962234w4ers2aaa,sid=abcdefg123456
						那么签名原文109sid=abcdefg123456202cb962234w4ers2aaa
						签名结果6e9c3c1e7d99293dfc0c81442f9a9984
					*/
					String signSource = me.game_info.cp_id+"sid="+me.login_info.login_session+me.game_info.app_key;//组装签名原文
					String sign = Util.getMD5Str(signSource);//MD5加密签名
					
					Log.w(TAG, "[签名原文]"+signSource);
					Log.w(TAG, "[签名结果]"+sign);
					
					params.put("sign", sign);
					
					String body = Util.encodeJson(params);//把参数序列化成一个json字符串
					Log.w(TAG, "[请求参数]"+body);
				
					String result = Util.doPost(svrUrl, body);//http post方式调用服务器接口,请求的body内容是参数json格式字符串
					Log.w(TAG, "[响应结果]"+result);//结果也是一个json格式字符串
					
					SidInfoResponse rsp = (SidInfoResponse)Util.decodeJson(result, SidInfoResponse.class);//反序列化
					if(rsp != null) {//反序列化成功，输出其对象内容
						Log.w(TAG, "[id]"+rsp.getId());
						Log.w(TAG, "[code]"+rsp.getState().getCode());
						Log.w(TAG, "[msg]"+rsp.getState().getMsg());
						Log.w(TAG, "[ucid]"+rsp.getData().getUcid());
						Log.w(TAG, "[nickName]"+rsp.getData().getNickName());
						//
						if (rsp.getState().getCode() == UCGameSDKStatusCode.SUCCESS || 
								rsp.getState().getCode() == 1)
						{
							me.login_info.login_result = PlatformAndGameInfo.enLoginResult_Success;
							me.login_info.account_uid = rsp.getData().getUcid();
							me.login_info.account_nick_name = rsp.getData().getNickName();
							me.login_info.account_uid_str = 
									PlatformAndGameInfo.enPlatformShort_UC+String.valueOf(me.login_info.account_uid);
							//
							me.mGameActivity.showToastMsg("登录成功，点击进入游戏");
							
							me.mIsLogined = true;
							
							me.mCallback3.notifyLoginResut(me.login_info);
						}
						else
						{
							me.mGameActivity.showToastMsg("登录失败，点击重新登录");
							me.callLogout();
						}
						//
					}else{
						Log.w(TAG, "接口返回异常");
					}
					Log.w(TAG, "调用sidInfo接口结束");
				}
				catch(Exception e) {
					e.printStackTrace();
					Log.w(TAG, "接口返回异常");
				}
				//
				me.mCallback3.showWaitingViewImp(false, -1, "请重新登录");
			}
		}.start();
	}

	@Override
	public LoginInfo getLoginInfo() {
		
		
		return login_info;
	}

	@Override
	public void callLogout() {
		
		try {
			UCGameSDK.defaultSDK().logout();
			
		} catch (UCCallbackListenerNullException e) {
			// 未设置退出侦听器
		}
		
		mIsLogined = false;
	}

	@Override
	public void callCheckVersionUpate() {
		
		
	}

	@Override
	public void notifyVersionUpateInfo(VersionInfo version_info) {
		
		this.mIsVersionUpdateCheckReturned = true;
		//
		this.version_info = null;
		this.version_info = version_info;
		if (version_info != null) {
			
			mCallback2.notifyVersionCheckResult(version_info);
		}
	}

	@Override
	public int callPayRecharge(PayInfo pay_info) {
		
		PaymentInfo pInfo= new PaymentInfo(); //创建Payment对象，用于传递充值信息
		pInfo.setAllowContinuousPay(true); //设置成功提交订单后是否允许用户连续充值，默认为true。
		
		String temp = pay_info.description + "-" + pay_info.product_id;
		
		pInfo.setCustomInfo(temp); //客户端区号-物品ID
		//充值自定义参数，此参数不作任何处理，在充值完成后通知游戏服务器充值结果时原封不动传给
		//游戏服务器。此参数为可选参数，默认为空。
		pInfo.setServerId(mUCServerId); //设置充值的游戏服务器ID，此为可选参数，默认是0，不
		//设置或设置为0时，会使用初始化时设置的服务器ID。UC分配
		//pInfo.setRoleId("102"); //设置用户的游戏角色的ID，此为可选参数
		//pInfo.setRoleName("游戏角色名"); //设置用户的游戏角色名字，此为可选参数
		//pInfo.setGrade("12"); //设置用户的游戏角色等级，此为可选参数
		pInfo.setAmount(pay_info.price); //设置允许充值的金额，此为可选参数，默认为0。如果设
		//置了此金额不为0，则表示只允许用户按指定金额充值；如果不指定金额或指定为0，则表示用户
		//在充值时可以自由选择或输入希望充入的金额。
		final PlatformUCLoginAndPay thisPlatform = this;
		this.pay_info = pay_info;
		try {
			UCGameSDK.defaultSDK().pay(this.game_ctx, pInfo,
					new UCCallbackListener<OrderInfo>() {
				@Override
				public void callback(int statudcode, OrderInfo orderInfo) {
					
					if(statudcode == UCGameSDKStatusCode.NO_INIT) {
						//没有初始化就进行登录调用，需要游戏调用SDK初始化方法
					}
					if(statudcode == UCGameSDKStatusCode.SUCCESS) {
						//成功充值
						if(orderInfo != null) {
							String ordered = orderInfo.getOrderId();//获取订单号
							float amount = orderInfo.getOrderAmount();//获取订单金额
							int payWay = orderInfo.getPayWay();//获取充值类型，具体可参考支付通道编码列表
							String payWayName = orderInfo.getPayWayName();//充值类型的中文名称
							
							thisPlatform.pay_info.order_serial = ordered;
							thisPlatform.pay_info.result = 0;
							
							thisPlatform.notifyPayRechargeRequestResult(thisPlatform.pay_info);
							
						}
					}
					if(statudcode == UCGameSDKStatusCode.PAY_USER_EXIT) {
						//用户退出充值界面。
					}
				}
			});
		}catch(UCCallbackListenerNullException e) {
			//异常处理
		}
		
		return 0;
	}

	@Override
	public void notifyPayRechargeRequestResult(PayInfo pay_info) {
		
		mCallback3.notifyPayRechargeResult(pay_info);
	}

	@Override
	public void callAccountManage() {
		
		if (this.mIsLogined == false)
		{
			this.callLogin();
			return;
		}
		
		try{
			UCGameSDK.defaultSDK().enterUserCenter(this.game_ctx, new
					UCCallbackListener<String>(){
				@Override
				public void callback(int statucode,String dumpdata) {
					if(statucode==UCGameSDKStatusCode.NO_INIT){
						//没有初始化，需要进行初始化操作
					}else if(statucode==UCGameSDKStatusCode.NO_LOGIN) {
						//没有登录，需要先登录
					}else if(statucode==UCGameSDKStatusCode.SUCCESS){
						//用户管理界面正常关闭，返回游戏的界面逻辑
					}
				}
			});
		}catch(UCCallbackListenerNullException e){
			//处理异常
		}
		
	}

	@Override
	public String generateNewOrderSerial() {
		
		return DeviceUtil.generateUUID();
	}

	@Override
	public void callPlatformFeedback() {
		
		this.callAccountManage();
		
	}

	@Override
	public void callPlatformSupportThirdShare(ShareInfo share_info) {
		
		if (share_info.bitmap == null) {
			share_info.bitmap = BitmapFactory.decodeFile(share_info.img_path);
		}
		
	}

	@Override
	public void callPlatformGameBBS() {
		
		this.callAccountManage();
		
	}

	@Override
	public void onGamePause() {
		
		
	}

	@Override
	public void onGameResume() {
		
		
	}

	@Override
	public void onGameExit() {
		
		
	}

	////////////////////
	/**
	 * 游戏信息类。
	 */
	private static class UCGame {
		//cp编号
		int cpId=0;
		//游戏编号
		int gameId=0;
		//游戏发行id
		String channelId="";
		//游戏服务器id
		int serverId=0;
		
		public int getCpId() {
			return cpId;
		}
		/**
		 * 设置cp编号
		 * @param cpId cp编号
		 */
		public void setCpId(int cpId) {
			this.cpId = cpId;
		}
		public int getGameId() {
			return gameId;
		}
		/**
		 * 设置游戏编号。
		 * @param gameId 游戏编号
		 */
		public void setGameId(int gameId) {
			this.gameId = gameId;
		}
		public String getChannelId() {
			return channelId;
		}
		/**
		 * 游戏发行渠道编号。
		 * @param channelId 游戏发行渠道编号
		 */
		public void setChannelId(String channelId) {
			this.channelId = channelId;
		}
		public int getServerId() {
			return serverId;
		}
		public void setServerId(int serverId) {
			this.serverId = serverId;
		} 
	}
	////////////////////
	/**
	 * sid验证结果响应类。
	 */
	private static class SidInfoResponse {
		long id;
		SidInfoResponseState state;
		SidInfoResponseData data;
		
		public long getId(){
			return this.id;
		}
		public void setId(long id){
			this.id =id;
		}
		public SidInfoResponseState getState(){
			return this.state;
		}
		public void setState(SidInfoResponseState state){
			this.state = state;
		}
		
		public SidInfoResponseData getData(){
			return this.data;
		}
		public void setData(SidInfoResponseData data){
			this.data = data;
		}
		
		
		public class SidInfoResponseState{
			int code;
			String msg;
			public int getCode(){
				return this.code;
			}
			public void setCode(int code){
				this.code = code;
			}
			public String getMsg(){
				return this.msg;
			}
			public void setMsg(String msg){
				this.msg = msg;
			}
		}
		public class SidInfoResponseData{
			private int ucid;
			private String nickName;
			public int getUcid(){
				return this.ucid;
			}
			
			public void setUcid(int ucid){
				this.ucid = ucid;
			}
			public String getNickName(){
				return this.nickName;
			}
			public void setNickName(String nickName){
				this.nickName = nickName;
			}
		}
	}
	////////////////////
	@Override
	public void setPlatformSDKStateCallback(IPlatformSDKStateCallback callback1) {
		
		mCallback1 = callback1;
	}

	@Override
	public void setGameUpdateStateCallback(IGameUpdateStateCallback callback2) {
		
		mCallback2 = callback2;
	}

	@Override
	public void setGameAppStateCallback(IGameAppStateCallback callback3) {
		
		mCallback3 = callback3;
	}

	@Override
	public int isSupportInSDKGameUpdate() {
		/*
		 *
		 * */
		return PlatformAndGameInfo.DoNotSupportUpdate;
	}

	@Override
	public int getPlatformLogoLayoutId() {
		
		return R.layout.logo_uc;
	}
}

package com.youai.dreamonepiece.platform.thirdlogin;

import com.youai.dreamonepiece.platform.nd91.R;


/** 第三方登录的需要的用户信息*/
public class ThirdUserInfo {
	
	
	private String uidStr="";
	private long uid=0l;
	
    public void setUid(long uid) {
		this.uid = uid;
	}
	private String nickname="";
    
	public long getUid() {
		if(uid==0l){
			try {
				uid = Long.valueOf(uidStr);
			} catch (Exception e) {
				// TODO: handle exception
			}
				
		}
		return uid;
	}
	
	public String getUidStr() {
		return uidStr;
	}
	
	
	public void setUidStr(String uid) {
		this.uidStr = uid;
	}
	public String getNickname() {
		return nickname;
	}
	public void setNickname(String nickname) {
		this.nickname = nickname;
	}
    

}

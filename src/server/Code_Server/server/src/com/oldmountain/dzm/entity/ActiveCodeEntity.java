package com.oldmountain.dzm.entity;

public class ActiveCodeEntity {
	private String active_code;	//激活码
	private int status;		//使用状态 0:未使用  1:已使用
	private int create_time;
	private int used_time;
    private String playerid;
	public ActiveCodeEntity(){
		
    }

    public String getActive_code() {
        return active_code;
    }

    public void setActive_code(String active_code) {
        this.active_code = active_code;
    }

	public int getStatus() {
		return status;
	}
	public void setStatus(int status) {
		this.status = status;
	}
	public int getCreate_time() {
		return create_time;
	}
	public void setCreate_time(int createTime) {
		create_time = createTime;
	}
	public int getUsed_time() {
		return used_time;
	}
	public void setUsed_time(int usedTime) {
		used_time = usedTime;
	}
	public String getPlayerid() {
		return playerid;
	}
	public void setPlayerid(String playerid) {
		this.playerid = playerid;
	}
	
	
}

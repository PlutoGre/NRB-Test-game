package com.oldmountain.dzm.persist;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.oldmountain.dzm.net.ObjectAccessor;
import com.oldmountain.dzm.util.Util;

public class OnlineUserNum implements Runnable{
	private static final Logger logger = LoggerFactory.getLogger("OnlineCount");
	public void run(){
		int currentOnlineUserCount = ObjectAccessor.getOnlineSession().size();
		
		int current_time = Util.getServerTime();
		logger.info("online count: {}, {}", new Object[]{current_time,currentOnlineUserCount});
	}
}

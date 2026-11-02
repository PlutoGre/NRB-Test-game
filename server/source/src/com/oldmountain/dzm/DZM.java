package com.oldmountain.dzm;

import java.net.InetSocketAddress;
import java.util.ArrayList;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.TimeUnit;
import java.util.logging.Level;
import java.util.logging.Logger;

import com.oldmountain.dzm.util.LoadDbToRAMUtil;
import org.apache.commons.configuration.XMLConfiguration;
import org.apache.mina.filter.codec.ProtocolCodecFilter;
import org.apache.mina.transport.socket.nio.SocketAcceptor;
import org.apache.mina.transport.socket.nio.SocketAcceptorConfig;
import com.oldmountain.dzm.net.SessionHandler;
import com.oldmountain.dzm.net.coderc.Decoder;
import com.oldmountain.dzm.net.coderc.Encoder;
import com.oldmountain.dzm.net.packet.PacketHandler;
import com.oldmountain.dzm.persist.CheckXml;
import com.oldmountain.dzm.persist.DataLander;
import com.oldmountain.dzm.persist.EntityManager;
import com.oldmountain.dzm.persist.EntityManagerImpl;
import com.oldmountain.dzm.persist.OnlineUserNum;
import com.oldmountain.dzm.persist.PushChatMsg;
import com.oldmountain.dzm.util.XMLTemplateService;


/**
 * 游戏接入口
 * 
 * 负责创建主线程， 初始化网络模块， 初始化持久层
 * 
 */
public class DZM implements Runnable{
	
	private static final Logger logger = Logger.getLogger(DZM.class.getName());
	private static DZM server = null;

	public static final int CURRENT_VERSION = 1;
	public static String resource_version = "";
	public static int canUseGmTools = 0;
	public static String platForm = "";
	public static String serverId = "";
	public static String basePath = "";
	public static String serverStartDate = "";
	public static int totalUserCount = 0;
	public static int totalChallengeCount = 0;
	public static int serverMaxUser = 0;
	public static int serverStat = 0;
	public static int serverCloseTime = 0;
	public static int closeNeedTime = 0;
	public static ArrayList<String> testerArray = new ArrayList<String>();
	
	
	static SocketAcceptor acceptor;
	static EntityManager entityManager;
	public static XMLConfiguration conf;
	static ScheduledExecutorService scheduler = Executors.newScheduledThreadPool(2);
	static ExecutorService threadPool;
	 
	public static EntityManager getEntityManager() {
		return entityManager;
	}
	
	public static ScheduledExecutorService getScheduler(){
		return scheduler;
	}
	
	public static ExecutorService getThreadPool(){
		return threadPool;
	}
	
	public static SocketAcceptor getAcceptor(){
		return acceptor;
	}
	
	public static XMLConfiguration getConf(){
		return conf;
	}
	
	public synchronized static void addChallengeCount(){
		totalChallengeCount ++;
	}
	
	int HeartBeatTime = 80;
	long preTime;
	long currentTime;
	
	public static void main(String[] args) {
		ServerShutDown shutDown = new ServerShutDown();
		try {
			server = new DZM();
			server.Init();
			Thread thread = new Thread(server);
			thread.setName("MainLoop");
			thread.start();
			scheduler.scheduleAtFixedRate(new CheckXml(), 600, 600, TimeUnit.SECONDS);
			scheduler.scheduleAtFixedRate(new DataLander(), 900, 600, TimeUnit.SECONDS);
			scheduler.scheduleAtFixedRate(new OnlineUserNum(), 60, 300, TimeUnit.SECONDS);
			logger.info("server start ok;");
			new Thread(new PushChatMsg()).start();
		} catch (Throwable t) {
			logger.log(Level.SEVERE, "An error occurred while loading the server.", t);
		}
	}
	
	public void Init() throws Exception
	{
		basePath = System.getProperty("user.dir");
		String itemConfPath = DZM.basePath + "/xml/config.xml";
		conf = new XMLConfiguration(itemConfPath);
		platForm = conf.getString("Platform");
		serverId = conf.getString("ServerId");
		canUseGmTools = conf.getInt("CanUseGm");
		resource_version = conf.getString("ResourceVersion");
		
		XMLTemplateService.initTemplateData(0);
		logger.info("Init...");
		entityManager = new EntityManagerImpl();
        LoadDbToRAMUtil.initialize();
		acceptor = new SocketAcceptor();
		SocketAcceptorConfig cfg = new SocketAcceptorConfig();
        cfg.getFilterChain().addLast("codec", new ProtocolCodecFilter(Encoder.class,Decoder.class));
        acceptor.bind( new InetSocketAddress(conf.getString("connection.address"), conf.getInt("connection.port")), new SessionHandler(), cfg);
        threadPool = Executors.newFixedThreadPool(conf.getInt("PoolSize"));
		logger.info("Initializing game...");
	}
	
	

	@Override
	public void run() {
		while(true)
		{
			try {
				currentTime = System.currentTimeMillis();
				//update
				try {
					PacketHandler.Update();
					//逻辑
					
				} catch (InterruptedException e1) {
					
					e1.printStackTrace();
				} 
				
				if( currentTime - preTime < HeartBeatTime )
				{
					try
					{
						Thread.sleep( HeartBeatTime - ( currentTime - preTime ) );
					}
					catch(InterruptedException e)
					{
						logger.info(e.getMessage());
					}
				}
				currentTime = System.currentTimeMillis();
				preTime = currentTime ;
			} catch (Throwable e) {
				e.printStackTrace();
			}
		}
	}

}


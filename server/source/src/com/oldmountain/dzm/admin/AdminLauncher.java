package com.oldmountain.dzm.admin;

import java.io.FileInputStream;
import java.util.Properties;
import java.util.concurrent.Callable;
import com.oldmountain.dzm.DZM;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

/** @brief 启动原游戏服，初始化成功后开放仅本机可用的管理接口。 */
public final class AdminLauncher {
    private static final Logger LOG = LoggerFactory.getLogger(AdminLauncher.class);

    /**
     * 使用原游戏入口和数据库；管理模块初始化失败时退出，避免误报可用。
     * @param args 原游戏参数
     * @throws Exception 启动异常
     */
    public static void main(String[] args) throws Exception {
        try {
            DZM.main(args);
            if (DZM.getThreadPool() == null || DZM.getAcceptor() == null) {
                throw new IllegalStateException("Game initialization did not complete");
            }
            Properties config = new Properties();
            try (FileInputStream in = new FileInputStream("../private/admin.properties")) {
                config.load(in);
            }
            final AdminServer admin = new AdminServer(config);
            AdminTask.call(new Callable<Object>() {
                public Object call() throws Exception { admin.initialize(); return null; }
            }, 30);
            admin.start();
            LOG.info("main: local administration ready, port={}", config.getProperty("port"));
        } catch (Exception e) {
            LOG.error("main: server administration initialization failed", e);
            System.exit(2);
        }
    }
}

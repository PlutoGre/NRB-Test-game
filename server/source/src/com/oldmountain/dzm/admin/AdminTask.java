package com.oldmountain.dzm.admin;

import java.util.concurrent.Callable;
import java.util.concurrent.FutureTask;
import java.util.concurrent.TimeUnit;
import com.oldmountain.dzm.net.packet.Packet;
import com.oldmountain.dzm.net.packet.PacketHandler;

/**
 * @brief 将管理请求排入游戏主循环，避免 HTTP 线程直接改动玩家缓存。
 */
public final class AdminTask extends Packet {
    private final FutureTask<Object> work;

    private AdminTask(Callable<Object> action) {
        super(0, new byte[0]);
        work = new FutureTask<Object>(action);
    }

    /** 在游戏主循环中执行已鉴权的管理任务。 */
    public void execute() { work.run(); }

    /**
     * 提交任务并等待结果；超时的发奖请求必须用原批次号查询或重试。
     * @param action 管理动作
     * @param seconds 等待上限，单位秒
     * @return 管理结果
     * @throws Exception 主循环超时或业务失败
     */
    public static Object call(Callable<Object> action, int seconds) throws Exception {
        AdminTask task = new AdminTask(action);
        PacketHandler.RecvPacket(task);
        return task.work.get(seconds, TimeUnit.SECONDS);
    }
}

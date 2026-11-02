package com.youai;

/**
 * @brief 旧版 Ant 客户端的统一日志入口。
 *
 * 保持 Android 2.3 兼容，不要求迁移 Gradle 或新增日志库。
 */
public final class LogUtils {
    private LogUtils() { }

    /** 输出调试过程。 */
    public static void d(String tag, String message) {
        android.util.Log.d(tag, message);
    }

    /** 输出关键状态变化。 */
    public static void i(String tag, String message) {
        android.util.Log.i(tag, message);
    }

    /** 输出可恢复问题。 */
    public static void w(String tag, String message) {
        android.util.Log.w(tag, message);
    }

    /** 输出异常及其堆栈。 */
    public static void e(String tag, String message, Throwable error) {
        android.util.Log.e(tag, message, error);
    }
}

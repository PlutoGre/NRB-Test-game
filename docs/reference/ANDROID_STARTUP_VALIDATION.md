# Android 启动验证（2026-11-01）

用户报告安智包安装后立即崩溃，异常链为 `AnZhiActivity.<init>` → `GameConfig.<init>` → `GameConfig.loadGameConfig` → `getAssets()`。Activity 构造期间 Context 尚未挂接，读取资源会触发空指针。

已将安智入口的配置初始化移到 `onCreate`，并保持在父类启动游戏状态机之前完成。主源码与渠道副本同步修改，使用现有 `com.youai.LogUtils` 记录初始化阶段。

## 环境和结果

- Windows x64，仓库内自动准备的 SDK Tools 25.2.5、API 21、Build Tools 23.0.3、NDK r8e、Ant 1.10.15，复用服务端 JDK 8。
- `OnePiece_Debug` 模拟器，Android 11 / API 30、x86，提供 `armeabi` / `armeabi-v7a` 转译。
- 本机构建首次受到原源码包残留的 `/cygdrive/...` 依赖缓存影响；已将旧 `obj` 目录备份到 `runtime/android/qa/legacy-native-objects`，之后完整编译两个 ARM 架构并通过 Ant 打包。
- 修复前 APK 安装后复现了用户提供的同一条 Java 异常链；对应进程退出。
- 修复后重新打包、覆盖安装成功；3 次冷启动均进入服务器选择页面，后两次各等待至少 8 秒，进程保持存活。
- 前后台切换后进程未重启，崩溃缓冲中未发现该应用的新异常。
- APK 元数据：包名 `com.youai.dreamonepiece.platform.anzhi`，最低和目标 API 均为 21，包含 `armeabi`、`armeabi-v7a`。

本次验证到服务器选择页面，不代表完整玩法回归或 Android 14/15 实机验证。目标 API 21 在更高安装门槛的系统上仍受限制，见 [打包文档](../CLIENT_BUILD.md)。

## 本机证据和产物

APK 为 `outputs/android/DreamOnePiece-debug.apk`，SHA256：

```text
2a14b2ccea0502a6ecb8399a1ee4be07670c8927b27cdc18388ed985f9d14d60
```

日志、截图和旧测试应用备份位于忽略目录 `runtime/android/qa`，不随仓库提交：

- `baseline-clean-build.log`、`baseline-crash.log`：修复前的构建和崩溃证据。
- `fixed-build.log`、`fixed-first-launch.log`、`verified-config-initialization.log`：修复后的构建与启动日志。
- `repeated-launch.json`、`resume.json`、`fixed-crash.log`：重复冷启动和前后台切换结果。
- `cold-launch-3.png`、`resume.png`：服务器选择页面截图。
- `previous-installed.apk`、`previous-app-data.tgz`、`previous-external-data.tgz`：更换本机测试包之前保留的安装包与数据。

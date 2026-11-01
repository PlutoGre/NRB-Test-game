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

## 雷电 14 原生存储崩溃

随后在用户安装的雷电模拟器 14 上继续验证，关闭了之前启动的官方模拟器。设备指纹为 `Xiaomi/unicorn/unicorn:14/UQ1A.240205.10211507/ucsm.20261021.150701:user/release-keys`，与用户崩溃日志一致，使用 `libhoudini.so` 运行 ARM32 包。

旧 APK 成功复现 `GLThread` 的 `SIGSEGV`，地址为 `0x2c`。完整日志额外提供了 ARM 回溯：`libc.so (fwrite+4)` → `libDreamOnePiece.so (saveFileInPath+44)`。游戏自行拼接的 `/Android/Data/...` 路径在该环境中被拒绝访问；原生代码未检查 `fopen` 结果就调用 `fwrite`。

本次修改：

- Java 使用 `getExternalFilesDir(null)` 获取应用专属目录，实际创建并写入检查文件；失败时回退到 `getFilesDir()`。两个目录均不可写时，在启动页显示错误，停止进入原生流程。
- 修正存储位置后重新准备资源，后续启动复用当前目录；旧路径中的文件不被删除。
- 原生 `saveFileInPath` 检查打开、写入和关闭结果，失败时记录路径与系统错误，不再向 `fwrite` 传递空文件指针。
- 共享渠道准备脚本同步 `GameLogoState`，安智渠道副本与主源码一并更新。

最终 APK 在该雷电实例上覆盖安装成功。连续 3 次冷启动，各等待 10 秒，均保持游戏 Activity 在前台并进入服务器选择页面；前后台切换后 PID 保持不变。崩溃缓冲为空，日志确认正确目录和 `reuseAssets=true`。首次从失败版本升级时也已验证路径切换、资源解压以及磁盘配置文件生成。

## 本机证据和产物

APK 为 `outputs/android/DreamOnePiece-debug.apk`，SHA256：

```text
309c4610fdaa7fff26ca4c0b053de882fa0b9a3d8d5eea87859558906f7f45fc
```

日志、截图和旧测试应用备份位于忽略目录 `runtime/android/qa`，不随仓库提交：

- `baseline-clean-build.log`、`baseline-crash.log`：修复前的构建和崩溃证据。
- `fixed-build.log`、`fixed-first-launch.log`、`verified-config-initialization.log`：修复后的构建与启动日志。
- `repeated-launch.json`、`resume.json`、`fixed-crash.log`：重复冷启动和前后台切换结果。
- `cold-launch-3.png`、`resume.png`：服务器选择页面截图。
- `previous-installed.apk`、`previous-app-data.tgz`、`previous-external-data.tgz`：更换本机测试包之前保留的安装包与数据。

雷电 14 的证据位于 `runtime/android/qa-ldplayer14`：

- `baseline-logcat.log`、`baseline-crash.log`、`baseline.apk`：复现原生崩溃的完整日志与旧包。
- `final-build.log`、`final-logcat.log`、`final-crash.log`：最终构建、运行日志及空崩溃缓冲。
- `final-results.json`、`final-launches.log`、`final-resume.log`：最终 APK 的 3 次冷启动、前后台切换和 SHA256。
- `final-cold-launch-3.png`、`final-resume.png`：雷电上的服务器选择页面截图。

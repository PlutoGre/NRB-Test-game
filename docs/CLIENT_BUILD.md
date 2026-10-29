# 客户端打包流程

先按 [README](../README.md) 完成服务端部署。下面只保留个人测试需要的两条打包路线，不需要制作或上传源码分卷。

| 目标 | 使用方式 | 产物 |
|---|---|---|
| iPhone（iOS 15+） | GitHub Actions 云端 Mac 构建，本机可用 Windows | 未签名 IPA，另需签名安装 |
| Android | Windows 上的旧 SDK + NDK + Ant | 自动使用调试签名的 APK，需要支持 ARM32 的设备 |

## 1. 取得服务器参数

在仓库根目录 PowerShell 执行：

```powershell
$gameRuntime = '.\runtime\server'
Get-Content (Join-Path $gameRuntime 'client-config\deployment.json')
```

若安装时指定了其他运行目录，将 `$gameRuntime` 改成对应路径。下列命令在仓库根目录执行；脚本参数里的相对路径也统一以仓库根目录为基准。记下输出值；后续以 `host=192.168.1.100`、`webPort=18081`、`gamePort=25524`、`resourceVersion=1.78.6` 为例，按自己的输出替换。

## 2. iOS：云端构建并安装

### 2.1 生成 IPA

确认最新代码已经推送到自己的 GitHub 仓库；只在 Windows 本地修改文件不会更新云端构建内容。

1. 打开自己的仓库 → **Actions**。
2. 选择 **Build iOS from repository** → **Run workflow**。
3. 选择需要构建的分支，通常为 `main`，填写下表参数后运行。

| 参数 | 填写内容 |
|---|---|
| `server_host` | `host`，例如 `192.168.1.100`；这是局域网后备地址，不填 HTTPS URL |
| `web_port` | `webPort`，默认 `18081` |
| `game_port` | `gamePort`，默认 `25524` |
| `resource_version` | `resourceVersion`，默认 `1.78.6` |
| `bundle_id` | 自己的应用标识，例如 `com.yourname.nrbgame` |

构建成功后，打开该次运行 → **Summary → Artifacts → ios-unsigned-app**，下载并解压，取得 `onepiece-unsigned-for-resigning.ipa`。失败时下载 **ios-build-logs**，从第一条实际构建错误排查。

找不到工作流时，确认默认分支包含 `.github/workflows/ios-build.yml`、仓库启用了 Actions，且当前账号有运行权限。不要选择旧的 **iOS unsigned build** 快照工作流。

### 2.2 签名并安装到 iPhone

**上一步的 IPA 尚未签名，不能直接安装到普通 iPhone。** Windows 个人测试可按 [AltStore Classic 官方 Windows 教程](https://faq.altstore.io/altstore-classic/how-to-install-altstore-windows) 操作：

1. 在 Windows 按官方教程安装 iTunes、iCloud 和 AltServer。
2. 用 USB 连接并信任 iPhone，通过 AltServer 安装 AltStore Classic。
3. 按工具提示在本机登录自己的 Apple 账号、完成手机端信任或开发者模式设置。
4. 将 IPA 传到 iPhone，在 AltStore Classic 中导入并签名安装。

个人测试签名需要定期刷新，按工具提示处理。不要把 Apple 密码、证书私钥或签名配置提交到仓库。此签名路线还没有用本项目真机验证。

### 2.3 打开游戏

- 手机与电脑在同一局域网：在启动页选择“使用打包时的局域网配置”，允许本地网络访问。
- 手机通过外网连接：按 [外网接入](EXTERNAL_DEPLOYMENT.md) 启动隧道，在启动页填写 HTTPS 域名和邀请码。

局域网 IP 改变后需要用新参数重新打包。外网模式只换临时域名时，强制关闭游戏后重新填写即可，无需重打包。

## 3. Android：本机生成 APK

以下使用安智渠道的本地测试登录。新入口把准备、NDK 编译、Ant 打包串起来；项目仍需要旧工具链，不能只安装最新 Android Studio 就打包。

### 3.1 准备工具

服务端已经自动下载 Python 和 JDK 8，下面直接复用。另行准备：

| 工具 | 示例目录与要求 |
|---|---|
| Apache Ant | 解压到项目 `tools/apache-ant`，应包含 `bin/ant.bat`，使用兼容 JDK 8 的版本 |
| 旧 Android SDK | 解压到 `tools/android-sdk`，须含 `tools/ant/build.xml`、`platforms/android-17/android.jar` 及旧 Build Tools 的 aapt/dx |
| Windows 版 Android NDK r8e | 解压到 `tools/android-ndk-r8e`，应包含 `ndk-build.cmd`；依赖 gnustl 和 armeabi，不能随意换成新版 NDK |

解压后应直接符合表中的目录结构，避免多套一层版本目录。工具不随仓库上传，换电脑需要重新放入或自行复制 `tools`。新入口使用 Windows `ndk-build.cmd`，不要求额外安装 Cygwin；该方式来自 [NDK 官方历史说明](https://android.googlesource.com/platform/ndk/+/refs/heads/tools_r20/docs/CHANGES.html)。

### 3.2 一键生成 APK

双击 **`deployment/build-android.cmd`**，或在仓库根目录执行：

```powershell
.\deployment\build-android.cmd
```

它会读取 `runtime/server/client-config/deployment.json`，生成当前电脑对应的配置，编译原生库，再生成 APK。成功后的统一输出为：

```text
outputs/android/DreamOnePiece-debug.apk
```

打包时会自动选择空闲盘符给旧工具使用，正常退出时释放属于本次构建的映射；源码和产物始终留在项目原目录。不要在构建中途移动项目。

工具或运行目录不同，可传入相对路径，不需要改脚本：

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File .\deployment\Build-Android.ps1 -InstallDir runtime/server-2 -AndroidSdk tools/legacy-sdk -AndroidNdk tools/ndk-r8e -AntDir tools/ant
```

Debug APK 已带调试签名，可传到支持 ARM32 的 Android 手机安装；新版系统可能限制这类旧 App，纯 ARM64 设备无法运行当前包。测试时让手机与服务端电脑处于可互访的局域网，当前 Android 不能直接使用 iOS 的 WSS 临时域名入口。

## 4. 后续更新

代码有改动时，Android 重新执行打包入口；iOS 将改动推送到 GitHub 后重新运行工作流。换开服电脑按 README 重新部署；原电脑 IP 变化时先停服，再用新的 `-PublicHost` 执行部署命令，之后使用对应参数打包。

发布版签名、Mac 本地构建和更多排查步骤放在 [详细参考](reference/CLIENT_BUILD.md)，个人测试不必先阅读。

**新入口、临时盘符映射和中文/空格路径尚未完成新环境实跑；iOS 尚未通过实际 Xcode 构建和 iPhone 验证。本次按项目要求只修改代码和做静态检查，没有执行部署、构建或测试。**

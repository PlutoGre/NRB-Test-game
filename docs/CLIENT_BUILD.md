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

首次部署会避开已占用的端口，实际端口可能与示例不同，务必以 `deployment.json` 为准。新旧服务并存时，旧安装包仍使用旧配置；连接新部署需使用对应参数重新打包。

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

以下使用安智渠道的本地测试登录。支持没有任何 Android 编译环境的 Windows x64 新电脑：先完成服务端部署，再双击 Android 打包入口，脚本会自动准备旧工具链。

### 3.1 首次自动准备工具

复用服务端下载的 Python 和 JDK 8；缺少的下列组件会从 Google/Apache 官方地址下载、校验并解压，不需要安装 Android Studio、Cygwin 或配置系统环境变量：

| 工具 | 自动准备的版本与项目内目录 |
|---|---|
| SDK Tools | 25.2.5，`tools/android-sdk/tools`，包含旧 Ant 构建文件 |
| Android Platform | API 21 r2，`tools/android-sdk/platforms/android-21` |
| Build Tools | 23.0.3，`tools/android-sdk/build-tools/23.0.3`，包含 aapt/dx |
| Platform Tools | 29.0.6，`tools/android-sdk/platform-tools` |
| Android NDK | r8e Windows x64，`tools/android-ndk-r8e`，保留 gnustl 和 armeabi |
| Apache Ant | 1.10.15，`tools/apache-ant` |

首次共需下载约 850 MiB，解压需要数 GB 空间，电脑需能访问 Google 和 Apache 官方下载站。下载缓存位于 `deployment/download-cache`；网络中断后重新运行，会尝试续传。已准备完整的组件会复用，不会每次重新下载。

Windows 拒绝移动刚解压的目录时，脚本会重试，再回退为复制文件；复制中断后重新运行会继续安装。若仍提示 `WinError 5/32/33`，关闭占用该工具目录的程序，并确认项目目录可写，再重试；不必删除下载缓存或已经准备好的 SDK。

版本、地址、大小及校验值固定在 `deployment/android-toolchain.json`。SDK 校验值来自 [Google 官方仓库元数据](https://dl.google.com/android/repository/repository-12.xml)，Ant 使用 [官方 SHA512](https://archive.apache.org/dist/ant/binaries/apache-ant-1.10.15-bin.zip.sha512)，NDK 的 SHA256 由官方 Google HTTPS 压缩包计算。各包自带的许可及 NOTICE 文件会保留。

已有兼容工具也可按上述目录放入，完整组件会直接复用；不会覆盖已有的不完整或无法识别的工具目录。现代 SDK 的 `cmdline-tools` 不等于旧的 `tools/ant`，遇到冲突可用下面的参数指定另一个 SDK 目录。

### 3.2 一键生成 APK

双击 **`deployment/build-android.cmd`**，或在仓库根目录执行：

```powershell
.\deployment\build-android.cmd
```

它会先自动准备工具，再读取 `runtime/server/client-config/deployment.json`，生成当前电脑对应的配置，编译原生库并生成 APK。成功后的统一输出为：

```text
outputs/android/DreamOnePiece-debug.apk
```

打包时会自动选择空闲盘符给旧工具使用，正常退出时释放属于本次构建的映射；源码和产物始终留在项目原目录。不要在构建中途移动项目。

SDK 路径会根据当前项目和工具参数重新生成，并以绝对路径传给所有 Ant 子工程（[Ant 属性传递规则](https://ant.apache.org/manual/Tasks/ant.html)），避免依赖库报 `SDK Location '.../tools/android-sdk' is not valid`。这里的绝对路径由脚本每次计算，无需固定项目目录或盘符；更新代码后重新运行打包入口即可刷新旧配置。

安智资源依赖库的空 `src` 目录由仓库占位文件保留，准备脚本也会自动补齐，避免新克隆报 `AnZhiSdkPro/src does not exist`。NDK 状态提示使用英文，避免旧 Windows 控制台显示中文乱码。

工具或运行目录不同，可传入相对路径，不需要改脚本：

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File .\deployment\Build-Android.ps1 -InstallDir runtime/server-2 -AndroidSdk tools/legacy-sdk -AndroidNdk tools/ndk-r8e -AntDir tools/ant
```

Debug APK 已带调试签名，可传到支持 ARM32 的 Android 手机安装；新版系统可能限制这类旧 App，纯 ARM64 设备无法运行当前包。测试时让手机与服务端电脑处于可互访的局域网，当前 Android 不能直接使用 iOS 的 WSS 临时域名入口。

当前一键入口使用 **minSdkVersion=21、targetSdkVersion=21、编译平台 API 21**：最低系统为 Android 5.0，同时满足报错中要求目标版本至少为 19 的设备。更新后重新打包，脚本会自动补充 API 21 平台，无需删除现有 SDK。NDK r8e 继续使用它支持的原生 API 9 编译旧 C++ 代码，APK 的最低安装版本由 Manifest 的 21 决定。

这次调整不代表已兼容所有新系统：[Android 14](https://developer.android.com/about/versions/14/behavior-changes-all#minimum-target-api-level) 默认要求目标版本至少 23，[Android 15](https://developer.android.com/about/versions/15/behavior-changes-all#minimum-target-sdk-version) 至少 24。在这些系统上普通安装仍可能被拒绝，需要继续提高目标版本并适配运行时权限等行为。

### 3.3 只准备环境或离线使用（可选）

只下载、解压工具，暂不编译：

```powershell
.\deployment\build-android.cmd -PrepareToolsOnly
```

无法连接官方站点时，可从能下载的电脑复制上述 `tools` 目录，或把清单对应的原始 ZIP 放入 `deployment/download-cache`；脚本仍会校验缓存。只使用本地工具/缓存、不联网下载：

```powershell
.\deployment\build-android.cmd -Offline
```

## 4. 后续更新

代码有改动时，Android 重新执行打包入口；iOS 将改动推送到 GitHub 后重新运行工作流。换开服电脑按 README 重新部署；原电脑 IP 变化时先停服，再用新的 `-PublicHost` 执行部署命令，之后使用对应参数打包。

发布版签名、Mac 本地构建和更多排查步骤放在 [详细参考](reference/CLIENT_BUILD.md)，个人测试不必先阅读。

2026-11-01 已在 Windows x64 完成自动准备 Android 工具、NDK 双 ARM 架构编译、Ant 打包和安装验证。已修复 Activity 构造阶段的 Context 空指针，以及雷电 14 上旧存储路径不可写、原生 `fwrite` 使用空文件指针导致的崩溃；资源目录改用系统 API 获取，外部目录不可写时回退到内部目录。Android 11 官方模拟器与雷电 14 均通过重复冷启动和前后台切换检查，验证到服务器选择页面，详见 [启动验证记录](reference/ANDROID_STARTUP_VALIDATION.md)。

本次未验证完整玩法、Android 14/15 实机及中文/空格项目路径；iOS 仍未通过实际 Xcode 构建和 iPhone 验证。

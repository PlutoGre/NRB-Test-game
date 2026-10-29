> 参考归档：日常操作请先看 [一键部署](../../README.md) 和 [客户端打包](../CLIENT_BUILD.md)。本页保留迁移、排查及其他构建方式的详细说明。

# Android 与 iOS 客户端生成流程

## 1. 当前项目状态与路径约定

这不是一个可以直接用最新 Android Studio 打开的 Gradle 项目。游戏使用旧 Cocos2d-x、C++/Lua、Protobuf，Android 构建入口为 NDK + Ant；新 iOS 入口是 `src/client/ios-modern`。

| 平台 | 入口 | 当前范围 |
|---|---|---|
| Android | `src/client/Game/proj.android` 和 `antbuild/anzhi` | ARM32：armeabi、armeabi-v7a；本地登录、公告及打包准备已修改 |
| iOS | `src/client/ios-modern` | ARM64/CMake/Xcode、本地账号、系统桥接及外网 WSS；待首次实际编译和真机验证 |

**本次未运行编译、构建、assemble、测试或 lint。** 本文是交给你执行的操作流程，不代表所有命令已在新电脑通过。此前 `.debug/onepiece/local-offline.apk` 为实际验证过玩法的临时二进制调试包，不是由最新修复源码重新构建的正式交付包。

示例约定：

```text
源码根目录：D:\test\NRB-Test-game
服务端运行目录：D:\OnePieceServer
新电脑局域网 IP：192.168.1.100
资源 HTTP 端口：18081
游戏 TCP 端口：25524
资源版本：1.78.6
```

所有示例 IP、目录和版本都要替换成你实际使用的值。除注明 Cygwin/Mac 外，命令在 Windows PowerShell 中执行。请先按 [服务端文档](SERVER_DEPLOYMENT.md) 完成部署。

**外网 iPhone 测试**：新增的启动连接页可填写临时 HTTPS 域名和邀请码，资源使用 HTTPS、游戏字节流使用 WSS。详见 [本项目外网部署](EXTERNAL_DEPLOYMENT.md)。本文的 IP/端口仍用于局域网后备配置；旧 IPA 和 Android 客户端没有新增的外网传输。首次必须从最新代码重新生成客户端，后续只换域名不需重打包。

## 2. 先统一客户端地址与资源版本

服务端工具生成：

```text
D:\OnePieceServer\client-config\deployment.json
D:\OnePieceServer\client-config\version_android.cfg
D:\OnePieceServer\client-config\version_ios_local.cfg
D:\OnePieceServer\client-config\game.properties
```

先打开 `deployment.json`，记下实际 `host`、`webPort`、`gamePort`、`resourceVersion`。Android APK、iOS 包和服务器列表应使用同一套部署参数。

首次部署已经生成安智渠道的 `deployment` 配置。只想重新生成配置时，在源码根目录执行下面的工具；它不编译、不启动服务：

```powershell
python .\src\tools\configure_deployment.py `
  --host 192.168.1.100 --web-port 18081 --game-port 25524 `
  --resource-version 1.78.6 --channel anzhi --local-login `
  --web-root D:\OnePieceServer\web
```

`web-root` 必须是独立空目录或工具之前创建的目录，不要指向源码根目录或旧网站根目录。若游戏服资源版本也需修改，按服务端文档停止后重新部署，或显式使用 `--server-dir` 指向正确的运行服目录并重启。

新生成的服务器列表在：

```text
http://192.168.1.100:18081/gameserver/dzm/1.8/servers_list.php
```

尽管扩展名是 `.php`，这里由工具生成的是 JSON 文本，静态 HTTP 服务即可提供，不需要安装旧 PHP 管理网站。

手机真机使用电脑局域网 IP。Android 官方模拟器访问宿主机通常用 `10.0.2.2`；该地址不能写进 iPhone 或普通 Android 真机配置。iOS 准备脚本会拒绝这个别名。

## 3. Android：准备旧构建环境

### 3.1 工具清单

| 工具 | 要求与原因 |
|---|---|
| JDK 8 x64 | 旧 Java/Ant 工程使用 Java 6 级别源码；不要直接用 JDK 17 编译旧 Ant 工程 |
| Python 3.10+ | 公共源码、部署覆盖文件和 ABI 准备工具 |
| Apache Ant | 建议使用兼容 JDK 8 的 Ant 1.9.x/1.10.x |
| 旧 Android SDK | 要包含 `tools/ant/build.xml`、`platforms/android-17/android.jar` |
| 旧 SDK Build Tools | 需含旧构建所需的 aapt、dx 等；可使用与旧 ADT 配套的工具，或 SDK Tools 25.2.x 配套 Build Tools 19.x/23.x |
| Android NDK | 原项目说明指定 r8e，使用对应 Windows 版本；需要 gnustl 和 armeabi 支持 |
| Cygwin | 原项目使用 Bash、make 等旧工具链；至少有 bash、coreutils、sed、make、unzip |
| 新版 platform-tools | 单独用于 adb 安装、日志，不替换旧 Ant/NDK 工具链 |

原项目推荐的是 2013 年 ADT + NDK r8e。可以复用已有的配套工具包，但不要仅安装最新 Android Studio 后假定旧 Ant 支持也存在。Android SDK Tools 25.3.0 起移除了旧工具；NDK r17 移除了 armeabi，r18 移除了 gnustl，所以任意替换为新 NDK 都会产生额外迁移任务。[SDK 说明](https://developer.android.com/tools/releases/sdk-tools)、[NDK 历史](https://developer.android.com/ndk/downloads/revision_history)

旧工具下载参考 [Android NDK 官方历史下载](https://github.com/android/ndk/wiki/Unsupported-Downloads)。不要把第三方修改版工具当作已验证构建环境。

目录示例：

```text
D:\Tools\jdk8
D:\Tools\apache-ant
D:\Tools\AndroidLegacySDK
D:\Tools\android-ndk-r8e
C:\cygwin64
D:\Tools\AndroidCurrentSDK\platform-tools
```

建议旧 C++ 构建使用没有中文和空格的源码路径，例如 `D:\test\NRB-Test-game`。把现有中文目录完整复制到该路径即可，不要在原目录中批量删除构建产物。

服务端一键工具的 JDK 位置是 `D:\OnePieceServer\runtimes\java`，可以用作下方 `JAVA_HOME`。其便携 Python 也可以显式调用；如用普通 `python` 命令，确保已安装正常 Python 3。

### 3.2 设置当前终端环境

```powershell
Set-Location D:\test\NRB-Test-game
$env:JAVA_HOME = 'D:\OnePieceServer\runtimes\java'
$env:ANT_HOME = 'D:\Tools\apache-ant'
$env:ANDROID_HOME = 'D:\Tools\AndroidLegacySDK'
$env:PATH = "$env:JAVA_HOME\bin;$env:ANT_HOME\bin;$env:ANDROID_HOME\tools;$env:PATH"
```

这些只是当前终端变量，不会修改整个 Windows 的系统 Java。用文件管理器确认旧 SDK 的 `tools/ant/build.xml` 和 `platforms/android-17/android.jar` 确实存在。

## 4. Android：准备渠道、构建原生库、生成 APK

本文以安智渠道为稳定的操作入口，但启用 `localDebugLogin=true`，不连接旧渠道账号服务。其他渠道保留原 SDK 依赖，不能仅改文件夹名就保证能构建。

### 4.1 准备源码和资源

先完成第 2 节的服务器配置，再运行：

```powershell
python .\deployment\prepare_android.py --sdk D:\Tools\AndroidLegacySDK --skip-native-sync
```

它会更新安智渠道已有的公共 Java 源码、资源、JAR、旧 SDK 位置与 library 引用，并保存被改写的 SDK/项目配置的 `.before-portable` 备份。它不会复制所有其他渠道源码，不会编译，也不会签名。

### 4.2 用 NDK 构建游戏 `.so`

打开 Cygwin Bash。在 Bash 内执行，路径需使用 Cygwin 形式：

```bash
cd /cygdrive/d/test/NRB-Test-game/src/client/Game/proj.android
export NDK_ROOT=/cygdrive/d/Tools/android-ndk-r8e
export CLIENT_ROOT="$(cd ../.. && pwd)"
export NDK_MODULE_PATH="$CLIENT_ROOT:$CLIENT_ROOT/cocos2dx/platform/third_party/android/prebuilt"
"$NDK_ROOT/ndk-build" -C "$PWD" -j2
```

项目 `jni/Application.mk` 当前指定：

```make
APP_STL := gnustl_static
APP_ABI := armeabi armeabi-v7a
APP_PLATFORM := android-9
```

成功后应生成：

```text
src/client/Game/proj.android/libs/armeabi/libDreamOnePiece.so
src/client/Game/proj.android/libs/armeabi-v7a/libDreamOnePiece.so
```

不要直接使用 `build_native.sh` 中残留的旧开发者绝对资源路径。本流程直接调用 NDK，资源由准备工具复制。若出现 r8e 自身兼容问题，保留第一条实际编译错误再处理；不能把只生成新 APK 外壳、却沿用旧 `.so` 的结果当作完成了 C++ 修复构建。

### 4.3 同步新原生库与部署覆盖文件

回到 PowerShell 源码根目录：

```powershell
python .\deployment\prepare_android.py --sdk D:\Tools\AndroidLegacySDK
```

这个步骤会调用原有 `prepare_channel.py`，最后应用安智 `deployment` 下的地址和本地登录配置，并同步根工程刚编译的 `.so`。Ant 的 `-pre-build` 也会再次执行公共准备步骤。

若有 ABI 目录只含支付 SDK 的 `.so`、没有游戏主库，会报错或对 armeabi-v7a 使用通用 ARM 回退。**armeabi 的库不能复制到 arm64-v8a/x86 目录来冒充对应架构。**

### 4.4 生成本地调试 APK

```powershell
Set-Location D:\test\NRB-Test-game\src\client\Game\proj.android\antbuild\anzhi
ant "-Ddeployment.python=C:/你的Python目录/python.exe" debug
```

把 Python 路径替换为实际 Python 3 可执行文件，也可以用：

```powershell
ant "-Ddeployment.python=D:/test/NRB-Test-game/deployment/.bootstrap/python/python.exe" debug
```

第二种方式要求已经执行过服务端入口、解压便携 Python。参数使用正斜杠，避免 Java properties 路径转义问题。

成功后的 APK 在该渠道 `bin` 目录，通常为 `DreamOnePiece-debug.apk`；以 Ant 实际输出文件名为准。调试包用本机调试签名，不需要 Apple 账号或外购证书。

### 4.5 自己签名的 Release APK

先生成你自己的 JKS，妥善保留，后续覆盖安装应继续使用同一把密钥：

```powershell
New-Item -ItemType Directory -Path D:\Keys -Force
& "$env:JAVA_HOME\bin\keytool.exe" -genkeypair -v `
  -keystore D:\Keys\onepiece-release.jks -storetype JKS -alias onepiece `
  -keyalg RSA -keysize 2048 -validity 10000
```

按终端提示在本机输入密码。不要继续使用仓库中已附带且密码公开的旧渠道签名。

安智目录的 `ANT.properties` 原本带有旧的 `key.store`、`key.alias` 和两个密码属性。先备份此文件，再删除/注释这四条签名属性；使 `ant release` 生成未签名 Release APK。不要把自己的密码提交到仓库。

```powershell
ant "-Ddeployment.python=D:/test/NRB-Test-game/deployment/.bootstrap/python/python.exe" release
```

找到 `bin` 中实际生成的 `*-release-unsigned.apk`。然后在另一个终端使用你安装的新版 Android Build Tools 签名工具；例如工具位于 `D:\Tools\AndroidCurrentSDK\build-tools\35.0.0` 时，使用其要求的 JDK：

```powershell
$env:JAVA_HOME = 'D:\Tools\jdk17'
$env:PATH = "$env:JAVA_HOME\bin;$env:PATH"
New-Item -ItemType Directory -Path D:\BuildOutput -Force
& 'D:\Tools\AndroidCurrentSDK\build-tools\35.0.0\zipalign.exe' -f -v 4 `
  '.\bin\DreamOnePiece-release-unsigned.apk' 'D:\BuildOutput\onepiece-aligned.apk'
& 'D:\Tools\AndroidCurrentSDK\build-tools\35.0.0\apksigner.bat' sign `
  --ks D:\Keys\onepiece-release.jks --ks-key-alias onepiece `
  --out D:\BuildOutput\onepiece-release.apk D:\BuildOutput\onepiece-aligned.apk
```

根据提示输入密钥密码；先 zipalign，再 apksigner，签名后不要再修改 APK 内容。[官方签名工具说明](https://developer.android.com/tools/apksigner)

这里的新版 Build Tools 只做 APK 后处理，不用于替换前面旧 Ant/NDK 的编译环境。

## 5. Android：安装与常见失败

使用支持 ARM32 的 Android 设备，或确认具备 ARM32 翻译能力的模拟器。64 位系统不一定支持 32 位 App。前面使用过 Android 11 x86 模拟器的 ARM 翻译能力；普通 x86_64 模拟器不保证可以运行 ARM 库。

```powershell
& 'D:\Tools\AndroidCurrentSDK\platform-tools\adb.exe' devices
& 'D:\Tools\AndroidCurrentSDK\platform-tools\adb.exe' -s 目标设备序列号 install -r D:\BuildOutput\onepiece-release.apk
```

有多个设备时必须指定 `-s`，避免操作无关设备。安智包名是 `com.youai.dreamonepiece.platform.anzhi`。

| 错误/现象 | 说明 |
|---|---|
| `INSTALL_FAILED_NO_MATCHING_ABIS` | 目标不支持所打包的原生架构；当前 Android 尚未适配 ARM64 |
| `INSTALL_FAILED_UPDATE_INCOMPATIBLE` | 已安装包签名不同；用原密钥重签，或先备份数据再决定是否卸载，卸载可能丢本地账号身份 |
| `INSTALL_FAILED_DEPRECATED_SDK_VERSION` | targetSdk 太旧；这是后续 Android 平台升级任务，不是资源地址问题 |
| `UnsatisfiedLinkError` | 所选 ABI 缺游戏库或依赖库；检查新 `.so` 是否完整复制 |
| `tools/ant/build.xml` 不存在 | SDK 太新或路径错误，需要旧 Ant 支持 |
| `source option 6 is no longer supported` | Ant 编译终端误用了新 JDK，改回 JDK 8 |
| `unknown module` | 检查 NDK_MODULE_PATH 和完整 client 目录是否复制 |
| 仍跳渠道登录/只有旧页面 | 检查最终 assets 中的覆盖配置，确认实际安装了新编译 APK，而不是 bin 里的旧包 |

在自己的开发设备上诊断低 targetSdk 安装限制时，新版 adb 可按系统支持情况使用：

```powershell
adb -s 目标设备序列号 install --bypass-low-target-sdk-block -r D:\BuildOutput\onepiece-release.apk
```

这不是 ARM64 兼容修复，也不代表满足应用商店的 targetSdk 要求。[Android 平台说明](https://developer.android.com/about/versions/15/behavior-changes-all)

## 6. iOS：仅有 Windows/Ubuntu 时使用 GitHub Actions

GitHub 提供临时 macOS runner，运行 Xcode 编译并上传日志和产物。你本机继续使用 Windows/Ubuntu；首次无签名编译不需要 Apple 开发者会员。

### 完整项目仓库：优先使用直接构建

如果上传的是本项目完整仓库，根目录已经包含 `.github/workflows/ios-build.yml`。直接进入 **Actions → Build iOS from repository → Run workflow**，填写部署参数即可；不需要准备或上传分卷 ZIP。

使用外网模式时，`server_host` 仍填写局域网后备 IP，临时 HTTPS 域名和邀请码在安装后输入 App 启动页，不放入 GitHub 参数或资源配置。域名变化后强制关闭 App 再打开；资源版本保持与游戏服一致。

完整的首次上传、clone 和直接构建操作见 [完整仓库说明](GITHUB_REPOSITORY.md)。下面 6.1～6.4 保留的是“只上传 iOS 精简快照”的可选方案，其工作流名为 **iOS unsigned build**，不要与完整仓库工作流混淆。

### 6.1 可选：生成精简快照上传目录

在源码根目录执行：

```powershell
python .\deployment\prepare_github_upload.py `
  --host 192.168.1.100 --web-port 18081 --game-port 25524 --resource-version 1.78.6
```

它顺序执行 iOS 资源/UTF-8 源码准备、源码包生成、20 MiB 分卷拆分，并复制当前工作流。**它不上传 GitHub、不编译、不签名。**

输出：

```text
deployment/github-upload/
├─ .github/workflows/ios-build.yml
├─ CLIENT_BUILD.md
└─ source/
   ├─ onepiece-ios-source.zip.001
   ├─ onepiece-ios-source.zip.002
   ├─ onepiece-ios-source.zip.003
   └─ onepiece-ios-source.zip.sha256
```

分卷数量以实际生成结果为准，源码变化后可能不止三个。脚本会清理其管理输出中多余的旧分卷。

不要继续上传早期 `.debug/ios-github-actions` 的旧快照；当前可维护的模板在 `deployment/github/ios-build.yml`。每次修改源码后，都要重新运行生成脚本并替换远程分卷及 SHA256 文件。

### 6.2 创建仓库与网页上传

1. 登录 GitHub，右上角 **+ → New repository**。
2. 仓库名例如 `onepiece-ios-build`，选择 **Private**，勾选创建 README。
3. **Code → Add file → Upload files**，把 `github-upload/source` 文件夹整体拖入，提交到默认分支 `main`。
4. **Add file → Create new file**，文件名填 `.github/workflows/ios-build.yml`。
5. 用文本编辑器打开本地生成的同名文件，复制全部内容，保留缩进，提交到默认分支。

GitHub 网页单文件上传限制 25 MiB，所以已拆成每个不超过 20 MiB 的文件。不需要把整个手游源码目录或 `.debug` 数据库上传。[上传限制](https://docs.github.com/en/repositories/working-with-files/managing-files/adding-a-file-to-a-repository)

### 6.3 手动运行

1. 点击仓库顶部 **Actions**。
2. 左侧选择 **iOS unsigned build**。
3. 点击 **Run workflow**，选择 `main`。
4. 填入实际 `server_host`、`web_port`、`game_port`、`resource_version`。
5. 点击绿色运行按钮，再进入运行记录查看 `build` 的步骤日志。

工作流仅手动触发，不会每次提交就自动消耗构建分钟数。没有按钮时检查：YAML 是否位于正确目录、是否提交到默认分支、Actions 是否启用，以及账号是否有写权限。[手动运行说明](https://docs.github.com/en/actions/how-tos/manage-workflow-runs/manually-run-a-workflow)

云机器只把地址写进客户端，不需要连接你家里的 `192.168.*` 游戏服。因此编译阶段不需要内网穿透；安装完成后，由手机连接局域网电脑。

私有仓库的托管分钟数和存储有额度，超额可能收费。工作流使用标准 `macos-15`、45 分钟超时、产物保留 7 天；运行前在 GitHub 账号 Billing 中确认预算。[计费说明](https://docs.github.com/en/actions/concepts/billing-and-usage)

### 6.4 下载结果

运行结束后回到 Summary，下方 Artifacts：

| 产物 | 内容 |
|---|---|
| `ios-build-logs` | Xcode/Python/CMake 环境、构建日志和成功结果记录；失败时也尽量上传 |
| `ios-unsigned-app` | 编译成功后包含 `.app` 压缩包、供重签使用的未签名 `.ipa`，以及未签名说明 |

文件名 `onepiece-unsigned-for-resigning.ipa` 中的 **unsigned 表示尚未签名**。有 IPA 扩展名仍不能直接装到普通 iPhone。首轮构建也可能因旧引擎兼容错误失败，应先下载日志继续修正，不能保证第一次运行就成功。

## 7. iOS：在 Mac 本地或远程 SSH 构建

也可把完整项目或 `src/client/ios-modern/dist/onepiece-ios-source.zip` 交给一台可用 Mac。需要完整 Xcode、Python 3.10+、CMake 3.24+。打开 Xcode 完成首次初始化；命令行工具应指向完整 Xcode。

在 Mac 的 `src/client/ios-modern` 内执行：

```bash
python3 prepare.py --host 192.168.1.100 --web-port 18081 --game-port 25524 --resource-version 1.78.6
python3 build.py --action configure --bundle-id com.yourname.onepiece
python3 build.py --action build --bundle-id com.yourname.onepiece
```

需要生成 Xcode 工程就停在 configure，工程为：

```text
generated/build-iphoneos/OnePieceIOS.xcodeproj
Scheme: OnePiece
```

无签名构建输出位于 `generated/build-iphoneos/DerivedData/Build/Products/Debug-iphoneos`。模拟器另用：

```bash
python3 build.py --action build --sdk iphonesimulator --bundle-id com.yourname.onepiece
```

**不要用旧 `xcode/workspace.xcworkspace`、Game91/PP/TB 或 `Game/proj.ios` 模板构建本适配版本。** 当前新入口从源码重建依赖，不链接旧 ARM32 渠道库。渲染仍是旧 OpenGL ES 2；未迁移 Metal。

## 8. iOS：签名和安装到自己的 iPhone

### 8.1 免费 Apple 账号 + Windows 个人测试

适合先在自己的手机验证，不必立即购买开发者会员：

1. 先让第 6 节云构建通过，下载新的 `ios-unsigned-app`，解压出未签名 IPA。
2. 按 [AltStore Classic 官方 Windows 教程](https://faq.altstore.io/altstore-classic/how-to-install-altstore-windows) 安装其要求的 Apple 组件和 AltServer。
3. USB 连接 iPhone，解锁并信任电脑，在 AltServer 选择安装 AltStore Classic。
4. 在工具本机界面使用自己的 Apple 账号，按手机提示完成信任/开发者模式设置。
5. 将 IPA 传到手机，在 AltStore Classic 导入，由它完成个人测试签名和安装；电脑 AltServer 保持可访问。

免费个人签名通常 7 天到期，需要刷新，且有应用数量限制。[AltStore 说明](https://faq.altstore.io/altstore-classic/your-altstore)

这个路线尚未用本项目实际验证；如果重签报错，需要结合工具错误和最终 App 的签名/权限检查继续处理。不要把 Apple 密码、私钥或会话令牌发到聊天、写入 YAML 或放入仓库。

### 8.2 有 Mac，使用普通 Apple ID

在 Xcode 打开生成的工程，选择 OnePiece Target → Signing & Capabilities：

- 勾选 Automatically manage signing。
- 选择自己的 Personal Team。
- 修改 Bundle Identifier 为自己唯一的标识。
- 连接 iPhone，选择该设备后 Run。

普通账号有期限和功能限制；这不是可分发给任意手机的正式安装包。再次运行 build.py 会按其参数重新生成签名设置，注意保留自己的选择。

### 8.3 付费开发者账号与签名 IPA

长期开发、TestFlight 或上架可考虑 Apple Developer Program。费用和权限以 [Apple 官方说明](https://developer.apple.com/programs/whats-included/) 为准。开发/Ad Hoc 测试还需要把目标设备加入相应描述文件。

典型签名材料：

| 材料 | 用途 |
|---|---|
| Team ID | 指定 Apple 开发者团队 |
| Bundle ID/App ID | 标识该应用，需与描述文件匹配 |
| 签名证书和对应私钥（常见为 `.p12`） | 为安装包签名 |
| `.mobileprovision` | 约束应用、权限、证书、设备及有效期 |

在已经配置好账号和签名环境的 Mac 上可使用：

```bash
python3 build.py --action export --sign --team-id ABCDE12345 --bundle-id com.yourname.onepiece
```

替换 Team ID 和 Bundle ID，签名 IPA 在 `generated/build-iphoneos/ipa`。`--configuration Release` 可生成 Release 归档；`--action archive` 只归档。

**当前 GitHub Actions 模板只做无签名构建。** 它没有 Apple 登录环境，不能只填写 Team ID 就自动变成已签名构建。若要在 GitHub 签名，需要新增导入临时钥匙串、证书/描述文件和匹配导出选项的步骤；材料通过 Secrets 传入，而非提交到仓库。[GitHub 签名文档](https://docs.github.com/en/actions/how-tos/deploy/deploy-to-third-party-platforms/sign-xcode-applications)

## 9. 新电脑上最后需要验证的流程

这些步骤由你在构建成功、安装后执行；本次没有自动操作手机或模拟器：

1. 手机能访问服务器列表 URL，游戏 TCP 端口也可达。
2. 冷启动、资源版本检查、本地账号登录、创建角色和新手流程正常。
3. 远航进入战斗、胜利结算、奖励和角色状态保存。
4. 满足条件后进入勇闯；新账号未解锁时不能据此判断玩法缺失。
5. 退出重进、断网重连、前后台切换，公告关闭/失败后不留下透明遮罩。
6. iOS 首次允许局域网访问；检查刘海/安全区域、触摸位置、音频。
7. Android 检查安装目标的 ABI、实际打包的新 `.so` 和 Java 修复是否生效。

默认本地登录不支持旧渠道充值，不会伪造支付成功；Android 与 iOS 的本地账号身份不同。保留玩家数据迁移时还要保留客户端本地身份、原包签名和数据库备份，见 [服务端迁移说明](SERVER_DEPLOYMENT.md)。

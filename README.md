# NRB-Test-game 使用说明

换一台电脑后，在任意有写权限的目录克隆仓库并原地部署，再按 [客户端打包流程](docs/CLIENT_BUILD.md) 生成 Android 或 iOS 客户端。

项目已按客户端、服务端、公共资源、脚本、文档和历史资料分目录维护，入口见 [项目目录说明](docs/PROJECT_STRUCTURE.md)；全部文档见 [文档索引](docs/README.md)。

适用环境：联网的 Windows 10/11 x64。服务端自动下载 Python、Java 和 MariaDB；Android 首次打包自动准备旧版 SDK、NDK 和 Ant，无需预装 Android Studio；iOS 使用 GitHub Actions 云端 Mac 构建。

## 1. 克隆仓库

安装 [Git for Windows](https://git-scm.com/download/win)，在 PowerShell 执行：

```powershell
git clone https://github.com/PlutoGre/NRB-Test-game.git
Set-Location .\NRB-Test-game
```

无需指定盘符或修改脚本中的工程路径。服务端和 Android 新入口会自动给旧工具分配兼容盘符，处理工程父目录中的中文和空格；数据仍留在原目录。这些路径场景仍待实际运行验证。

## 2. 一键部署服务端

双击项目根目录的 **`启动服务.cmd`**，或在仓库根目录运行：

```powershell
.\启动服务.cmd
```

首次会下载运行时、初始化数据库并启动游戏服；已经部署过则直接启动已有实例，保留配置和玩家数据。`deployment/server.cmd` 保留为手动部署入口。

默认仅配置一个区服：**男人帮1区**（区服 ID 为1）。已有部署的旧单区服 `Local` 会在下一次点击“启动服务.cmd”时备份列表并自动改名，原角色和连接地址保留；完全退出客户端后重开即可获取新名称，无需重新打包。手工设置的其他区服列表或名称不会自动覆盖。

首次安装优先使用资源 `18081`、游戏 `25524`、数据库 `13306`；被占用时自动选空闲端口，不会停止原有服务。选定端口写入部署记录，后续一键启动会沿用，不会每次改变。显式传入的端口保持固定，冲突时仍会报错。

默认运行目录为项目内的 `runtime/server`，数据库、日志、备份和资源都放在这里。安装完成后，在 PowerShell 查看客户端打包需要的参数：

```powershell
Get-Content .\runtime\server\client-config\deployment.json
```

记下 `host`、`webPort`、`gamePort`、`resourceVersion`，接着打开 **[客户端打包流程](docs/CLIENT_BUILD.md)**。

网页客户端新增独立入口：先运行 `deployment/build-web.cmd` 构建，再运行 `deployment/start-web.cmd`，浏览器访问窗口显示的网址。也可在 GitHub Actions 手动运行 **Build Web client**，下载产物放到 `outputs/web` 后启动。详见 [网页客户端流程及验证状态](docs/WEB_CLIENT.md)。本次未执行 Web 编译或浏览器验证。

### 仅在部署遇到问题时使用

| 情况 | 处理 |
|---|---|
| 多网卡，无法自动选 IP | 用下面的命令显式指定本机局域网 IP |
| 需指定另一套运行目录 | 同一命令加 `-InstallDir runtime/server-2`；后续管理命令也加相同参数 |
| 端口占用 | 首次未指定端口时会自动避开；已有部署或指定端口冲突时，停服后用 `-WebPort 18091 -GamePort 25534 -DatabasePort 13316` 调整 |
| 手机在同一 Wi-Fi 下仍无法连接 | 确认电脑和手机可互访；管理员 PowerShell 中执行同一命令并加 `-OpenFirewall` |
| 下载或进程启动失败 | 查看运行目录 `logs`；缺少 VC++ DLL 时安装微软对应的 x64 运行库 |

显式指定 IP 的基础命令，按需添加上表参数：

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File .\deployment\Deploy-Server.ps1 -PublicHost 192.168.1.100
```

## 3. 日常启动和关闭

直接双击项目根目录的两个文件即可，启动和停止都使用本项目的 `runtime/server`：

| 文件 | 作用 |
|---|---|
| `启动服务.cmd` | 首次自动部署，以后启动已有服务；重复点击不会多开同一实例 |
| `停止服务.cmd` | 停止当前实例的游戏服、资源服务、数据库及外网入口，保留玩家数据 |

窗口显示“请按任意键继续”后可以关闭；启动的服务在后台运行，关闭启动窗口不会停服。查看状态仍可在项目根目录执行：

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File .\deployment\Deploy-Server.ps1 -Action status
```

默认是局域网连接。iPhone 如需在外地或用移动流量访问，安装新客户端后按 [外网接入简明步骤](docs/EXTERNAL_DEPLOYMENT.md) 操作；Android 当前仍使用直接 TCP 连接。

所有入口脚本的相对路径参数均以**仓库根目录**为基准，支持显式绝对路径。常用目录：

| 相对项目目录 | 用途 |
|---|---|
| `client/source` | 共享游戏源码、Android 工程、旧 Cocos 引擎和依赖 |
| `client/ios` | 当前 iOS 平台实现、准备与构建脚本 |
| `client/web` | 网页平台接口、画布宿主页、WebAssembly 构建脚本 |
| `server/source`、`server/config` | Java 源码及服务端配置模板 |
| `server/distribution`、`server/database` | 部署使用的基础 JAR 和初始 SQL |
| `resources/client` | 游戏公共资源的唯一发布源 |
| `scripts`、`deployment` | 共享脚本、一键部署和客户端构建入口 |
| `archive`、`docs` | 历史资料及维护文档 |
| `runtime/server` | 服务端、数据库、日志、备份和客户端连接参数 |
| `tools` | Android 首次打包自动下载的 SDK、NDK、Ant，也可放入已有的兼容工具 |
| `outputs/android` | Android 新入口输出的 APK |
| `outputs/web` | 网页构建产物，使用网页网关提供访问 |
| `client/ios/generated` | iOS 生成文件；云构建成功后从 Actions 下载 IPA |

运行数据、便携构建工具和客户端构建产物均被 Git 忽略；管理 EXE 和配套 JAR 随仓库提供。服务端会在运行期间保留自己的盘符映射，完整停服后释放；不要手动删除运行中的映射。移动整个项目之前先停止服务和构建；启动时会按新位置重写数据库路径，客户端 IP 改变时仍需更新部署参数。旧版本放在 `%LOCALAPPDATA%\OnePieceServer` 的数据不会自动迁移，管理旧实例时显式传入它的 `-InstallDir`，迁移方法见下面的参考说明。

克隆仓库只会得到初始数据，旧电脑的玩家进度需另行迁移。备份恢复等低频操作保存在 [服务端参考说明](docs/reference/SERVER_DEPLOYMENT.md)。

后续修改游戏内容，阅读 [代码逻辑与扩展指南](docs/extension/README.md)：包含服务端保存与发奖、资源替换、卡牌和缘分、装备、奥义、船舱与修行六个专题，以及完整模板清单。

发放测试物品：服务启动后双击根目录 **NRB游戏管理.exe**，账号 `op_root`、密码 `hy1212`。支持按完整游戏名或全服现有角色发送传书附件，详见 [Windows 管理工具](docs/ADMIN_TOOL.md)。已有部署更新后先停服再启动一次，客户端无需重新打包。

**验证状态：目录整理前已通过 iOS 云端编译、链接和 IPA 打包。本次目录调整仅作文件和路径静态检查，未重新部署或构建；新布局及其他未验证环境仍需按文档实际确认。**

# NRB-Test-game 使用说明

换一台电脑后，在任意有写权限的目录克隆仓库并原地部署，再按 [客户端打包流程](docs/CLIENT_BUILD.md) 生成 Android 或 iOS 客户端。

适用环境：联网的 Windows 10/11 x64。服务端自动下载 Python、Java 和 MariaDB；Android 打包另外需要旧工具链，iOS 使用 GitHub Actions 云端 Mac 构建。

## 1. 克隆仓库

安装 [Git for Windows](https://git-scm.com/download/win)，在 PowerShell 执行：

```powershell
git clone https://github.com/PlutoGre/NRB-Test-game.git
Set-Location .\NRB-Test-game
```

无需指定盘符或修改脚本中的工程路径。服务端和 Android 新入口会自动给旧工具分配兼容盘符，处理工程父目录中的中文和空格；数据仍留在原目录。这些路径场景仍待实际运行验证。

## 2. 一键部署服务端

双击仓库里的 **`deployment/server.cmd`**，或在仓库根目录运行：

```powershell
.\deployment\server.cmd
```

首次会下载运行时、初始化数据库并启动游戏服；再次用相同配置运行会启动已有部署，保留玩家数据。

首次安装优先使用资源 `18081`、游戏 `25524`、数据库 `13306`；被占用时自动选空闲端口，不会停止原有服务。选定端口写入部署记录，后续一键启动会沿用，不会每次改变。显式传入的端口保持固定，冲突时仍会报错。

默认运行目录为项目内的 `runtime/server`，数据库、日志、备份和资源都放在这里。安装完成后，在 PowerShell 查看客户端打包需要的参数：

```powershell
Get-Content .\runtime\server\client-config\deployment.json
```

记下 `host`、`webPort`、`gamePort`、`resourceVersion`，接着打开 **[客户端打包流程](docs/CLIENT_BUILD.md)**。

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

以下使用默认运行目录，在仓库根目录执行；也可从其他目录通过脚本完整路径调用，脚本会自行定位所属项目：

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File .\deployment\Deploy-Server.ps1 -Action start
powershell -NoProfile -ExecutionPolicy Bypass -File .\deployment\Deploy-Server.ps1 -Action status
powershell -NoProfile -ExecutionPolicy Bypass -File .\deployment\Deploy-Server.ps1 -Action stop
```

默认是局域网连接。iPhone 如需在外地或用移动流量访问，安装新客户端后按 [外网接入简明步骤](docs/EXTERNAL_DEPLOYMENT.md) 操作；Android 当前仍使用直接 TCP 连接。

所有入口脚本的相对路径参数均以**仓库根目录**为基准，支持显式绝对路径。常用目录：

| 相对项目目录 | 用途 |
|---|---|
| `runtime/server` | 服务端、数据库、日志、备份和客户端连接参数 |
| `tools` | 自行解压的 Android SDK、NDK、Ant |
| `outputs/android` | Android 新入口输出的 APK |
| `src/client/ios-modern/generated` | iOS 生成文件；云构建成功后从 Actions 下载 IPA |

运行数据、工具和产物均被 Git 忽略。服务端会在运行期间保留自己的盘符映射，完整停服后释放；不要手动删除运行中的映射。移动整个项目之前先停止服务和构建；启动时会按新位置重写数据库路径，客户端 IP 改变时仍需更新部署参数。旧版本放在 `%LOCALAPPDATA%\OnePieceServer` 的数据不会自动迁移，管理旧实例时显式传入它的 `-InstallDir`，迁移方法见下面的参考说明。

克隆仓库只会得到初始数据，旧电脑的玩家进度需另行迁移。备份恢复等低频操作保存在 [服务端参考说明](docs/reference/SERVER_DEPLOYMENT.md)。

**验证状态：新电脑部署、外网链路及 iOS 构建/真机运行仍待实际验证。文档中的步骤不代表已全部实跑通过。**

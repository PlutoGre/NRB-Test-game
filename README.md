# NRB-Test-game 使用说明

换一台电脑后，按本页部署服务端，再按 [客户端打包流程](docs/CLIENT_BUILD.md) 生成 Android 或 iOS 客户端。

适用环境：联网的 Windows 10/11 x64。服务端自动下载 Python、Java 和 MariaDB；Android 打包另外需要旧工具链，iOS 使用 GitHub Actions 云端 Mac 构建。

## 1. 克隆仓库

安装 [Git for Windows](https://git-scm.com/download/win)，在 PowerShell 执行：

```powershell
git clone https://github.com/PlutoGre/NRB-Test-game.git D:\test\NRB-Test-game
Set-Location D:\test\NRB-Test-game
```

示例使用 D 盘；没有 D 盘可换成其他可写目录，源码路径尽量不含中文和空格。仓库需已上传完整源码和 `.github/workflows/ios-build.yml`；首次上传步骤保存在 [参考说明](docs/reference/GITHUB_REPOSITORY.md)。

## 2. 一键部署服务端

双击仓库里的 **`deployment/server.cmd`**，或在仓库根目录运行：

```powershell
.\deployment\server.cmd
```

首次会下载运行时、初始化数据库并启动游戏服；再次用相同配置运行会启动已有部署，保留玩家数据。

默认运行目录为 `%LOCALAPPDATA%\OnePieceServer`。安装完成后，在 PowerShell 查看客户端打包需要的参数：

```powershell
Get-Content "$env:LOCALAPPDATA\OnePieceServer\client-config\deployment.json"
```

记下 `host`、`webPort`、`gamePort`、`resourceVersion`，接着打开 **[客户端打包流程](docs/CLIENT_BUILD.md)**。

### 仅在部署遇到问题时使用

| 情况 | 处理 |
|---|---|
| 多网卡，无法自动选 IP | 用下面的命令显式指定本机局域网 IP |
| 用户目录含中文或需指定安装位置 | 同一命令加 `-InstallDir D:\OnePieceServer`；后续管理命令也加相同参数 |
| 端口占用 | 同一命令加 `-WebPort 18091 -GamePort 25534 -DatabasePort 13316` |
| 手机在同一 Wi-Fi 下仍无法连接 | 确认电脑和手机可互访；管理员 PowerShell 中执行同一命令并加 `-OpenFirewall` |
| 下载或进程启动失败 | 查看运行目录 `logs`；缺少 VC++ DLL 时安装微软对应的 x64 运行库 |

显式指定 IP 的基础命令，按需添加上表参数：

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File .\deployment\Deploy-Server.ps1 -PublicHost 192.168.1.100
```

## 3. 日常启动和关闭

以下使用默认运行目录，在仓库根目录执行：

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File .\deployment\Deploy-Server.ps1 -Action start
powershell -NoProfile -ExecutionPolicy Bypass -File .\deployment\Deploy-Server.ps1 -Action status
powershell -NoProfile -ExecutionPolicy Bypass -File .\deployment\Deploy-Server.ps1 -Action stop
```

默认是局域网连接。iPhone 如需在外地或用移动流量访问，安装新客户端后按 [外网接入简明步骤](docs/EXTERNAL_DEPLOYMENT.md) 操作；Android 当前仍使用直接 TCP 连接。

克隆仓库只会得到初始数据，旧电脑的玩家进度需另行迁移。备份恢复等低频操作保存在 [服务端参考说明](docs/reference/SERVER_DEPLOYMENT.md)。

**验证状态：新电脑部署、外网链路及 iOS 构建/真机运行仍待实际验证。文档中的步骤不代表已全部实跑通过。**

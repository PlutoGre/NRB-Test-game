> 参考归档：日常操作请先看 [一键部署](../../README.md) 和 [客户端打包](../CLIENT_BUILD.md)。本页保留迁移、排查及其他构建方式的详细说明。

# Windows 服务端一键部署与迁移

## 1. 交付内容和当前验证状态

入口是 [`deployment/Deploy-Server.ps1`](../../deployment/Deploy-Server.ps1)，可双击 [`deployment/server.cmd`](../../deployment/server.cmd)。它调用仅使用 Python 标准库的部署程序。

脚本会准备便携式 Python、JDK 8、MariaDB，复制游戏服、初始化数据库、生成公开资源目录和客户端地址配置，并启动数据库、HTTP 资源服务和 Java 游戏服。

**本次只编写脚本和检查代码，没有运行新部署程序、数据库初始化、备份恢复、编译、构建或测试。** 之前验证过的 Android 调试环境使用另一套临时启动脚本和 MariaDB 10.6；这里固定的是 MariaDB 10.11.19，尚需在目标 Windows 完成首次实跑及业务验证。iOS 的代码适配也尚未经过 Xcode 编译或真机验证。

本工具提供本地、局域网开发部署的基础，启动的是源码中现成的 `DZM.jar`，不自动重新编译 Java 服务端。初始数据库来自仓库 SQL，可能包含演示数据；它不是当前调试账号数据的自动快照。

需要手机通过外网连接时，在本部署基础上增加 [Quick Tunnel + WebSocket 网关](EXTERNAL_DEPLOYMENT.md)。入口为 `deployment/Start-External.ps1`，目前只适配新 iOS 客户端；不要直接把资源端口发布后当成游戏已经支持外网。`Deploy-Server.ps1 -Action stop` 会同时关闭本部署拥有的隧道和网关，备份/恢复前也应先完整停止部署。

## 2. 新电脑需要复制什么

如果已上传完整 GitHub 仓库，直接 `git clone` 即可取得下列目录，不需要复制 `.debug`。首次上传及 iOS 直接构建见 [完整仓库说明](GITHUB_REPOSITORY.md)。

至少复制以下目录，并保留相对关系：

```text
OnePiece/
├─ deployment/
│  ├─ server.cmd
│  ├─ Deploy-Server.ps1
│  ├─ server.py
│  ├─ external.cmd
│  ├─ Start-External.ps1
│  ├─ external.py
│  ├─ websocket_gateway.py
│  ├─ tunnel-manifest.json
│  ├─ runtime-manifest.json
│  ├─ prepare_android.py
│  ├─ prepare_github_upload.py
│  └─ github/ios-build.yml
├─ docs/
└─ src/
   ├─ server/
   ├─ client/
   └─ tools/
```

不必复制 `.debug`、模拟器镜像、旧截图、`src.7z` 或旧调试数据库目录来完成一套全新部署。若需要保留已有玩家数据，按第 9 节先备份再恢复。

建议源码放在 `D:\test\NRB-Test-game`。源码路径含中文时部署程序可读取，但 **数据库和 Java 的运行目录要求使用 ASCII 路径**，例如 `D:\OnePieceServer`，避免旧工具对中文路径的兼容问题。运行目录不能是整个源码目录、磁盘根目录或 `src` 内部目录。

## 3. 系统和运行时

- Windows 10/11 x64，PowerShell 5.1 或更新版本。
- 至少预留数 GB 磁盘空间供运行时、资源、数据库和备份使用。
- 首次在线部署能访问 Python、Adoptium/GitHub 和 MariaDB 官方下载地址。
- 便携式二进制可能依赖 Microsoft Visual C++ 运行库；若启动日志报告缺少 `VCRUNTIME`/`MSVCP` 等 DLL，安装 Microsoft 官方对应 x64 运行库，不要从第三方网站单独下载 DLL。

版本和 SHA256 固定在 `deployment/runtime-manifest.json`：

| 组件 | 固定版本 | 用途 |
|---|---|---|
| Python | 3.14.10 Windows embedded x64 | 运行部署工具、资源 HTTP 服务 |
| Java | Eclipse Temurin JDK 8u504-b01 x64 | 运行旧 Java 服务端，也可供旧 Android Java/Ant 构建使用 |
| MariaDB | 10.11.19 x64 | 独立本地数据库实例 |

脚本不修改系统 `JAVA_HOME`/`PATH`，不卸载已有 MySQL，不注册开机启动服务。Python 解压在 `deployment/.bootstrap`；Java 和数据库解压在运行目录 `runtimes`。

## 4. 首次部署

### 4.1 查新电脑的局域网 IP

在 PowerShell 执行 `ipconfig`，选择手机能访问的有线或 Wi-Fi 网卡 IPv4，例如 `192.168.1.100`。手机和电脑应位于可互通网络。

不要继续使用旧电脑的 `192.168.30.101`，也不要使用手机自身的 `127.0.0.1` 或 Android 模拟器专用的 `10.0.2.2`。

### 4.2 一键安装并启动

在仓库根目录打开 PowerShell：

若要直接执行后面的 `.ps1` 命令，当前窗口可先运行 `Set-ExecutionPolicy -Scope Process Bypass`。该设置只影响本窗口；双击 CMD 入口或使用下方带 `-ExecutionPolicy Bypass` 的完整命令时无需另设系统策略。

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File .\deployment\Deploy-Server.ps1 `
  -InstallDir D:\OnePieceServer `
  -PublicHost 192.168.1.100
```

也可双击 `deployment/server.cmd`，默认运行目录为 `%LOCALAPPDATA%\OnePieceServer`，自动选择带默认网关的唯一 IPv4 网卡。有多个候选网卡、VPN 或默认路径含中文时，使用上面的显式参数命令。

首次执行顺序：

1. 准备 Python，并检查下载包 SHA256。
2. 检查独立运行目录和端口，拒绝接管其他程序。
3. 下载、校验、解压 JDK 和 MariaDB。
4. 生成随机数据库密码，初始化自己的数据库目录。
5. 导入 `src/server/dzm.sql` 和 `Code_Server/dzm_stat.sql`，创建仅能操作游戏库的应用账号。
6. 复制游戏服；配置 `Platform=lo`、`ServerId=01`，默认关闭 GM。
7. 生成静态 Web 资源、服务器列表、更新清单，以及 Android 本地登录覆盖配置。
8. 依次启动数据库、资源服务和 Java 游戏服，并报告进程和端口状态。

该状态检查只说明进程和 TCP 监听，不代表远航、勇闯等业务已验证。若中途失败，已有进程和日志保留供排查；不会自动删除数据库。解决原因后先 `stop` 再重新 `install`。SQL 导入中断会阻止自动重导，避免覆盖数据。

### 4.3 端口和可选防火墙配置

| 服务 | 默认端口 | 可访问范围 |
|---|---:|---|
| 游戏 | 25524/TCP | 本机网卡 |
| 资源 HTTP | 18081/TCP | 本机网卡 |
| MariaDB | 13306/TCP | 仅 127.0.0.1 |

如果端口已被占用，选择新端口，不要随意结束其他程序：

```powershell
.\deployment\Deploy-Server.ps1 -InstallDir D:\OnePieceServer -PublicHost 192.168.1.100 `
  -WebPort 18082 -GamePort 25525 -DatabasePort 13307
```

在管理员 PowerShell 中给安装或启动加 `-OpenFirewall`，可允许**专用网络、本地子网**访问该部署的游戏和资源 TCP 端口：

```powershell
.\deployment\Deploy-Server.ps1 -Action start -InstallDir D:\OnePieceServer -OpenFirewall
```

这不会开放数据库端口，也不会把网卡从公用网络自动改为专用网络。普通安装不要求管理员权限，只要对安装目录可写。

## 5. 安装后的目录

```text
D:\OnePieceServer/
├─ .onepiece-server.json       # 地址、端口、进程身份、安装状态
├─ runtimes/java/             # 包含 bin/java.exe、bin/javac.exe
├─ runtimes/mariadb/
├─ server/                    # 实际运行的 DZM.jar 和 XML
├─ dbdata/                    # 玩家数据；不要在运行中复制来迁移
├─ private/                   # 数据库凭据、导入/恢复状态
├─ web/                       # 只提供客户端公开资源
├─ client-config/             # 客户端部署参数和版本配置
├─ logs/
└─ backups/
```

`private` 和运行服 Hibernate 配置含数据库凭据，不要上传到 GitHub，也不要将它们作为 Web 根目录。公开站点仅使用 `web`。

客户端配置在 `client-config/deployment.json`，例如：

```json
{
  "host": "192.168.1.100",
  "webPort": 18081,
  "gamePort": 25524,
  "resourceVersion": "1.78.6",
  "channel": "anzhi"
}
```

部署时也会生成仓库 `src/client/Game/proj.android/antbuild/anzhi/deployment` 的覆盖配置。默认使用本地设备账号，未接商业充值。

## 6. 日常启动、停止和状态

必须使用与首次安装相同的运行目录：

```powershell
.\deployment\Deploy-Server.ps1 -Action status -InstallDir D:\OnePieceServer
.\deployment\Deploy-Server.ps1 -Action stop   -InstallDir D:\OnePieceServer
.\deployment\Deploy-Server.ps1 -Action start  -InstallDir D:\OnePieceServer
```

- 重复 `start` 不会重复启动已经记录并运行的进程。
- `stop` 根据 PID、进程创建时间和可执行文件识别自己的进程，不按 `java.exe` 名称批量杀进程。
- 游戏和资源进程会终止，数据库用管理工具正常关闭。停止前应先退出游戏，避免在业务写入中强制停服；这不是游戏自带的优雅停服协议。
- 不删除玩家数据，不清空数据库。启动不会重新导入初始 SQL。
- 不会自动随 Windows 开机启动；需要时在登录后执行 `start`。

如果把项目源码移动到另一条路径，可从新路径调用同一运行目录的 `start`；但正在使用的旧 Python 目录不要在服务运行时删除。更换运行目录不等于迁移数据库，应使用备份恢复。

## 7. 地址变化、版本变化和更新服务端

IP、资源或游戏配置变更时，先停止，再重新安装到同一个受管理目录。数据库保留，游戏 JAR/XML 和公开资源会重新复制生成：

```powershell
.\deployment\Deploy-Server.ps1 -Action stop -InstallDir D:\OnePieceServer
.\deployment\Deploy-Server.ps1 -InstallDir D:\OnePieceServer -PublicHost 192.168.1.101 `
  -ResourceVersion 1.78.7
```

**重新安装会以仓库模板覆盖运行目录中的 JAR 和游戏 XML。** 如果曾直接修改运行目录中的数值配置、GM 设置或 JAR，先备份，并同步到仓库模板或在重新部署后重新应用。数据库连接配置由工具生成。

有运行进程时不允许变更参数。改变数据库端口也要重新 `install`；仅 `start` 不修改安装参数。更换 MariaDB/Java 大版本不要直接覆盖运行时目录，使用新运行目录并备份恢复。

客户端需要与新地址、资源版本一致，按 [客户端生成流程](CLIENT_BUILD.md) 重新生成或更新。修改仓库配置不会自动改变已经安装的 APK/IPA。

## 8. 离线部署

先在有网的 Windows 上，仅下载运行时：

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File .\deployment\Deploy-Server.ps1 -Action download
```

将完整源码与 `deployment/download-cache` 一起复制到新电脑，再执行：

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File .\deployment\Deploy-Server.ps1 `
  -InstallDir D:\OnePieceServer -PublicHost 192.168.1.100 -Offline
```

离线缓存中必须包含 manifest 指定的三个 ZIP。脚本不会在缺包或校验不一致时静默换版本。当前目录只预先放入了 Python 的官方 ZIP；JDK、MariaDB 需要执行下载操作或手工准备同名、同哈希文件。

## 9. 保留账号和玩家进度进行迁移

### 9.1 旧电脑已经使用本部署工具

先退出游戏并停止服务，再备份：

```powershell
.\deployment\Deploy-Server.ps1 -Action stop -InstallDir D:\OnePieceServer
.\deployment\Deploy-Server.ps1 -Action backup -InstallDir D:\OnePieceServer
```

备份生成 `backups/onepiece-时间.sql.gz` 和 `.sql.gz.sha256`，包含 `dzm`、`dzm_stat` 数据库。备份过程会启动数据库，结束后游戏和资源服务仍保持停止。

新电脑先完成一套全新安装，再停止它，将备份及校验文件复制过来：

```powershell
.\deployment\Deploy-Server.ps1 -Action stop -InstallDir D:\OnePieceServer
.\deployment\Deploy-Server.ps1 -Action restore -InstallDir D:\OnePieceServer `
  -RestoreFile D:\Backups\onepiece-实际时间.sql.gz -ConfirmRestore
.\deployment\Deploy-Server.ps1 -Action start -InstallDir D:\OnePieceServer
```

恢复会替换目标游戏数据，所以需要 `-ConfirmRestore`。工具在恢复前自动备份目标库；恢复中断会留下标记并禁止启动游戏，需查日志后重新完成恢复。

逻辑备份不包含运行目录里手工修改的 JAR/XML、客户端签名或手机本地身份。另行保留这些文件。即使数据库恢复成功，卸载客户端、清空 App 数据或换设备导致本地账号 ID 变化，也可能登录成新角色。

### 9.2 从前面临时的 `.debug/onepiece` 环境迁移

临时环境没有本工具的管理标记，不能用新工具接管或停止它。先通过原来的调试管理方式退出游戏、停止对应 Java 游戏进程，并保留数据库运行，然后用旧目录中自带的数据库工具导出。

下面只是路径示例，替换为实际旧目录；`-p` 会在本机提示输入旧数据库密码，不在命令行写明文密码：

```powershell
New-Item -ItemType Directory -Path D:\Backups -Force
& 'D:\旧项目\.debug\onepiece\database\mariadb-10.6.28-winx64\bin\mariadb-dump.exe' `
  --protocol=tcp --host=127.0.0.1 --port=13306 --user=root -p `
  --single-transaction --routines --triggers --hex-blob --databases dzm `
  --result-file=D:/Backups/legacy-onepiece.sql
```

使用 `--result-file`，不要通过旧 Windows PowerShell 的 `>` 文本重定向导出 SQL，以免改成 UTF-16。此示例导出玩家所在的 `dzm`；如果旧环境还有 `dzm_stat` 且需要保留统计，可将它加在 `--databases dzm` 后面。

确认导出命令成功后，用 Python 生成恢复工具接受的压缩文件和校验：

```powershell
@'
from pathlib import Path
import gzip, hashlib, shutil
source = Path('D:/Backups/legacy-onepiece.sql')
target = Path('D:/Backups/legacy-onepiece.sql.gz')
with source.open('rb') as data, gzip.open(target, 'wb') as output:
    shutil.copyfileobj(data, output)
target.with_suffix(target.suffix + '.sha256').write_text(
    hashlib.sha256(target.read_bytes()).hexdigest() + '  ' + target.name + '\n', encoding='ascii')
'@ | python -
```

然后按 9.1 在新电脑恢复。不要在新电脑另建一个空库后误以为旧账号会自动出现。

## 10. 常见问题

| 表现 | 检查和处理 |
|---|---|
| 下载超时 | 检查官方域名访问；在有网环境准备 download-cache 后使用 Offline |
| SHA256 不一致 | 不执行该包；重新取得 manifest 对应的完整官方 ZIP |
| 端口被占用 | 更换端口；新工具不会停止已有 MySQL、旧调试服或其他 Java 程序 |
| 多个网卡无法选 IP | 显式传 `-PublicHost`，使用手机能访问的网卡 |
| Database exited / Java exited | 看 logs 下对应 stdout/stderr，检查缺 DLL、端口和配置 |
| Access denied for user | 不要混用别的实例；检查本运行目录 private 与 Hibernate 配置是否配套 |
| schema-importing 标记存在 | 初次导入未完成；保留现场，确认原因后用独立新目录重装或人工恢复，工具不会自动 DROP 重导 |
| restore-in-progress 标记存在 | 恢复未成功；查日志并重新完成可信备份恢复，不能直接把它当成功启动 |
| 电脑能访问，手机不能 | 检查新 IP、同网互通、Private 防火墙规则，确认手机中的地址不是 127.0.0.1 |
| 资源正常但游戏连接失败 | HTTP 端口不等于游戏 TCP 端口；两者都应可达 |
| 打开只有页面或无法点击 | 使用重新生成的源码客户端；旧 APK 可能仍有旧地址、旧公告遮罩和旧渠道登录 |
| 看不到勇闯 | 完成新手并满足等级/阵位解锁条件；前面验证时船长 11 级、5 个阵位后出现 |

## 11. 官方参考

- [Python Windows 便携包](https://docs.python.org/3/using/windows.html#the-embeddable-package)
- [Eclipse Temurin 下载](https://adoptium.net/temurin/releases/)
- [MariaDB 官方归档和校验](https://archive.mariadb.org/mariadb-10.11.19/winx64-packages/)
- [MariaDB Windows 初始化工具](https://mariadb.com/docs/server/server-management/install-and-upgrade-mariadb/installing-mariadb/installing-system-tables-mariadb-install-db/mariadb-install-db-exe)

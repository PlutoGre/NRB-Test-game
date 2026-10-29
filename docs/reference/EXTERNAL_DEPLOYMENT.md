> 参考归档：日常操作请先看 [一键部署](../../README.md) 和 [客户端打包](../CLIENT_BUILD.md)。本页保留迁移、排查及其他构建方式的详细说明。

# 本项目的免费外网测试部署（Windows + iPhone）

本页把 [原始方案](卡牌游戏免费外网部署方案.md) 落到当前 NRB-Test-game 项目。目标是家中 Windows 电脑开服，约 4～5 台 iPhone 通过不同 Wi-Fi/移动网络玩游戏；手机无需安装组网或 VPN 软件。

**当前状态：已加入网关、Windows 启停入口和 iOS 网络适配代码；尚未启动新部署、隧道，未执行构建、测试或 iPhone 验证。以下是待你执行的流程，不代表已经联调通过。** 第一次 Xcode 构建仍可能需要根据日志修正旧引擎兼容问题。

## 1. 与原始方案的对应关系

这个游戏的玩法服务不是 HTTP REST 服务，而是 Java TCP/Protobuf 服务。原文中的 `/api/rooms`、`/api/action`、8080 端口都是示例，本次没有新增这些业务接口，也没有重写远航、勇闯等玩法。

```text
iPhone（新 ios-modern 客户端）
  ├─ HTTPS：连接检测、服务器列表、资源下载
  └─ WSS：原来的游戏二进制字节流
        ↓ 同一个 https://随机名称.trycloudflare.com
Cloudflare Quick Tunnel
        ↓ cloudflared 主动建立出站连接
Windows：127.0.0.1:18082，Python 网关
  ├─ 读取运行目录中的公开游戏资源
  └─ TCP → 127.0.0.1:25524，原 Java 游戏服
                  ↓
             127.0.0.1:13306，MariaDB
```

Quick Tunnel 直接代理 HTTP 资源服务器，不能让旧客户端的裸 TCP 自动穿过隧道。Cloudflare 的其他 TCP 代理方式要求最终用户运行额外的 `cloudflared access tcp`，不满足手机只安装游戏的目标；因此这里在应用内部加入 WSS 传输。[官方协议说明](https://developers.cloudflare.com/cloudflare-one/networks/connectors/cloudflare-tunnel/routing-to-tunnel/protocols/)

| 项目 | 本次实现 |
|---|---|
| iOS 外网游戏连接 | `NSURLSessionWebSocketTask` → 本地 socketpair → 现有游戏收发代码 |
| 游戏包格式 | 保留 TCP 字节序、原包头和 Protobuf，不把 WebSocket 消息边界当成游戏包边界 |
| 资源下载 | 将缓存/打包配置里的资源地址重映射到启动页所选的 HTTPS 域名 |
| 更换临时域名 | 强制关闭 App 后重新填写；不需要为域名变化重新打包 |
| 连接检测 | `/ping` 检查网关标识；带邀请码的 `/check` 检查配置及本地游戏端口 |
| 接入限制 | 随机邀请码；默认最多 8 条游戏连接，给约 4～5 台手机留出断线释放余量 |
| Android | 继续原有局域网 TCP；本次没有接入 WSS |
| 服务端操作系统 | 新的一键部署和隧道管理入口面向 Windows x64；未提供 Ubuntu 一键部署 |

## 2. 首次准备

1. 在 Windows 克隆包含本次修改的完整仓库；保留 `src`、`deployment`、`.github`、`docs` 的相对位置。
2. 按 [服务端文档](SERVER_DEPLOYMENT.md) 部署到独立运行目录，例如 `D:\OnePieceServer`。
3. 按 [完整仓库 GitHub 流程](GITHUB_REPOSITORY.md) 构建新的 `ios-modern` 客户端，并按 [客户端文档](CLIENT_BUILD.md) 完成签名和安装。

示例，在仓库根目录 PowerShell 执行；把 IP 改成本机局域网 IP：

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File .\deployment\Deploy-Server.ps1 -InstallDir D:\OnePieceServer -PublicHost 192.168.1.100
```

`-PublicHost` 仍用于生成局域网后备配置；外网域名不填写在这里。`server.cmd` 部署的是默认 `%LOCALAPPDATA%\OnePieceServer`，如果使用该入口，后续命令也应使用这个运行目录，不能混用 `D:\OnePieceServer`。

新网关要求该目录有部署工具生成的 `.onepiece-server.json`、私有配置、`server` 和 `web` 目录。此前 `.debug` 下的手动调试服务不能直接当成新部署目录；需要按服务端文档迁移玩家数据。脚本不会接管旧服务或覆盖占用端口的其他进程。

如果本机旧调试服务占用默认端口，请先自行停止旧环境，或给新部署选择三个未占用的端口。例如安装时指定 `-WebPort 18091 -GamePort 25534 -DatabasePort 13316`。网关从部署状态读取实际游戏端口，不必修改 Python 源码。

## 3. 启动外网入口

仍在同一个仓库根目录运行：

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File .\deployment\Start-External.ps1 -InstallDir D:\OnePieceServer
```

它会：

1. 检查已经完成安装、数据库没有处于中断恢复状态，且运行目录的 `server/xml/config.xml` 中 `CanUseGm=0`。部署工具原本就会关闭 GM。
2. 下载固定版本 `cloudflared` 并校验 SHA256，缓存到仓库下被忽略的 `deployment/download-cache`。
3. 启动本部署拥有的数据库、资源服务、游戏服；不会重新导入初始 SQL。
4. 启动仅监听 `127.0.0.1:18082` 的网关，然后将这个端口发布成 Quick Tunnel。
5. 最多等待 120 秒提取临时 HTTPS 域名，显示域名和本机生成的邀请码。

示例输出形式：

```text
iPhone HTTPS URL: https://随机名称.trycloudflare.com
Invitation (keep private): 本机随机生成的一串字符
```

本仓库固定的 cloudflared 版本和校验值在 [`tunnel-manifest.json`](../../deployment/tunnel-manifest.json)，来自 [Cloudflare 官方发布页](https://github.com/cloudflare/cloudflared/releases/tag/2026.10.5)。以后需要升级时应同时更新版本、下载地址和官方 SHA256；不要只替换缓存文件。

不需要 Cloudflare 账号、自有域名、路由器端口映射或开放公网入站端口。电脑需要能向 Cloudflare 建立出站连接；Quick Tunnel 面向测试，地址临时分配，服务可能中断，当前官方限制为 200 个并发在途请求且不支持 SSE。[官方 Quick Tunnel 说明](https://developers.cloudflare.com/tunnel/get-started/quick-tunnels/)

脚本提取到域名只表示地址已分配；手机端的“检测并进入”才会检查从手机到网关和本地游戏端口的链路，后续还需实际登录和玩法验证。

## 4. 在 iPhone 选择服务器

必须安装由包含本次代码的分支重新生成的客户端。旧 IPA 或 Android APK 不会因为开了网关就自动具有 WSS 功能。

1. 完全关闭并重新打开游戏，首先看到“选择游戏服务器”。
2. 地址栏填写完整的 `https://随机名称.trycloudflare.com`，可以带最后一个 `/`；不要添加 `/game`、资源目录、查询参数或邀请码。
3. 邀请码栏填写开服电脑显示的整串邀请码。
4. 点击“检测并进入外网游戏”。检测通过后，才启动原游戏。
5. 登录后逐项进入远航、勇闯等已解锁玩法，验证实际操作和存档。

地址和邀请码在检测成功后保存在本 App 的本地偏好中；没有写入工程或资源。编辑地址会清空邀请码，防止把上一个服务器的邀请码自动发送给另一个域名。修改后即使是同一服务器，也需要重新粘贴邀请码。应用使用系统 HTTPS/WSS 证书验证，拒绝 HTTP 跳转和资源重定向，没有关闭 TLS 校验。

选择“使用打包时的局域网配置”会使用原来的直接 TCP/HTTP 路径；这条路径要求打包时的局域网 IP/端口对手机可达，不会使用临时域名。

连接地址在本次游戏启动后固定；需要更换服务器或域名时，强制关闭 App 再打开。临时域名变化只需更新启动页，**不用重做签名或重打包**。但首次引入本次网络代码仍需重新构建、签名、安装。

### GitHub Actions 的参数

继续运行根目录工作流 **Build iOS from repository**。`server_host` 填局域网后备 IP，例如 `192.168.1.100`，不是带 `https://` 的临时域名；`resource_version` 与服务端部署版本一致，例如 `1.78.6`。

工作流会重新运行 `prepare.py`，包含最新原生 `.mm` 文件和游戏收发代码。不要上传旧的 `generated` 目录替代源码。如果使用旧的分卷源码快照方案，修改代码后必须重新执行 `prepare.py` 并重新打包快照；旧 ZIP 不会包含这些改动。

外网转发不代替 Apple 签名。Windows/Ubuntu 仍不能直接运行 Xcode；可使用现有 GitHub macOS 构建流程生成未签名包，再按客户端文档签名安装。本次没有添加 Apple 账号、证书或付费服务。

## 5. 日常启停与邀请码

下面所有命令使用同一个 `-InstallDir`：

```powershell
# 显示当前运行状态，以及域名和邀请码（请勿直播或共享终端输出）
powershell -NoProfile -ExecutionPolicy Bypass -File .\deployment\Start-External.ps1 -Action status -InstallDir D:\OnePieceServer

# 关闭公网入口，保留游戏服、资源服务和数据库
powershell -NoProfile -ExecutionPolicy Bypass -File .\deployment\Start-External.ps1 -Action stop -InstallDir D:\OnePieceServer

# 再次开启；会分配新的临时域名，邀请码保持不变
powershell -NoProfile -ExecutionPolicy Bypass -File .\deployment\Start-External.ps1 -InstallDir D:\OnePieceServer

# 邀请码泄露时，关闭现有外网连接并生成新的邀请码；然后再执行 start
powershell -NoProfile -ExecutionPolicy Bypass -File .\deployment\Start-External.ps1 -Action rotate-invite -InstallDir D:\OnePieceServer

# 关闭该部署拥有的隧道、网关、游戏服、资源服务和数据库
powershell -NoProfile -ExecutionPolicy Bypass -File .\deployment\Deploy-Server.ps1 -Action stop -InstallDir D:\OnePieceServer
```

外网进程在命令结束后继续后台运行。运行时记录 PID、进程创建时间和程序路径，停止时只操作属于本部署的进程。它不是 Windows 系统服务，不会开机自动恢复；重启电脑后需要重新运行启动命令。启动失败时会清理本次网关/隧道，已启动的游戏与数据库保留，便于检查日志。

需要换网关端口时，先 stop，再 start 加 `-GatewayPort 18092`；下次 start 同样传入这个端口。已运行时重复 start 保留原隧道，不会无故换域名。

## 6. 配置、接口和暴露范围

| 位置或接口 | 用途 |
|---|---|
| `运行目录/private/gateway.json` | 随机邀请码和当前公网 origin；由管理脚本原子更新 |
| `运行目录/.onepiece-server.json` | 拥有的进程身份、网关端口和部署参数；不保存邀请码 |
| `运行目录/logs/gateway.*.log` | 网关启停/游戏连接日志，不记录请求头或邀请码 |
| `运行目录/logs/tunnel.*.log` | cloudflared 连接日志；包含临时域名 |
| `GET /ping` | 无需邀请码，返回 `gateway=nrb`、`protocol=nrb-tcp-v1` |
| `GET /check` | Bearer 邀请码验证；检查恢复标记、GM 配置和 Java 端口可连接 |
| `GET /game` | Bearer 邀请码 + 子协议 `nrb-tcp-v1` 的二进制 WebSocket |
| `GET /gameserver/dzm/1.8/...` | 需要邀请码，只读取生成的资源目录；不执行 PHP |

网关固定转发本部署的 `127.0.0.1:游戏端口`，不接受请求参数指定任意目标。只允许二进制游戏数据，处理分片和 Ping/Pong；单消息最多 1 MiB，收发队列有上限，游戏连接默认最多 8 条、请求线程最多 48 条。网关约每 20 秒发起心跳，等待响应超时后断开；iOS 将断线交给现有错误界面处理，不在传输层重放游戏操作。Cloudflare 可能因服务更新等原因中断 WebSocket，心跳不能保证永不断线。[官方 WebSocket 说明](https://developers.cloudflare.com/network/websockets/)

服务器列表中的资源 origin 来自本机隧道管理配置，不信任公网请求的 `Host`。资源文件本身保持原字节，原更新清单 CRC 继续有效；旧缓存域名在 iOS 下载入口统一重映射。GM/PHP 管理目录、源码和数据库不经此网关发布。

原来的 LAN 资源服务和 Java 服务仍按部署工具配置监听本机网卡；本次没有把它们改成仅回环。路由器不要为游戏、资源、MariaDB 做公网映射；已有局域网防火墙规则仍只应允许专用网络、本地子网。新网关自身只监听回环，外网流程无需使用 `-OpenFirewall`。

这是共享邀请码的个人测试接入，不是正式的逐玩家身份认证、支付或运营安全方案。现有本地账号机制、安装内身份和数据库保存规则保持原来设计；同一邀请码不会把多台手机合成一个玩家账号。不要公开运行目录、玩家数据库或邀请码。仓库 `.gitignore` 已忽略私有网关和数据库配置；正常运行目录应放在 Git 仓库外。

## 7. 排查顺序与手动验收

1. **先确认本地部署**：运行 `Deploy-Server.ps1 -Action status`，确认数据库、资源、游戏均属于本部署且正在监听。网关不负责修复玩法解锁、数据库缺失或游戏业务逻辑错误。
2. **再看外网进程**：运行 `Start-External.ps1 -Action status`，确认网关和隧道都在；进程在不等于网络链路一直健康，出站断网仍需看 cloudflared 日志。
3. **手机浏览器访问 `/ping`**：用移动流量打开 `https://当前域名.trycloudflare.com/ping`，应返回网关 JSON。这里只证明 HTTP 到达，不证明登录/WSS 成功。
4. **游戏内检测**：401 通常是邀请码错误；503 通常是网关配置、隧道起步阶段或本地 Java 端口不可用；无网关标识/超时先检查域名、DNS、运营商链路和电脑出站联网。
5. **实际登录及操作**：观察网关日志出现游戏连接；分别进入远航、勇闯等已解锁玩法，完成一轮操作、重新登录检查保存。端口检测通过不代表这些业务都通过。
6. **多机与重启**：逐步增加到 4～5 台手机，验证各自账号、资源更新、后台恢复、断网提示；关闭隧道时客户端应报错，重新开隧道后冷启动 App 并填写新域名。不要重复提交未确认是否成功的消费操作。

如果 cloudflared 长时间拿不到域名或连接失败，查看 `tunnel.stderr.log`。网络限制 QUIC 时，可以 stop 后重新 start 加 `-TunnelProtocol http2`。QUIC/HTTP2 分别需要相应的出站 UDP/TCP 7844；切换不能保证所有网络都可达。[出站要求](https://developers.cloudflare.com/cloudflare-one/networks/connectors/cloudflare-tunnel/configure-tunnels/tunnel-with-firewall/)

如果用户目录已有 `.cloudflared/config.yml` 或 `config.yaml`，Quick Tunnel 可能受现有配置影响。先备份并检查已有隧道用途，再按官方说明处理；脚本不会替你移动或覆盖用户配置。不要额外启用要求浏览器邮箱登录的保护模式，当前游戏使用自己的 Bearer 邀请码。

家庭电脑休眠、断网或关闭进程后不可用。临时域名也可能在特定地区/运营商不可达，需要用实际手机网络确认；免费方案没有稳定域名或全天可用承诺。如果后续采用 Tailscale，手机需额外安装组网 App，属于原文中的另一条路线，本次未实现自动切换。

## 8. 本次代码变更位置

- `deployment/websocket_gateway.py`：受限资源服务及 WebSocket/TCP 转发。
- `deployment/external.py`、`Start-External.ps1`、`tunnel-manifest.json`：下载校验、隧道启停、邀请码管理。
- `src/client/ios-modern/native/ConnectionSettings.*`：启动选择、配置持久化、连接检测、资源地址重映射。
- `src/client/ios-modern/native/RemoteSocket.*`：系统 WSS 与旧 socket 接口之间的桥接。
- `IoSocket.cpp`、`AsyncSocket.cpp`：仅 `PROJECTIOSLOCAL` 下接入外网传输，其他平台保留 TCP。
- `PacketManager.*`、`ThreadSocket.*`：处理半包/多包、重连清空缓存，修复连接失败被覆盖和对端关闭未上报的问题。这些基础收发修复也影响原 TCP 路径，构建后应一并回归局域网连接。

仍待完成的运行验证：全新 Windows 部署、网关鉴权及边界行为、真实 Quick Tunnel、首次 Xcode 构建、签名安装、iPhone 多机玩法及断线恢复。未执行这些验证前不能称为已可用的 iOS 成品包。

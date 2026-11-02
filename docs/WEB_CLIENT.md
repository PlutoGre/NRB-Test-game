# 网页客户端：构建与部署

网页版复用 `client/source` 的 C++ 游戏、Lua、Protobuf、CCBI 布局及 `resources/client` 资源，使用 WebAssembly + WebGL 绘制原有游戏画面。浏览器通过同源 WebSocket 网关连接已有 Java 游戏服，玩家数据仍保存在当前部署的数据库中。

**当前状态（2026-11-02）：已在 Windows 本地完成编译、部署和真实 Chromium 浏览器游玩，并修复点击输入、经验同步、上阵重试及经验道具保存问题。普通/精英类型关卡、卡牌升级、装备、海鲜饭、抽卡、黑胡子、勇闯和现有活动的具体结果见[实测记录](reference/WEB_GAMEPLAY_VALIDATION.md)。运宝、All Blue 和转盘缺少完整实现，不能认定全部玩法已可用。**

## Windows 最短操作流程

在任意目录完整 clone 仓库；以下命令在仓库根目录执行。

1. 双击根目录 `启动服务.cmd`，等待数据库、资源服务及游戏服启动成功。
2. 双击 `deployment/build-web.cmd`。第一次自动在 `tools/emsdk` 下载固定版本 Emscripten 3.1.74，并编译共享游戏源码。下载和首次编译可能较久，需要联网及足够的磁盘空间。无需 Android SDK 或 Mac。
3. 构建成功后双击 `deployment/start-web.cmd`，窗口会显示 `Open this URL: http://本机局域网IP:18084/`。
4. 在电脑或同一局域网手机浏览器打开该网址，点击“进入男人帮1区”。首次进入需要下载完整游戏资源；之后使用游戏原有登录、建角和玩法界面。
5. 关闭服务仍双击根目录 `停止服务.cmd`，会一起停止本项目管理的网页网关、游戏服、资源服务及数据库，保留玩家进度。

网页服务与游戏服可以共用当前部署。重复执行网页启动入口会复用同一个受管进程；端口被其他程序占用时会报错，避免覆盖其他服务。关闭启动窗口不会关闭后台服务。

手机连接同一 Wi-Fi；如 Windows 防火墙拦截，允许网页监听端口的入站 TCP 通信。网页客户端只需要访问网页网关端口，不需要让浏览器直连数据库或 Java 游戏端口。

## 使用 GitHub Actions 构建

1. 把本次源码同步到自己的 GitHub 仓库。
2. 打开仓库的 **Actions → Build Web client → Run workflow**，选分支并运行。
3. 成功后下载 **nrb-web-client**，解压到部署电脑的 `outputs/web`。该目录下应直接包含 `index.html`、`game.js`、`game.wasm`、`game.data`、`web.js`、`style.css`、`web-client.json`，不要再多套一层目录。
4. 部署电脑执行 `deployment/start-web.cmd`。

网页包不绑定电脑 IP，可以复用到其他已部署好同一版本服务端的电脑。构建失败时下载 **web-build-logs**，对应本地的 `client/web/generated/logs`。工作流仅支持手工触发；本次验证使用本地构建，没有触发云端构建。

## 可选参数和其他构建环境

限制编译并行数、指定已激活的 SDK：

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File .\deployment\Build-Web.ps1 -Jobs 2 -Emsdk tools/emsdk
```

指定网页端口或其他部署目录：

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File .\deployment\Start-Web.ps1 -Port 18085 -InstallDir runtime/server -BuildDir outputs/web
```

`-Emsdk` 要求目录中有已安装并激活的 **3.1.74**（Windows 同时需要 SDK 配套 Python），否则使用自动下载。相对路径均以仓库根目录为基准；不要求固定盘符。Emscripten 和 Python 会从本项目读取配置，不修改系统 PATH。同一目录内请一次只执行一个构建。

Ubuntu/macOS 装好 Python 3.10 或以上后，也可从仓库根目录构建：

```bash
python3 client/web/build.py --jobs 2
```

此命令生成同样的 `outputs/web`；服务端一键部署入口仍是现有 Windows 方案。不要直接双击 HTML，也不要仅用普通静态文件服务器启动游戏：登录会话、资源路由和 WebSocket 需要 `web_client.py`。

## 公网网址

个人测试可以直接双击根目录 **`启动网页外网.cmd`**。首次会自动部署并启动服务端；缺少完整 Web 包时自动下载工具链并编译，已有包直接复用。随后下载固定版本的 cloudflared，建立 Quick Tunnel，自动配置 HTTPS 域名并启动网页网关。

窗口会显示：

```text
Web HTTPS URL: https://本次生成的名称.trycloudflare.com/
URL saved: 项目目录\runtime\server\web-external-url.txt
Public readiness: confirmed at ...
```

将 **Web HTTPS URL** 发给玩家，浏览器直接打开即可；不需要安装游戏包或填写 iOS 邀请码。电脑保持开机联网。关闭命令窗口后服务仍在后台运行。

- **`查看网页外网.cmd`**：显示当前网址、进程状态，并重新检查公网连接。
- **`停止网页外网.cmd`**：只关闭这条 Web 外网入口，删除已失效的网址文件，保留游戏服、数据库及局域网/iOS 入口。
- 根目录 **`停止服务.cmd`**：同时关闭本部署的全部入口和服务端。

外网网页网关默认从本机回环端口 **18085** 选择可用端口；局域网网页入口仍可使用 18084，二者共用同一游戏服与数据库。重复启动会复用仍在运行的隧道和网址；更换构建包时只重启网页网关，尽量保留隧道。更新客户端源码后，仍需重新运行 `deployment/build-web.cmd` 生成新包；一键入口不会自动覆盖已有完整包。

脚本会比较本机与公网 `/web/config` 的实例标识、协议和构建版本。若显示 **`Public readiness: NOT confirmed`**，表示本机网关已启动、域名已分配，但尚未确认公网能访问；进程会保留，供网络恢复后重试。公网用户是否能顺利加载和游玩，仍取决于各自网络。

Quick Tunnel 免费、无需自有域名，定位为临时测试；隧道进程重启后会更换域名。**当前游客 Cookie 与网址关联，域名变化可能进入新游客角色，原角色仍在数据库中。** 不要把临时网址视为固定账号入口。

也可在仓库根目录运行相同入口，所有相对路径均按项目目录解析：

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File .\deployment\Start-Web.ps1 -External
powershell -NoProfile -ExecutionPolicy Bypass -File .\deployment\Start-Web.ps1 -External -Action status
powershell -NoProfile -ExecutionPolicy Bypass -File .\deployment\Start-Web.ps1 -External -Action stop
```

可加 `-Port 18086` 指定端口，`-TunnelProtocol auto` 或 `quic` 切换隧道传输，`-InstallDir` 指定部署目录，`-BuildDir` 指定已有网页包。默认使用 `http2`。`-SkipBuild` 禁止缺包时自动编译；`-Offline` 只复用已缓存的运行时、工具和完整网页包，建立隧道本身仍需联网。

日志在运行目录 `logs/web_tunnel.stderr.log`、`logs/web_external.stderr.log`。只有网页网关端口通过隧道对外提供服务，不转发数据库、管理工具或裸 TCP 游戏端口。原有 `deployment/external.cmd` 仍用于 iOS 专用外网入口。

本次已使用当前电脑的已有 Web 包实际启动外网入口，下载隧道工具，取得 HTTPS 地址，并确认公网就绪响应与本机网关一致。没有重新编译客户端，也没有执行全新 Windows 初始化或公网浏览器玩法测试；此前玩法实测使用本地浏览器入口。

### 提示 getaddrinfo failed / Errno 11001

`Local administration ready`、`web_external started` 表示游戏服和外网网页网关已经启动；`browser: not running port 18084` 只是可选的局域网网页入口未启动，外网网页入口实际使用 18085 或自动选择的其他端口。

`Public readiness: NOT confirmed` 加上 `getaddrinfo failed` / `11001`，表示**开服电脑无法解析本次公网域名，HTTPS 检查还没有连接到 Cloudflare**。这份日志不能证明所有网络都打不开，也不能证明隧道已连通；隧道进程存在不等于已建立连接。[Windows 错误码说明](https://learn.microsoft.com/en-us/windows/win32/winsock/windows-sockets-error-codes-2)

按以下顺序排查，先保留正在运行的隧道和网址：

1. 用手机关闭 Wi-Fi，改用流量打开当前 HTTPS 网址。若能显示游戏页面，说明该网络可以到达入口，开服电脑自身的域名解析仍需检查。
2. 在开服电脑的管理员终端运行 `ipconfig /flushdns` 清理本机缓存，然后双击 `查看网页外网.cmd` 重试。该命令不会修改 DNS 服务器设置。
3. 若仍失败，运行 `nslookup 当前网址的域名`；也可运行 `nslookup 当前网址的域名 1.1.1.1` 对比查询结果。这里仅作查询，不会改变电脑的 DNS 配置。DNS 缓存、上游解析和网络问题需根据实际结果区分，不能仅凭这份日志确定是哪一种。
4. 若手机流量也无法访问，提供具体提示和 `runtime/server/logs/web_tunnel.stderr.log` 末尾约 60 行。`Registered tunnel connection` 表示这次运行曾注册连接；后续的 `ERR`、断开或重连记录同样需要查看。Cloudflare 的 `1033` 表示找不到健康隧道连接，`502` 则需继续检查隧道到本机网页网关的连接。[Cloudflare 排查说明](https://developers.cloudflare.com/tunnel/troubleshooting/)

更新脚本只需 `git pull` 后双击 `查看网页外网.cmd` 获取更具体的 DNS 提示；无需重新编译客户端或重建数据库。此次 DNS 提示修改只进行了代码和文档检查，尚未在报错的另一台电脑上验证。

### 手机浏览器显示 Error 1033

这表示手机已经访问到 Cloudflare，但 Cloudflare 找不到可用的隧道连接。可能同时存在开服电脑的 DNS 解析失败，不能仅清理浏览器缓存就认定解决。[1033 官方说明](https://developers.cloudflare.com/support/troubleshooting/http-status-codes/cloudflare-1xxx-errors/error-1033/)

1. 保持服务和当前隧道运行，在仓库根目录执行 `git pull`，再双击 `查看网页外网.cmd`。脚本会在公网检查失败时显示近期隧道连接日志；这些是历史记录，最终仍以实际访问结果为准。
2. 需要完整上下文时，在仓库根目录的 PowerShell 中执行：

   ```powershell
   Get-Content .\runtime\server\logs\web_tunnel.stderr.log -Tail 80
   ```

3. 查看 `precheck`、`ERR` 和连接重试信息：域名查询失败需要检查开服电脑的 DNS；`dial tcp ...:7844` 超时或拒绝连接需要检查出站 TCP 连通性。当前默认 `http2` 使用出站 TCP 7844；`quic` 使用出站 UDP 7844，切换协议不保证能绕过当前网络限制。[Cloudflare 网络要求](https://developers.cloudflare.com/cloudflare-one/networks/connectors/cloudflare-tunnel/configure-tunnels/tunnel-with-firewall/)
4. 如果日志有注册成功后又断开的记录，应结合最后的错误排查。修改网络后先用状态入口和手机重试；需要重启隧道或切换传输协议时，网址可能改变，使用新显示的网址。

### 自有固定域名或其他 HTTPS 代理

外网需要 HTTPS 反向代理或隧道，将**整个站点**代理到本机网页端口，并支持 WebSocket Upgrade。例如把一个固定的 `https://game.example.com` 转发到 `http://127.0.0.1:18084` 后，先停止局域网网页网关，再执行：

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File .\deployment\Start-Web.ps1 -Action stop
powershell -NoProfile -ExecutionPolicy Bypass -File .\deployment\Start-Web.ps1 -PublicOrigin https://game.example.com -BindAddress 127.0.0.1
```

代理保留原来的 `Host` 和 `Origin`，透传 Cookie，不能剥掉 `/web/` 路径。此时用户统一访问配置的 HTTPS 域名；网关只接受已配置来源。HTTPS 页面使用 WSS，资源同源下载。手工管理的随机隧道地址改变后，应停止该网页网关，再用新的 `-PublicOrigin` 重新启动，浏览器身份也可能随网址改变。

现有 iOS 外网入口的邀请鉴权保持独立；不要用其 `/game` 地址替换网页的 `/web/game`。

## 游客身份、进度和支持边界

- 第一次进入时，网关生成 `web_...` 游客 ID，使用签名的 HttpOnly Cookie 保存在该浏览器，游戏服按该 ID 保存角色。页面只有服务端就绪后才建立游戏会话。
- 同一网址、同一浏览器再次进入会沿用角色。浏览器 Cookie 清除、隐身窗口、换浏览器或换域名可能变成新游客；不同电脑不会自动共享角色。当前没有新增手机号/密码登录，也没有把 Android、iOS 游客角色自动合并。
- IndexedDB 保存客户端设置及更新资源；浏览器不允许持久化时会记录控制台提示。角色进度在服务端，不在 IndexedDB 中。不要把游戏管理工具账号当作玩家账号。
- 网关的签名 Cookie 只管理个人测试入口及游客标识；原游戏协议仍是原来的游客登录协议，不是可直接用于公开商业运营的账号认证体系。
- 原游戏界面、战斗、卡牌、装备、远航和活动逻辑继续使用共享源码。鼠标点击/拖动和手机触摸进入原来的触控系统；中文输入使用浏览器输入框，文字显示使用原有位图字体或浏览器系统字体。动态系统字体的字形和原生平台可能有差异。
- 背景音乐和音效使用 Web Audio，“进入”按钮的用户手势用于解锁播放。未接入充值、第三方渠道 SDK、系统定时推送及体感权限。旧公告地址可能已失效；公告层保留关闭按钮和超时处理。
- 建议先使用较新的桌面 Chrome/Edge 验证，再验证手机 Safari/Chrome。浏览器后台冻结、图形上下文丢失及网络切换需要实际确认；上下文丢失时页面显示刷新提示。

## 构建产物、源码与维护

| 目录/文件 | 用途 |
|---|---|
| `client/web/native` | 浏览器平台、WebSocket、下载、图片/文字、音频、输入、游客渠道 |
| `client/web/shell` | 画布宿主页、启动进度、错误提示、浏览器交互桥 |
| `client/web/prepare.py` | 生成 UTF-8 副本，应用仅 Web 生效的适配；原 GBK 移动端源码保持原编码 |
| `client/web/build.py` | 编译共享游戏及引擎，打包公共资源；首次初始化 SDK 后并行编译 |
| `client/web/setup.py` | 下载固定 Emscripten SDK 源码并安装对应工具链 |
| `client/web/generated` | 临时源码、资源、编译对象及日志；不提交 |
| `outputs/web` | 发布文件；不提交，可从 Actions 下载 |
| `deployment/web_client.py` | 同源 HTTP、浏览器会话、WebSocket→本地 TCP |
| `deployment/web_external.py` | Web 免费隧道、网址发现、就绪检查与进程管理 |
| `runtime/server/web-external-url.txt` | 当前 Web 外网网址；停止外网入口后删除 |
| `runtime/server/private/web-session.key` | 本机游客 Cookie 签名密钥；保留在当前部署，不提交或公开 |
| `runtime/server/logs/browser.*.log` | 网页网关运行日志 |

图片、布局、Lua 和配置仍修改 `resources/client`，C++ 玩法仍修改 `client/source/Game/Classes`。更改后重新构建 Web 客户端；服务端资源/数值更新按现有部署文档操作。不要长期编辑 `generated` 或 `outputs` 内的副本。

Web 构建复用 `client/ios/Sources.cmake` 中的引擎和 Protobuf 文件清单，但筛掉原生平台文件，使用自己的平台实现。WebP 从仓库中的固定版本源码构建，不链接 Android ARM 或旧 iOS 静态库。首次下载 SDL/PNG/JPEG 编译端口需要外网，错误保留在编译日志中。

## 首次验证顺序

换电脑、浏览器或服务端配置后，建议按以下顺序复查；本机已验证的范围及未覆盖项见[实测记录](reference/WEB_GAMEPLAY_VALIDATION.md)。

1. 构建结束产生完整的 `outputs/web`，启动网页服务后 `/web/config` 返回就绪。
2. 点击进入，确认资源下载、WebAssembly 初始化和游戏原始登录页显示。
3. 点击“男人帮1区”，完成建角/登录，确认重新刷新页面仍进入原角色。
4. 简单游玩普通/精英远航、卡牌升级、装备穿戴/升级、海鲜饭、运宝、黑胡子、勇闯、抽卡和当前开放活动；服务端等级或时间门槛仍按原游戏规则。
5. 验证中文输入、拖动、音效、断线后重新登录以及同一 Wi-Fi 手机访问。
6. 点击停止服务，确认网页入口一起关闭；重新用网页启动入口开启后角色进度保留。

排错时同时提供 `client/web/generated/logs`、浏览器控制台/Network 中失败的请求，以及 `runtime/server/logs/browser.stderr.log`。不要把“链接成功”当成已完成全部玩法验证。

平台实现依据：[Emscripten WebGL 支持](https://emscripten.org/docs/porting/multimedia_and_graphics/OpenGL-support.html)、[浏览器网络限制与 WebSocket](https://emscripten.org/docs/porting/networking.html)、[SDK 安装说明](https://emscripten.org/docs/getting_started/downloads.html)。

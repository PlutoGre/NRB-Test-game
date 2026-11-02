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

默认入口适合同一局域网。外网需要 HTTPS 反向代理或隧道，将**整个站点**代理到本机网页端口，并支持 WebSocket Upgrade。例如把一个固定的 `https://game.example.com` 转发到 `http://127.0.0.1:18084` 后，先停止现有受管服务，再执行：

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File .\deployment\Start-Web.ps1 -PublicOrigin https://game.example.com -BindAddress 127.0.0.1
```

代理保留原来的 `Host` 和 `Origin`，透传 Cookie，不能剥掉 `/web/` 路径。此时用户统一访问配置的 HTTPS 域名；网关只接受已配置来源。HTTPS 页面使用 WSS，资源同源下载。随机隧道地址改变后，应停服并用新的 `-PublicOrigin` 重新启动，浏览器身份也可能随网址改变。

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

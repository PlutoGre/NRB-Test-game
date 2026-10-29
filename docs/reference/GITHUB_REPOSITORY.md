> 参考归档：日常操作请先看 [一键部署](../../README.md) 和 [客户端打包](../CLIENT_BUILD.md)。本页保留迁移、排查及其他构建方式的详细说明。

# 完整项目上传 GitHub，然后在 Windows 克隆部署

## 1. 最终使用方式

这个仓库现在按下面的流程组织：

1. 把项目的源码、资源和必需依赖提交到 GitHub 公开仓库。
2. 在任意 Windows x64 电脑安装 Git，`git clone` 后双击 `deployment/server.cmd` 部署服务端。
3. 在 GitHub 的 Actions 页面手动运行 **Build iOS from repository**。
4. 云端直接读取所选分支的源码，生成未签名 iOS 客户端和日志。
5. 下载 IPA，在自己的 Windows/iPhone 上完成个人测试签名和安装。
6. 若需外网测试，按 [外网部署文档](EXTERNAL_DEPLOYMENT.md) 启动本地网关和 Quick Tunnel，在新 iOS 客户端的启动页填写域名和邀请码。

外网入口和 iOS WSS 适配已经包含在源码中，尚未运行验证。工作流的 `server_host` 是局域网后备地址；临时域名在 App 内填写，不需要因为它变化而重新触发构建。不要上传服务端运行目录的 `private/gateway.json` 或把邀请码写进公开仓库。

**完整仓库方式不需要上传分卷 ZIP，也不需要重新制作源码快照。** 修复源码后正常提交并推送，再运行 Actions 就会构建新版本。

当前只准备了仓库文件、脚本和文档，没有替你创建 GitHub 仓库、提交、推送或运行云构建；也没有执行新部署脚本、编译、构建或测试。首次 Xcode 构建仍可能暴露旧 Cocos 兼容错误，需根据实际日志继续修正。

## 2. “整个项目”应该提交什么

这里指源码、资源和运行/构建必需的依赖，而不是把本机所有文件原样装入 Git：

| 提交 | 不提交 |
|---|---|
| `src/server` 的 Java 源码、现成 DZM.jar、XML、初始 SQL、客户端资源 | `.debug` 中的运行数据库、模拟器、日志、截图 |
| `src/client` 的 C++/Lua/Java/Objective-C 源码、项目配置、必需 `.a/.so/.jar` | `obj/bin/build/dist/generated`、Debug/Release 编译缓存、旧 APK/IPA |
| `src/client/ios-modern/vendor` 中固定哈希的依赖源码压缩包 | `src.7z` 等已解压的重复整包 |
| `.github/workflows/ios-build.yml`、`deployment`、`docs` | 下载缓存、解压后的本机 Python/Java/MariaDB |
| `.gitignore`、`.gitattributes`、`README.md` | 个人签名密钥、证书私钥、描述文件、本机 SDK 路径配置 |

`.gitignore` 已按此设置，不会删除本机原有文件。不要加一个全局 `*.jar`、`*.a` 或 `*.so` 忽略规则，否则服务端或旧客户端所需依赖会缺失。数据库运行目录建议始终放在仓库之外。

原工程 `src/client/.gitignore` 曾整体忽略 `libs/` 和 `assets/`；现已调整为保留随源码提供的 SDK/原生依赖，只忽略 Android 生成的 APK 资源目录。初始资源仍统一来自 `src/server/ResourcesAndroid_Encrypt`，克隆后由准备工具复制。

原目录确实存在大于 100 MiB 的中间文件，例如 Android `obj` 中的未剥离 `.so` 和 Visual Studio 缓存，这些已排除。GitHub 普通 Git 会阻止大于 100 MiB 的文件；必要的大文件才考虑 Git LFS。本方案没有预设 LFS，以免额外引入 LFS 依赖和流量计费。[GitHub 大文件说明](https://docs.github.com/en/repositories/working-with-files/managing-large-files/about-large-files-on-github)

## 3. 首次上传：创建空的公开仓库

1. 在 Windows 安装 [Git for Windows](https://git-scm.com/download/win)。
2. 登录 GitHub，右上角 **+ → New repository**。
3. 名称例如 `NRB-Test-game`，选择 **Public**。
4. **这次不要勾选 README、.gitignore 或 License 初始化选项**，因为本机已经有这些仓库入口文件；先创建空仓库。
5. 记下仓库 HTTPS 地址，例如 `https://github.com/YOUR_NAME/NRB-Test-game.git`。

应通过 Git 推送完整仓库，不要用网页拖拽几万个文件。大文件和文件数量会超过网页上传的限制。

## 4. 在本机提交并推送

在当前源码根目录打开 PowerShell；该目录应该能看到 `src`、`deployment`、`docs`、`.github` 和本文件引用的 README。

如果本机目录还没有 Git 仓库：

```powershell
git init -b main
git config user.name "Your Name"
git config user.email "YOUR_GITHUB_EMAIL"
git add .
```

替换姓名和邮箱；这里设置的是当前仓库身份，不修改整个电脑的 Git 全局配置。

在 commit/push 前，使用文件清单工具检查暂存区的文件名和大小：

```powershell
python .\deployment\publish_inventory.py
git status --short
```

该工具不编译、不执行测试、不读取文件内容或上传任何东西；它检查暂存/已跟踪条目是否含超限单文件、签名文件、本机调试缓存，以及关键部署/iOS 依赖是否漏提交。它不是完整敏感信息扫描，也不能保证所有历史第三方渠道配置均可用于新账号。

如果没有普通 Python，但已经使用部署工具准备过 Python，可显式调用：

```powershell
& .\deployment\.bootstrap\python\python.exe .\deployment\publish_inventory.py
```

清单没有阻断项后，提交并推送：

```powershell
git commit -m "Add game sources, Windows deployment and iOS workflow"
git remote add origin https://github.com/YOUR_NAME/NRB-Test-game.git
git push -u origin main
```

把 URL 换成你自己的空仓库地址。Git for Windows 通常会通过凭据管理器引导浏览器登录 GitHub；账号验证在本机完成，不把密码或 Token 写进仓库或脚本。

首次上传包含较多原始资源和旧 SDK，可能耗时较长。不要因为进度慢就强制 `git add -f` 把被忽略的大缓存补进去。

### 已经初始化/提交过 Git 的情况

- 已有 origin：先看 `git remote -v`，确认目标，不要盲目再添加或覆盖。
- `.gitignore` 不会自动取消跟踪旧文件。若只是新暂存的多余文件，可用 `git rm --cached -- "具体文件路径"` 从索引移除，文件仍留在本机；目录需要明确路径及 `-r`。
- 如果超大文件或私钥已经进入历史提交，后面添加 `.gitignore` 不能清理历史；不要直接推送该历史。对于这次首次发布，最简单的是复制有效工作文件到独立干净目录，不带旧 `.git`，从新的干净历史发布。
- 不需要为了上传而删除本机数据库、模拟器或旧工程，它们可以继续留在被忽略的目录中。

## 5. 在另一台 Windows 克隆并一键部署

先安装 Git，然后在目标电脑执行：

```powershell
git clone https://github.com/YOUR_NAME/NRB-Test-game.git D:\test\NRB-Test-game
Set-Location D:\test\NRB-Test-game
```

公开仓库的普通克隆不要求这台电脑登录你的 GitHub 账号。然后选择一种方式：

### 默认一键部署

双击 `D:\test\NRB-Test-game\deployment\server.cmd`。默认安装到当前用户 `%LOCALAPPDATA%\OnePieceServer`，自动选择有默认网关的唯一 IPv4 网卡。

### 指定参数部署

更推荐把运行目录与源码分开，并明确新电脑 IP：

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File .\deployment\Deploy-Server.ps1 `
  -InstallDir D:\OnePieceServer -PublicHost 192.168.1.100
```

默认资源端口为 `18081`，游戏端口为 `25524`，数据库仅监听 `127.0.0.1:13306`。首次会下载并校验便携式 Python、JDK、MariaDB，不要求预装这些组件。

如果需要添加局域网防火墙规则，在管理员 PowerShell 中用同样命令加 `-OpenFirewall`；仅允许专用网络、本地子网访问游戏和资源端口，不开放数据库。

多网卡无法自动选择、目标路径含中文、缺少运行库或下载失败时，按 [服务端文档](SERVER_DEPLOYMENT.md) 处理。当前脚本不是“无需网络、无需运行库、适配任何 Windows”的安装器；支持的是文档中的 Windows x64 环境。

**GitHub 克隆出来的是源码和初始 SQL，不包含旧电脑实际玩家进度。** 要保留账号/进度，按服务端文档的备份恢复流程迁移，不能把“新服务器能启动”当作“旧数据已迁移”。

## 6. 直接从完整仓库生成 iOS 客户端

### 6.1 取得实际服务器参数

部署后查看新电脑：

```text
D:\OnePieceServer\client-config\deployment.json
```

例如 host 为 `192.168.1.100`，webPort 为 `18081`，gamePort 为 `25524`，resourceVersion 为 `1.78.6`。

### 6.2 启动云构建

1. 在 GitHub 打开你上传的仓库。
2. 点击顶部 **Actions**。
3. 左侧选择 **Build iOS from repository**。
4. 点击 **Run workflow**，选择要构建的分支（通常为 `main`）。
5. 按实际部署填写下表，再点击绿色运行按钮。

| 参数 | 示例 | 含义 |
|---|---|---|
| server_host | `192.168.1.100` | 手机能访问的新电脑 IPv4 或域名 |
| web_port | `18081` | 资源 HTTP 端口 |
| game_port | `25524` | 游戏 TCP 端口 |
| resource_version | `1.78.6` | 与该次部署资源一致的版本 |
| bundle_id | `org.onepiece.local` | 无签名测试用包标识；后续正式签名应换为自己匹配的标识 |

工作流必须存在于默认分支才会显示手动运行入口；当前账号也需有仓库写权限。[GitHub 手动运行文档](https://docs.github.com/en/actions/how-tos/manage-workflow-runs/manually-run-a-workflow)

工作流使用 `macos-15` 标准构建机，只在你手动操作时运行，不会每次 push 自动构建。公开仓库的标准 GitHub-hosted runner 按当前规则免费；大型 runner、LFS 等其他服务不包含在这个结论中。[官方说明](https://docs.github.com/en/actions/how-tos/write-workflows/choose-where-workflows-run/choose-the-runner-for-a-job)

云构建不需要访问你家里的 `192.168.*` 服务，它只把地址写入客户端。没有必要为了编译开内网穿透；实际手机安装后才连接新电脑的局域网服务。

### 6.3 下载与继续处理

运行结束后，在该次运行 Summary 的 Artifacts 中下载：

- **ios-build-logs**：构建环境、CMake/Xcode 日志和结果记录。构建失败时也尽量上传。
- **ios-unsigned-app**：成功时提供 `.app` 压缩包及 `onepiece-unsigned-for-resigning.ipa`。

这是无签名构建，不需要在 GitHub 填 Apple 密码，也没有上传 App Store 的步骤。**未签名 IPA 不能直接在普通 iPhone 上运行**；可以按 [客户端文档第 8 节](CLIENT_BUILD.md#8-ios签名和安装到自己的-iphone) 在 Windows 使用 AltStore Classic 完成个人测试签名，或后续配置自己的开发者证书。

如果第一轮构建失败，先查看 **Build unsigned ARM64 iPhone app** 步骤的第一条实际错误，下载日志后继续修正。当前工程的 iOS 适配尚未完成真实构建和真机验证，不应预期“上传后第一次运行必定成功”。

## 7. 后续代码更新

在任何有写权限的开发电脑上：

```powershell
git pull --ff-only
# 修改源码后，查看需要提交的改动。
git status --short
git add .
python .\deployment\publish_inventory.py
git commit -m "Describe your change"
git push
```

再去 Actions 手动运行即可。所选分支的新源码会直接参与构建；无需运行 `prepare_github_upload.py`、无需替换 source 分卷。

运行目录中的数据库、服务器配置、客户端下载缓存不会被 `git pull` 自动迁移或刷新。更新游戏服 JAR/XML 时按服务端文档停止后重新部署，先备份直接修改过的运行配置。

换电脑或 DHCP 导致 IP 变化后，重新部署正确地址，并使用对应参数重新生成客户端。旧 IPA 中写入的 IP 不会因为仓库更新而自动改变。

## 8. 仍保留的精简 ZIP 方式

`deployment/github/ios-build.yml` 和 `prepare_github_upload.py` 是之前“只上传 iOS 快照”的可选方案，工作流名称为 **iOS unsigned build**。

本次完整仓库使用的则是 **根目录** `.github/workflows/ios-build.yml`，名称为 **Build iOS from repository**。二者不要混用：完整仓库不需要 `source/*.zip.001`；精简快照仓库必须有分卷。

# iPhone 外网接入（可选）

只在手机不与开服电脑处于同一局域网时使用。先完成 [一键开服](../README.md) 和 [新 iOS 客户端打包、签名安装](CLIENT_BUILD.md)。Android 当前不支持这条 WSS 路线。

## 1. 开启外网入口

在仓库根目录双击 **`deployment/external.cmd`**，或运行：

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File .\deployment\Start-External.ps1
```

它会启动本部署的服务端、网关和 Quick Tunnel，显示两项内容：

```text
iPhone HTTPS URL: https://随机名称.trycloudflare.com
Invitation (keep private): 本机生成的邀请码
```

默认自动使用项目内的 `runtime/server`。若开服时指定了其他运行目录，命令也要加相同的 `-InstallDir`，例如 `-InstallDir runtime/server-2`；相对路径均以项目根目录为基准。

## 2. 手机连接

1. 完全关闭并重新打开游戏。
2. 在启动页填写上述 **HTTPS 域名**和**邀请码**，不要在域名后加 `/game`。
3. 点击“检测并进入外网游戏”，然后登录并开始游戏。

电脑需保持开机、联网。每次重新建立隧道可能换域名，手机重新填写即可，不用重打包；修改域名时邀请码会被清空，需要重新粘贴。

## 3. 查看和关闭

在仓库根目录执行：

```powershell
# 显示当前域名、邀请码和进程状态
powershell -NoProfile -ExecutionPolicy Bypass -File .\deployment\Start-External.ps1 -Action status

# 只关闭外网入口，保留本地服务端
powershell -NoProfile -ExecutionPolicy Bypass -File .\deployment\Start-External.ps1 -Action stop
```

完整关闭服务端可双击项目根目录的 `停止服务.cmd`，会同时关闭该部署的网关和隧道。

无法连接时先确认开服成功，再查看运行目录 `logs` 下的 `gateway`、`tunnel` 日志。原理、邀请码更换及更多排查见 [外网参考说明](reference/EXTERNAL_DEPLOYMENT.md)。此链路尚未实际联调验证。

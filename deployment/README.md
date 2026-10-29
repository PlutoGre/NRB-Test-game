# 部署工具入口

- **启动：** 双击项目根目录的 `启动服务.cmd`；首次自动部署，以后启动已有实例。
- **停止：** 双击项目根目录的 `停止服务.cmd`；停止该实例并保留数据。
- 完整操作见 [仓库 README](../README.md)，`server.cmd` 保留为手动部署入口。
- **客户端打包：** 按 [iOS / Android 打包流程](../docs/CLIENT_BUILD.md) 执行。
- **Android 打包：** 工具放到项目 `tools` 后，双击 `build-android.cmd`。
- **iPhone 外网接入（可选）：** 双击 `external.cmd`，见 [接入步骤](../docs/EXTERNAL_DEPLOYMENT.md)。

默认运行目录是项目内的 `runtime/server`，Android 产物在 `outputs/android`。相对路径参数以项目根目录为准，脚本不依赖启动时的工作目录；自定义 `-InstallDir` 时各命令使用同一个参数。

旧的迁移、签名和故障排查说明保存在 [参考资料](../docs/reference/README.md)。

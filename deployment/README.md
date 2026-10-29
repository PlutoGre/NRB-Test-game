# 部署工具入口

- **一键开服：** 双击 `server.cmd`，完整操作见 [仓库 README](../README.md)。
- **客户端打包：** 按 [iOS / Android 打包流程](../docs/CLIENT_BUILD.md) 执行。
- **iPhone 外网接入（可选）：** 双击 `external.cmd`，见 [接入步骤](../docs/EXTERNAL_DEPLOYMENT.md)。

默认运行目录是 `%LOCALAPPDATA%\OnePieceServer`。使用自定义 `-InstallDir` 时，部署、启停及外网命令都要指定同一个目录。

旧的迁移、签名和故障排查说明保存在 [参考资料](../docs/reference/README.md)。

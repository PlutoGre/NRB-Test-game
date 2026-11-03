# 历史归档

这里保留原始工程附带的历史资料，不参与当前一键部署和客户端打包。

| 目录 | 原始内容 |
|---|---|
| `client-resources` | 原明文客户端资源、Lua 源码及旧 Windows 资源附带文件 |
| `server/legacy-source` | 原 Code_Server 附带的 PHP 后台、激活码、充值示例、导表工具源码和历史日志等 |
| `server/embedded-admin` | 原客户端资源包 `title/summary_dzm` 中夹带的运营后台和表格资料 |
| `server/mobile` | 原 HzwMobile 发布资料 |
| `server/launchers` | 原服务端直接启动脚本 |
| `server/resource-update.php` | 原 PHP 资源更新入口；当前更新接口由部署脚本生成 |

历史文件保留原有内部内容和路径写法，不能把归档脚本作为当前部署入口。需要参考其中的实现时，将有用改动整合到当前 `client`、`server`、`resources` 或 `deployment` 中。

原使用说明、引擎文档和数据库设计资料另存于 [`docs/legacy`](../docs/legacy)。

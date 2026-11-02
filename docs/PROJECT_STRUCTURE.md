# 项目目录与维护约定

本次将客户端、服务端、公共资源、共享脚本、文档和历史资料分开。保留根目录的部署入口，以及原 Cocos/Ant/NDK 工程内部需要的相对目录关系。

```text
NRB-Test-game/
├─ client/
│  ├─ source/              共享游戏源码、Android 工程、旧引擎及依赖
│  └─ ios/                 当前 iOS 平台实现和构建入口
├─ server/
│  ├─ source/              Java 源码、依赖及原工程文件
│  ├─ config/              XML 玩法模板和 Hibernate 部署模板
│  ├─ distribution/        一键部署使用的基础 DZM.jar
│  └─ database/            初始 SQL；legacy 中为旧快照
├─ resources/client/       游戏公共资源的唯一发布源
├─ scripts/                公共路径解析和部署配置生成脚本
├─ deployment/             一键部署、Android 构建、管理工具及外网入口
├─ docs/                   使用流程、扩展指南、验证记录和历史文档
├─ archive/                原资源快照、旧 PHP 后台及历史工具
├─ runtime/                本机服务、玩家数据库、日志和备份，不入 Git
├─ outputs/                APK、IPA 等产物，不入 Git
├─ tools/                  本机 SDK、NDK、Ant 等，不入 Git
├─ .github/workflows/      GitHub Actions
├─ 启动服务.cmd
├─ 停止服务.cmd
└─ NRB游戏管理.exe
```

## 修改内容时去哪里

| 维护任务 | 编辑位置 |
|---|---|
| 客户端玩法、页面、战斗、数据解析 | `client/source/Game/Classes` |
| Android Java 与原生工程 | `client/source/Game/proj.android` |
| 协议定义和生成文件 | 客户端 `client/source/Game/Protobuf`，服务端 `server/source/src` 中相应协议文件 |
| iOS 系统接口、网络、启动页 | `client/ios/native` |
| 服务端玩法、实体、奖励、持久化 | `server/source/src/com/oldmountain/dzm` |
| 服务端卡牌、装备、奥义、活动数值 | `server/config/xml` |
| 游戏内图片、布局、Lua、文本和音效 | `resources/client` |
| Android 图标、原生布局、渠道覆盖资源 | Android 工程中的 `res_*` 及渠道目录 |
| 初始化新数据库 | `server/database/dzm.sql`、`dzm_stat.sql` |
| 已有玩家进度 | 当前部署的数据库；见服务端扩展指南 |
| 一键部署和构建方式 | `deployment`、`scripts`、`.github/workflows` |

## 旧路径对照

| 原路径 | 当前路径 |
|---|---|
| `src/client` | `client/source`，以下两个例外单列 |
| `src/client/ios-modern` | `client/ios` |
| `src/client/document` | `docs/legacy/client-engine` |
| `src/server/Code_Server/server/src`、`lib` | `server/source/src`、`lib` |
| `src/server/Code_Server/server/xml`、`hibernateconfig` | `server/config/xml`、`hibernateconfig` |
| `src/server/Code_Server/server/DZM.jar` | `server/distribution/DZM.jar` |
| `src/server/dzm.sql`、`src/server/Code_Server/dzm_stat.sql` | `server/database/dzm.sql`、`dzm_stat.sql` |
| `src/server/ResourcesAndroid_Encrypt` | `resources/client`，内嵌后台另列 |
| 上述资源目录的 `title/summary_dzm` | `archive/server/embedded-admin` |
| `src/server/haizei/Resource_Client/Resource_Client` | `archive/client-resources` |
| 原 Code_Server 其余文件 | 历史 SQL 到 `server/database/legacy`，设计资料到 `docs/legacy/database`，其余到 `archive/server/legacy-source` |
| `src/tools` | `scripts`，原文字说明到 `docs/reference/RESOURCE_CONFIGURATION.txt` |
| 根目录 `部署与构建.md` | `docs/DEPLOYMENT_AND_BUILD.md` |

## 生成目录与升级

- 日常入口仍是根目录的启动、停止、管理工具，以及 `deployment/build-android.cmd`。
- GitHub iOS 工作流和精简源码包入口已改用 `client/ios`，准备时从 `client/source` 和 `resources/client` 取文件。
- 复制到新电脑时优先完整 clone 仓库，不要只复制某一个源码目录。相对路径参数仍以仓库根目录为基准。
- 已有 `runtime`、`outputs` 和 `tools` 未移动。原本本机修改的客户端部署配置随文件移动保留，未并入这次内容修改。
- 生成的 `assets`、`generated`、部署后的 HTTP 资源副本不作长期编辑入口。修改公共资源后按打包文档刷新配置、资源版本和客户端。
- `archive` 和 `docs/legacy` 保留原资料，里面的旧路径不属于当前部署约定。

## 本次验证范围

按当前 AGENTS.md，本次只检查文件移动清单、路径引用和 Git 差异，未执行服务端部署、构建、打包、测试或 GitHub Actions。目录整理前的 iOS 成功构建见 [验证记录](reference/IOS_BUILD_VALIDATION.md)；新目录布局仍需维护者重新构建确认。

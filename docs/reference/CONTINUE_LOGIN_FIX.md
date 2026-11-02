# 冒险“连续”页等待后空白

## 原因

问题发生在冒险中的“连续登录”活动（id=10），与独立的普通登录奖励不是同一个接口，也不是整个冒险系统缺少总开关。

默认配置与现有服务端JAR有两处不一致：

1. `adventuresConfig.xml` 把id=10设成 `timeType=1`，因此活动列表始终返回入口；但是 `AdventureContinueLoginHandler._checkValid()` 仍强制检查日期，默认结束时间是 `2020-07-11_12:00:00`，现在详情请求会被拒绝。
2. 查询请求操作码为186，客户端等待187；拒绝请求时，现有JAR却用错误编号126（勇闯领奖接口）返回错误，无法解除187对应的等待。之后请求超时，页面也没有收到用于创建奖励列表的登录状态。

已静态检查源码和仓库自带 `DZM.jar` 的字节码，二者均有上述日期校验和错误编号问题。这能解释“其他冒险页能显示，只有连续页等待后为空白”，无需先把所有限时活动都开启。

## 本次修改

- 默认id=10改为明确的日期活动 `timeType=3`，结束时间改为 `2037-12-31_23:59:59`。入口列表和现有JAR详情接口都能识别这一格式，因此配置修复不依赖重新编译Java，也不需要重新生成APK。
- 选择2037年是因为旧JAR把秒时间戳转成有符号32位整数，不能随意写成2099年。新配置不是对无限期运行能力的承诺，后续时间字段现代化应另做迁移。
- 一键启动增加旧配置迁移：仅匹配id=10、`timeType=1`、2015年原开始日期、2020年原结束日期、`version=1`这一组旧默认值；保留其他字段。自定义排期、已关闭活动等不自动启用。
- 修改运行目录前，备份原文件到 `adventuresConfig.xml.before-continue-login-fix`。只有游戏进程停止时才执行迁移；服务已运行时提示先停止再启动。
- 不更改活动版本号、奖励内容或玩家领取记录。

Java源码另外修正了详情错误编号187、领奖错误编号189、领奖响应的秒时间戳，以及常驻活动的校验语义。这些源码修改需要以后重新构建服务端JAR才生效；本次没有更新自带JAR。默认配置兼容修复可以先用于当前JAR。

## 已部署电脑怎么更新

在那台Windows电脑的仓库根目录执行：

```powershell
git pull
```

然后：

1. 确认近期游戏进度已保存，再双击根目录的 **停止服务.cmd**。
2. 双击根目录的 **启动服务.cmd**。
3. 完全退出客户端后重新打开，再进入“冒险 → 连续”。

旧默认配置被迁移时，启动输出会包含 `Updated legacy continuous-login activity configuration`，并显示XML备份路径。新电脑首次部署直接使用修正后的配置，不需要出现迁移提示。

自定义运行目录时，继续使用原 `-InstallDir` 停止和启动同一个实例。仅 `git pull` 后保持旧游戏进程运行，配置修复不会自动生效。

若未自动迁移，可只读检查实际运行目录的 `server/xml/adventuresConfig.xml` 中id=10的记录：它可能已经被手工修改，不符合旧默认配置的精确匹配条件。不要为触发迁移而覆盖整个运行目录或重新导入数据库。

当前Windows停止工具的保存边界见 [服务端数据文档](../extension/01_SERVER_DATA_AND_REWARDS.md#停服和保存的实际边界)。本修复不处理尚未落库的内存进度。

## 检查状态

本次完成源码、字节码和差异检查；按照当前AGENTS.md约束，没有执行编译、迁移脚本、停服启动或模拟器复测。仍需在发生问题的电脑验证：奖励列表可显示、首日奖励可领取、重复领取被拒绝、重登后记录保留。

相关文件：

- [启动与旧配置迁移](../../deployment/server.py)
- [默认活动配置](../../src/server/Code_Server/server/xml/adventuresConfig.xml)
- [连续登录服务端处理](../../src/server/Code_Server/server/src/com/oldmountain/dzm/packethandler/AdventureContinueLoginHandler.java)
- [客户端连续页](../../src/client/Game/Classes/AdventureContinuePage.cpp)
- [客户端等待逻辑](../../src/client/Game/Classes/waitingManager.cpp)

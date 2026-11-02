# 01 服务端数据保存、修改、更新与奖励发放

[返回扩展指南](README.md)。分析基线与边界见总览。下列命令是后续维护示例，本次没有执行停服、SQL更新、GM命令或构建。

## 1. 先区分三类“数据”

| 类型 | 编辑源 | 实际生效位置 |
|---|---|---|
| 玩家进度、资产、背包 | 数据库中的玩家记录 | MariaDB 的 `dzm` 数据库，加上 Java 内存缓存 |
| 卡牌、装备、奥义、活动模板 | `src/server/Code_Server/server/xml` | 默认 `runtime/server/server/xml`，启动/热加载后进入模板缓存 |
| 服务端 Java 逻辑 | `src/server/Code_Server/server/src` | 默认 `runtime/server/server/DZM.jar` 中的字节码 |

`src/server/dzm.sql` 是新部署的初始库快照，修改它不会改变已经运行的数据库。`runtime/server/dbdata` 才是本机 MariaDB 数据目录，不应直接编辑其中的数据文件。`dzm_stat` 用于统计记录，不是玩家资产主库。

默认部署资料在 `runtime/server/client-config/deployment.json`，进程/端口状态在 `runtime/server/.onepiece-server.json`。数据库登录参数由部署脚本放在 `runtime/server/private/db-client.ini`；维护工具引用该文件，不要把密码复制到文档或仓库。数据库端口应以本机配置为准。

## 2. 请求、内存修改和落库链路

1. [DZM.java](../../src/server/Code_Server/server/src/com/oldmountain/dzm/DZM.java) 加载 `xml/config.xml`、Hibernate配置和 `XMLTemplateService` 模板，启动 MINA TCP 服务。
2. [SessionHandler.java](../../src/server/Code_Server/server/src/com/oldmountain/dzm/net/SessionHandler.java) 收包后交给 [PacketHandler.java](../../src/server/Code_Server/server/src/com/oldmountain/dzm/net/packet/PacketHandler.java)，按 Protobuf 操作码调用 `packethandler`。
3. 业务通过 `session.playerid` 获取 [ObjectAccessor.java](../../src/server/Code_Server/server/src/com/oldmountain/dzm/net/ObjectAccessor.java) 中的 `Player`。玩家对象持有卡牌、装备、奥义、背包、阵容等集合。
4. [Player.java](../../src/server/Code_Server/server/src/com/oldmountain/dzm/entity/Player.java) 的 setter 及 `addNeedUpdate()` 标记脏数据；新增对象的一部分调用 `createSync()` 立即插入，已有对象更新常通过待落库集合处理，具体取决于业务路径。
5. [DataLander.java](../../src/server/Code_Server/server/src/com/oldmountain/dzm/persist/DataLander.java) 调用 [EntityManagerImpl.java](../../src/server/Code_Server/server/src/com/oldmountain/dzm/persist/EntityManagerImpl.java) 的 `updateSync()`，经 Hibernate事务提交到 MariaDB。定时任务源码设置为启动后900秒首次执行、以后每600秒执行。
6. 响应通过 `Player.sendPacket()` 发回；客户端 `PackageLogic.cpp` 解析更新 `ServerDateManager` 等缓存，再刷新页面。

因此，服务端内存、客户端画面和数据库可能暂时不一致。关闭客户端连接时，`removeOnlineSession()` 主要移除会话，不能把它当作“立即保存全部玩家数据”的保证。

### 停服和保存的实际边界

[ServerShutDown.java](../../src/server/Code_Server/server/src/com/oldmountain/dzm/ServerShutDown.java) 注册了 JVM shutdown hook，其中调用 `DataLander.landingImmediately()`。但当前 Windows 部署工具的 `terminate_owned()` 使用 `TerminateProcess` 结束游戏进程，**不能依赖它执行 JVM hook**。

重要进度更新后，维护前应确认对应数据已落库：可以等待落库周期，并用只读 SQL 比较目标玩家状态。备份工具备份的是数据库中已有的数据，无法取回被强制结束进程时仍只在内存中的改动。后续可单独扩展经过鉴权的“立即落库并优雅停服”管理命令；本次没有实现它。

## 3. 常用数据库表与 ID

映射源在 `src/server/Code_Server/server/src/com/oldmountain/dzm/entity/*.hbm.xml`。

| 表 | 用途 | 关键字段 |
|---|---|---|
| `player` | 玩家基础信息 | `id`、`puid`、`level`、`exp`、`silvercoins`、`sysgoldcoins`、`rechargegoldcoins`、`viplevel` |
| `mapping` | 平台账号与游戏角色映射 | 用来定位账号对应的角色；GM 的 `player` 后缀使用 `puid`，不是昵称 |
| `disciple` | 玩家卡牌实例 | `id` 为实例ID，`itemid` 为卡牌模板ID，`playerid` 为所有者 |
| `equip` / `skill` | 装备/奥义实例 | 实例ID、模板ID、等级、当前属性及归属 |
| `bag` | 可堆叠道具 | `playerid`、`itemid`、`count`；内存集合按道具模板ID索引 |
| `soul` | 卡牌影子 | 玩家、卡牌模板和数量 |
| `battle` | 上阵位置、装备技能引用及合成后的战斗属性 | 多处保存的是实例ID；缘分列表和激活状态也在阵容数据中 |
| `career` / `stats` | 远航进度与日常状态 | `UserStats` 实体映射到 `stats`，避免只改等级遗漏开放条件 |
| `rewards` | 可领取奖励传书 | `playerId`、`rewardType`、`rewardContent`、`status`、时间 |
| `teambuffer` | 全队修行等级与经验 | 生命、攻击、防御、意志四组字段 |

同一个模板可以产生多个实例。把“装备模板32”当成“装备实例32”传给穿戴接口，会选错对象或报不存在。

钻石显示总额 = `sysgoldcoins + rechargegoldcoins`。`Player.consumeGold()` 先消耗系统钻石，再消耗充值钻石。`rechargenum` 是累计充值值，不能与当前钻石余额混为一谈。

## 4. 修改玩家数据

### 4.1 首选业务方法或奖励领取

扩展后台时复用已有逻辑：

| 目标 | 已有方法 |
|---|---|
| 系统赠送钻石 | `UserPropertyServices.changeProperty(player, 1001, count)` / `player.increaseSysGoldCoins(count)` |
| 贝里 | `UserPropertyServices.changeProperty(player, 1002, count)` |
| 可堆叠道具 | `BagToolServices.addBagTool(player, itemId, count)` |
| 新卡牌 | `DiscipleUtil.newDisciple(player, itemId, level, exp)`，同时创建天生奥义 |
| 新装备 | `EquipServices.createEquitAlone(...)` 或 `EquipUtil.newEquip(...)`，注意前者原代码拼写 |
| 新奥义 | `SkillServices.createSkillAlone(...)` 或 `SkillUtil.newSkill(...)` |
| 可领取补偿 | `RewardServices.createCompensateReward(...)` |

这些是 Java 内部方法，不能直接在 PowerShell 中调用。新管理入口还需做角色定位、参数校验、持久化、响应及审计；调用 setter 不会自动替管理入口补齐客户端推送。业务中新增/改变卡牌、装备和奥义后，还要处理图鉴、阵容属性重算及相关业务统计。

### 4.2 停服后直接修改数据库

适用于个人测试的小范围修正，避免在线内存覆盖 SQL 结果：

1. 确认近期操作已保存，停止当前部署。
2. 执行备份；备份会按需启动数据库，游戏仍保持停止。
3. 查询角色ID和原值，事务内修改，核对影响记录，再提交。
4. 启动游戏服务，客户端重新登录，核对余额和对应功能。

仓库根目录 PowerShell：

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File .\deployment\Deploy-Server.ps1 -Action stop
powershell -NoProfile -ExecutionPolicy Bypass -File .\deployment\Deploy-Server.ps1 -Action backup
$dbOptions = (Resolve-Path '.\runtime\server\private\db-client.ini').Path
& '.\runtime\server\runtimes\mariadb\bin\mariadb.exe' "--defaults-file=$dbOptions" --database=dzm --execute='SELECT id,puid,name,level,sysgoldcoins,rechargegoldcoins FROM player ORDER BY id;'
```

以下 SQL 中的 `12345` 必须换成已核实的角色ID。示例仅增加系统钻石，不改变累计充值或VIP：

```sql
START TRANSACTION;
SELECT id,name,sysgoldcoins,rechargegoldcoins FROM player WHERE id=12345 FOR UPDATE;
UPDATE player SET sysgoldcoins=sysgoldcoins+1000 WHERE id=12345;
SELECT ROW_COUNT() AS changed_rows;
SELECT id,sysgoldcoins,rechargegoldcoins FROM player WHERE id=12345;
-- 只在核对 changed_rows=1 且目标正确后，交互执行 COMMIT；否则 ROLLBACK。
```

不要只改 `disciple.itemid` 来“换一张卡”：实例属性、天生技能、阵容和图鉴都可能仍对应旧卡。道具入库也应优先复用服务方法；直接插入要兼顾已有堆叠、实例自增ID及来源字段。

## 5. 发物品和钻石给客户端

日常操作现在可以直接使用 [Windows 管理工具](../ADMIN_TOOL.md)：服务启动后以管理账号登录，选择游戏名或全服现有角色，发送传书附件。配套模块补齐了完整卡牌、残章及多件装备/奥义的领取；下面保留原始 GM 和后台协议的行为，勿把原 JAR 接口的限制与新工具混用。

### 5.1 GM 直接修改

[UpdateUserInfoByGMhandler.java](../../src/server/Code_Server/server/src/com/oldmountain/dzm/packethandler/UpdateUserInfoByGMhandler.java) 支持以下源码格式：

```text
/add gold 1000
/add coin 10000
/additem tool 1503001 2
/additem hero 2 1
/additem equip 32 1
```

`/add gold` 增加的是**充值钻石和累计充值**，还会计算VIP，不能用于模拟普通系统赠钻。`tool 1503001` 是普通传承药剂；卡牌2、装备32只是现有模板示例，新增卡牌命令中第4段应保留，以避开解析器对 `messages[3]` 的直接访问。

可用性有两层限制：

- 服务端 `DZM.canUseGmTools` 必须为1，由 `xml/config.xml` 的 `CanUseGm` 加载；源码配置为1，但当前部署工具会改为0。源码只看到总开关，不能把它当作完整的管理员权限系统。
- 客户端 [LeaveMsgPage.cpp](../../src/client/Game/Classes/LeaveMsgPage.cpp) 的命令分流在 `_DEBUG` 下才启用。当前 NDK `Application.mk` 为 release 原生配置，**APK 名叫 debug 不代表 C++ 定义了 `_DEBUG`**。不能假设在普通聊天窗口输入这些文本就会发GM包。

`/additem` 的匹配还依赖 `/add` 子串；若改命令识别器，应改成明确的命令解析。针对他人的 `player <puid>` 后缀并非所有子命令都会使用，不能把所有发放命令机械地加后缀。正式扩展建议实现明确鉴权的本地管理入口。

### 5.2 补偿传书：创建奖励，玩家点击领取

调用链：`BackStagePushRewardInfoHandler` 或 GM `/compensate` → `RewardServices.createCompensateReward()` → `RewardDaoImpl` 写 `rewards` → 标记奖励缓存需刷新/推送传书 → `DisposeSysMsgInfoHandler` 领取 → 改资产并回包。

附件统一使用 `t/i/q`：

| 附件 | t（类别） | i（模板/属性ID） | q（数量） |
|---|---:|---:|---:|
| 系统钻石 | 11001 | 1001 | 1000 |
| 贝里 | 11001 | 1002 | 10000 |
| 普通传承药剂 | 61001 | 1503001 | 2 |
| 卡牌影子 | 32000 | 对应卡牌ID | 数量 |
| 完整奥义 | 41000 | 对应奥义ID | 当前领取实现每条只创建1个 |
| 装备 | 51000 | 对应装备ID | 当前领取实现每条只创建1个 |

例：

```json
[{"t":11001,"i":1001,"q":1000},{"t":61001,"i":1503001,"q":2}]
```

对应源码 GM 示例（仅在上述GM条件满足后使用；最后的 `12345` 是角色数据库ID）：

```text
/compensate TestReward 11001:1001:1000,61001:1503001:2 0 12345
```

GM解析器要求内容中包含逗号，不能照普通单附件格式直接少写。`0` 是最近登录时间筛选的下限；标题不能含空格。省略最后目标ID会进入全服补偿分支，应明确指定目标。

领取端的实际限制：未实现直接发完整卡牌的 `DISCIPLE_TYPE` 分支；影子可以发，完整卡牌应走专门逻辑。装备和奥义分支未按 `q` 循环创建，不要用一条 `q=10` 假设会给10件。当前代码也不能视为带幂等键、完整事务和重试保护的现代发奖系统，扩展批量发奖时应补充。

`BackStagePushRewardInfoHandler` 是自定义 TCP/Protobuf 后台协议，源码包含固定异或处理；不是可直接访问的 HTTP REST 接口。当前 Python静态资源服务器不执行旧PHP后台。本次只梳理已有入口，没有开启GM或部署后台。

## 6. 修改后如何更新

### 玩家数据

使用服务端业务回包后客户端更新；SQL维护后重启服务并重登。SQL不会主动触发客户端页面刷新，也不会更新仍在线的内存对象。

### XML模板

[CheckXml.java](../../src/server/Code_Server/server/src/com/oldmountain/dzm/persist/CheckXml.java) 每600秒检查部分XML修改时间，例如卡牌、装备、技能和缘分。它不是所有文件的通用热更新；模板重载也不等于重建所有既有玩家实例。为便于复现，个人测试优先采用明确的停服更新与重启。

修改源码XML后，可在完成备份、停服后，以保存的部署参数重新执行 `-Action install`，它会同步 `DZM.jar`、`xml` 和 `hibernateconfig`，保留已初始化数据库。**“启动服务.cmd”在已有部署上只启动，不会复制新源码文件。**

```powershell
$cfg = Get-Content '.\runtime\server\.onepiece-server.json' -Raw | ConvertFrom-Json
# 在已保存进度、停服和备份之后执行；不要改成示例固定端口。
powershell -NoProfile -ExecutionPolicy Bypass -File .\deployment\Deploy-Server.ps1 -Action install -PublicHost $cfg.host -WebPort $cfg.webPort -GamePort $cfg.gamePort -DatabasePort $cfg.dbPort -ResourceVersion $cfg.resourceVersion
```

使用 `.onepiece-server.json` 是因为它包含数据库端口；`client-config/deployment.json` 只面向客户端配置，不能据此假设存在 `dbPort`。

### Java逻辑

部署脚本复制现有 `DZM.jar`，不编译 Java。修改 `.java` 后必须先生成可用JAR，再更新运行目录。现有 `server/build.xml` 的 `jars` 目标生成 `build/lib/DZM.jar`，但只打包 class，未配置运行入口，也未完整合入依赖和Hibernate资源；不能把它直接当现有自包含JAR的替代品。

一个保留旧自包含结构的维护方案是：用仓库的JDK8和Ant编译业务class，在**备份副本**上用 `jar uf` 合入 `build/classes`，同时合入变更的 `.hbm.xml` 等资源，保留原 `META-INF/MANIFEST.MF`、依赖和 `Main-Class`。确认新的 `DZM.jar` 可用后再替换源码根JAR，重新部署。本方案是后续构建准备说明，本次未执行，不能当作已验证的一键服务端构建入口。

若新增数据库字段，还需提供数据库迁移SQL、实体、Hibernate映射；若新增协议，还需同步两端Protobuf定义与生成文件。只改Java类不够。

## 7. 后续验证要点

新增发放功能至少验证：正确目标、在线/离线领取、余额与VIP语义、道具数量、重复请求、重登及重启后保存。修改模板要分别检查新实例与旧实例，记录旧实例是否需要迁移。资源相关更新继续看 [客户端资源](02_CLIENT_RESOURCES.md)。

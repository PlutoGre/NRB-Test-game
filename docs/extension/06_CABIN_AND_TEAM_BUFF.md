# 06 船舱是否存在，以及阵容与修行的扩展边界

[返回扩展指南](README.md)。这里的“船舱”按可安置非上阵船员、提供助缘/属性或独立舱位升级的功能理解。

## 1. 当前结论

**在当前主客户端、服务端业务、配置、数据库映射和资源资料中，未找到可以确认完整实现的独立船舱系统。** 现有主页的相关功能是“阵容”和“修行”，不能把它们直接称为船舱。

本次检索了 `船舱/船仓/船倉/Cabin/cabin` 等名称，检查卡牌关系、阵容、缘分、修行、页面入口及实体映射；没有找到独立的舱位持久化、安置/移除协议和助缘计算链路。这是基于当前主工程的静态结论，不代表其他版本或原商业游戏没有该功能。

如果用户所说的“船舱”只是“放不在阵容中的卡牌”，当前卡牌背包已保存未上阵船员；它们不会因此自动获得独立舱位、成长或参与人物缘分判定。

## 2. 已有功能：阵容

| 位置 | 作用 |
|---|---|
| `TeamPage.cpp`、`BattleArrayInfo.cpp` | 客户端船员、装备、技能、布阵界面与阵容缓存 |
| `ChangeDiscipleHandler`、`ChangeBattlePositionHandler` | 换船员和位置 |
| `ChangeEquipHandler`、`ChangeSkillHandler` | 穿戴装备和奥义 |
| `Player.battleids`、`Battle`、`BattleArrayObject`、`dzm.battle` | 玩家上阵顺序、位置、实例引用及战斗快照 |
| `BattleUtil.resetBattleProperty()` | 合并船员基础、装备、技能、缘分、修行属性 |

人物缘分在 [BattleUtil.java](../../server/source/src/com/oldmountain/dzm/util/BattleUtil.java) 中与 `battleDiscipleItems` 做匹配，判断的是参与上阵的人物模板列表。没有看到把“船舱/未上阵所有卡牌”合入这个列表的现成规则。

## 3. 已有功能：修行（TeamBuff）

入口在 [MainPage.cpp](../../client/source/Game/Classes/MainPage.cpp) 的 `onTeamBufferPage`，角色等级不足5时提示，达到条件切入 `TeamBuffPage`。

| 层 | 文件 |
|---|---|
| 客户端页面 | `client/source/Game/Classes/TeamBuffPage.cpp`、资源 `TeamBuff*.ccbi` |
| 客户端表 | `TeamBuff.txt`、`TeamBuffBoldness.txt` |
| 服务端模板 | [teamBuff.xml](../../server/config/xml/teamBuff.xml)、`teamBuffBoldness.xml` |
| 协议 | `TeamBuffInfo`、`UpgradeTeamBuffInfo` |
| 服务端处理 | [TeamBuffInfoHandler.java](../../server/source/src/com/oldmountain/dzm/packethandler/TeamBuffInfoHandler.java) |
| 持久化 | `TeamBuffer.java`、[TeamBuffer.hbm.xml](../../server/source/src/com/oldmountain/dzm/entity/TeamBuffer.hbm.xml) → `dzm.teambuffer` |
| 战斗加成 | `BattleUtil.resetBattleProperty()` |

现有四项修行通过消耗船员或影子增加豪气/经验，再提升全队属性等级。它不是把船员留在一个舱位持续生效；消耗素材的语义应与“安置船员”明确区分。

### 已发现的客户端与服务端不一致

| 修行ID | 属性 | 客户端TeamBuff.txt解锁等级 | 服务端模板与Handler等级 |
|---:|---|---:|---:|
| 1 | 生命 | 5 | 5 |
| 2 | 攻击 | 10 | 20 |
| 3 | 防御 | 15 | 15 |
| 4 | 意志 | 20 | 10 |

攻击和意志的解锁顺序相反。扩展或开放高等级测试前应统一两端与UI提示；只看客户端表会误判服务端可用等级。

另有两点需要在修改修行规则时一并核对：

- `getAddExpByQuality()`、`getLevelByExp()` 含硬编码换算，不能只改 `TeamBuffBoldness.txt` 当作服务端同时改变。
- `BattleUtil` 中的修行属性加成含固定系数；修改模板 `increaseLevel` 不等于战斗逻辑一定跟着变化。服务端返回时间还存在毫秒强转int的旧写法，应在实际修行回归时检查时间相关表现。

以上为静态发现，本次没有改动或通过实际高等级游玩验证这些修行分支。

## 4. 如果要新增船舱，需要做哪些层

下面是扩展设计建议，**不是仓库已有实现**。

### 明确规则

先确定：几个舱位、解锁等级、船员能否同时上阵和入舱、能否重复模板、换人是否有冷却、入舱是否参与人物缘分、提供固定属性还是百分比、是否消耗卡牌。不同规则会影响存档和战斗重算方式。

### 数据模型与迁移

可新增 `cabin_slot` 表，至少保存 `playerid/slot_index/disciple_instance_id`；需要舱位等级则另存等级/经验。唯一约束应防止同玩家槽位重复及同实例重复安置。引用船员**实例ID**，人物缘分计算时再转换为模板ID。

相应增加Java实体、Hibernate映射、数据库迁移、登录加载及 `Player` 内存集合，并接入脏数据/落库逻辑。现有账号默认空舱位，迁移应可重复执行。不要把船员ID随意拼入现有 `battleids`，否则会影响上阵位置和战斗人数。

### 服务端协议与业务

增加查询、安置、移除、升级的Protobuf请求/响应和明确操作码；在 `PacketHandler` 注册。服务端校验归属、实例存在、舱位解锁、重复安置和与上阵规则的冲突。材料被传承/修行消耗、卡牌移除时，也要检查是否仍在船舱中。

船舱若提供助缘：把“战斗人数列表”和“缘分参与模板列表”分开构建，再由 `BattleUtil` 使用后者；不要让入舱卡参与实际战斗排位。安置/移除、船员替换等操作后重算所有受影响战斗属性并保存、推送。

### 客户端页面与资源

增加入口、CCBI/Lua或C++页面、舱位锁定/空/占用状态、船员选择、规则说明和属性预览；增加协议解析及缓存，完成加载依赖和资源版本更新。如果新增C++接口暴露给Lua，还要更新绑定。

### 后续验证范围

验证旧账号迁移、解锁、安置/移除、上阵互斥、重复卡、素材消耗、缘分激活与解除、战斗结果、断线重登和重启保存。单纯复制“修行”页面并改名，无法获得这套行为。

## 5. 建议的扩展顺序

先统一修行解锁规则、把现有阵容/缘分回归清楚，再做空舱位存取和无属性的安置流程，最后接属性与助缘。每一步单独记录协议、数据库迁移和实际验证结果，避免与已实现的阵容、修行混淆。

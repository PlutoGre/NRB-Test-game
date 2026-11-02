# 03 卡牌数量、新增卡牌与缘分

[返回扩展指南](README.md)。[完整卡牌清单](data/cards.csv) 可按ID、名称、品质、显示标记和缘分ID筛选。

## 1. 现在有多少张卡牌

按不同模板ID统计，服务端 `disciple.xml` 和客户端 `Disciples.txt` **均为2324条**，ID集合相同，范围1～23168且不连续。发布表解码后与明文资料一致。

| 品质编码 | 品质 | 全部模板 | “是否显示=1”的模板 |
|---|---|---:|---:|
| 1 | A | 64 | 40 |
| 2 | B | 251 | 68 |
| 3 | C | 549 | 60 |
| 4 | D | 1460 | 31 |
| 合计 | | 2324 | 199 |

2324条中含同名、变体、隐藏显示等记录；不同名称共379个。`showEnable` 显示标记不等于卡池许可，也不等于账号已经拥有。不能把2324解读成2324名互不相同且全部可抽取的角色。当前可获得范围还受招募池、掉落、影子合成和活动配置控制。

## 2. 模板与玩家实例

| 层次 | 文件/数据 | 作用 |
|---|---|---|
| 服务端模板 | [disciple.xml](../../server/config/xml/disciple.xml) | 品质、阵营、默认奥义、初始属性、成长、经验基数、潜力、六个缘分 |
| 客户端模板 | [Disciples.txt](../../archive/client-resources/Disciples.txt) | 相同ID下的名称、描述、显示数值、缘分引用、图片音效路径 |
| 发布模板 | `resources/client/Disciples.txt` | 实际进入安装包/HTTP资源的版本 |
| 玩家实例 | `dzm.disciple` | 实例ID、模板 `itemid`、等级经验、当前属性、天生奥义实例、上阵关联 |
| 上阵快照 | `dzm.battle` 及内存Battle对象 | 合并装备、技能、修行、缘分后的战斗属性和激活状态 |

[XMLTemplateService.loadDiscipleTemplates()](../../server/source/src/com/oldmountain/dzm/util/XMLTemplateService.java) 读XML；[GamePrecedure.cpp](../../client/source/Game/Classes/GamePrecedure.cpp) 初始化 `DiscipleTableManager`，字段顺序定义在 [DataTableManager.h](../../client/source/Game/Classes/DataTableManager.h)。

实例创建由 [DiscipleUtil.newDisciple()](../../server/source/src/com/oldmountain/dzm/util/DiscipleUtil.java) 完成：从模板计算初始/等级成长属性，通过 `SkillUtil.newSkill()` 创建天生奥义，再 `Player.addOneDisciple()` 保存到集合/数据库。后续战斗、升级、传承、培养、突破及上阵分别有业务处理器，不能只插一条表记录替代完整创建。

更改模板的初始攻击或成长后，旧实例的已存属性不会凭空变成新数值。要明确是“只作用于新卡”还是需要迁移旧卡，并重算相关阵容。

## 3. 缘分由哪边控制

**服务端判定激活并计算加成，客户端提供显示资料，两端必须同步。**

服务端链路：

```text
disciple.xml 的 fate1～fate6
  → XMLTemplateService 的卡牌模板
  → ChangeDiscipleHandler 创建/更新上阵缘分列表
  → BattleUtil.resetBattleProperty 检查触发条件、计算加成
  → UserBattleArrayHandler 返回缘分ID和激活status
  → 客户端 PackageLogic / ServerDateManager / Disciple 等显示
```

核心文件：[fate.xml](../../server/config/xml/fate.xml)、[BattleUtil.java](../../server/source/src/com/oldmountain/dzm/util/BattleUtil.java)、[ChangeDiscipleHandler.java](../../server/source/src/com/oldmountain/dzm/packethandler/ChangeDiscipleHandler.java)、[UserBattleArrayHandler.java](../../server/source/src/com/oldmountain/dzm/packethandler/UserBattleArrayHandler.java)。客户端文字见 [Yuanfens.txt](../../archive/client-resources/Yuanfens.txt)，读取与展示见 `YuanfenTableManager`、`Disciple.cpp`。

现有缘分688条：人物592、奥义18、装备78。

| 字段 | 含义 |
|---|---|
| `Fate.id` / 客户端ID | 两端同一个缘分ID |
| `type=1` | 人物组合；`require` 用 `_` 分隔，要求这些模板ID都在上阵成员列表里 |
| `type=2` | 奥义；检查该人物的可装备奥义引用是否匹配，现有逻辑取第一个条件 |
| `type=3` | 装备；检查该人物穿戴的三类装备，现有逻辑取第一个条件 |
| `upgradeType` | 1生命、2攻击、3防御、4意志（旧代码 `rp`） |
| 服务端 `quantity` | 加成比例，例如 `0.25` 为25% |
| 客户端属性提升额度 | 展示整数百分比，例如25；与服务端小数单位不同 |

服务端按卡牌基础属性乘比例加到战斗属性上；不是直接在客户端显示值上乘，也不是只改文案就产生效果。人物缘分只要求同队不自动代表双向关系：如果希望两张卡都得到加成，要分别给两张卡配置缘分。

当前激活逻辑检查的是上阵列表，不能直接用“背包拥有该人物”实现同样效果；船舱助缘也没有自动纳入，见 [船舱专题](06_CABIN_AND_TEAM_BUFF.md)。

## 4. 新增一张卡牌的最小步骤

建议先复制一张现有、能够正常上阵的模板，复用已有技能和图片把流程跑通，再换新美术和数值。下面的 `30001`、缘分 `689` 是基于当前快照未占用的示例，实际新增前重新查重。

### 4.1 选ID并补服务端模板

在 `disciple.xml` 的 `Disciples` 节点新增 `Disciple`，至少完整填写：

```text
id、quality、group、defaultSkill
defaultHealth、defaultAttack、defaultDefence、defaultRp
defaultPotentiality、defaultExp
growHealth、growAttack、growDefence、growRp
fate1～fate6
```

`defaultSkill` 必须是现有 `skill.xml` 中有效ID；如果新增天生奥义，按 [奥义扩展](05_SKILLS.md) 同步配置。卡牌名称与图片不在该XML中。

复制有效卡牌时保留原有缘分结构，或用至少一个已定义的有效缘分替换，不要随意造出空缘分列表：当前服务端多处假定列表含可解析ID。非零缘分从 `fate1` 起连续填写，剩余填0，避免 `fate1=0`、后续槽非零造成拼接字符串以逗号开头。

### 4.2 添加/复用缘分

示例：新卡与白胡子（模板2）同时上阵，攻击提高20%。

```xml
<Fate id="689" type="1" require="2" upgradeType="2" quantity="0.20"/>
```

给新卡 `fate1="689"`，同步 `Yuanfens.txt` 的同ID记录，字段如下：

| ID | 名称 | 触发类别 | 触发条件 | 加成类型 | 展示额度 | 描述 |
|---:|---|---:|---:|---:|---:|---|
| 689 | 新的同盟 | 1 | 2 | 2 | 20 | 与白胡子齐上阵,攻击提升20%。 |

文件实际需要按原格式以TAB分隔，不是Markdown表。再同步发布目录的 `Yuanfens.txt`。如果还希望白胡子得到此加成，需要另外调整白胡子的缘分槽和客户端对应记录，并考虑已有玩家的上阵快照迁移。

### 4.3 添加客户端模板和资源

在明文 `Disciples.txt` 复制记录改为新ID，按原24列格式填写；特别核对：

- 前11列：ID、名称、品质、阵营、描述、默认奥义、初始生命/攻/防/意志、经验基数。
- 第12～17列：六个缘分ID；第18列“是否显示”。
- 第19～24列：头像、海报、名字图、背景、战斗图、音效。

新增美术建议使用新的文件名，例如 `members/head_char_custom30001.png`、`person/poster_char_custom30001.png`，避免覆盖其他卡。涉及合图或位图文字时还需更新帧索引及依赖。完整发布方式见 [客户端资源](02_CLIENT_RESOURCES.md)。

修改明文表后必须同步到 `resources/client`。两端模板ID相同是必要条件；只在服务器增加卡牌会使客户端找不到资料或图片。

### 4.4 添加获取途径

有模板不会自动进入商店卡池。可选路径：

| 获取方式 | 对应入口 |
|---|---|
| 测试发卡 | `DiscipleUtil.newDisciple()`；已有GM `/additem hero 30001 1` 仅在GM条件满足时使用 |
| 商店招募 | `MarketRecruitDiscipleHandler`、`kindSelecter.xml`、`discipleSelecter.xml`；结合品质池和 `rate1～rate6` 等权重 |
| 影子招募 | `soulSelecter.xml`、影子发放、`RecruitDiscipleHandler` 及品质所需数量 |
| 关卡/宝箱/活动 | 相应奖励模板和业务分支，确认该分支确实支持完整卡牌或影子 |

权重是选择器输入，不能把 `rate=10` 直接解释成10%而忽略同池其他权重、保底和重复卡转影子逻辑。普通补偿传书当前没有完整卡牌领取分支，不能仅加入附件类别就当作发卡成功。

### 4.5 发布、存量兼容与验证

服务端XML更新运行目录并重载/重启，客户端同步发布源并提升资源版本、重新打包。已有实例和 `battle.fates/fatesstatus` 可能保存旧数据：更改旧卡缘分时需重建受影响上阵配置，不能假设XML热加载会迁移它们。

后续至少验证：创建、背包和图鉴显示、上阵/下阵、天生奥义、升级、装备、缘分未激活/激活/解除三个状态、战斗和重登保存。首轮只新增模板和图片、复用已有算法时通常不需要改数据库结构；新增第七个缘分槽、全新技能类型或船舱助缘则需要改代码与协议。

## 5. 文档和清单维护

更新 [cards.csv](data/cards.csv) 的来源为服务端 `disciple.xml` 与客户端 `Disciples.txt` 按模板ID关联；保留实例ID与模板ID区别。每次扩展记下新ID、关联技能/缘分、资源路径、卡池改动和资源版本，不要只更新“卡牌总数”。

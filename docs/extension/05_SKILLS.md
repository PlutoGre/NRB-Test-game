# 05 奥义数量、学习升级与新增奥义

[返回扩展指南](README.md)。[完整奥义清单](data/skills.csv) 包含名称、类型、品质、触发率、基础值、成长和战斗表现资源。

## 1. 数量与分类

服务端 [skill.xml](../../server/config/xml/skill.xml) 与客户端 [Skills.txt](../../archive/client-resources/Skills.txt) 均有 **77个不同模板ID，连续1～77**；两端ID集合一致，发布表解码后与明文资料一致。

| type | 含义 | 数量 |
|---:|---|---:|
| 1 | 单体攻击 | 55 |
| 2 | 群体攻击 | 13 |
| 4 | 攻击属性被动 | 3 |
| 5 | 防御属性被动 | 3 |
| 6 | 意志属性被动 | 3 |
| 合计 | | 77 |

品质分布：A=24、B=29、C=20、D=4，品质编码仍为1～4。代码还存在其他类型分支，例如服务端的生命被动type3，但当前77条配置没有type3；这不等于所有页面和表现已支持任意新增类型。

`skillBookConfig.xml` 有322条碎片配置，关联73种不同奥义。碎片ID和完整奥义ID是不同层次，不能把322计为奥义种类，也不能假设77种都有完整碎片获取途径。

## 2. 服务端数据与客户端表现

| 文件/模块 | 职责 |
|---|---|
| `xml/skill.xml` | `id/quality/type/rate/quantity/growth`：品质、类别、触发率、基础强度和成长 |
| `xml/skillSelecter.xml`、`skillbookSelecter.xml` | 获取/随机选择配置，具体入口还需看业务使用 |
| `xml/skillBookConfig.xml`、`skillBookNpcConfig.xml`、`skillBookSnatchConfig.xml` | 碎片关联、NPC及争夺配置 |
| `Skills.txt` | 名称、说明、显示数值、图标、动画时长、CCB名、时间轴、替换帧和音效 |
| `SkillBooks.txt` | 客户端碎片资料 |
| `skillposter/`、`skilltex`帧、特效图和CCBI、音频 | 奥义卡面及战斗表现 |
| `dzm.skill` | 玩家奥义实例、模板ID、等级、数量值、消耗/升级进度、天生标记 |
| `dzm.skillbook` | 玩家残章状态，对应 `SkillBook.hbm.xml` |
| Battle中defaultskill/skill1/skill2相关字段 | 上阵技能实例及计算用快照 |

客户端技能类型枚举在 [DataTableManager.h](../../client/source/Game/Classes/DataTableManager.h) 的 `SkillItem`；表初始化在 `GamePrecedure.cpp`。服务端 [SkillUtil.newSkill()](../../server/source/src/com/oldmountain/dzm/util/SkillUtil.java) 用模板创建实例，`isDefault` 标记天生技能。

天生奥义由卡牌模板 `defaultSkill` 指定，创建船员时同时创建。额外奥义通过 [ChangeSkillHandler.java](../../server/source/src/com/oldmountain/dzm/packethandler/ChangeSkillHandler.java) 装配；卡牌天生技能和可装配奥义不是无限槽位的同一种绑定。

## 3. 升级和战斗计算

[UpgradeSkillHandler.java](../../server/source/src/com/oldmountain/dzm/packethandler/UpgradeSkillHandler.java) 读取待升级技能及消耗技能实例，检查贝里、实例存在和上阵限制，移除材料，增加升级进度，在升级时按模板 `growth` 增加 `quantity`，更新对应Battle快照并返回结果。

服务端战斗逻辑主要在 [Fight.java](../../server/source/src/com/oldmountain/dzm/util/Fight.java)，被动对阵容属性的加成还在 [BattleUtil.resetBattleProperty()](../../server/source/src/com/oldmountain/dzm/util/BattleUtil.java)。客户端 [FightTasksSkill.cpp](../../client/source/Game/Classes/FightTasksSkill.cpp) 等负责技能动画和表现；替换动画不会自动修改服务端伤害。

被动比例沿用小数单位，例如ID8“酒樽舞曲” `quantity=0.093`、`growth=0.031`。攻击技能的强度参数不能直接等同于“最终造成多少点伤害”，还要结合战斗算法、攻防、目标与触发条件。旧代码的 `rp` 在界面中对应意志。

修改已有模板时，旧实例的 `quantity`、等级及Battle快照可能仍保存旧值。需要明确旧实例迁移规则，不能只改模板并期待所有已有技能立即变化。

## 4. 新增一种奥义

以当前未占用的ID `78` 为示例，优先复制已有同类型技能：

1. 在 `skill.xml` 新增完整 `Skill`，填写品质、类型、概率、基础值、成长。不要新增一个没有服务端计算和客户端枚举支持的type。
2. 在 `Skills.txt` 添加相同ID；第3列是品质，第4列是类型，后续保留原15列结构。
3. 首轮复用已有 `ccbFile` 和 `timeLine`；确认能释放后再替换美术。新的替换资源变量名必须存在于该CCBI，音效和持续时间要与动画匹配。
4. 加入发布目录的表、图片、音频、CCBI和依赖。仅在明文资料中新增记录不会进入安装包。
5. 添加获取途径：作为新卡 `defaultSkill`、加入掉落/选择器、奖励发放或碎片合成。
6. 如果新增碎片路线，补服务端 `skillBookConfig` 等关联与客户端 `SkillBooks.txt`、图标/页面使用资料。不能照抄一个碎片ID就认为它会对应新奥义。
7. 如果作为缘分触发条件，新增 `fate.xml type=2 require=78`，同步 `Yuanfens.txt` 并让卡牌引用缘分ID。
8. 更新服务端模板、客户端资源版本和安装包，按下面的检查范围验证。

完整奥义补偿附件可用 `t=41000,i=78`，但当前领取代码每条创建1个，不按q批量循环。已有GM中的 `skillbook` 分支涉及完整奥义创建，命名容易误导，使用前按源码确认；普通APK的GM条件见 [服务端文档](01_SERVER_DATA_AND_REWARDS.md)。

## 5. 后续验证与扩展边界

最小验证：获得、列表/详情显示、天生与额外槽位区别、装备/卸下、消耗材料升级、余额与材料正确扣除、缘分激活、单体/群体/被动实际效果、动画音效和重登保存。

若增加控制、治疗、持续伤害等当前未完整实现的新机制，需同时设计服务端算法和战斗协议、客户端回放/动画与数据结构，不能只加一条XML和技能描述。新增已有类型的模板通常不需要新数据库字段；新增状态、槽位或效果字段则可能需要协议、映射和数据库迁移。

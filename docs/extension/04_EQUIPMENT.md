# 04 装备数量、穿戴强化与新增装备

[返回扩展指南](README.md)。[完整装备清单](data/equipment.csv) 包含91种装备的ID、名称、类型、品质、基础属性、成长和图片路径。

## 1. 数量和分类

服务端 [equip.xml](../../server/config/xml/equip.xml) 与客户端 [Equips.txt](../../archive/client-resources/Equips.txt) 均有 **91个不同模板ID，连续1～91**。两端ID集合一致，发布表解码后与明文资料一致。

| type | 类别 | A品质 | B品质 | C品质 | D品质 | 总计 |
|---:|---|---:|---:|---:|---:|---:|
| 1 | 武器，主要增加攻击 | 5 | 12 | 12 | 8 | 37 |
| 2 | 防具，主要增加防御 | 5 | 8 | 9 | 6 | 28 |
| 3 | 饰品，主要增加生命 | 5 | 8 | 7 | 6 | 26 |
| 合计 | | 15 | 28 | 28 | 20 | 91 |

品质编码1/2/3/4分别对应A/B/C/D。91指装备种类，不含宝石、装备碎片等其他道具，也不计强化等级或重复实例。

## 2. 文件与数据链路

| 位置 | 作用 |
|---|---|
| `xml/equip.xml` | `id/quality/type/quantity/growth` 数值模板 |
| `xml/equip_level_base.xml` | 强化消耗的等级基数 |
| `xml/equipSelecter.xml`、关卡/活动/宝箱配置 | 获取途径 |
| `xml/Diamond*.xml` | 宝石、镶嵌相关配置，区别于货币钻石 |
| `Equips.txt`、`EquipLevelParam.txt` | 客户端名称、说明、图片、显示及等级参数 |
| `equipposter/`、合图帧 | 装备美术资源 |
| `equipmentFragment.txt`、相关页面 | 碎片/锻造资料，不计入91个完整装备模板 |
| `dzm.equip`、`dzm.battle` | 玩家装备实例及穿戴引用/战斗属性 |

实例创建：[EquipUtil.newEquip()](../../server/source/src/com/oldmountain/dzm/util/EquipUtil.java) / [EquipServices.java](../../server/source/src/com/oldmountain/dzm/services/EquipServices.java)。创建时按类型把 `quantity` 写入 attack/defence/health，再加入玩家装备集合与保存逻辑。

穿戴：[ChangeEquipHandler.java](../../server/source/src/com/oldmountain/dzm/packethandler/ChangeEquipHandler.java) 校验实例与位置，更新Battle引用，调用阵容属性重算。客户端入口为 `TeamPage`、`EquipmentInfo`、装备选择页。

强化：[UpgradeEquipHandler.java](../../server/source/src/com/oldmountain/dzm/packethandler/UpgradeEquipHandler.java) 检查条件和贝里、计算强化结果、写等级/属性、重算阵容并响应；客户端 `EquipEnhancePage.cpp` 负责交互和结果显示。

精炼与宝石：`RefineEquipHandler`、`EquipDiamondHandler`、`Equip.calcEquipAttr()`。基础成长可理解为 `quantity + (level-1)*growth`，最终属性还应看精炼、宝石等计算，不能只用这一式解释所有实例。

## 3. 修改现有装备

只改名称和说明：修改明文 `Equips.txt`，同步发布源、提高资源版本并重新发布。替换图片按 [资源文档](02_CLIENT_RESOURCES.md) 操作。

改属性和成长：服务端 `equip.xml` 和客户端表同步。已有实例存有自己的属性，不保证改XML后立即重算；强化路径会重新读取部分模板参数，但不能把它当成批量迁移工具。若希望所有旧装备一起调整，应制定实例与已穿戴阵容的迁移方案。

改穿戴类型：不仅改显示分类，还影响实例属性字段、位置校验、缘分与战斗加成。对已拥有/穿戴的装备直接把武器改饰品，会使历史数据不一致，应新增模板或提供迁移。

## 4. 新增装备的步骤

以当前未占用的模板ID `92` 为示例：

1. 在 `equip.xml` 复制同类型同品质记录，新增ID92，设置 `type/quality/quantity/growth`。
2. 在 `Equips.txt` 新增相同ID、名称、类型、品质、描述、初始值、成长、图标/海报/名字图。注意该表第3列是**类型**，第4列才是品质。
3. 添加 `equipposter` 图片或合图帧，必要时更新依赖；同步 `resources/client` 的表和图片。
4. 为关卡、选择器、宝箱、商店或管理发放逻辑添加获取途径。需要人物装备缘分时，添加 `fate.xml type=3 require=92` 和对应客户端 `Yuanfens.txt`，再引用到卡牌缘分槽。
5. 更新运行端XML、资源版本及安装包。单纯增加现有类型的装备模板一般不需要改数据库结构。
6. 后续验证：获得实例、装备列表、穿戴/替换/卸下、强化扣费和属性、缘分触发、战斗与重登。

测试发放源码格式为 `/additem equip 92 1`，但普通APK不直接具备GM使用条件，见 [服务端发放说明](01_SERVER_DATA_AND_REWARDS.md#51-gm-直接修改)。补偿传书可发装备，当前每条附件只创建一个实例，不按 `q` 批量循环。

## 5. 扩展边界

新增第4类装备、额外穿戴槽或新的装备属性，需要同时修改服务端Equip/Battle实体与映射、穿戴校验/计算、协议、客户端数据结构、表解析和布局。现有91条记录不能证明这些新类型已受支持。

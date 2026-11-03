# 首批移植内容 Android 实测记录（2026-11-03）

已实际编译服务端补丁与 Android 包，在当前电脑的雷电 14 上覆盖安装 x86 包，使用 `NRBQA02` 角色完成领取、卡面展示、上阵、升级、装备强化、被动属性、奥义参悟和普通远航验证。数据库与游戏服重启后，测试进度仍然保留。

## 环境和安装包

- Windows，本地便携 JDK 8、SDK Tools 25.2.5、API 21、Build Tools 23.0.3、NDK r8e、Ant。
- 雷电 14，Android 14，ADB 设备 `emulator-5554`；使用原生 x86 游戏库，不走 ARM 转译。
- 包名 `com.youai.dreamonepiece.platform.anzhi`，覆盖安装成功，没有卸载或清空应用数据；原 APK 和外部配置已备份。
- 区服“男人帮1区”，资源版本 **1.78.8**。
- 服务端测试前备份：`runtime/server/backups/onepiece-20261103-100420-349158.sql.gz`。

| 产物 | 字节数 | SHA-256 |
|---|---:|---|
| `outputs/android/DreamOnePiece-debug.apk` | 55,223,530 | `f933ff6b970995638b35e868a592035f420d2bac5c7e29346e87f710063c8647` |
| `outputs/android/DreamOnePiece-emulator-x86-debug.apk` | 46,738,702 | `aed4aed62630ffe89ed76083dcfed866307959c0e77f3a4b262c48ff82386695` |

通用包的游戏库包含 armeabi、armeabi-v7a、x86。模拟器包只包含 `lib/x86/libDreamOnePiece.so`。**本次实际安装和游玩的是 x86 包；通用包未在手机真机验证。** 本轮没有重新构建或实测 iOS、Web 客户端。

## 实际验证结果

| 项目 | 操作和结果 |
|---|---|
| 服务端启动、物品目录 | Java 补丁编译成功；游戏服和管理接口正常启动，管理目录 5,361 项，新增卡牌、影子、装备和奥义均有正确名称 |
| 发放与领取 | 通过现有管理接口只给 `NRBQA02` 发测试物品，在游戏传书界面实际点击领取；重启后数据库确认新增卡牌44张、装备29件、9种被动奥义齐全 |
| 新卡展示 | 打开乌塔卡面，名称、头像、海报、简介、六条缘分和天生奥义正常显示；大和上阵后卡面及战斗图正常。[乌塔截图](images/ported-android/uta-card.png) |
| 上阵与人物缘分 | 乌塔、大和依次进入第3、第4阵位。“四皇之女”激活，大和1级基础攻击150，经33%人物缘分提升为200 |
| 卡牌成长 | 背包实际使用一瓶葡萄酒，乌塔249、大和250均由1级升到9级，随后远航再获得33经验；数据库经验2033，基础生命294、攻击210.8、防御114.4、意志414.6、潜力272，与成长表一致 |
| 装备三类型 | 给大和穿戴雷鸣龙牙棒107、海神羽织94、海洋之心96。武器缘分激活，三类属性正常计入阵容；没有开启套装等新增机制 |
| 装备强化 | 在游戏强化界面消耗720贝里，雷鸣龙牙棒触发原有多级强化，从1级升到4级，攻击220→268；重启后仍为4级。[强化截图](images/ported-android/weapon-upgraded.png) |
| 防御被动 | 大和1级、穿戴海神羽织后，防御200；装上降无赖线89后变207，对应基础防御80的9%加成 |
| 攻击被动 | 同一1级阵容，装上霸王色缠绕88，攻击521→536，对应基础攻击150的10%加成 |
| 意志被动 | 将上一格替换为草帽约定86，攻击恢复521，意志333→376，对应13%意志加成；防御被动仍生效。[被动截图](images/ported-android/passive-buffs.png) |
| 奥义参悟 | 选择本次发放的闲置奥义作为材料，游戏界面显示60%成功率；实际参悟成功，降无赖线1→2级，界面9%→12%，数据库精确加成0.124。消耗的测试材料随后通过传书补回。[参悟截图](images/ported-android/skill-upgraded.png) |
| 普通远航 | 带乌塔、大和完成“陌生海岛”，正常播放战斗并三星获胜，获得84威望、216贝里，4名上阵成员各获得33经验；没有调用接口伪造战斗或跳关。[结算截图](images/ported-android/voyage-result.png) |
| 保存与重启 | 核对真实数据库值后，停止并重新启动数据库、资源服务和游戏服；再连续3次冷启动客户端并实际点击登录，卡牌等级、经验、装备4级、奥义2级及穿戴关系保留。[重启后截图](images/ported-android/persisted-team.png) |
| 崩溃检查 | 完成本轮操作和3次冷启动后，ADB crash buffer为空。其他两个角色的等级、经验与测试前一致，未使用全服发放 |

最终大和的阵容属性为生命634、攻击623、防御248、意志469；配置为9级大和、4级雷鸣龙牙棒、1级海神羽织及海洋之心、2级降无赖线和1级草帽约定。数据库和游戏服重启后的界面数值一致。

全部44/29/9条内容做了发奖、领取及入库检查，**没有逐一操作每张卡牌的每个技能、每件装备的强化、每条缘分或全部关卡**。原移植文档列出的缺资料卡牌和未满足的缘分条件仍按原范围保留，见 [首批移植说明](../extension/08_PORTED_BASIC_CONTENT.md)。

## 本次实测发现并修复的问题

### 源缘分错列导致服务端启动失败

APK `Yuanfens.txt` 的源ID1026“化敌为友”在触发条件后多了一个空列。初次移植得到本地ID937，属性类型为空、额度错误地变成3%，导致 Commons Configuration 读取 `upgradeType` 时抛出整数转换异常，游戏服退出。

导入器现在按已知源记录移除该空列，正确得到 **防御加成26%** 和完整描述；所有导入缘分必须通过数值字段检查。运行目录的更新器仅修复与这份已知错误记录完全相同的条目，手工设置的其他值仍报冲突，不擅自覆盖。资源版本升到1.78.8，避免客户端复用错误表缓存。

### 旧原生依赖缓存仍引用目录整理前的路径

本机 NDK 缓存引用 `W:/src/client/...`，当前目录已经是 `client/source/...`，编译报 `No rule to make target`。

Android 构建脚本新增构建路径记录；缓存没有对应记录或工程、盘符映射、SDK、NDK路径变化时，将旧 `obj` 目录移到 `runtime/android/native-cache-backups/`，再重新编译。缓存不会直接删除。本次已经过备份和三个架构的完整编译。

## 检查和证据

已执行：

```powershell
python -m unittest discover -s scripts/tests -p test_apk_catalog.py -v
```

6项检查覆盖已知错列修复、保护同ID自定义配置、两端数值一致、卡牌缘分引用、字库覆盖，以及图片和音效的内容哈希与文件名大小写。另编译运行了12项 Java 缘分条件用例，覆盖旧单ID、下划线必需组、井号可选形态、组合条件及无效输入，全部通过。

本机原始证据在忽略目录：

- `outputs/android/ported-v330-build.log`：最终 Android 构建日志。
- `outputs/android-qa/`：截图、UI树、实际点击记录、发奖回执、数据库前后快照、冷启动结果、回归日志及安装包摘要。
- `runtime/server/logs/deployment.log`：服务端补丁生成日志。

APK、SDK、原始账号会话和完整数据库快照不提交 Git；文档仅附测试角色的选定截图。测试结束时保留本地服务和模拟器；原本运行的网页网关及外网进程已恢复，沿用原 Web 构建输出。Quick Tunnel 重启后网址会改变，当前地址以 `runtime/server/web-external-url.txt` 为准；本轮未进行网页玩法或外网可达性验收。

## 后续复测的只读场景检查

OpenGL页面没有常规 Android 控件树。本次新增只读场景树输出，用节点位置和CCB按钮名称定位真实ADB输入，不提供发奖、战斗、跳关或直接调用按钮回调的功能。

需要时由开发者显式构建：

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File deployment/Build-Android.ps1 -SceneInspection
```

该选项只在打包期间将渠道清单设为 debuggable，完成后恢复原清单。安装并打开应用后，在应用 `files` 目录创建 `nrb-scene-inspection.enabled`，才开始每秒输出 `nrb-scene.json`。非debuggable包不注册观察器；默认构建不启用检查，删除标记即停止输出。

```powershell
adb -s emulator-5554 shell touch /sdcard/Android/data/com.youai.dreamonepiece.platform.anzhi/files/nrb-scene-inspection.enabled
python scripts/android_scene.py capture --output outputs/android-qa/current
python scripts/android_scene.py tap --action onEnter
```

滚动列表中的按钮需要先滚动至可见区域；工具也支持使用 `--within-text` 选择对应文字所在行。测试结束已移除本次创建的设备端检查标记和临时场景文件。

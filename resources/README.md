# 游戏公共资源

[`client`](client) 是 Android、iOS 和 HTTP 资源服务共同使用的发布源。三条准备流程都从这里复制资源；图片、CCBI、Lua、表和音频的相对路径保持不变。

| 内容 | 路径示例 |
|---|---|
| 人物图片 | `client/members`、`client/person` |
| 装备、奥义、道具图片 | `client/equipposter`、`client/skillposter`、`client/itemposter` |
| 页面及动画 | `client/*.ccbi`、`client/mainScene`、`client/effect` |
| 客户端表 | `client/Disciples.txt`、`client/Equips.txt`、`client/Skills.txt` |
| Lua、音频、文字 | `client/lua`、`client/sound`、`client/Lang` |

修改方法与发布步骤见 [客户端资源指南](../docs/extension/02_CLIENT_RESOURCES.md)。部分表和 Lua 使用旧封装格式，不能直接把乱码当普通文本修改。

`archive/client-resources` 保留历史明文资源供阅读和取材；实际发布以本目录为准。原资源包内夹带的旧 PHP 管理后台已移到 `archive/server/embedded-admin`，不参与资源准备。

服务端 XML 模板和 Hibernate 配置位于 `server/config`。Android 原生图标、布局和渠道覆盖资源仍位于对应 Android 工程内。

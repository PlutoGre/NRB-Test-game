【部署说明更新】
下面的旧步骤针对 Windows 本机 Game.exe，不能直接用于手机 APK。
Android、本机模拟器或局域网真机部署，请先阅读 src/tools/DEPLOYMENT.txt，使用统一工具生成配套地址和资源配置。
不要把手机的游戏服务器地址设为 127.0.0.1，也不要直接安装仍指向旧运营地址的原始 APK。

1.将 version_win32.cfg 放入客户端 Resource_Client 根目录。
2.本地创建一个Web站点，将 Web_hzwMobile.zip 的内容解压到站点根目录。
3.新建一个 dzm 数据库，导入Code_Server目录中的除 dzm_stat.sql 之外的全部sql文件。（优先导入dzm.sql）
4.修改 Code_Server\server\hibernateconfig\hibernate_lo_01.cfg.xml 的配置中的数据库连接信息。
5.将 runSever.bat 放入 Code_Server\server 启动服务端。
6.启动 Resource_Client 中的 Game.exe 进入游戏。
7.关闭服务端时 在服务端窗口 Ctrl+C 。
# Qt TCP 学习项目

基于 Qt 5.14.2、C++11 和 qmake 的客户端/服务端练习项目，目前仍在开发中。

## 工程

- `Client/Client.pro`：Qt Widgets 客户端，使用 `QTcpSocket` 连接服务端。
- `Server/Server.pro`：使用 `QTcpServer` 监听连接；当前不显示服务端窗口。
- 两端通过各自的 `connect.config` 配置地址和端口，当前为 `127.0.0.1:5000`。

用 Qt Creator 分别打开两个 `.pro` 文件，选择 Qt 5.14.2 的 MinGW 64-bit Kit 构建。先运行 Server，再运行 Client；在应用程序输出中查看连接日志。

## 当前进度

客户端已能读取资源配置并发起 TCP 连接。服务端已能监听连接、创建 `MyTcpSocket` 并保存到列表。客户端新增了 PDU 协议定义；服务端协议文件尚为空。消息收发、断连清理和协议解析仍未完成。

学习过程记录在个人 Obsidian 笔记库中，未包含在本仓库。

# Qt TCP 学习项目

基于 Qt 5.14.2、C++11 和 qmake 的客户端/服务端练习项目，目前仍在开发中。

## 开发环境

- 开发工具（IDE）：Qt Creator 4.11.1。
- Qt 库：Qt 5.14.2。
- 构建套件（Kit）：Desktop Qt 5.14.2 MinGW 64-bit。
- C++ 标准与构建工具：C++11、qmake。

Qt Creator 与 Qt 库分别有自己的版本号；开发工具中显示的 4.11.1 是 Qt Creator 的版本。

## 工程

- `Client/Client.pro`：Qt Widgets 客户端，使用 `QTcpSocket` 连接服务端。
- `Server/Server.pro`：使用 `QTcpServer` 监听连接；当前不显示服务端窗口。
- 两端通过各自的 `connect.config` 配置地址和端口，当前为 `127.0.0.1:5000`。

用 Qt Creator 分别打开两个 `.pro` 文件，选择 Qt 5.14.2 的 MinGW 64-bit Kit 构建。先运行 Server，再运行 Client；在应用程序输出中查看连接日志。

## 当前进度

更新日期：2026-11-03。

- 已完成：客户端读取资源配置、通过单例获取窗口并发起 TCP 连接；服务端监听连接、创建 `MyTcpSocket` 并保存到列表。
- 已编写：两端的 PDU 协议结构、消息类型枚举和 `mkPDU()` 动态分配函数。
- 已编写：服务端 `OperateDB` 单例和 `QMYSQL` 连接配置，`main()` 中会尝试连接 `qt_project` 数据库。
- 已搭建：客户端注册/登录界面，包含用户名、密码输入框及注册、登录按钮；按钮业务逻辑尚未接入。
- 安全配置：数据库密码从 `QT_DB_PASSWORD` 环境变量读取，不写入仓库。
- 验证情况：MySQL 驱动已在本机 Qt 5.14.2 环境验证可加载；数据库连接结果尚未验证，注册/登录流程也未实现或验证。
- 已编写：客户端组装 PDU 发送消息，服务端通过 `readyRead` 触发接收槽函数并输出调试信息；TCP 半包、粘包处理尚未完成。

下一步：配置 `QT_DB_PASSWORD` 并验证数据库连接；实现注册/登录按钮对应的信号槽和 PDU 业务处理；补充 TCP 半包、粘包和长度校验，再完善断连清理及消息展示。

学习过程记录在个人 Obsidian 笔记库中，未包含在本仓库。

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

## 数据库

项目服务端使用 Qt SQL 的 `QMYSQL` 驱动，默认数据库名为 `qt_project`。[`database/init.sql`](database/init.sql) 创建数据库及 `user_info`、`friend` 两张表，字段和外键按项目提供的结构定义；脚本不包含测试数据。

1. 在 MySQL 命令行客户端中运行 `SOURCE path/to/database/init.sql;`。脚本创建数据库和数据表，不会删除或清空已有数据。
2. 将 `Server/database.ini.example` 复制为 `database.ini`，放到运行时 `Server.exe` 所在目录（Debug 构建通常是 `build-Server-.../debug/`）。
3. 编辑本地 `database.ini`，填写 MySQL 主机、端口、用户名、密码和数据库名。该本地配置已加入 `.gitignore`，不要提交真实密码。
4. 确保运行环境安装了与 Qt 版本、编译器和架构匹配的 `QMYSQL` 插件及其 MySQL 客户端依赖库。

数据库配置项说明和后续维护约定见 [`database/README.md`](database/README.md)。

## 当前进度

更新日期：2026-11-03。

- 已完成：客户端读取资源配置、通过单例获取窗口并发起 TCP 连接；服务端监听连接、创建 `MyTcpSocket` 并保存到列表。
- 已编写：两端的 PDU 协议结构、消息类型枚举和 `mkPDU()` 动态分配函数。
- 已编写：服务端 `OperateDB` 单例和 `QMYSQL` 连接配置；数据库初始化脚本包含 `user_info`、`friend` 表结构。
- 已搭建：客户端注册/登录界面，包含用户名、密码输入框及注册、登录按钮；按钮业务逻辑尚未接入。
- 安全配置：数据库连接参数从运行目录中的本地 `database.ini` 读取；仓库只保留不含真实密码的示例配置。
- 验证情况：MySQL 驱动已在本机 Qt 5.14.2 环境验证可加载；数据库连接结果尚未验证，注册/登录流程也未实现或验证。
- 已编写：客户端组装 PDU 发送消息，服务端通过 `readyRead` 触发接收槽函数并输出调试信息；TCP 半包、粘包处理尚未完成。

学习过程记录在个人 Obsidian 笔记库中，未包含在本仓库。

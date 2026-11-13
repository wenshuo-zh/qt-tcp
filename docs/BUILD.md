# 编译与运行指南

本文介绍项目所需环境、数据库初始化、连接配置、构建和启动顺序。数据库表结构见 [`../database/init.sql`](../database/init.sql)，额外说明见 [`../database/README.md`](../database/README.md)。

## 环境依赖

仓库中的 Qt 工程使用 qmake 和 C++11。项目目前的开发环境记录如下：

| 依赖 | 版本或说明 |
| --- | --- |
| Qt | 5.14.2 |
| Qt Creator | 4.11.1（原项目开发环境记录） |
| Qt Kit | Desktop Qt 5.14.2 MinGW 64-bit |
| MinGW GCC | 7.3.0（当前构建环境记录） |
| qmake | 3.1（随 Qt 5.14.2 提供） |
| C++ 标准 | C++11 |
| MySQL Server | 仓库未固定版本；需与所安装的 `QMYSQL` 驱动兼容 |

客户端使用 Qt Widgets 和 Qt Network。服务端还使用 Qt SQL，并依赖 `QMYSQL` 插件及与 Qt、MinGW、64 位架构相匹配的 MySQL 客户端运行库。更换 Qt Kit 时，需确认驱动和客户端运行库与该 Kit 匹配。

## 初始化数据库

项目连接名为 `qt_project` 的数据库。初始化脚本创建 `user_info`、`friend` 两张表，不包含测试数据。

可以使用 MySQL Workbench 打开 [`../database/init.sql`](../database/init.sql) 并执行，也可以在 MySQL 命令行中运行。先打开命令提示符并进入克隆仓库目录，然后启动 MySQL 客户端：

```text
mysql -u root -p
```

在 MySQL 提示符中执行脚本，将路径替换为本机仓库的实际位置：

```sql
SOURCE C:/path/to/qt-tcp/database/init.sql;
```

脚本创建数据库时使用 `IF NOT EXISTS`，不会删除或清空现有数据；如果同名数据表已经存在，创建表语句会报错并保留原表。请在新数据库中首次初始化，或先确认现有表结构符合项目要求。

数据库账号需要有访问 `qt_project` 的权限。MySQL Server 的具体版本未由仓库指定。

## 构建项目

安装 Qt 5.14.2 的 MinGW 64-bit Kit，并确保安装 Qt Widgets、Qt Network 和 Qt SQL 模块。使用 Qt Creator 时，分别打开 [`../Client/Client.pro`](../Client/Client.pro) 和 [`../Server/Server.pro`](../Server/Server.pro)，为两个工程选择同一个 Qt Kit，再分别构建。

也可以在已配置好 Qt 5.14.2 MinGW 环境的 Windows 命令提示符中使用 qmake。以下命令均从仓库根目录开始，并为客户端和服务端分别创建构建目录：

```bat
mkdir build-client
cd build-client
qmake ..\Client\Client.pro
mingw32-make
```

回到仓库根目录后，单独构建服务端：

```bat
mkdir build-server
cd build-server
qmake ..\Server\Server.pro
mingw32-make
```

如果构建目录已存在，请使用新的空目录，或直接使用 Qt Creator 配置的构建目录。通过 Qt Creator 构建时，程序通常位于对应构建目录的 `debug` 或 `release` 子目录。

## 配置数据库连接

将 [`../Server/database.ini.example`](../Server/database.ini.example) 复制为 `database.ini`，填写本机 MySQL 主机、端口、用户名、密码和数据库名。服务端从 `QCoreApplication::applicationDirPath()` 获取程序目录，并从该目录读取 `database.ini`，因此配置文件必须放在**实际运行的 `Server.exe` 同一目录**。

从仓库根目录在 PowerShell 中复制示例文件，并将路径替换为实际包含 `Server.exe` 的目录：

```powershell
$serverExeDir = "C:\path\to\directory\containing\Server.exe"
Copy-Item .\Server\database.ini.example (Join-Path $serverExeDir "database.ini")
```

复制后编辑该目录下的 `database.ini`，将 `your_mysql_user` 和 `replace_me` 等示例值改为本地连接信息。仓库中的 `Server/database.ini` 和 `build-*` 构建目录已被 `.gitignore` 忽略；不要将真实密码写入示例配置或提交到其他未忽略路径。

如果运行时提示数据库配置文件不存在，先确认 `database.ini` 与实际启动的 `Server.exe` 位于同一目录，而不是只放在源代码目录或构建目录的上一级。

## 配置 TCP 地址

客户端目标地址和服务端监听地址分别保存在 [`../Client/connect.config`](../Client/connect.config) 与 [`../Server/connect.config`](../Server/connect.config)，这两个配置由 qrc 编入程序。仓库默认值均为 `127.0.0.1:5000`，适用于同一台电脑上的客户端和服务端。

如需在不同主机间连接，按实际部署修改客户端的服务端地址和服务端的监听地址，确保端口配置一致，然后重新构建对应工程。

## 启动顺序

1. 启动 MySQL Server。
2. 确认数据库初始化完成，`database.ini` 与 `Server.exe` 位于同一目录，且 `QMYSQL` 驱动可加载。
3. 启动 `Server.exe`，从 Qt Creator 的应用程序输出中查看监听和数据库连接状态。
4. 启动 `Client.exe`，从应用程序输出中查看 TCP 连接状态。

服务端和客户端的实际可执行文件位置取决于使用 Qt Creator 还是命令行构建，请根据所选构建目录运行对应文件。

# Qt Chat Room

一个基于 Qt 框架开发的客户端-服务器架构聊天室应用程序，支持实时消息传输、用户注册登录、文件传输等功能。

## 功能特性

### 客户端功能
- **用户认证**: 支持用户注册和登录功能
- **实时聊天**: 支持多用户实时消息收发
- **文件传输**: 支持文件发送和接收功能
- **界面美观**: 使用 QSS 样式表提供现代化的用户界面
- **无边框设计**: 支持无边框窗口，提供更好的视觉效果
- **快捷键支持**: 支持 Enter 换行、Ctrl+Enter 发送消息等快捷操作

### 服务器功能
- **TCP 服务器**: 基于 Qt Network 模块构建的稳定服务器
- **客户端管理**: 支持多个客户端同时连接
- **消息处理**: 高效的消息组装和分发机制
- **协议支持**: 自定义文本消息协议

## 技术栈

- **开发框架**: Qt 5/6
- **编程语言**: C++11
- **网络协议**: TCP/IP
- **数据库**: Qt SQL 模块（用于用户认证）
- **UI 设计**: Qt Widgets + QSS 样式表

## 项目结构

```
qt_chatroom/
├── client/                 # 客户端源代码
│   ├── mainwindow.cpp      # 主窗口
│   ├── loginer.cpp        # 登录界面
│   ├── regist.cpp         # 注册界面
│   ├── tcpclient.cpp      # TCP 客户端
│   ├── tcpserver.cpp      # TCP 服务器（客户端侧）
│   ├── database.cpp       # 数据库操作
│   ├── textmessage.cpp    # 消息处理
│   └── ...
├── server/                 # 服务器源代码
│   ├── main.cpp           # 服务器入口
│   ├── serverdemo.cpp     # 服务器主类
│   ├── tcpserver.cpp      # TCP 服务器
│   ├── serverhandler.cpp  # 服务器消息处理
│   └── ...
└── README.md              # 项目说明文档
```

## 编译运行

### 环境要求
- Qt 5.12 或更高版本
- 支持的操作系统：Windows、Linux、macOS

### 编译客户端

```bash
cd client
qmake mychatroom.pro
make  # 或 nmake（Windows下）
```

### 编译服务器

```bash
cd server
qmake ChatRoomServer.pro
make  # 或 nmake（Windows下）
```

### 运行程序

1. 首先启动服务器：
   ```bash
   ./ChatRoomServer
   ```
   服务器默认监听 8080 端口

2. 启动客户端：
   ```bash
   ./mychatroom
   ```

## 使用说明

1. **注册账号**：首次使用需要注册新账号
2. **登录系统**：使用注册的账号密码登录
3. **开始聊天**：
   - 在消息输入框中输入消息
   - 按 Enter 键换行
   - 按 Ctrl+Enter 键发送消息
   - 可以选择发送文件给其他用户

## 主要界面

- **登录界面**: 用户登录
- **注册界面**: 新用户注册
- **主聊天界面**: 显示消息列表、在线用户、消息输入区
- **文件传输界面**: 发送和接收文件

## 协议说明

应用程序使用自定义的文本消息协议，支持的消息类型包括：
- 聊天消息
- 系统消息
- 文件传输消息

## 开者信息

这是一个基于 Qt 框架的练习项目，展示了 Qt 网络编程、UI 设计、数据库操作等多个方面的应用。

## 许可证

本项目采用 MIT 许可证，详情请参阅 LICENSE 文件。

## 贡献

欢迎提交 Issue 和 Pull Request！
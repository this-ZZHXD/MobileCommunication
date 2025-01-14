# ChatServer - 跨平台聊天服务器管理系统

## 项目概述
这是一个基于Qt框架开发的跨平台聊天服务器管理系统，支持Windows桌面端和Android移动端。
服务端提供用户管理、连接历史记录查看、聊天记录管理等功能。
客户端提供用户登录注册、和其他用户聊天的功能。
## 技术栈
- Qt 6.2+ (核心框架)
- C++17
- QMake (构建系统)
- Android SDK (移动端支持)
- Material Design (UI设计规范)

## 开发环境要求
- Qt Creator 6.2+
- Visual Studio 2019+ (Windows开发)
- Android Studio (Android开发)
- Android SDK API 21+ 
- CMake 3.16+

## 项目结构

### 核心类

#### MainWindow (mainwindow.h/cpp)
主窗口类，负责：
- 整体UI布局管理
- 服务器初始化
- 标签页管理
- 在线用户数量显示
- 网络连接管理

#### Server (server.h)
服务器核心类，负责：
- TCP连接管理
- 客户端会话管理
- 消息广播
- 在线用户统计

#### UserManager (usermanager.h/cpp)
用户管理类，功能包括：
- 用户信息的增删改查
- 用户认证
- 用户列表显示
- 密码管理

#### ConnectionHistory (connectionhistory.h/cpp)
连接历史记录类，负责：
- 显示客户端连接历史
- IP地址记录
- 连接时间记录
- 连接状态管理

#### ChatHistory (chathistory.h/cpp)
聊天记录管理类，功能包括：
- 聊天记录显示
- 记录实时刷新
- 历史记录清理
- 格式化显示

### 配置文件

#### ChatServer.pro
Qt项目配置文件：
- 项目依赖配置
- 编译选项设置
- 平台特定配置
- 源文件管理

#### AndroidManifest.xml
Android平台配置文件：
- 应用权限声明
- 活动配置
- 屏幕适配
- 系统要求

## 主要功能

### 1. 用户管理
- 添加新用户
- 修改用户信息
- 删除用户
- 用户信息本地存储

### 2. 连接历史
- 记录客户端IP
- 记录连接时间
- 显示连接状态
- 支持历史查询

### 3. 在线用户统计
- 实时显示在线人数
- 定期自动更新
- 断线检测

### 4. 聊天记录管理
- 实时显示聊天记录
- 支持记录清理
- 格式化显示
- 自动滚动到最新

## 双端适配特性

### 界面适配
- 响应式布局
- 适配不同屏幕尺寸
- 触摸友好的控件大小
- 合适的字体大小

### 操作优化
- 触摸操作优化
- 手势支持
- 按钮大小适配
- 输入框优化

### 样式统一
- Material Design风格
- 统一的颜色方案
- 清晰的视觉层次
- 良好的可读性

## 文件存储
- userInfo.txt: 用户信息存储
- history.txt: 连接历史记录
- chatHistory.txt: 聊天记录存储

## 编译和部署

### Windows平台
1. 使用Qt Creator打开项目
2. 选择MSVC编译器
3. 编译运行

### Android平台
1. 配置Android SDK
2. 在Qt Creator中选择Android工具链
3. 构建APK
4. 部署到设备

## 注意事项
1. 确保所需权限（网络、存储）已正确配置
2. Android设备需要API 21以上版本
3. 服务器默认端口为8080
4. 文件存储路径需要具有读写权限


环境已装好，你只需要专注代码就行，保持了原有的Windows功能，同时添加了移动端支持。

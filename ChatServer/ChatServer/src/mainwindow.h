#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTimer>
#include <QMessageBox>
#include <QHostAddress>
#include "usermanager.h"
#include "chathistory.h"
#include "connectionhistory.h"
#include "server.h"
#include "ui_mainwindow.h"

#define APP_DATA_DIR "./data"

/**
 * @brief 主窗口类
 * 
 * 管理服务器界面，包含：
 * - 用户管理
 * - 连接历史
 * - 聊天历史
 * - 在线用户显示
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    /**
     * @brief 设置界面布局和组件
     */
    void setupUI();
    
    /**
     * @brief 创建用户管理标签页
     */
    void createUserManagementTab();
    
    /**
     * @brief 创建连接历史标签页
     */
    void createConnectionHistoryTab();
    
    /**
     * @brief 创建聊天历史标签页
     */
    void createChatHistoryTab();
    
    /**
     * @brief 设置服务器
     */
    void setupServer();
    
    /**
     * @brief 设置网络连接
     */
    void setupNetworkConnections();
    
    /**
     * @brief 获取数据存储路径
     * @return 数据目录的路径
     */
    QString getDataPath() const;

private slots:
    /**
     * @brief 更新在线用户数量显示
     * @param count 当前在线用户数
     */
    void updateOnlineUsers(int count);

private:
    Ui::MainWindow *ui;              ///< UI界面
    QTabWidget *tabWidget;           ///< 标签页控件
    UserManager *userManager;        ///< 用户管理器
    ChatHistory *chatHistory;        ///< 聊天历史
    ConnectionHistory *connHistory;  ///< 连接历史
    QLabel *onlineUsersLabel;       ///< 在线用户数量标签
    Server *server;                 ///< 服务器实例
    QString dataPath;               ///< 数据存储路径
};

#endif 
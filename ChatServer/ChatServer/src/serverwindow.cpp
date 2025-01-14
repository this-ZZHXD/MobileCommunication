#include "serverwindow.h"
#include <QVBoxLayout>
#include <QTabWidget>
#include <QMessageBox>
#include <QHostAddress>
#include <QDebug>

ServerWindow::ServerWindow(QWidget *parent) : QWidget(parent)
{
    // 先创建服务器和历史记录实例
    server = new Server(this);
    connectionHistory = new ConnectionHistory(this);
    
    // 然后设置连接
    setupConnections();
    
    // 最后设置UI
    setupUI();
    
    // 启动服务器
    if (!server->listen(QHostAddress::Any, 12345)) {
        QMessageBox::critical(this, tr("Error"), 
                            tr("Server failed to start. Error: %1")
                            .arg(server->errorString()));
    } else {
        qDebug() << "Server is listening on port" << server->serverPort();
    }
}

void ServerWindow::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // 创建标签页
    QTabWidget *tabWidget = new QTabWidget(this);
    
    // 添加标签页
    tabWidget->addTab(connectionHistory, tr("Connection History"));
    // ... 其他标签页
    
    mainLayout->addWidget(tabWidget);
    
    // 设置窗口标题
    setWindowTitle(tr("Chat Server Management"));
}

void ServerWindow::setupConnections()
{
    qDebug() << "Setting up connections...";
    
    // 连接服务器的newConnectionLogged信号到connectionHistory的刷新槽
    connect(server, &Server::newConnectionLogged,
            connectionHistory, &ConnectionHistory::onNewConnection,
            Qt::QueuedConnection);  // 使用队列连接
            
    // 连接服务器的clientCountChanged信号
    connect(server, &Server::clientCountChanged,
            this, &ServerWindow::updateOnlineUsers);
            
    qDebug() << "Connections set up completed";
}

void ServerWindow::updateOnlineUsers(int count)
{
    setWindowTitle(QString("Chat Server Management - Online Users: %1").arg(count));
}
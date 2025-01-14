/**
 * @brief 主窗口实现文件
 * 
 * 实现服务器管理界面的各项功能
 */

#include "mainwindow.h"
#include <QHostAddress>
#include <QMessageBox>
#include <QDir>
#include <QStandardPaths>

/**
 * @brief 构造函数，初始化主窗口
 */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setupUI();
    setupServer();
    setupNetworkConnections();
    
    // 设置窗口标题和初始大小
    setWindowTitle(tr("Chat Server Management"));
    resize(800, 600);
}

/**
 * @brief 析构函数，清理资源
 */
MainWindow::~MainWindow()
{
    if (server) {
        server->close();
    }
    delete ui;
}

/**
 * @brief 获取数据存储路径
 * 
 * 优先使用编译时定义的数据目录，
 * 如果不存在则使用应用程序目录下的 data 目录
 */
QString MainWindow::getDataPath() const
{
    QString dataPath = QString(APP_DATA_DIR);
    QDir dir(dataPath);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    return dataPath;
}

void MainWindow::setupUI()
{
    // 创建中心部件
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    // 创建主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    
    // 创建在线用户显示标签
    onlineUsersLabel = new QLabel(tr("Online Users: 0"));
    mainLayout->addWidget(onlineUsersLabel);
    
    // 创建标签页控件
    tabWidget = new QTabWidget;
    mainLayout->addWidget(tabWidget);
    
    // 创建各个功能标签页
    createUserManagementTab();
    createConnectionHistoryTab();
    createChatHistoryTab();
}

void MainWindow::createUserManagementTab()
{
    QString dataPath = getDataPath();
    userManager = new UserManager(dataPath, this);
    tabWidget->addTab(userManager, tr("User Management"));
}

void MainWindow::createConnectionHistoryTab()
{
    QString dataPath = getDataPath();
    connHistory = new ConnectionHistory(this);
    if (!dataPath.isEmpty()) {
        connHistory->setDataPath(dataPath);
    }
    tabWidget->addTab(connHistory, tr("Connection History"));
}

void MainWindow::createChatHistoryTab()
{
    QString dataPath = getDataPath();
    chatHistory = new ChatHistory(this);
    if (!dataPath.isEmpty()) {
        chatHistory->setDataPath(dataPath);
    }
    tabWidget->addTab(chatHistory, tr("Chat History"));
}

void MainWindow::updateOnlineUsers(int count)
{
    onlineUsersLabel->setText(tr("Online Users: %1").arg(count));
}

void MainWindow::setupNetworkConnections()
{
    // 创建定时器定期更新在线用户数
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this]() {
        if (server) {
            int onlineCount = server->getClientNums();
            updateOnlineUsers(onlineCount);
        }
    });
    timer->start(5000); // 每5秒更新一次
}

void MainWindow::setupServer()
{
    server = new Server(this);
    
    connect(server, &Server::clientCountChanged,
            this, &MainWindow::updateOnlineUsers);
    
    if (!server->listen(QHostAddress::Any, 8080)) {
        QMessageBox::critical(this, tr("Server Error"),
                            tr("Unable to start the server: %1.")
                            .arg(server->errorString()));
        return;
    }
} 
#include "loginwindow.h"
#include "ui_loginwindow.h"
#include "chatwindow.h"
#include "registerwindow.h"
#include <QMessageBox>
#include <QDebug>

LoginWindow::LoginWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::LoginWindow)
{
    ui->setupUi(this);
    setupUi();  // 调用我们的自定义UI设置
    
    socket = new QTcpSocket(this);
    connect(socket, &QTcpSocket::connected, this, &LoginWindow::onConnected);
    connect(socket, &QTcpSocket::readyRead, this, &LoginWindow::onReadyRead);
    connect(socket, &QTcpSocket::disconnected, this, &LoginWindow::onDisconnected);
    
    connectToServer();
}

void LoginWindow::setupUi()
{
    // 设置窗口基本属性
    setWindowTitle("Login");
    setFixedSize(400, 300);
    
    // 设置中心部件
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    // 设置主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(40, 40, 40, 40);
    
    // Logo或标题
    QLabel *titleLabel = new QLabel("Chat Room", this);
    titleLabel->setStyleSheet(
        "QLabel {"
        "   font-size: 24px;"
        "   font-weight: bold;"
        "   color: #2196F3;"
        "   margin-bottom: 20px;"
        "}"
    );
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);
    
    // 输入框样式
    QString inputStyle = 
        "QLineEdit {"
        "   border: 1px solid #ccc;"
        "   border-radius: 4px;"
        "   padding: 8px;"
        "   font-size: 14px;"
        "   background-color: white;"
        "   color: #000000;"
        "}"
        "QLineEdit:focus {"
        "   border-color: #2196F3;"
        "}";
    
    ui->usernameEdit->setStyleSheet(inputStyle);
    ui->usernameEdit->setPlaceholderText("Username");
    ui->passwordEdit->setStyleSheet(inputStyle);
    ui->passwordEdit->setPlaceholderText("Password");
    ui->passwordEdit->setEchoMode(QLineEdit::Password);
    
    // 按钮样式
    QString buttonStyle = 
        "QPushButton {"
        "   background-color: #2196F3;"
        "   color: white;"
        "   border: none;"
        "   padding: 8px 0px;"
        "   border-radius: 4px;"
        "   font-weight: bold;"
        "   min-width: 120px;"
        "   height: 35px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #1976D2;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #1565C0;"
        "}";
    
    ui->loginButton->setStyleSheet(buttonStyle);
    ui->loginButton->setText("Login");
    ui->loginButton->setCursor(Qt::PointingHandCursor);
    ui->loginButton->setFixedSize(120, 35);
    
    QString registerButtonStyle = 
        "QPushButton {"
        "   background-color: transparent;"
        "   color: #2196F3;"
        "   border: 1px solid #2196F3;"
        "   padding: 8px 0px;"
        "   border-radius: 4px;"
        "   font-weight: bold;"
        "   min-width: 120px;"
        "   height: 35px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #E3F2FD;"
        "}";
    
    ui->registerButton->setStyleSheet(registerButtonStyle);
    ui->registerButton->setText("Register");
    ui->registerButton->setCursor(Qt::PointingHandCursor);
    ui->registerButton->setFixedSize(120, 35);
    
    // 设置按钮布局
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch();
    buttonLayout->addWidget(ui->loginButton);
    buttonLayout->addSpacing(20);
    buttonLayout->addWidget(ui->registerButton);
    buttonLayout->addStretch();
    
    mainLayout->addWidget(ui->usernameEdit);
    mainLayout->addWidget(ui->passwordEdit);
    mainLayout->addLayout(buttonLayout);
    mainLayout->addStretch();
    
    // 设置Tab顺序
    setTabOrder(ui->usernameEdit, ui->passwordEdit);
    setTabOrder(ui->passwordEdit, ui->loginButton);
    setTabOrder(ui->loginButton, ui->registerButton);
    
    // 设置初始焦点
    ui->usernameEdit->setFocus();
}

void LoginWindow::on_loginButton_clicked()
{
    QString username = ui->usernameEdit->text();
    QString password = ui->passwordEdit->text();
    
    qDebug() << "Login button clicked!";
    qDebug() << "Username:" << username;
    
    if(username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "警告", "用户名和密码不能为空");
        return;
    }
    
    if(socket->state() != QAbstractSocket::ConnectedState) {
        QMessageBox::warning(this, "错误", "未连接到服务器，请等待重新连接");
        connectToServer();
        return;
    }
    
    QString msg = QString("LOGIN %1 %2").arg(username).arg(password);
    qDebug() << "Sending login message:" << msg;
    
    socket->write(msg.toUtf8());
    socket->flush();
}

void LoginWindow::onReadyRead()
{
    QByteArray data = socket->readAll();
    QString response = QString::fromUtf8(data).trimmed();
    qDebug() << "\n=== 服务器响应 ===";
    qDebug() << "原始数据:" << data;
    qDebug() << "转换后:" << response;
    qDebug() << "数据长度:" << data.length();
    
    if(response.startsWith("LOGIN_SUCCESS")) {
        qDebug() << "登录成功，创建聊天窗口...";
        QString username = ui->usernameEdit->text();
        
        // 创建 NetworkManager 并传递现有的 socket
        NetworkManager *netManager = new NetworkManager(this);
        
        // 在设置socket之前先断开当前的连接
        disconnect(socket, nullptr, this, nullptr);
        netManager->setSocket(socket);
        
        qDebug() << "聊天连接建立成功";
        
        // 创建聊天窗口
        ChatWindow *chatWindow = new ChatWindow(netManager, username, nullptr);
        chatWindow->show();
        
        // 发送历史记录请求
        QTimer::singleShot(500, [netManager]() {
            netManager->requestChatHistory();
            netManager->requestOnlineUsers();
        });
        
        // 释放 LoginWindow 的所有权但不删除 socket
        socket->setParent(nullptr);
        this->close();
    } else if(response.startsWith("LOGIN_FAILED")) {
        QString reason = response.mid(12);
        qDebug() << "Login failed:" << reason;
        QMessageBox::warning(this, "登录失败", reason.isEmpty() ? "登录失败" : reason);
    }
}

void LoginWindow::connectToServer()
{
    qDebug() << "Attempting to connect to server at" << serverAddress << ":" << serverPort;
    socket->connectToHost(serverAddress, serverPort);
}

void LoginWindow::onConnected()
{
    qDebug() << "Connected to server successfully";
}

void LoginWindow::onDisconnected()
{
    qDebug() << "Disconnected from server";
    QMessageBox::warning(this, "连接断开", "与服务器的连接已断开");
    QTimer::singleShot(3000, this, &LoginWindow::connectToServer);
}

// 这个槽函数会通过命名规则自动连接
void LoginWindow::on_registerButton_clicked()
{
    // 暂时断开登录窗口的 readyRead 信号
    disconnect(socket, &QTcpSocket::readyRead, this, &LoginWindow::onReadyRead);
    
    RegisterWindow *registerWindow = new RegisterWindow(socket, this);
    
    // 当注册窗口关闭时，重新连接登录窗口的信号
    connect(registerWindow, &RegisterWindow::registerWindowClosed, this, [this]() {
        connect(socket, &QTcpSocket::readyRead, this, &LoginWindow::onReadyRead);
    });
    
    // 当窗口被销毁时自动删除对象
    registerWindow->setAttribute(Qt::WA_DeleteOnClose);
    registerWindow->show();
}

LoginWindow::~LoginWindow()
{
    delete ui;
} 
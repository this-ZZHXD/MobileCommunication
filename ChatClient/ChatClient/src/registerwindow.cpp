#include "registerwindow.h"
#include "ui_registerwindow.h"
#include <QMessageBox>
#include <QDebug>
#include <QRegularExpression>

RegisterWindow::RegisterWindow(QTcpSocket *socket, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::RegisterWindow),
    socket(socket),
    usernameEdit(nullptr),
    passwordEdit(nullptr),
    confirmPasswordEdit(nullptr),
    registerButton(nullptr),
    backButton(nullptr)
{
    ui->setupUi(this);
    setupUi();
    
    // 设置为模态窗口
    setWindowModality(Qt::ApplicationModal);
    
    // 只在这里连接 readyRead 信号
    connect(socket, &QTcpSocket::readyRead, this, &RegisterWindow::onReadyRead);
    connect(backButton, &QPushButton::clicked, this, &RegisterWindow::close);
    connect(registerButton, &QPushButton::clicked, this, &RegisterWindow::on_registerButton_clicked);
}

void RegisterWindow::setupUi()
{
    // 创建 UI 元素
    usernameEdit = new QLineEdit(this);
    passwordEdit = new QLineEdit(this);
    confirmPasswordEdit = new QLineEdit(this);
    registerButton = new QPushButton("Register", this);
    backButton = new QPushButton("Back", this);
    
    // 设置密码输入框
    passwordEdit->setEchoMode(QLineEdit::Password);
    confirmPasswordEdit->setEchoMode(QLineEdit::Password);
    
    // 设置窗口基本属性
    setWindowTitle("Register");
    setFixedSize(400, 350);
    
    // 设置中心部件
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    // 设置主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(40, 40, 40, 40);
    
    // 标题
    QLabel *titleLabel = new QLabel("Create Account", this);
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
    
    usernameEdit->setStyleSheet(inputStyle);
    usernameEdit->setPlaceholderText("Username");
    
    passwordEdit->setStyleSheet(inputStyle);
    passwordEdit->setPlaceholderText("Password");
    
    confirmPasswordEdit->setStyleSheet(inputStyle);
    confirmPasswordEdit->setPlaceholderText("Confirm Password");
    
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
    
    QString backButtonStyle = 
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
    
    registerButton->setStyleSheet(buttonStyle);
    registerButton->setCursor(Qt::PointingHandCursor);
    registerButton->setFixedSize(120, 35);
    
    backButton->setStyleSheet(backButtonStyle);
    backButton->setCursor(Qt::PointingHandCursor);
    backButton->setFixedSize(120, 35);
    
    // 设置按钮布局
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch();
    buttonLayout->addWidget(registerButton);
    buttonLayout->addSpacing(20);
    buttonLayout->addWidget(backButton);
    buttonLayout->addStretch();
    
    // 添加所有控件到主布局
    mainLayout->addWidget(usernameEdit);
    mainLayout->addWidget(passwordEdit);
    mainLayout->addWidget(confirmPasswordEdit);
    mainLayout->addLayout(buttonLayout);
    mainLayout->addStretch();
    
    // 设置Tab顺序
    setTabOrder(usernameEdit, passwordEdit);
    setTabOrder(passwordEdit, confirmPasswordEdit);
    setTabOrder(confirmPasswordEdit, registerButton);
    setTabOrder(registerButton, backButton);
    
    // 设置初始焦点
    usernameEdit->setFocus();
}

void RegisterWindow::on_registerButton_clicked()
{
    QString username = usernameEdit->text().trimmed();
    QString password = passwordEdit->text();
    QString confirmPass = confirmPasswordEdit->text();
    
    qDebug() << "Register button clicked";
    qDebug() << "Username:" << username;
    
    // 基本验证
    if(username.isEmpty() || password.isEmpty() || confirmPass.isEmpty()) {
        QMessageBox::warning(this, "Warning", "All fields must be filled");
        return;
    }

    // 用户名长度验证
    if(username.length() < 3 || username.length() > 16) {
        QMessageBox::warning(this, "Warning", "Username must be between 3 and 16 characters");
        return;
    }

    // 用户名格式验证 - 使用 QRegularExpression
    QRegularExpression usernameRegex("^[a-zA-Z0-9_]+$");
    if(!usernameRegex.match(username).hasMatch()) {
        QMessageBox::warning(this, "Warning", "Username can only contain letters, numbers and underscore");
        return;
    }

    // 密码长度验证
    if(password.length() < 6 || password.length() > 20) {
        QMessageBox::warning(this, "Warning", "Password must be between 6 and 20 characters");
        return;
    }

    // 密码匹配验证
    if(password != confirmPass) {
        QMessageBox::warning(this, "Warning", "The passwords entered twice are inconsistent");
        passwordEdit->clear();
        confirmPasswordEdit->clear();
        passwordEdit->setFocus();
        return;
    }
    
    if(socket->state() != QAbstractSocket::ConnectedState) {
        QMessageBox::warning(this, "Error", "Not connected to the server");
        return;
    }
    
    QString msg = QString("REGISTER %1 %2").arg(username).arg(password);
    qDebug() << "Sending register message:" << msg;
    
    socket->write(msg.toUtf8());
    socket->flush();
}

void RegisterWindow::onReadyRead()
{
    QByteArray data = socket->readAll();
    qDebug() << "Raw data received:" << data;
    qDebug() << "Raw data size:" << data.size();
    
    QString response = QString::fromUtf8(data).trimmed();
    qDebug() << "Converted response:" << response;
    qDebug() << "Response length:" << response.length();
    
    if(response.isEmpty()) {
        qDebug() << "Warning: Empty response received";
        return;
    }
    
    if(response.startsWith("REGISTER_SUCCESS")) {
        QString message = response.mid(16); // Remove "REGISTER_SUCCESS "
        qDebug() << "Success message:" << message;
        QMessageBox::information(this, "Success", 
            message.isEmpty() ? "Registration successful, please return to login" : message);
        this->close();
    } else if(response.startsWith("REGISTER_FAILED")) {
        QString reason = response.mid(15); // Remove "REGISTER_FAILED "
        qDebug() << "Failure reason:" << reason;
        QMessageBox::warning(this, "Registration failed", 
            reason.isEmpty() ? "Registration failed" : reason);
        
        // 根据错误类型清空相应输入框
        if(reason.contains("Username")) {
            usernameEdit->clear();
            usernameEdit->setFocus();
        } else if(reason.contains("Password")) {
            passwordEdit->clear();
            confirmPasswordEdit->clear();
            passwordEdit->setFocus();
        }
    } else {
        qDebug() << "Unexpected response format:" << response;
    }
}

void RegisterWindow::closeEvent(QCloseEvent *event)
{
    // 在窗口关闭时断开信号连接
    disconnect(socket, &QTcpSocket::readyRead, this, &RegisterWindow::onReadyRead);
    emit registerWindowClosed();  // 发送自定义信号
    QMainWindow::closeEvent(event);
}

RegisterWindow::~RegisterWindow()
{
    // 确保在析构时断开信号连接
    if (socket) {
        disconnect(socket, &QTcpSocket::readyRead, this, &RegisterWindow::onReadyRead);
    }
    
    delete ui;
    delete usernameEdit;
    delete passwordEdit;
    delete confirmPasswordEdit;
    delete registerButton;
    delete backButton;
} 
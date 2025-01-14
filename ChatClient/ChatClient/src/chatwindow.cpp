#include "chatwindow.h"
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QScrollBar>
#include <QtCore/QDateTime>
#include <QtWidgets/QLabel>
#include <QtGui/QScreen>
#include <QtWidgets/QApplication>
#include <QtGui/QTextCursor>
#include <QtGui/QTextDocument>
#include <QtGui/QTextBlock>
#include <QtGui/QTextCharFormat>
#include <QtCore/QTimer>

ChatWindow::ChatWindow(NetworkManager *netManager, const QString &username, QWidget *parent)
    : QMainWindow(parent), networkManager(netManager), currentUsername(username)
{
    qDebug() << "\n=== 初始化聊天窗口 ===";
    qDebug() << "用户名:" << username;
    
    if (isMobileDevice()) {
        setupMobileUI();
    } else {
        setupUI();
    }
    setupConnections();
    
    qDebug() << "请求聊天历史...";
    networkManager->requestChatHistory();
    
    qDebug() << "启动在线用户更新定时器...";
    networkManager->startUpdateTimer();
    
    qDebug() << "=== 聊天窗口初始化完成 ===\n";
}

void ChatWindow::setupUI()
{
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    // 使用 QSplitter 来允许用户调整列表和聊天区域的宽度
    QSplitter *splitter = new QSplitter(Qt::Horizontal, centralWidget);
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->addWidget(splitter);
    mainLayout->setContentsMargins(5, 5, 5, 5);
    
    // 创建左侧用户列表
    QWidget *leftWidget = new QWidget(splitter);
    QVBoxLayout *leftLayout = new QVBoxLayout(leftWidget);
    
    QLabel *onlineLabel = new QLabel("在线用户", leftWidget);
    onlineLabel->setStyleSheet(
        "QLabel {"
        "   font-weight: bold;"
        "   padding: 8px;"
        "   color: #2196F3;"
        "   font-size: 16px;"
        "}"
    );
    
    userListWidget = new QListWidget(leftWidget);
    userListWidget->setMinimumWidth(150);
    userListWidget->setStyleSheet(
        "QListWidget {"
        "   border: 1px solid #ddd;"
        "   border-radius: 4px;"
        "   background-color: white;"
        "   font-size: 14px;"
        "   color: #333333;"
        "}"
        "QListWidget::item {"
        "   padding: 8px;"
        "   color: #2196F3;"
        "}"
        "QListWidget::item:hover {"
        "   background-color: #f0f0f0;"
        "   color: #1976D2;"
        "}"
    );
    
    leftLayout->addWidget(onlineLabel);
    leftLayout->addWidget(userListWidget);
    
    // 创建右侧聊天区域
    QWidget *chatWidget = new QWidget(splitter);
    QVBoxLayout *chatLayout = new QVBoxLayout(chatWidget);
    chatLayout->setSpacing(10);
    
    chatDisplay = new QTextEdit(chatWidget);
    chatDisplay->setReadOnly(true);
    chatDisplay->setStyleSheet(
        "QTextEdit {"
        "   border: 1px solid #ddd;"
        "   border-radius: 4px;"
        "   background-color: white;"
        "   padding: 10px;"
        "   color: black;"
        "   font-size: 15px;"
        "   line-height: 1.5;"
        "}"
    );
    
    QWidget *inputWidget = new QWidget(chatWidget);
    QHBoxLayout *inputLayout = new QHBoxLayout(inputWidget);
    inputLayout->setContentsMargins(0, 0, 0, 0);
    
    messageInput = new QLineEdit(inputWidget);
    messageInput->setPlaceholderText(" 输入消息...");
    messageInput->setStyleSheet(
        "QLineEdit {"
        "   border: 1px solid #ddd;"
        "   border-radius: 4px;"
        "   padding: 8px;"
        "   background-color: white;"
        "   color: black;"
        "   font-size: 14px;"
        "}"
    );
    
    sendButton = new QPushButton("发送", inputWidget);
    sendButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #2196F3;"
        "   color: white;"
        "   border: none;"
        "   padding: 8px 20px;"
        "   border-radius: 4px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #1976D2;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #1565C0;"
        "}"
    );
    sendButton->setFixedWidth(80);
    
    inputLayout->addWidget(messageInput);
    inputLayout->addWidget(sendButton);
    
    chatLayout->addWidget(chatDisplay);
    chatLayout->addWidget(inputWidget);
    
    // 设置初始分割比例
    splitter->setStretchFactor(0, 1);  // 用户列表
    splitter->setStretchFactor(1, 4);  // 聊天区域
    
    resize(900, 600);
    setWindowTitle("聊天室 - " + currentUsername);
}

void ChatWindow::setupConnections()
{
    qDebug() << "\n=== 设置信号连接 ===";
    
    // 网络信号连接
    connect(networkManager, &NetworkManager::onlineUsersReceived,
            this, &ChatWindow::updateOnlineUsers,
            Qt::QueuedConnection);
    
    connect(networkManager, &NetworkManager::chatHistoryReceived,
            this, &ChatWindow::displayChatHistory,
            Qt::QueuedConnection);
    
    connect(networkManager, &NetworkManager::newMessageReceived,
            this, &ChatWindow::displayNewMessage,
            Qt::QueuedConnection);
    
    connect(networkManager, &NetworkManager::systemMessageReceived,
            this, &ChatWindow::displaySystemMessage,
            Qt::QueuedConnection);
    
    qDebug() << "网络信号连接完成";
    
    // UI信号连接
    connect(sendButton, &QPushButton::clicked, this, &ChatWindow::sendMessage);
    connect(messageInput, &QLineEdit::returnPressed, this, &ChatWindow::sendMessage);
    
    // 修改消息发送状态处理
    connect(networkManager, &NetworkManager::messageSent, this, [this](bool success) {
        messageInput->setEnabled(true);
        sendButton->setEnabled(true);
        
        if (success) {
            messageInput->clear();
            qDebug() << "消息发送成功";
        } else {
            qDebug() << "消息发送失败";
            chatDisplay->append("<div style='color: red; text-align: center; margin: 5px;'>"
                              "<span style='background-color: #FFE0E0; padding: 5px; border-radius: 5px;'>"
                              "消息发送失败，请检查网络连接</span></div>");
        }
    });
    
    // 添加连接状态显示
    connect(networkManager, &NetworkManager::connected, this, [this]() {
        setWindowTitle("聊天室 - 已连接");
    });
    
    connect(networkManager, &NetworkManager::disconnected, this, [this]() {
        setWindowTitle("聊天室 - 连接断开，正在重连...");
    });
    
    connect(networkManager, &NetworkManager::connectionError, this, [this](const QString &error) {
        QMessageBox::warning(this, "连接错误", error);
    });
    
    qDebug() << "=== 信号连接设置完成 ===\n";
}

void ChatWindow::updateOnlineUsers(const QJsonArray& users) {
    userListWidget->clear();
    for (const QJsonValue& user : users) {
        userListWidget->addItem(user.toString());
    }
}

void ChatWindow::displayChatHistory(const QString &history)
{
    qDebug() << "\n=== 开始显示聊天历史 ===";
    qDebug() << "接收到的历史记录:" << history;
    
    chatDisplay->clear();
    
    if (history.isEmpty()) {
        qDebug() << "警告: 历史记录为空";
        return;
    }
    
    // 按行处理历史记录
    QStringList lines = history.split('\n', Qt::SkipEmptyParts);
    qDebug() << "历史记录行数:" << lines.size();
    
    for (const QString &line : lines) {
        if (line.trimmed().isEmpty()) continue;
        
        if (line.contains("[SYSTEM]")) {
            displaySystemMessage(line);
            continue;
        }
        
        // 解析时间戳和消息内容
        QString timestamp = line.section(']', 0, 0).mid(1);  // 移除方括号
        QString content = line.section(']', 1).trimmed();
        
        // 分离用户名和消息内容
        QString username, messageText;
        int colonPos = content.indexOf(": ");
        if (colonPos != -1) {
            username = content.left(colonPos).trimmed();
            messageText = content.mid(colonPos + 2).trimmed();
        }
        
        QString formattedMessage;
        if (username == currentUsername) {
            formattedMessage = QString(
                "<table width='100%' cellpadding='0' cellspacing='0'>"
                "  <tr>"
                "    <td align='right'>"
                "      <div style='margin: 8px 10px; max-width: 80%%; display: inline-block;'>"
                "        <div style='text-align: right;'>"
                "          <span style='color: #4CAF50; font-weight: bold;'>%1</span>"
                "          <span style='color: #666666; font-size: 12px; margin-left: 10px;'>%2</span>"
                "        </div>"
                "        <div style='text-align: right; margin-top: 4px;'>%3</div>"
                "      </div>"
                "    </td>"
                "  </tr>"
                "</table>"
            ).arg(username, timestamp, messageText);
        } else {
            formattedMessage = QString(
                "<table width='100%' cellpadding='0' cellspacing='0'>"
                "  <tr>"
                "    <td align='left'>"
                "      <div style='margin: 5px 10px; max-width: 80%%; display: inline-block;'>"
                "        <div style='text-align: left;'>"
                "          <span style='color: #2196F3; font-weight: bold;'>%1</span>"
                "          <span style='color: #666666; font-size: 12px; margin-left: 10px;'>%2</span>"
                "        </div>"
                "        <div style='text-align: left; margin-top: 4px;'>%3</div>"
                "      </div>"
                "    </td>"
                "  </tr>"
                "</table>"
            ).arg(username, timestamp, messageText);
        }
        
        chatDisplay->append(formattedMessage);
    }
    
    // 滚动到底部
    QScrollBar *scrollBar = chatDisplay->verticalScrollBar();
    scrollBar->setValue(scrollBar->maximum());
}

void ChatWindow::displayNewMessage(const QString &message)
{
    qDebug() << "\n=== 显示新消息 ===";
    qDebug() << "消息内容:" << message;
    
    // 添加时间戳
    QString timestamp = QDateTime::currentDateTime().toString("HH:mm:ss");
    
    // 分离用户名和消息内容
    QString username, content;
    int colonPos = message.indexOf(": ");
    if (colonPos != -1) {
        username = message.left(colonPos);
        content = message.mid(colonPos + 2);
    }
    
    QString formattedMessage;
    if (username == currentUsername) {
        // 当前用户的消息 - 右对齐
        formattedMessage = QString(
            "<table width='100%' cellpadding='0' cellspacing='0'>"
            "  <tr>"
            "    <td align='right'>"  // 右对齐整个内容
            "      <div style='margin: 35px 10px;'>"  // 增加上下边距到15px
            "        <div style='text-align: right;'>"  // 用户名和时间右对齐
            "          <span style='color: #4CAF50; font-weight: bold;'>%1</span>"
            "          <span style='color: #666666; font-size: 12px; margin-left: 10px;'>%2</span>"
            "        </div>"
            "        <div style='text-align: right; margin-top: 4px;'>%3</div>"  // 增加与用户名的间距
            "      </div>"
            "    </td>"
            "  </tr>"
            "</table>"
        ).arg(username, timestamp, content);
    } else {
        // 其他用户的消息 - 左对齐
        formattedMessage = QString(
            "<table width='100%' cellpadding='0' cellspacing='0'>"
            "  <tr>"
            "    <td align='left'>"  // 左对齐整个内容
            "      <div style='margin: 35px 10px;'>"  // 增加上下边距到15px
            "        <div style='text-align: left;'>"  // 用户名和时间左对齐
            "          <span style='color: #2196F3; font-weight: bold;'>%1</span>"
            "          <span style='color: #666666; font-size: 12px; margin-left: 10px;'>%2</span>"
            "        </div>"
            "        <div style='text-align: left; margin-top: 4px;'>%3</div>"  // 增加与用户名的间距
            "      </div>"
            "    </td>"
            "  </tr>"
            "</table>"
        ).arg(username, timestamp, content);
    }
    
    chatDisplay->append(formattedMessage);
    
    // 滚动到底部
    QScrollBar *scrollBar = chatDisplay->verticalScrollBar();
    scrollBar->setValue(scrollBar->maximum());
}

void ChatWindow::displaySystemMessage(const QString &message)
{
    qDebug() << "\n=== 显示系统消息 ===";
    qDebug() << "消息内容:" << message;
    
    QString timestamp = QDateTime::currentDateTime().toString("HH:mm:ss");
    
    // 系统消息使用特殊样式
    chatDisplay->append(QString(
        "<div style='text-align: center; margin: 16px 0;'>"
        "  <div style='display: inline-block; border: 1px solid #E8E8E8; "
        "       padding: 8px 16px; border-radius: 16px;'>"
        "    <span style='color: #888888; font-size: 12px; margin-right: 8px;'>%1</span>"
        "    <span style='color: #9E9E9E; font-size: 14px;'>%2</span>"
        "  </div>"
        "</div>").arg(timestamp, message));
}

void ChatWindow::sendMessage()
{
    QString text = messageInput->text().trimmed();
    if (!text.isEmpty()) {
        messageInput->setEnabled(false);
        sendButton->setEnabled(false);
        
        // 添加用户名到消息中
        QString fullMessage = QString("CHAT %1: %2").arg(currentUsername, text);
        qDebug() << "准备发送消息:" << fullMessage;
        networkManager->sendMessage(fullMessage);
    }
}

void ChatWindow::updateChatHistory(const QJsonArray& history)
{
    chatDisplay->clear();
    // 设置聊天记录的文字颜色为黑色
    chatDisplay->setStyleSheet(
        "QTextEdit {"
        "   border: 1px solid #ddd;"
        "   border-radius: 4px;"
        "   background-color: white;"
        "   padding: 5px;"
        "   color: #000000;"
        "}"
    );
    
    // 遍历历史记录数组并显示每条消息
    for (const QJsonValue &value : history) {
        QString message = value.toString();
        chatDisplay->append(message);
    }
}

void ChatWindow::displayMessage(const QString &message)
{
    // 添加时间戳
    QString timestamp = QDateTime::currentDateTime().toString("[hh:mm:ss] ");
    
    // 格式化消息
    QString formattedMessage = QString("<div style='margin: 5px;'>"
                                     "<span style='color: gray;'>%1</span>"
                                     "<span style='background-color: #E8E8E8; padding: 5px; border-radius: 5px; "
                                     "color: black;'>%2</span></div>").arg(timestamp, message);
    
    chatDisplay->append(formattedMessage);
    
    // 滚动到底部
    QScrollBar *scrollBar = chatDisplay->verticalScrollBar();
    scrollBar->setValue(scrollBar->maximum());
}

bool ChatWindow::isCurrentUser(const QString& username) const {
    return username == currentUsername;
}

void ChatWindow::displayMessage(const QString& username, const QString& message, const QString& timestamp) {
    QTextCursor cursor(chatDisplay->document());
    cursor.movePosition(QTextCursor::End);
    
    QTextBlockFormat blockFormat;
    if (isCurrentUser(username)) {
        blockFormat.setAlignment(Qt::AlignRight);
    } else {
        blockFormat.setAlignment(Qt::AlignLeft);
    }
    
    cursor.insertBlock(blockFormat);
    
    QTextCharFormat timeFormat;
    timeFormat.setForeground(Qt::gray);
    cursor.insertText(timestamp + " ", timeFormat);
    
    QTextCharFormat nameFormat;
    nameFormat.setFontWeight(QFont::Bold);
    cursor.insertText(username + ": ", nameFormat);
    
    QTextCharFormat messageFormat;
    cursor.insertText(message, messageFormat);
    
    chatDisplay->setTextCursor(cursor);
    chatDisplay->ensureCursorVisible();
}

bool ChatWindow::isMobileDevice()
{
#ifdef Q_OS_ANDROID
    return true;
#else
    return false;
#endif
}

void ChatWindow::setupMobileUI()
{
    // 创建主布局
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    
    // 创建聊天显示区域
    chatDisplay = new QTextEdit(this);
    chatDisplay->setReadOnly(true);
    chatDisplay->setStyleSheet(
        "QTextEdit {"
        "   border: 1px solid #ddd;"
        "   border-radius: 8px;"
        "   padding: 12px;"
        "   font-size: 16px;"
        "}"
    );
    
    // 创建用户列表（可折叠）
    QWidget *userListContainer = new QWidget(this);
    QVBoxLayout *userListLayout = new QVBoxLayout(userListContainer);
    
    QPushButton *toggleUserListBtn = new QPushButton("在线用户 ▼", this);
    userListWidget = new QListWidget(this);
    userListWidget->setMaximumHeight(150);
    userListWidget->hide();
    
    userListLayout->addWidget(toggleUserListBtn);
    userListLayout->addWidget(userListWidget);
    
    // 创建输入区域
    QWidget *inputContainer = new QWidget(this);
    QHBoxLayout *inputLayout = new QHBoxLayout(inputContainer);
    
    messageInput = new QLineEdit(this);
    messageInput->setPlaceholderText("输入消息...");
    messageInput->setStyleSheet(
        "QLineEdit {"
        "   border: 1px solid #ddd;"
        "   border-radius: 8px;"
        "   padding: 12px;"
        "   font-size: 16px;"
        "   min-height: 48px;"
        "}"
    );
    
    sendButton = new QPushButton("发送", this);
    sendButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #2196F3;"
        "   color: white;"
        "   border: none;"
        "   border-radius: 8px;"
        "   padding: 12px 24px;"
        "   font-size: 16px;"
        "   min-height: 48px;"
        "}"
    );
    
    inputLayout->addWidget(messageInput);
    inputLayout->addWidget(sendButton);
    
    // 组装主布局
    mainLayout->addWidget(userListContainer);
    mainLayout->addWidget(chatDisplay);
    mainLayout->addWidget(inputContainer);
    
    // 设置边距
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(10);
    
    // 连接信号
    connect(toggleUserListBtn, &QPushButton::clicked, this, [=]() {
        if (userListWidget->isHidden()) {
            userListWidget->show();
            toggleUserListBtn->setText("在线用户 ▲");
        } else {
            userListWidget->hide();
            toggleUserListBtn->setText("在线用户 ▼");
        }
    });
    
    // 设置窗口属性
    setWindowState(Qt::WindowMaximized);
}

void ChatWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    if (isMobileDevice()) {
        // 重新布局UI元素
        QWidget *central = centralWidget();
        if (central) {
            central->setGeometry(0, 0, width(), height());
        }
        
        // 调整输入框位置，确保在软键盘弹出时可见
        if (messageInput->hasFocus()) {
            QTimer::singleShot(100, this, [this]() {
                messageInput->setVisible(true);
                messageInput->raise();  // 将输入框提升到顶层
                messageInput->setFocus();
            });
        }
    }
}

bool ChatWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (isMobileDevice()) {
        if (event->type() == QEvent::TouchBegin) {
            if (watched == chatDisplay) {
                messageInput->clearFocus();
                return true;
            }
        } else if (event->type() == QEvent::FocusIn) {
            if (watched == messageInput) {
                // 当输入框获得焦点时，确保它可见（不被软键盘遮挡）
                QTimer::singleShot(100, this, [this]() {
                    messageInput->setVisible(true);
                    messageInput->raise();  // 将输入框提升到顶层
                    messageInput->setFocus();
                    chatDisplay->ensureCursorVisible();
                });
            }
        }
    }
    return QMainWindow::eventFilter(watched, event);
} 

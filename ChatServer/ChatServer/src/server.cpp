#include "server.h"
#include "utils.h"
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QHostAddress>
#include <QCoreApplication>
#include <QDir>

// 定义静态成员变量
const QString Server::USER_INFO_FILE = "userInfo.txt";
const QString Server::HISTORY_FILE = "history.txt";
const QString Server::CHAT_HISTORY_FILE = "chatHistory.txt";

/**
 * @brief 服务器构造函数
 * @param parent 父对象
 */
Server::Server(QObject *parent)
    : QTcpServer(parent)
    , hasNewMessage(false)
    , hasUserStatusChanged(false)
    , broadcastTimer(nullptr)
{
    initializeDataFiles();
    initBroadcast();

    // 初始广播
    broadcastOnlineUsers();
    broadcastChatHistory();
}

/**
 * @brief 处理新的客户端连接
 *
 * 当有新客户端连接时：
 * 1. 创建新的套接字
 * 2. 设置连接
 * 3. 记录连接历史
 * 4. 更新在线用户数
 */
void Server::incomingConnection(qintptr socketDescriptor)
{
    QTcpSocket *clientSocket = new QTcpSocket(this);
    if (clientSocket->setSocketDescriptor(socketDescriptor)) {
        connect(clientSocket, &QTcpSocket::readyRead,
                this, &Server::processClientMessage);
        connect(clientSocket, &QTcpSocket::disconnected,
                this, &Server::handleClientDisconnected);

        clientSockets.append(clientSocket);
        emit clientCountChanged(clientSockets.size());

        // 设置用户状态变化标志
        hasUserStatusChanged = true;
        
        // 移除这里的连接记录
        // logConnection(clientSocket->peerAddress().toString());
    } else {
        delete clientSocket;
    }
}

/**
 * @brief 处理客户端消息
 *
 * 消息格式：
 * - 登录：LOGIN username password
 * - 注册：REGISTER username password
 * - 聊天：CHAT message
 * - 获取在线用户：GET_ONLINE_USERS
 * - 获取聊天历史：GET_CHAT_HISTORY
 */
void Server::processClientMessage()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;

    QByteArray data = socket->readAll();
    QString message = QString::fromUtf8(data);
    QStringList parts = message.split(" ", Qt::SkipEmptyParts);

    if (parts.isEmpty()) return;

    QString command = parts[0];
    QString content = parts.mid(1).join(" ");

    if (command == "LOGIN") {
        handleLoginRequest(socket, content);
    }
    else if (command == "REGISTER") {
        handleRegisterRequest(socket, content);
    }
    else if (command == "CHAT") {
        handleChatMessage(socket, content);
    }
    else if (command == "GET_ONLINE_USERS") {
        sendOnlineUsers(socket);
    }
    else if (command == "GET_CHAT_HISTORY") {
        sendChatHistory(socket);
    }
}

/**
 * @brief 处理登录请求
 *
 * 验证流程：
 * 1. 检查消息格式
 * 2. 验证用户名密码
 * 3. 检查是否已在线
 * 4. 发送登录结果
 * 5. 广播上线消息
 */
void Server::handleLoginRequest(QTcpSocket *socket, const QString &data)
{
    QStringList credentials = data.split(" ");
    if (credentials.size() < 2) {
        socket->write("LOGIN_FAILED Invalid format");
        return;
    }

    QString username = credentials[0];
    QString password = credentials[1];

    // 检查用户是否已经在线
    for (auto it = clientUsernames.begin(); it != clientUsernames.end(); ++it) {
        if (it.value() == username) {
            socket->write("LOGIN_FAILED User already logged in");
            return;
        }
    }

    // 验证用户名和密码
    if (validateUser(username, password)) {
        // 登录成功
        clientUsernames[socket] = username;
        hasUserStatusChanged = true;

        // 发送成功消息给客户端
        QByteArray response = "LOGIN_SUCCESS";
        response.append(" ");
        response.append(username.toUtf8());
        socket->write(response);

        // 广播用户上线消息
        QString onlineMsg = QString("[SYSTEM] %1 has joined the chat").arg(username);
        broadcastMessage(onlineMsg);

        // 记录登录日志
        QString cleanIP = socket->peerAddress().toString();
        if (cleanIP.startsWith("::ffff:")) {
            cleanIP = cleanIP.mid(7);
        }
        QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        
        // 使用正确的文件路径
        QString filePath = getDataFilePath(HISTORY_FILE);
        
        QFile file(filePath);
        if (file.open(QIODevice::Append | QIODevice::Text)) {
            QTextStream out(&file);
            out << cleanIP << " " << username << " " << timestamp << " CONNECTED\n";
            file.close();
            
            emit newConnectionLogged();
            qDebug() << "New connection logged and signal emitted";
        }
    } else {
        // 登录失败
        socket->write("LOGIN_FAILED Invalid username or password");
    }
}

void Server::handleRegisterRequest(QTcpSocket *socket, const QString &data)
{
    QStringList credentials = data.split(" ");
    if (credentials.size() < 2) {
        socket->write("REGISTER_FAILED Invalid format");
        return;
    }

    QString username = credentials[0];
    QString password = credentials[1];

    // 检查用户名长度
    if (username.length() < 3 || username.length() > 16) {
        socket->write("REGISTER_FAILED Username must be between 3 and 16 characters");
        return;
    }

    // 检查密码长度
    if (password.length() < 6 || password.length() > 20) {
        socket->write("REGISTER_FAILED Password must be between 6 and 20 characters");
        return;
    }

    // 检查用户名是否已存在
    QFile checkFile(USER_INFO_FILE);
    if (checkFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&checkFile);
        while (!in.atEnd()) {
            QString line = in.readLine();
            QStringList fields = line.split(" ");
            if (fields.size() >= 1 && fields[0] == username) {
                checkFile.close();
                socket->write("REGISTER_FAILED Username already exists");
                return;
            }
        }
        checkFile.close();
    }

    // 注册新用户
    QFile file(USER_INFO_FILE);
    if (file.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&file);
        out << username << " " << password << "\n";
        file.close();
        socket->write("REGISTER_SUCCESS Registration successful");
        
        qDebug() << "New user registered:" << username;
    } else {
        socket->write("REGISTER_FAILED Server error");
        qDebug() << "Failed to register user:" << username << ", error:" << file.errorString();
    }
}

void Server::handleChatMessage(QTcpSocket *socket, const QString &data)
{
    if (!clientUsernames.contains(socket)) return;

    QString timestamp = QDateTime::currentDateTime()
                            .toString("yyyy-MM-dd hh:mm:ss");

    // 打印原始消息内容进行调试
    qDebug() << "Raw message:" << data;

    QString messageContent;
    QString username = clientUsernames[socket];

    // 检查消息格式
    if (data.contains(": ")) {
        // 如果消息中包含": "，说明可能包含了用户名
        int colonPos = data.indexOf(": ");
        messageContent = data.mid(colonPos + 2);
        
        // 打印解析结果
        qDebug() << "Found colon at position:" << colonPos;
        qDebug() << "Extracted content:" << messageContent;
    } else {
        // 如果没有": "，则整个内容都是消息
        messageContent = data;
        qDebug() << "Using full message as content:" << messageContent;
    }

    // 如果消息以 "CHAT " 开头，去掉这个前缀
    if (messageContent.startsWith("CHAT ")) {
        messageContent = messageContent.mid(5);
        qDebug() << "Removed CHAT prefix:" << messageContent;
    }

    // 格式化消息用于广播
    QString formattedMessage = QString("[%1] %2: %3")
                                   .arg(timestamp)
                                   .arg(username)
                                   .arg(messageContent);

    qDebug() << "Final formatted message:" << formattedMessage;

    // 广播消息给所有客户端
    for (QTcpSocket *clientSocket : clientSockets) {
        if (clientSocket != socket) {
            clientSocket->write(("CHAT " + formattedMessage).toUtf8() + "\n");
        }
    }

    // 记录到历史文件
    QFile file(CHAT_HISTORY_FILE);
    if (file.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&file);
        out << formattedMessage << "\n";
        file.close();
        
        hasNewMessage = true;
    }
}

void Server::broadcastMessage(const QString &message, QTcpSocket *sender)
{
    // 如果是系统消息，设置新消息标志
    if (message.startsWith("[SYSTEM]")) {
        hasNewMessage = true;
    }

    for (QTcpSocket *socket : clientSockets) {
        if (socket != sender) {
            socket->write(message.toUtf8() + "\n");
        }
    }
}

bool Server::validateUser(const QString &username, const QString &password)
{
    if (username.isEmpty() || password.isEmpty()) {
        return false;
    }

    QFile file(USER_INFO_FILE);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList fields = line.split(" ");
        if (fields.size() >= 2 &&
            fields[0] == username &&
            fields[1] == password) {
            file.close();
            return true;
        }
    }
    file.close();
    return false;
}

void Server::logConnection(const QString &info)
{
    QFile file(HISTORY_FILE);
    if (file.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&file);
        QString timestamp = QDateTime::currentDateTime()
                                .toString("yyyy-MM-dd hh:mm:ss");
        
        // 解析传入的信息
        QStringList parts = info.split(" ");
        QString cleanIP = parts[0];
        if (cleanIP.startsWith("::ffff:")) {
            cleanIP = cleanIP.mid(7);
        }
        
        // 如果包含用户名（登录情况）
        if (parts.size() >= 2) {
            QString username = parts[1];
            out << cleanIP << " " << username << " " << timestamp << " CONNECTED\n";
        } else {
            // 仅连接情况
            out << cleanIP << " " << timestamp << " CONNECTED\n";
        }
        file.close();
    }
}

void Server::logChat(const QString &username, const QString &message)
{
    QFile file(CHAT_HISTORY_FILE);
    if (file.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&file);
        QString timestamp = QDateTime::currentDateTime()
                                .toString("[yyyy-MM-dd hh:mm:ss]");
        
        if (message.startsWith("[SYSTEM]")) {
            // 系统消息格式
            out << QString("%1 %2\n")
                       .arg(timestamp)
                       .arg(message);
        } else {
            // 普通聊天消息格式
            out << QString("%1 %2: %3\n")
                       .arg(timestamp)
                       .arg(username)
                       .arg(message);
        }
        file.close();
        
        // 设置新消息标志
        hasNewMessage = true;
    }
}

void Server::handleClientDisconnected()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;

    if (clientUsernames.contains(socket)) {
        QString username = clientUsernames[socket];
        QString offlineMsg = QString("[SYSTEM] %1 has left the chat").arg(username);
        broadcastMessage(offlineMsg);
        clientUsernames.remove(socket);
    }

    clientSockets.removeOne(socket);
    socket->deleteLater();
    emit clientCountChanged(clientSockets.size());

    // 用户断开连接后设置状态变化标志
    hasUserStatusChanged = true;
    
    // 只在用户状态改变时广播
    if (hasUserStatusChanged) {
        broadcastOnlineUsers();
        hasUserStatusChanged = false;  // 广播后重置标志
    }
}

int Server::getClientNums() const
{
    return clientSockets.size();
}

bool Server::isUserOnline(const QString &username)
{
    return clientUsernames.values().contains(username);
}

/**
 * @brief 发送在线用户列表给客户端
 */
void Server::sendOnlineUsers(QTcpSocket *socket)
{
    QStringList users = clientUsernames.values();
    QString response = "ONLINE_USERS " + users.join(",");
    socket->write(response.toUtf8());
}

/**
 * @brief 发送聊天历史记录给客户端
 */
void Server::sendChatHistory(QTcpSocket *socket)
{
    QFile file(CHAT_HISTORY_FILE);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }

    QTextStream in(&file);
    QString history = "CHAT_HISTORY\n" + in.readAll();
    socket->write(history.toUtf8());
    file.close();
}

Server::~Server()
{
    if (broadcastTimer) {
        broadcastTimer->stop();
        delete broadcastTimer;
        broadcastTimer = nullptr;
    }
}

void Server::initBroadcast()
{
    if (!broadcastTimer) {
        broadcastTimer = new QTimer(this);
        connect(broadcastTimer, &QTimer::timeout, this, &Server::broadcastUpdate);
        broadcastTimer->start(3000);  // 每3秒广播一次
    }
}

void Server::broadcastUpdate()
{
    // 只在有更新时才广播
    if (hasNewMessage) {
        broadcastChatHistory();
        hasNewMessage = false;
    }
    
    if (hasUserStatusChanged) {
        broadcastOnlineUsers();
        hasUserStatusChanged = false;
    }
}

void Server::broadcastChatHistory()
{
    QFile file(CHAT_HISTORY_FILE);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }

    QJsonObject data;
    QJsonArray historyArray;

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        historyArray.append(line);
    }
    file.close();

    data["history"] = historyArray;
    QByteArray message = packMessage("chat_history", data);

    for (QTcpSocket* socket : clientSockets) {
        socket->write(message);
    }
}

void Server::broadcastOnlineUsers()
{
    QJsonObject data;
    QJsonArray usersArray;

    for (const QString& username : clientUsernames.values()) {
        usersArray.append(username);
    }

    data["users"] = usersArray;
    QByteArray message = packMessage("online_users", data);

    for (QTcpSocket* socket : clientSockets) {
        socket->write(message);
    }
}

QByteArray Server::packMessage(const QString& type, const QJsonObject& data)
{
    QJsonObject message;
    message["type"] = type;
    message["data"] = data;

    QJsonDocument doc(message);
    return doc.toJson(QJsonDocument::Compact) + "\n";
}

void Server::initializeDataFiles()
{
    QString appDir = QCoreApplication::applicationDirPath();
    QDir dir(appDir);
    dir.cdUp();
    QString dataPath = dir.absolutePath() + "/data";
    
    // 确保 data 目录存在
    QDir dataDir(dataPath);
    if (!dataDir.exists()) {
        dataDir.mkpath(".");
    }

    // 确保所有必要的文件存在
    QStringList files;
    files << USER_INFO_FILE << HISTORY_FILE << CHAT_HISTORY_FILE;
    
    for (const QString& file : files) {
        QString filePath = getDataFilePath(file);
        QFile f(filePath);
        if (!f.exists()) {
            f.open(QIODevice::WriteOnly);
            f.close();
        }
    }
}

QString Server::getDataFilePath(const QString &fileName) const
{
    QString appDir = QCoreApplication::applicationDirPath();
    QDir dir(appDir);
    dir.cdUp();
    QString dataPath = dir.absolutePath() + "/data";
    return QDir::cleanPath(dataPath + "/" + fileName);
}

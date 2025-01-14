#include "networkmanager.h"
#include <QtNetwork/QNetworkInformation>
#include <QtCore/QDebug>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>

NetworkManager::NetworkManager(QObject *parent) : QObject(parent) {
    socket = new QTcpSocket(this);
    onlineUsersTimer = new QTimer(this);
    
    connect(socket, &QTcpSocket::connected, this, &NetworkManager::connected);
    connect(socket, &QTcpSocket::disconnected, this, &NetworkManager::disconnected);
    connect(socket, &QTcpSocket::readyRead, this, &NetworkManager::handleReadyRead);
    connect(socket, &QTcpSocket::errorOccurred, this, &NetworkManager::handleSocketError);
    
    connect(onlineUsersTimer, &QTimer::timeout, this, &NetworkManager::updateOnlineUsers);
    
    setupReconnection();
    initNetwork();
}

void NetworkManager::connectToServer(const QString &host, quint16 port) {
    qDebug() << "正在连接到服务器..." << host << ":" << port;
    socket->connectToHost(host, port);
}

void NetworkManager::disconnectFromServer() {
    socket->disconnectFromHost();
    onlineUsersTimer->stop();
}

void NetworkManager::sendMessage(const QString &message) {
    if (!socket) {
        qDebug() << "发送失败: socket为空";
        emit messageSent(false);
        return;
    }
    
    if (socket->state() == QAbstractSocket::ConnectedState) {
        QString formattedMessage;
        if (message.startsWith("CHAT ")) {
            // 从 "CHAT username: message" 格式中提取实际消息
            int colonPos = message.indexOf(": ", 5);  // 跳过 "CHAT "
            if (colonPos != -1) {
                QString username = message.mid(5, colonPos - 5);
                QString content = message.mid(colonPos + 2);
                formattedMessage = QString("CHAT %1: %2\n").arg(username, content);
            }
        } else {
            // 对于系统消息和其他类型的消息
            formattedMessage = message + "\n";
        }
        
        QByteArray data = formattedMessage.toUtf8();
        qint64 bytesWritten = 0;
        qint64 totalBytes = data.size();
        
        // 循环写入，确保所有数据都被发送
        while (bytesWritten < totalBytes) {
            qint64 written = socket->write(data.mid(bytesWritten));
            if (written == -1) {
                qDebug() << "写入错误:" << socket->errorString();
                emit messageSent(false);
                return;
            }
            bytesWritten += written;
        }
        
        if (socket->flush()) {
            qDebug() << "消息发送成功，总字节数:" << totalBytes;
            emit messageSent(true);
        } else {
            qDebug() << "消息刷新失败";
            emit messageSent(false);
        }
    } else {
        qDebug() << "发送失败: 未连接到服务器, 当前状态:" << socket->state();
        emit messageSent(false);
    }
}

void NetworkManager::requestOnlineUsers() {
    sendMessage("GET_ONLINE_USERS");
}

void NetworkManager::requestChatHistory() {
    sendMessage("GET_CHAT_HISTORY");
}

void NetworkManager::handleReadyRead() {
    qDebug() << "\n=== 开始处理服务器数据 ===";
    while (socket->canReadLine()) {
        QByteArray data = socket->readLine();
        QString message = QString::fromUtf8(data).trimmed();
        qDebug() << "原始消息:" << message;
        processMessage(message);
    }
}

void NetworkManager::processMessage(const QString &message) {
    qDebug() << "\n=== 开始处理消息 ===";
    qDebug() << "消息内容:" << message;
    
    // 尝试解析 JSON
    QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8());
    if (!doc.isNull()) {
        // 消息是 JSON 格式
        QJsonObject jsonObj = doc.object();
        QString type = jsonObj["type"].toString();
        QJsonObject data = jsonObj["data"].toObject();
        
        if (type == "chat_history") {
            QJsonArray historyArray = data["history"].toArray();
            QString history;
            for (const QJsonValue &value : historyArray) {
                QString line = value.toString();
                if (!line.isEmpty()) {
                    history += line + "\n";
                }
            }
            qDebug() << "发送聊天历史到界面:" << history;
            emit chatHistoryReceived(history);
        }
        else if (type == "online_users") {
            QJsonArray users = data["users"].toArray();
            emit onlineUsersReceived(users);
        }
        return;
    }
    
    // 处理普通文本消息
    if (message.startsWith("ONLINE_USERS ")) {
        QString userList = message.mid(13);
        QJsonDocument doc = QJsonDocument::fromJson(userList.toUtf8());
        if (!doc.isNull() && doc.isArray()) {
            emit onlineUsersReceived(doc.array());
        }
    }
    else if (message.startsWith("CHAT_HISTORY")) {
        QString history = message.mid(12);
        emit chatHistoryReceived(history);
    }
    else if (message.startsWith("CHAT ")) {
        QString chatMessage = message.mid(5);
        emit newMessageReceived(chatMessage);
    }
    else if (message.startsWith("[SYSTEM]")) {
        emit systemMessageReceived(message);
    }
    
    qDebug() << "=== 消息处理完成 ===\n";
}

void NetworkManager::handleSocketError(QAbstractSocket::SocketError error) {
    QString errorMessage = socket->errorString();
    qDebug() << "Socket错误:" << errorMessage << "错误类型:" << error;
    emit connectionError(errorMessage);
}

void NetworkManager::updateOnlineUsers() {
    requestOnlineUsers();
}

void NetworkManager::setupReconnection() {
    reconnectTimer = new QTimer(this);
    connect(reconnectTimer, &QTimer::timeout, this, [this]() {
        if (reconnectAttempts < MAX_RECONNECT_ATTEMPTS) {
            qDebug() << "尝试重新连接..." << reconnectAttempts + 1;
            socket->connectToHost(socket->peerName(), socket->peerPort());
            reconnectAttempts++;
        } else {
            reconnectTimer->stop();
            emit connectionError("重连失败，请检查网络连接");
        }
    });
    
    connect(socket, &QTcpSocket::connected, this, [this]() {
        reconnectTimer->stop();
        reconnectAttempts = 0;
        requestChatHistory(); // 重连成功后重新获取聊天历史
    });
    
    connect(socket, &QTcpSocket::disconnected, this, [this]() {
        reconnectTimer->start(3000); // 3秒后尝试重连
    });
}

void NetworkManager::startUpdateTimer() {
    onlineUsersTimer->start(UPDATE_INTERVAL);
}

void NetworkManager::stopUpdateTimer() {
    onlineUsersTimer->stop();
}

bool NetworkManager::waitForConnected(int msecs)
{
    if (socket->state() == QAbstractSocket::ConnectedState) {
        return true;
    }
    return socket->waitForConnected(msecs);
}

void NetworkManager::setSocket(QTcpSocket *existingSocket) {
    if (socket && socket != existingSocket) {
        // 断开旧的连接信号
        disconnect(socket, nullptr, this, nullptr);
        socket->deleteLater();
    }
    
    socket = existingSocket;
    if (socket) {
        socket->setParent(this);
        
        // 重新连接信号
        connect(socket, &QTcpSocket::connected, this, &NetworkManager::connected);
        connect(socket, &QTcpSocket::disconnected, this, &NetworkManager::disconnected);
        connect(socket, &QTcpSocket::readyRead, this, &NetworkManager::handleReadyRead);
        connect(socket, &QTcpSocket::errorOccurred, this, &NetworkManager::handleSocketError);
        
        // 如果socket已经连接，发送connected信号
        if (socket->state() == QAbstractSocket::ConnectedState) {
            emit connected();
        }
    }
}

void NetworkManager::initNetwork()
{
#ifdef Q_OS_ANDROID
    if (QNetworkInformation::instance()) {
        // 监听网络状态变化
        connect(QNetworkInformation::instance(), &QNetworkInformation::reachabilityChanged,
                this, [this](QNetworkInformation::Reachability newReachability) {
            if (newReachability == QNetworkInformation::Reachability::Online) {
                // 重新连接服务器
                if (socket && socket->state() != QAbstractSocket::ConnectedState) {
                    socket->connectToHost(socket->peerName(), socket->peerPort());
                }
            } else {
                emit networkError(tr("网络连接已断开"));
            }
        });
    }
#endif
}
 
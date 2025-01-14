#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QObject>
#include <QTcpSocket>
#include <QTimer>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

class NetworkManager : public QObject {
    Q_OBJECT
    
public:
    explicit NetworkManager(QObject *parent = nullptr);
    
    void connectToServer(const QString &host, quint16 port);
    void disconnectFromServer();
    void sendMessage(const QString &message);
    void requestOnlineUsers();
    void requestChatHistory();
    
    void startUpdateTimer();
    void stopUpdateTimer();
    
    bool waitForConnected(int msecs = 3000);
    
    void setSocket(QTcpSocket *existingSocket);
    void initNetwork();
    
signals:
    void connected();
    void disconnected();
    void connectionError(const QString &error);
    void onlineUsersReceived(const QJsonArray& users);
    void chatHistoryReceived(const QString& history);
    void newMessageReceived(const QString &message);
    void systemMessageReceived(const QString &message);
    void messageSent(bool success);
    void messageReceived(const QString& message);
    void userListUpdated(const QJsonArray& users);
    
private slots:
    void handleReadyRead();
    void handleSocketError(QAbstractSocket::SocketError error);
    void updateOnlineUsers();
    
private:
    void processMessage(const QString &message);
    void setupReconnection();
    
    QTcpSocket *socket;
    QTimer *onlineUsersTimer;
    QTimer *reconnectTimer;
    int reconnectAttempts = 0;
    static const int UPDATE_INTERVAL = 10000; // 10秒更新一次在线用户
    static const int MAX_RECONNECT_ATTEMPTS = 5;
};

#endif // NETWORKMANAGER_H 
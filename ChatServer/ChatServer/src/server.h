#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QMap>
#include <QDateTime>
#include <QObject>
#include <QTimer>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QSet>
#include <QDir>
#include <QFile>
#include <QCoreApplication>

/**
 * @brief 聊天服务器类
 * 
 * 负责处理客户端连接、消息转发和用户管理
 */
class Server : public QTcpServer
{
	Q_OBJECT
	
public:
	explicit Server(QObject *parent = nullptr);
	virtual ~Server();
	
	/**
	 * @brief 获取当前在线用户数量
	 * @return 在线用户数
	 */
	int getClientNums() const;
	
	/**
	 * @brief 初始化广播
	 */
	void initBroadcast();

signals:
	/**
	 * @brief 在线用户数量变化信号
	 * @param count 当前在线用户数
	 */
	void clientCountChanged(int count);
	void newConnectionLogged();

protected:
	/**
	 * @brief 处理新的客户端连接
	 * @param socketDescriptor 套接字描述符
	 */
	void incomingConnection(qintptr socketDescriptor) override;

private slots:
	/**
	 * @brief 处理客户端断开连接
	 */
	void handleClientDisconnected();
	
	/**
	 * @brief 处理客户端消息
	 * 支持的消息类型：
	 * - LOGIN username password
	 * - REGISTER username password
	 * - CHAT message
	 */
	void processClientMessage();
	
	/**
	 * @brief 广播更新
	 */
	void broadcastUpdate();

private:
	/**
	 * @brief 成员变量
	 */
	bool hasNewMessage;                         ///< 是否有新消息标志
	bool hasUserStatusChanged;                  ///< 是否有用户状态改变标志
	QMap<QTcpSocket*, QString> clientUsernames; ///< 存储客户端套接字和用户名的映射
	QList<QTcpSocket*> clientSockets;          ///< 存储所有已连接的客户端套接字
	QTimer* broadcastTimer;                    ///< 定时广播计时器
	QVector<QString> chatHistory;              ///< 聊天历史记录
	QSet<QString> onlineUsers;                 ///< 在线用户集合
	
	/**
	 * @brief 成员函数
	 */
	void handleLoginRequest(QTcpSocket *socket, const QString &data);
	void handleRegisterRequest(QTcpSocket *socket, const QString &data);
	void handleChatMessage(QTcpSocket *socket, const QString &data);
	void broadcastMessage(const QString &message, QTcpSocket *sender = nullptr);
	bool validateUser(const QString &username, const QString &password);
	void logConnection(const QString &ip);
	void logChat(const QString &username, const QString &message);
	bool isUserOnline(const QString &username);
	void sendOnlineUsers(QTcpSocket *socket);
	void sendChatHistory(QTcpSocket *socket);
	void broadcastChatHistory();
	void broadcastOnlineUsers();
	QByteArray packMessage(const QString& type, const QJsonObject& data);
	void initializeDataFiles();
	
	/**
	 * @brief 常量
	 */
	static const QString USER_INFO_FILE;
	static const QString HISTORY_FILE;
	static const QString CHAT_HISTORY_FILE;
	
	/**
	 * @brief 获取数据文件路径
	 */
	QString getDataFilePath(const QString &fileName) const;
};

#endif
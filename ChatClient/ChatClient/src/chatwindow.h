#pragma once

#include <QtWidgets/QMainWindow>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonDocument>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QListWidget>
#include <QtCore/QEvent>
#include <QtGui/QResizeEvent>
#include "networkmanager.h"
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtCore/QTimer>

class ChatWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit ChatWindow(NetworkManager *netManager, const QString &username, QWidget *parent = nullptr);
    
protected:
    void resizeEvent(QResizeEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    void setupUI();
    void setupConnections();
    void setupMobileUI();
    bool isMobileDevice();
    
    NetworkManager *networkManager;
    QString currentUsername;
    QTextEdit *chatDisplay;
    QLineEdit *messageInput;
    QPushButton *sendButton;
    QListWidget *userListWidget;
    bool isCurrentUser(const QString& username) const;

private slots:
    void sendMessage();
    void displayMessage(const QString &message);
    void displayMessage(const QString& username, const QString& message, const QString& timestamp);
    void displayNewMessage(const QString &message);
    void displaySystemMessage(const QString &message);
    void displayChatHistory(const QString &history);
    void updateOnlineUsers(const QJsonArray &users);
    void updateChatHistory(const QJsonArray &history);
}; 
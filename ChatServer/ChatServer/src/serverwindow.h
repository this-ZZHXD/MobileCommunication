#ifndef SERVERWINDOW_H
#define SERVERWINDOW_H

#include <QWidget>
#include "Server.h"
#include "connectionhistory.h"

class ServerWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ServerWindow(QWidget *parent = nullptr);

private slots:
    void updateOnlineUsers(int count);

private:
    void setupUI();
    void setupConnections();

    Server *server;
    ConnectionHistory *connectionHistory;
};

#endif // SERVERWINDOW_H 
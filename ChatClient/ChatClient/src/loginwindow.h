#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QDebug>
#include <QTimer>
#include <QMessageBox>
#include <QDateTime>
#include <QHostAddress>

QT_BEGIN_NAMESPACE
namespace Ui { class LoginWindow; }
QT_END_NAMESPACE

class LoginWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit LoginWindow(QWidget *parent = nullptr);
    ~LoginWindow();

private slots:
    void on_loginButton_clicked();
    void on_registerButton_clicked();
    void onConnected();
    void onReadyRead();
    void onDisconnected();

private:
    Ui::LoginWindow *ui;
    QTcpSocket *socket;
    QString serverAddress = "127.0.0.1";
    quint16 serverPort = 8080;
    void connectToServer();
    void setupUi();
};

#endif // LOGINWINDOW_H 
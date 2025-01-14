#ifndef REGISTERWINDOW_H
#define REGISTERWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>

QT_BEGIN_NAMESPACE
namespace Ui { class RegisterWindow; }
QT_END_NAMESPACE

class RegisterWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit RegisterWindow(QTcpSocket *socket, QWidget *parent = nullptr);
    virtual ~RegisterWindow();

signals:
    void registerWindowClosed();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void on_registerButton_clicked();
    void onReadyRead();

private:
    void setupUi();
    
    Ui::RegisterWindow *ui;
    QTcpSocket *socket;
    QLineEdit *usernameEdit;
    QLineEdit *passwordEdit;
    QLineEdit *confirmPasswordEdit;
    QPushButton *registerButton;
    QPushButton *backButton;
};

#endif // REGISTERWINDOW_H

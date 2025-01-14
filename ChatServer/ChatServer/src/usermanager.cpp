#include "usermanager.h"
#include <QHeaderView>
#include <QMessageBox>
#include <QLabel>
#include <QFormLayout>
#include <QCoreApplication>
#include <QDir>
#include "utils.h"

// 定义文件路径常量
const QString UserManager::USER_INFO_FILE = "userInfo.txt";

UserManager::UserManager(const QString &dataPath, QWidget *parent)
    : QWidget(parent)
    , m_dataPath(dataPath)
{
    setupUI();
    loadUsers();
}

void UserManager::setupUI()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    
    // 创建用户表格
    userTable = new QTableWidget(this);
    userTable->setColumnCount(2);
    userTable->setHorizontalHeaderLabels(QStringList() << tr("Username") << tr("Password"));
    userTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    userTable->verticalHeader()->setVisible(false);
    userTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    userTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    
    // 创建按钮布局
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    
    addButton = new QPushButton(tr("Add User"), this);
    editButton = new QPushButton(tr("Edit User"), this);
    deleteButton = new QPushButton(tr("Delete User"), this);
    
    // 设置按钮大小策略，使其在移动端更容易点击
    QSize buttonSize(120, 50);
    addButton->setMinimumSize(buttonSize);
    editButton->setMinimumSize(buttonSize);
    deleteButton->setMinimumSize(buttonSize);
    
    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(editButton);
    buttonLayout->addWidget(deleteButton);
    buttonLayout->addStretch();
    
    layout->addWidget(userTable);
    layout->addLayout(buttonLayout);
    
    // 连接信号槽
    connect(addButton, &QPushButton::clicked, this, &UserManager::addUser);
    connect(editButton, &QPushButton::clicked, this, &UserManager::editUser);
    connect(deleteButton, &QPushButton::clicked, this, &UserManager::deleteUser);
}

void UserManager::loadUsers()
{
    QString filePath = getDataFilePath(USER_INFO_FILE);
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("Error"), 
                           tr("Cannot open user information file"));
        return;
    }

    userTable->setRowCount(0);
    QTextStream in(&file);
    
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList fields = line.split(" ");
        
        if (fields.size() >= 2) {
            int row = userTable->rowCount();
            userTable->insertRow(row);
            
            // 用户名
            QTableWidgetItem *nameItem = new QTableWidgetItem(fields[0]);
            nameItem->setTextAlignment(Qt::AlignCenter);
            userTable->setItem(row, 0, nameItem);
            
            // 密码 - 直接显示明文
            QTableWidgetItem *passwordItem = new QTableWidgetItem(fields[1]);
            passwordItem->setTextAlignment(Qt::AlignCenter);
            userTable->setItem(row, 1, passwordItem);
            
            // 状态
            QTableWidgetItem *statusItem = new QTableWidgetItem(tr("Offline"));
            statusItem->setTextAlignment(Qt::AlignCenter);
            userTable->setItem(row, 2, statusItem);
        }
    }
    
    file.close();
    
    // 调整行高以适应触摸操作
    userTable->verticalHeader()->setDefaultSectionSize(50);
}

void UserManager::addUser()
{
    QDialog dialog(this);
    dialog.setWindowTitle(tr("Add New User"));
    
    QFormLayout form(&dialog);
    
    // 创建输入框
    QLineEdit *nameEdit = new QLineEdit(&dialog);
    QLineEdit *passEdit = new QLineEdit(&dialog);
    passEdit->setEchoMode(QLineEdit::Password);
    
    // 设置输入框大小适应触摸操作
    QSize inputSize(200, 40);
    nameEdit->setMinimumSize(inputSize);
    passEdit->setMinimumSize(inputSize);
    
    form.addRow(tr("Username:"), nameEdit);
    form.addRow(tr("Password:"), passEdit);
    
    // 添加确认按钮
    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                             Qt::Horizontal, &dialog);
    form.addRow(&buttonBox);
    
    // 设置按钮大小
    foreach(QAbstractButton* button, buttonBox.buttons()) {
        button->setMinimumSize(100, 40);
    }
    
    connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    
    if (dialog.exec() == QDialog::Accepted) {
        // 保存用户信息
        QString username = nameEdit->text().trimmed();
        QString password = passEdit->text();
        
        if (!username.isEmpty() && !password.isEmpty()) {
            QString filePath = getDataFilePath(USER_INFO_FILE);
            QFile file(filePath);
            if (file.open(QIODevice::Append | QIODevice::Text)) {
                QTextStream out(&file);
                out << username << " " << password << "\n";
                file.close();
                loadUsers();  // 刷新显示
            }
        }
    }
}

void UserManager::editUser()
{
    QModelIndexList selection = userTable->selectionModel()->selectedRows();
    if (selection.isEmpty()) {
        QMessageBox::warning(this, tr("Warning"), 
                           tr("Please select a user to edit"));
        return;
    }

    int row = selection.first().row();
    QString currentUsername = userTable->item(row, 0)->text();

    QDialog dialog(this);
    dialog.setWindowTitle(tr("Edit User"));
    
    QFormLayout form(&dialog);
    
    // 创建输入框
    QLineEdit *nameEdit = new QLineEdit(currentUsername, &dialog);
    QLineEdit *passEdit = new QLineEdit(&dialog);
    passEdit->setEchoMode(QLineEdit::Password);
    
    // 设置输入框大小适应触摸操作
    QSize inputSize(200, 40);
    nameEdit->setMinimumSize(inputSize);
    passEdit->setMinimumSize(inputSize);
    
    form.addRow(tr("Username:"), nameEdit);
    form.addRow(tr("New Password:"), passEdit);
    
    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                             Qt::Horizontal, &dialog);
    form.addRow(&buttonBox);
    
    // 设置按钮大小适配触摸
    foreach(QAbstractButton* button, buttonBox.buttons()) {
        button->setMinimumSize(100, 40);
    }
    
    connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    
    if (dialog.exec() == QDialog::Accepted) {
        QString newUsername = nameEdit->text().trimmed();
        QString newPassword = passEdit->text();
        
        if (!newUsername.isEmpty() && !newPassword.isEmpty()) {
            updateUserInFile(currentUsername, newUsername, newPassword);
            loadUsers();  // 刷新显示
        }
    }
}

void UserManager::deleteUser()
{
    QModelIndexList selection = userTable->selectionModel()->selectedRows();
    if (selection.isEmpty()) {
        QMessageBox::warning(this, tr("Warning"), 
                           tr("Please select a user to delete"));
        return;
    }

    QString username = userTable->item(selection.first().row(), 0)->text();
    
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, tr("Confirm Delete"),
                                tr("Are you sure you want to delete user '%1'?")
                                .arg(username),
                                QMessageBox::Yes | QMessageBox::No);
                                
    if (reply == QMessageBox::Yes) {
        deleteUserFromFile(username);
        loadUsers();  // 刷新显示
    }
}

void UserManager::updateUserInFile(const QString &oldUsername, 
                                 const QString &newUsername, 
                                 const QString &newPassword)
{
    QString filePath = getDataFilePath(USER_INFO_FILE);
    QVector<QPair<QString, QString>> users;
    
    // 读取所有用户
    QFile readFile(filePath);
    if (readFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&readFile);
        while (!in.atEnd()) {
            QString line = in.readLine();
            QStringList fields = line.split(" ");
            if (fields.size() >= 2) {
                if (fields[0] == oldUsername) {
                    users.append({newUsername, newPassword});
                } else {
                    users.append({fields[0], fields[1]});
                }
            }
        }
        readFile.close();
    }
    
    // 写入更新后的用户信息
    QFile writeFile(filePath);
    if (writeFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&writeFile);
        for (const auto &user : users) {
            out << user.first << " " << user.second << "\n";
        }
        writeFile.close();
    }
}

void UserManager::deleteUserFromFile(const QString &username)
{
    QString filePath = getDataFilePath(USER_INFO_FILE);
    QVector<QPair<QString, QString>> users;
    
    // 读取除要删除用户外的所有用户
    QFile readFile(filePath);
    if (readFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&readFile);
        while (!in.atEnd()) {
            QString line = in.readLine();
            QStringList fields = line.split(" ");
            if (fields.size() >= 2 && fields[0] != username) {
                users.append({fields[0], fields[1]});
            }
        }
        readFile.close();
    }
    
    // 写入剩余用户信息
    QFile writeFile(filePath);
    if (writeFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&writeFile);
        for (const auto &user : users) {
            out << user.first << " " << user.second << "\n";
        }
        writeFile.close();
    }
}
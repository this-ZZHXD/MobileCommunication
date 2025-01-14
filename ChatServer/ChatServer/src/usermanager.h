#ifndef USERMANAGER_H
#define USERMANAGER_H

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QDialog>
#include <QLineEdit>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QCoreApplication>
#include <QDir>

/**
 * @brief 用户管理类
 * 
 * 管理用户账户，包括：
 * - 添加用户
 * - 编辑用户
 * - 删除用户
 * - 显示用户列表
 */
class UserManager : public QWidget
{
    Q_OBJECT
    
public:
    explicit UserManager(const QString &dataPath, QWidget *parent = nullptr);

private slots:
    /**
     * @brief 添加新用户
     */
    void addUser();
    
    /**
     * @brief 编辑选中的用户
     */
    void editUser();
    
    /**
     * @brief 删除选中的用户
     */
    void deleteUser();

private:
    /**
     * @brief 设置界面布局
     */
    void setupUI();
    
    /**
     * @brief 加载用户列表
     */
    void loadUsers();
    
    /**
     * @brief 更新用户信息
     */
    void updateUserInFile(const QString &oldUsername, 
                         const QString &newUsername, 
                         const QString &newPassword);
    
    /**
     * @brief 从文件中删除用户
     */
    void deleteUserFromFile(const QString &username);
    
    QString m_dataPath;           ///< 数据存储路径
    QTableWidget *userTable;      ///< 用户列表表格
    QPushButton *addButton;       ///< 添加用户按钮
    QPushButton *editButton;      ///< 编辑用户按钮
    QPushButton *deleteButton;    ///< 删除用户按钮
    
    // 定义文件路径常量
    static const QString USER_INFO_FILE;
};

#endif 
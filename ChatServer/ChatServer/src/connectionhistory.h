#ifndef CONNECTIONHISTORY_H
#define CONNECTIONHISTORY_H

#include <QWidget>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QString>

/**
 * @brief 连接历史记录类
 * 
 * 显示和管理客户端连接历史
 */
class ConnectionHistory : public QWidget
{
    Q_OBJECT
    
public:
    explicit ConnectionHistory(QWidget *parent = nullptr);
    
    /**
     * @brief 刷新连接历史显示
     */
    void refreshHistory();
    
    /**
     * @brief 设置数据存储路径
     * @param path 数据目录路径
     */
    void setDataPath(const QString &path);

public slots:
    void onNewConnection();

private:
    /**
     * @brief 设置界面布局
     */
    void setupUI();
    
    QTableWidget *historyTable;  ///< 历史记录表格
    QString m_dataPath;          ///< 数据存储路径
    
    // 定义文件路径常量
    static const QString HISTORY_FILE;
};

#endif 
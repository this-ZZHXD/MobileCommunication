#ifndef CHATHISTORY_H
#define CHATHISTORY_H

#include <QWidget>
#include <QString>
#include <QTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

/**
 * @brief 聊天历史记录类
 * 
 * 显示和管理聊天历史记录
 */
class ChatHistory : public QWidget
{
    Q_OBJECT
public:
    explicit ChatHistory(QWidget *parent = nullptr);
    
    /**
     * @brief 设置数据存储路径
     * @param path 数据目录路径
     */
    void setDataPath(const QString &path);

private slots:
    /**
     * @brief 刷新历史记录显示
     */
    void refreshHistory();
    
    /**
     * @brief 清空历史记录
     */
    void clearHistory();

private:
    /**
     * @brief 设置界面布局
     */
    void setupUI();
    
    // 定义文件路径常量
    static const QString CHAT_HISTORY_FILE;
    
    QString m_dataPath;           ///< 数据存储路径
    QTextEdit *historyView;       ///< 历史记录显示区域
    QPushButton *refreshButton;   ///< 刷新按钮
    QPushButton *clearButton;     ///< 清空按钮
};

#endif // CHATHISTORY_H 
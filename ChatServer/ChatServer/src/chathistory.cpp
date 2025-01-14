/**
 * @brief 聊天历史记录实现文件
 * 
 * 实现聊天历史的显示和管理功能
 */

#include "chathistory.h"
#include "utils.h"  // 添加 utils.h 的包含
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QScrollBar>
#include <QDateTime>
#include <QFontMetrics>

// 定义文件路径常量
const QString ChatHistory::CHAT_HISTORY_FILE = "chatHistory.txt";

/**
 * @brief 构造函数，初始化界面并刷新历史记录
 */
ChatHistory::ChatHistory(QWidget *parent) : QWidget(parent)
{
    setupUI();
    refreshHistory();
}

/**
 * @brief 设置界面布局
 * 
 * 创建：
 * - 历史记录显示区域
 * - 刷新按钮
 * - 清空按钮
 */
void ChatHistory::setupUI()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    
    // 创建聊天历史显示区域
    historyView = new QTextEdit(this);
    historyView->setReadOnly(true);
    
    // 设置字体大小确保在移动端可读性
    QFont font = historyView->font();
    font.setPointSize(12);
    historyView->setFont(font);
    
    // 创建按钮布局
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    
    refreshButton = new QPushButton(tr("Refresh"), this);
    clearButton = new QPushButton(tr("Clear History"), this);
    
    // 设置按钮大小
    QSize buttonSize(120, 50);
    refreshButton->setMinimumSize(buttonSize);
    clearButton->setMinimumSize(buttonSize);
    
    buttonLayout->addWidget(refreshButton);
    buttonLayout->addWidget(clearButton);
    buttonLayout->addStretch();
    
    layout->addWidget(historyView);
    layout->addLayout(buttonLayout);
    
    // 连接信号槽
    connect(refreshButton, &QPushButton::clicked, this, &ChatHistory::refreshHistory);
    connect(clearButton, &QPushButton::clicked, this, &ChatHistory::clearHistory);
}

/**
 * @brief 刷新历史记录显示
 * 
 * 从文件读取并显示聊天历史：
 * 格式：[时间戳] 用户名: 消息内容
 */
void ChatHistory::refreshHistory()
{
    QString filePath = getDataFilePath(CHAT_HISTORY_FILE);
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("Error"), tr("Cannot open chat history file"));
        return;
    }

    historyView->clear();
    QTextStream in(&file);
    
    while (!in.atEnd()) {
        QString line = in.readLine();
        // 解析聊天记录格式 [时间] 用户名: 消息
        if (line.contains("] ")) {
            QStringList parts = line.split("] ");
            QString timestamp = parts[0] + "]";
            QString content = parts[1];
            
            // 将消息内容转换为密文显示
            QString encryptedContent;
            if (content.contains(":")) {
                QStringList contentParts = content.split(":");
                QString username = contentParts[0];
                QString message = contentParts[1];
                
                // 将消息内容替换为 * 号
                QString maskedMessage = QString(message.length(), '*');
                encryptedContent = username + ":" + maskedMessage;
            } else {
                encryptedContent = QString(content.length(), '*');
            }
            
            // 使用HTML格式美化显示
            QString formattedText = QString("<p><span style='color: gray;'>%1</span> %2</p>")
                                    .arg(timestamp)
                                    .arg(encryptedContent);
            historyView->append(formattedText);
        }
    }
    
    // 滚动到底部
    historyView->verticalScrollBar()->setValue(
        historyView->verticalScrollBar()->maximum()
    );
    
    file.close();
}

void ChatHistory::clearHistory()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, tr("Confirm"),
                                tr("Are you sure you want to clear chat history?"),
                                QMessageBox::Yes | QMessageBox::No);
                                
    if (reply == QMessageBox::Yes) {
        QString filePath = getDataFilePath(CHAT_HISTORY_FILE);
        QFile file(filePath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            file.close();
            refreshHistory();
        } else {
            QMessageBox::warning(this, tr("Error"), 
                               tr("Failed to clear chat history"));
        }
    }
}

void ChatHistory::setDataPath(const QString &path)
{
    m_dataPath = path;
    // 在这里可以添加数据路径设置后的初始化逻辑
} 
/**
 * @brief 连接历史记录实现文件
 * 
 * 实现连接历史的显示和管理功能
 */

#include "connectionhistory.h"
#include "utils.h"
#include <QHeaderView>
#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QCoreApplication>

// 定义文件路径常量
const QString ConnectionHistory::HISTORY_FILE = "history.txt";

/**
 * @brief 构造函数，初始化界面并刷新历史记录
 */
ConnectionHistory::ConnectionHistory(QWidget *parent) : QWidget(parent)
{
    setupUI();
    refreshHistory();
}

/**
 * @brief 设置界面布局
 * 
 * 创建表格并设置其属性：
 * - IP地址列
 * - 用户名列
 * - 连接时间列
 * - 状态列
 */
void ConnectionHistory::setupUI()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    
    historyTable = new QTableWidget(this);
    historyTable->setColumnCount(4);
    historyTable->setHorizontalHeaderLabels(QStringList() 
        << tr("IP Address") 
        << tr("Username") 
        << tr("Connection Time") 
        << tr("Status"));
    
    // 设置表格属性
    historyTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    historyTable->verticalHeader()->setVisible(false);
    historyTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    
    // 设置表格字体大小，确保在移动端可读性
    QFont font = historyTable->font();
    font.setPointSize(12);
    historyTable->setFont(font);
    
    // 设置表格大小策略
    historyTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    
    // 设置表格样式
    historyTable->setStyleSheet(
        "QTableWidget {"
        "   background-color: #2b2b2b;"
        "   color: white;"
        "   gridline-color: #3d3d3d;"
        "}"
        "QHeaderView::section {"
        "   background-color: #3d3d3d;"
        "   color: white;"
        "   padding: 5px;"
        "   border: none;"
        "}"
        "QTableWidget::item {"
        "   padding: 5px;"
        "}"
    );
    
    // 设置表格选择模式
    historyTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    historyTable->setSelectionMode(QAbstractItemView::SingleSelection);
    
    // 设置表格大小
    historyTable->setMinimumHeight(300);
    
    layout->addWidget(historyTable);
    setLayout(layout);  // 确保设置布局
}

/**
 * @brief 刷新历史记录显示
 * 
 * 从文件读取并显示连接历史：
 * 格式：IP地址 用户名 连接时间 状态
 */
void ConnectionHistory::refreshHistory()
{
    qDebug() << "Refreshing history - Start";
    
    // 使用 utils.h 中的函数获取正确的文件路径
    QString filePath = getDataFilePath(HISTORY_FILE);
    
    qDebug() << "Opening file:" << filePath;
    QFile file(filePath);
    
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Error opening file:" << file.errorString();
        QMessageBox::warning(this, tr("Error"), 
                           tr("Cannot open connection history file"));
        return;
    }

    historyTable->setRowCount(0);
    QTextStream in(&file);
    int rowCount = 0;
    
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList fields = line.split(" ");
        qDebug() << "Processing line:" << line;
        qDebug() << "Fields count:" << fields.size();
        
        if (fields.size() >= 4) {
            int row = historyTable->rowCount();
            historyTable->insertRow(row);
            rowCount++;
            
            // IP地址
            QTableWidgetItem *ipItem = new QTableWidgetItem(fields[0]);
            ipItem->setTextAlignment(Qt::AlignCenter);
            historyTable->setItem(row, 0, ipItem);
            
            // 用户名
            QTableWidgetItem *usernameItem = new QTableWidgetItem(fields[1]);
            usernameItem->setTextAlignment(Qt::AlignCenter);
            historyTable->setItem(row, 1, usernameItem);
            
            // 连接时间
            QString timestamp = fields[2] + " " + fields[3];
            QTableWidgetItem *timeItem = new QTableWidgetItem(timestamp);
            timeItem->setTextAlignment(Qt::AlignCenter);
            historyTable->setItem(row, 2, timeItem);
            
            // 状态
            QTableWidgetItem *statusItem = new QTableWidgetItem(fields[4]);
            statusItem->setTextAlignment(Qt::AlignCenter);
            historyTable->setItem(row, 3, statusItem);
        }
    }
    
    file.close();
    qDebug() << "Refresh completed - Added" << rowCount << "rows";
    
    // 强制更新表格显示
    historyTable->viewport()->update();
}

void ConnectionHistory::setDataPath(const QString &path)
{
    m_dataPath = path;
    // 在这里可以添加数据路径设置后的初始化逻辑
}

/**
 * @brief 处理新连接事件
 * 
 * 当收到新连接信号时刷新历史记录显示
 */
void ConnectionHistory::onNewConnection()
{
    qDebug() << "onNewConnection called - Starting refresh...";
    refreshHistory();
    qDebug() << "onNewConnection completed - History refreshed";
} 
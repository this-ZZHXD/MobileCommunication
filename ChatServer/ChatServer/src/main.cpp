/**
 * @brief 主程序入口
 * 
 * 初始化应用程序并启动主窗口
 */
#include <QApplication>
#include <QStyleFactory>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    // 设置应用程序样式为 Fusion
    a.setStyle(QStyleFactory::create("Fusion"));
    
    // 创建并显示主窗口
    MainWindow w;
    
    // 根据平台设置窗口属性
    #ifdef Q_OS_ANDROID
        w.showMaximized();  // Android 平台最大化显示
    #else
        w.show();          // 其他平台正常显示
    #endif
    
    return a.exec();
} 
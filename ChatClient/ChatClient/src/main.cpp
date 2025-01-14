#include <QApplication>
#include <QScreen>
#include <QStyleFactory>
#include "loginwindow.h"

void setupStyle(QApplication &app) {
    // 设置应用样式表
    app.setStyle(QStyleFactory::create("Fusion"));
    
    // 定义全局样式
    QString style = R"(
        QMainWindow {
            background-color: #f5f5f5;
        }
        QPushButton {
            background-color: #2196F3;
            color: white;
            border: none;
            border-radius: 4px;
            padding: 8px 16px;
        }
        QPushButton:hover {
            background-color: #1976D2;
        }
        QLineEdit {
            padding: 8px;
            border: 1px solid #ddd;
            border-radius: 4px;
        }
        QTextEdit {
            border: 1px solid #ddd;
            border-radius: 4px;
        }
        QListWidget {
            border: 1px solid #ddd;
            border-radius: 4px;
        }
    )";
    
    app.setStyleSheet(style);
}

void setupScreenScaling(QApplication &app) {
    // 获取主屏幕
    QScreen *screen = app.primaryScreen();
    qreal dpi = screen->logicalDotsPerInch();
    
    // 根据DPI设置缩放因子
    if (dpi > 96) {  // 96 DPI是标准DPI
        qreal scaleFactor = dpi / 96;
        qputenv("QT_SCALE_FACTOR", QString::number(scaleFactor).toLocal8Bit());
    }
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    // 设置高DPI缩放
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    
    // 设置样式和屏幕适配
    setupStyle(a);
    setupScreenScaling(a);
    
    LoginWindow w;
    w.show();
    
    return a.exec();
} 
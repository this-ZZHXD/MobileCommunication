#pragma once
#include <QString>
#include <QCoreApplication>
#include <QDir>
#include <QDebug>

inline QString getDataFilePath(const QString& fileName) {
    // 获取应用程序目录
    QString appDir = QCoreApplication::applicationDirPath();
    qDebug() << "App directory:" << appDir;
    
    // 获取构建目录（去掉最后一级目录）
    QDir dir(appDir);
    QString buildDir = dir.absolutePath();
    qDebug() << "Build directory:" << buildDir;
    
    // 构建数据目录路径
    QString dataPath = buildDir + "/data";
    qDebug() << "Data directory:" << dataPath;
    
    // 确保数据目录存在
    QDir dataDir(dataPath);
    if (!dataDir.exists()) {
        dataDir.mkpath(".");
        qDebug() << "Created data directory";
    }
    
    // 构建完整文件路径
    QString filePath = dataPath + "/" + fileName;
    qDebug() << "Full file path:" << filePath;
    
    return QDir::cleanPath(filePath);
} 
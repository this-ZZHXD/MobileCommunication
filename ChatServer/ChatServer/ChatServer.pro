# 基本配置
TARGET = ChatServer
TEMPLATE = app
CONFIG += c++17

# 移除所有 QML 相关定义和配置
CONFIG -= qml_debug qtquickcompiler qml quick
DEFINES += QT_NO_QML QT_NO_QUICK

# 基本 Qt 模块
QT = core gui network sql widgets

android {
    # Android 特定配置
    ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
    
    contains(ANDROID_TARGET_ARCH, x86_64) {
        # 指定目标架构
        ANDROID_ABIS = x86_64
        
        # 指定输出目录和库名称
        DESTDIR = $$OUT_PWD/android-build/libs/$$ANDROID_TARGET_ARCH
        TARGET = ChatServer-x86-64
        
        # 指定库文件路径
        ANDROID_EXTRA_LIBS += \
            $$PWD/android-build/libs/x86_64/libssl.so \
            $$PWD/android-build/libs/x86_64/libcrypto.so
    }
    
    # Android 版本设置
    ANDROID_MIN_SDK_VERSION = 23
    ANDROID_TARGET_SDK_VERSION = 33
    
    # 权限配置
    ANDROID_PERMISSIONS += \
        android.permission.INTERNET \
        android.permission.ACCESS_NETWORK_STATE
    
    # 禁用 QML 相关功能
    CONFIG += no_import_scan
    CONFIG += no_import_qml
    CONFIG += force_no_qml
    ANDROID_FEATURES -= android-qml
    ANDROID_EXTRA_PLUGINS -= qml
    
    # 使用旧版打包方式
    ANDROID_USE_LEGACY_PACKAGING = true
    
    # 部署设置
    target.path = /libs/$$ANDROID_TARGET_ARCH
    INSTALLS += target
    
    # 添加调试信息
    message("Build directory: $$OUT_PWD")
    message("Target architecture: $$ANDROID_TARGET_ARCH")
    message("Target name: $$TARGET")
    message("Destination directory: $$DESTDIR")
    
    # 创建assets目录并复制数据文件
    system(mkdir -p $$OUT_PWD/android-build/assets/data)
    data_files.files = $$PWD/data/*
    data_files.path = /assets/data
    INSTALLS += data_files
    
    # 复制数据文件到构建目录
    data_files_copy.commands = $(COPY_DIR) $$PWD/data $$OUT_PWD/android-build/assets
    PRE_TARGETDEPS += data_files_copy
    QMAKE_EXTRA_TARGETS += data_files_copy
} else:win32 {
    CONFIG(debug, debug|release) {
        # Windows 下的文件复制
        DESTDIR = $$OUT_PWD
        
        # 创建数据目录并复制文件
        build_data_dir = $$DESTDIR/data
        build_data_dir ~= s,/,\\,g
         src_dir = $$PWD/data
         src_dir ~= s,/,\\,g
        
        # # 使用 system() 函数直接执行命令
        # system(if not exist $$shell_quote($$build_data_dir) mkdir $$shell_quote($$build_data_dir))
        
        # # 复制数据文件
        # system(robocopy /y /e /i $$shell_quote($$src_dir\\*.*) $$shell_quote($$build_data_dir))

# 创建目录（使用 md 而不是 mkdir，更可靠）
       system(if not exist \"$$build_data_dir\" md \"$$build_data_dir\")

       # 使用 copy 命令替代 xcopy
       system(copy /y \"$$src_dir\\*.*\" \"$$build_data_dir\\*.*\")
        
        message("Source data directory: $$src_dir")
        message("Build data directory: $$build_data_dir")
    }
}

# 源文件
SOURCES += \
    src/main.cpp \
    src/mainwindow.cpp \
    src/server.cpp \
    src/usermanager.cpp \
    src/connectionhistory.cpp \
    src/chathistory.cpp

# 头文件
HEADERS += \
    src/server.h \
    src/mainwindow.h \
    src/usermanager.h \
    src/connectionhistory.h \
    src/chathistory.h

# 界面文件
FORMS += \
    resources/mainwindow.ui

QT       += core gui network widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

# 添加安卓支持
android {
    QT += androidextras
    ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
    ANDROID_ABIS = armeabi-v7a arm64-v8a x86 x86_64
}

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/main.cpp \
    src/loginwindow.cpp \
    src/registerwindow.cpp \
    src/chatwindow.cpp \
    src/networkmanager.cpp

HEADERS += \
    src/loginwindow.h \
    src/registerwindow.h \
    src/chatwindow.h \
    src/networkmanager.h

FORMS += \
    resources/loginwindow.ui \
    resources/registerwindow.ui \
    resources/chatwindow.ui

# 添加安卓权限配置
android {
    DISTFILES += \
        android/AndroidManifest.xml \
        android/build.gradle \
        android/res/values/libs.xml
}

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target 

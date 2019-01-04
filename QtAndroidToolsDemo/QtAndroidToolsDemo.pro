QT += quick quickcontrols2
CONFIG += c++11

TARGET = QtAndroidToolsDemo
TEMPLATE = app

SOURCES += \
        Main.cpp

RESOURCES += \
    QmlFiles.qrc \
    qtquickcontrols2.conf \
    icons/tools/index.theme \
    $$files(icons/*.png, true) \
    $$files(images/*.jpg)

DISTFILES += \
    android/AndroidManifest.xml \
    android/build.gradle

android
{
ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
include(../QtAndroidTools/QtAndroidTools.pri)
}


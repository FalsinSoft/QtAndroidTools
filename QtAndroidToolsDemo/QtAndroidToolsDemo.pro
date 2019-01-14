QT += quick quickcontrols2 svg
CONFIG += c++11

TARGET = QtAndroidToolsDemo
TEMPLATE = app

SOURCES += \
        Main.cpp

RESOURCES += \
    Sources.qrc \
    qtquickcontrols2.conf \
    icons/tools/index.theme \
    $$files(icons/*.png, true) \
    $$files(images/*.jpg) \
    $$files(images/*.svg)

OTHER_FILES += \
    android/AndroidManifest.xml \
    android/build.gradle

android {
ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
include(../QtAndroidTools/QtAndroidTools.pri)
}


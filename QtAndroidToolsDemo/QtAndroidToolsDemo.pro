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
DEFINES += \
    QTAT_APP_PERMISSIONS \
    QTAT_APK_EXPANSION_FILES \
    QTAT_APK_INFO \
    QTAT_SCREEN \
    QTAT_BATTERY_STATE \
    QTAT_SIGNAL_STRENGTH \
    QTAT_IMAGES \
    QTAT_NOTIFICATION \
    QTAT_ADMOB_BANNER \
    QTAT_ADMOB_INTERSTITIAL \
    QTAT_ADMOB_REWARDED_VIDEO \
    QTAT_PLAY_STORE
include(../QtAndroidTools/QtAndroidTools.pri)
}


QT += quick quickcontrols2 svg
CONFIG += c++11

TARGET = QtAndroidToolsDemo
TEMPLATE = app

SOURCES += \
        Main.cpp

RESOURCES += \
    QtAndroidToolsDemo.qrc \
    Sources.qrc

OTHER_FILES += \
    android/AndroidManifest.xml \
    android/build.gradle \
    android/gradle.properties

android {
ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
DEFINES += \
    QTAT_APP_PERMISSIONS \
    QTAT_APK_EXPANSION_FILES \
    QTAT_APK_INFO \
    QTAT_SCREEN \
    QTAT_SYSTEM \
    QTAT_BATTERY_STATE \
    QTAT_SIGNAL_STRENGTH \
    QTAT_IMAGES \
    QTAT_NOTIFICATION \
    QTAT_ADMOB_BANNER \
    QTAT_ADMOB_INTERSTITIAL \
    QTAT_ADMOB_REWARDED_VIDEO \
    QTAT_PLAY_STORE \
    QTAT_GOOGLE_ACCOUNT \
    QTAT_GOOGLE_DRIVE \
    QTAT_SHARING \
    QTAT_USER_MESSAGING_PLATFORM
include(../QtAndroidTools/QtAndroidTools.pri)
}



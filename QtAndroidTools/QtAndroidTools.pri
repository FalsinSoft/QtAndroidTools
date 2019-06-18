
QT += androidextras

equals(QMAKE_HOST.os, Windows) {
    make_dir.commands = $(CHK_DIR_EXISTS) $$shell_path($$ANDROID_PACKAGE_SOURCE_DIR/src/com/falsinsoft/qtandroidtools) $(MKDIR) $$shell_path($$ANDROID_PACKAGE_SOURCE_DIR/src/com/falsinsoft/qtandroidtools)
} else {
    make_dir.commands = $(MKDIR) $$shell_path($$ANDROID_PACKAGE_SOURCE_DIR/src/com/falsinsoft/qtandroidtools)
}
PRE_TARGETDEPS += make_dir
QMAKE_EXTRA_TARGETS += make_dir

HEADERS += $$PWD/QtAndroidTools.h
SOURCES += $$PWD/QtAndroidTools.cpp

contains(DEFINES, QTAT_APP_PERMISSIONS) {
    HEADERS += $$PWD/QAndroidAppPermissions.h
    SOURCES += $$PWD/QAndroidAppPermissions.cpp
}
contains(DEFINES, QTAT_APK_INFO) {
    HEADERS += $$PWD/QAndroidApkInfo.h
    SOURCES += $$PWD/QAndroidApkInfo.cpp
}
contains(DEFINES, QTAT_SCREEN) {
    HEADERS += $$PWD/QAndroidScreen.h
    SOURCES += $$PWD/QAndroidScreen.cpp
}
contains(DEFINES, QTAT_APK_EXPANSION_FILES) {
    HEADERS += $$PWD/QAndroidApkExpansionFiles.h
    SOURCES += $$PWD/QAndroidApkExpansionFiles.cpp
    OTHER_FILES += $$PWD/src/com/falsinsoft/qtandroidtools/AndroidApkExpansionFiles.java
    copy_apk_expansion_files.commands = $(COPY_FILE) $$shell_path($$PWD/src/com/falsinsoft/qtandroidtools/AndroidApkExpansionFiles.java) $$shell_path($$ANDROID_PACKAGE_SOURCE_DIR/src/com/falsinsoft/qtandroidtools/)
    equals(QMAKE_HOST.os, Windows) {
        copy_google.commands = $(COPY_DIR) $$shell_path($$PWD/src/com/google) $$shell_path($$ANDROID_PACKAGE_SOURCE_DIR/src/com/google)
        copy_aidl.commands = $(COPY_DIR) $$shell_path($$PWD/aidl) $$shell_path($$ANDROID_PACKAGE_SOURCE_DIR/aidl)
    } else {
        copy_google.commands = $(COPY_DIR) $$shell_path($$PWD/src/com/google) $$shell_path($$ANDROID_PACKAGE_SOURCE_DIR/src/com)
        copy_aidl.commands = $(COPY_DIR) $$shell_path($$PWD/aidl) $$shell_path($$ANDROID_PACKAGE_SOURCE_DIR)
    }
    PRE_TARGETDEPS += copy_apk_expansion_files copy_google copy_aidl
    QMAKE_EXTRA_TARGETS += copy_apk_expansion_files copy_google copy_aidl
}
contains(DEFINES, QTAT_BATTERY_STATE) {
    HEADERS += $$PWD/QAndroidBatteryState.h
    SOURCES += $$PWD/QAndroidBatteryState.cpp
    OTHER_FILES += $$PWD/src/com/falsinsoft/qtandroidtools/AndroidBatteryState.java
    copy_battery_state.commands = $(COPY_FILE) $$shell_path($$PWD/src/com/falsinsoft/qtandroidtools/AndroidBatteryState.java) $$shell_path($$ANDROID_PACKAGE_SOURCE_DIR/src/com/falsinsoft/qtandroidtools/)
    PRE_TARGETDEPS += copy_battery_state
    QMAKE_EXTRA_TARGETS += copy_battery_state
}
contains(DEFINES, QTAT_SIGNAL_STRENGTH) {
    HEADERS += $$PWD/QAndroidSignalStrength.h
    SOURCES += $$PWD/QAndroidSignalStrength.cpp
    OTHER_FILES += $$PWD/src/com/falsinsoft/qtandroidtools/AndroidSignalStrength.java
    copy_signal_strength.commands = $(COPY_FILE) $$shell_path($$PWD/src/com/falsinsoft/qtandroidtools/AndroidSignalStrength.java) $$shell_path($$ANDROID_PACKAGE_SOURCE_DIR/src/com/falsinsoft/qtandroidtools/)
    PRE_TARGETDEPS += copy_signal_strength
    QMAKE_EXTRA_TARGETS += copy_signal_strength
}
contains(DEFINES, QTAT_ADMOB_BANNER) {
    HEADERS += $$PWD/QAndroidAdMobBanner.h
    SOURCES += $$PWD/QAndroidAdMobBanner.cpp
    OTHER_FILES += $$PWD/src/com/falsinsoft/qtandroidtools/AndroidAdMobBanner.java
    copy_admob_banner.commands = $(COPY_FILE) $$shell_path($$PWD/src/com/falsinsoft/qtandroidtools/AndroidAdMobBanner.java) $$shell_path($$ANDROID_PACKAGE_SOURCE_DIR/src/com/falsinsoft/qtandroidtools/)
    PRE_TARGETDEPS += copy_admob_banner
    QMAKE_EXTRA_TARGETS += copy_admob_banner
    !contains(QMAKE_EXTRA_TARGETS, copy_sync_run_on_ui_thread) {
        copy_sync_run_on_ui_thread.commands = $(COPY_FILE) $$shell_path($$PWD/src/com/falsinsoft/qtandroidtools/SyncRunOnUiThread.java) $$shell_path($$ANDROID_PACKAGE_SOURCE_DIR/src/com/falsinsoft/qtandroidtools/)
        PRE_TARGETDEPS += copy_sync_run_on_ui_thread
        QMAKE_EXTRA_TARGETS += copy_sync_run_on_ui_thread
    }
}
contains(DEFINES, QTAT_ADMOB_INTERSTITIAL) {
    HEADERS += $$PWD/QAndroidAdMobInterstitial.h
    SOURCES += $$PWD/QAndroidAdMobInterstitial.cpp
    OTHER_FILES += $$PWD/src/com/falsinsoft/qtandroidtools/AndroidAdMobInterstitial.java
    copy_admob_interstitial.commands = $(COPY_FILE) $$shell_path($$PWD/src/com/falsinsoft/qtandroidtools/AndroidAdMobInterstitial.java) $$shell_path($$ANDROID_PACKAGE_SOURCE_DIR/src/com/falsinsoft/qtandroidtools/)
    PRE_TARGETDEPS += copy_admob_interstitial
    QMAKE_EXTRA_TARGETS += copy_admob_interstitial
    !contains(QMAKE_EXTRA_TARGETS, copy_sync_run_on_ui_thread) {
        copy_sync_run_on_ui_thread.commands = $(COPY_FILE) $$shell_path($$PWD/src/com/falsinsoft/qtandroidtools/SyncRunOnUiThread.java) $$shell_path($$ANDROID_PACKAGE_SOURCE_DIR/src/com/falsinsoft/qtandroidtools/)
        PRE_TARGETDEPS += copy_sync_run_on_ui_thread
        QMAKE_EXTRA_TARGETS += copy_sync_run_on_ui_thread
    }
}
contains(DEFINES, QTAT_ADMOB_REWARDED_VIDEO) {
    HEADERS += $$PWD/QAndroidAdMobRewardedVideo.h
    SOURCES += $$PWD/QAndroidAdMobRewardedVideo.cpp
    OTHER_FILES += $$PWD/src/com/falsinsoft/qtandroidtools/AndroidAdMobRewardedVideo.java
    copy_admob_rewarded_video.commands = $(COPY_FILE) $$shell_path($$PWD/src/com/falsinsoft/qtandroidtools/AndroidAdMobRewardedVideo.java) $$shell_path($$ANDROID_PACKAGE_SOURCE_DIR/src/com/falsinsoft/qtandroidtools/)
    PRE_TARGETDEPS += copy_admob_rewarded_video
    QMAKE_EXTRA_TARGETS += copy_admob_rewarded_video
    !contains(QMAKE_EXTRA_TARGETS, copy_sync_run_on_ui_thread) {
        copy_sync_run_on_ui_thread.commands = $(COPY_FILE) $$shell_path($$PWD/src/com/falsinsoft/qtandroidtools/SyncRunOnUiThread.java) $$shell_path($$ANDROID_PACKAGE_SOURCE_DIR/src/com/falsinsoft/qtandroidtools/)
        PRE_TARGETDEPS += copy_sync_run_on_ui_thread
        QMAKE_EXTRA_TARGETS += copy_sync_run_on_ui_thread
    }
}
contains(DEFINES, QTAT_IMAGES) {
    HEADERS += $$PWD/QAndroidImages.h
    SOURCES += $$PWD/QAndroidImages.cpp
    OTHER_FILES += $$PWD/src/com/falsinsoft/qtandroidtools/AndroidImages.java
    copy_images.commands = $(COPY_FILE) $$shell_path($$PWD/src/com/falsinsoft/qtandroidtools/AndroidImages.java) $$shell_path($$ANDROID_PACKAGE_SOURCE_DIR/src/com/falsinsoft/qtandroidtools/)
    PRE_TARGETDEPS += copy_images
    QMAKE_EXTRA_TARGETS += copy_images
}
contains(DEFINES, QTAT_NOTIFICATION) {
    HEADERS += $$PWD/QAndroidNotification.h
    SOURCES += $$PWD/QAndroidNotification.cpp
    OTHER_FILES += $$PWD/src/com/falsinsoft/qtandroidtools/AndroidNotification.java
    copy_notification.commands = $(COPY_FILE) $$shell_path($$PWD/src/com/falsinsoft/qtandroidtools/AndroidNotification.java) $$shell_path($$ANDROID_PACKAGE_SOURCE_DIR/src/com/falsinsoft/qtandroidtools/)
    PRE_TARGETDEPS += copy_notification
    QMAKE_EXTRA_TARGETS += copy_notification
    LIBS += -ljnigraphics
}

contains(DEFINES, QTAT_PLAY_STORE) {
    HEADERS += $$PWD/QAndroidPlayStore.h
    SOURCES += $$PWD/QAndroidPlayStore.cpp
    OTHER_FILES += $$PWD/src/com/falsinsoft/qtandroidtools/AndroidPlayStore.java
    copy_play_store.commands = $(COPY_FILE) $$shell_path($$PWD/src/com/falsinsoft/qtandroidtools/AndroidPlayStore.java) $$shell_path($$ANDROID_PACKAGE_SOURCE_DIR/src/com/falsinsoft/qtandroidtools/)
    PRE_TARGETDEPS += copy_play_store
    QMAKE_EXTRA_TARGETS += copy_play_store
}
	

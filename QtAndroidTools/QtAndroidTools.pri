
QT += androidextras

HEADERS += $$PWD/QtAndroidTools.h
SOURCES += $$PWD/QtAndroidTools.cpp

HEADERS += $$PWD/QAndroidAppPermissions.h
SOURCES += $$PWD/QAndroidAppPermissions.cpp

HEADERS += $$PWD/QAndroidApkInfo.h
SOURCES += $$PWD/QAndroidApkInfo.cpp

HEADERS += $$PWD/QAndroidApkExpansionFiles.h
SOURCES += $$PWD/QAndroidApkExpansionFiles.cpp
OTHER_FILES += $$PWD/src/com/falsinsoft/qtandroidtools/ApkExpansionDownloader.java

copy_src.commands = $(CHK_DIR_EXISTS) $$shell_path($$ANDROID_PACKAGE_SOURCE_DIR/src/com/falsinsoft/qtandroidtools) $(COPY_DIR) $$shell_path($$PWD/src) $$shell_path($$ANDROID_PACKAGE_SOURCE_DIR/src)
copy_aidl.commands = $(CHK_DIR_EXISTS) $$shell_path($$ANDROID_PACKAGE_SOURCE_DIR/aidl/com/android/vending/licensing) $(COPY_DIR) $$shell_path($$PWD/aidl) $$shell_path($$ANDROID_PACKAGE_SOURCE_DIR/aidl)
PRE_TARGETDEPS += copy_src copy_aidl
QMAKE_EXTRA_TARGETS += copy_src copy_aidl

	

cmake_minimum_required(VERSION 3.14)

project(QtAndroidToolsDemo LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#find_package(QT NAMES Qt6 Qt5 COMPONENTS Gui Core Quick QuickControls2 Svg REQUIRED)
find_package(Qt6 COMPONENTS Gui Core Quick QuickControls2 Svg REQUIRED)

if(ANDROID)
    set(ANDROID_PACKAGE_SOURCE_DIR ${PROJECT_SOURCE_DIR}/android CACHE INTERNAL "")
endif()

if(ANDROID)
    set(QTAT_APP_PERMISSIONS OFF)
    set(QTAT_APK_EXPANSION_FILES OFF)
    set(QTAT_APK_INFO OFF)
    set(QTAT_SCREEN OFF)
    set(QTAT_SYSTEM ON)
    set(QTAT_BATTERY_STATE ON)
    set(QTAT_SIGNAL_STRENGTH OFF)
    set(QTAT_IMAGES ON)
    set(QTAT_NOTIFICATION ON)
    set(QTAT_ADMOB_BANNER OFF)
    set(QTAT_ADMOB_INTERSTITIAL OFF)
    set(QTAT_ADMOB_REWARDED_VIDEO OFF)
    set(QTAT_PLAY_STORE OFF)
    set(QTAT_GOOGLE_ACCOUNT OFF)
    set(QTAT_GOOGLE_DRIVE OFF)
    set(QTAT_SHARING ON)
    set(QTAT_USER_MESSAGING_PLATFORM OFF)
        set(QTAT_AUDIO OFF)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../QtAndroidTools build)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../QtAndroidTools)
endif()

set(PROJECT_SOURCES
        Main.cpp
        Sources.qrc
        QtAndroidToolsDemo.qrc
        android/AndroidManifest.xml
        android/build.gradle
        android/gradle.properties
)

qt_add_executable(QtAndroidToolsDemo
    ${PROJECT_SOURCES}
)
#if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)

#else()
#    if(ANDROID)
#        add_library(QtAndroidToolsDemo SHARED
#            ${PROJECT_SOURCES}
#        )
#    else()
#        add_executable(QtAndroidToolsDemo
#          ${PROJECT_SOURCES}
#        )
#    endif()
#endif()
set_property(TARGET QtAndroidToolsDemo PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR "${ANDROID_PACKAGE_SOURCE_DIR}")

#target_compile_definitions(QtAndroidToolsDemo
#  PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
target_link_libraries(QtAndroidToolsDemo
   PRIVATE Qt6::Gui Qt6::Core Qt6::Quick Qt6::QuickControls2 Qt6::Qml QtAndroidTools)
#if(ANDROID)
##    target_include_directories(QtAndroidToolsDemo
##        PRIVATE ${Qt5AndroidExtras_INCLUDE_DIRS})
#    target_link_libraries(QtAndroidToolsDemo
#        PRIVATE Qt${QT_VERSION_MAJOR}::Gui
#        QtAndroidTools)
#endif()



cmake_minimum_required(VERSION 3.14)

project(QtAndroidToolsDemo LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(ANDROID)
    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
endif()

find_package(Qt6 COMPONENTS Core Quick QuickControls2 Svg REQUIRED)
if(ANDROID)
    set(QTAT_APP_PERMISSIONS ON)
    set(QTAT_APK_EXPANSION_FILES ON)
    set(QTAT_APK_INFO ON)
    set(QTAT_SCREEN ON)
    set(QTAT_SYSTEM ON)
    set(QTAT_BATTERY_STATE ON)
    set(QTAT_SIGNAL_STRENGTH ON)
    set(QTAT_IMAGES ON)
    set(QTAT_NOTIFICATION ON)
    set(QTAT_ADMOB_BANNER ON)
    set(QTAT_ADMOB_INTERSTITIAL ON)
    set(QTAT_ADMOB_REWARDED_VIDEO ON)
    set(QTAT_PLAY_STORE ON)
    set(QTAT_GOOGLE_ACCOUNT ON)
    set(QTAT_GOOGLE_DRIVE ON)
    set(QTAT_SHARING ON)
    set(QTAT_USER_MESSAGING_PLATFORM ON)
    set(QTAT_AUDIO ON)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../QtAndroidTools build)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../QtAndroidTools)
endif()

set(PROJECT_SOURCES
    Main.cpp
    Sources.qrc
    QtAndroidToolsDemo.qrc
)

qt_add_executable(QtAndroidToolsDemo ${PROJECT_SOURCES})

target_compile_definitions(QtAndroidToolsDemo
  PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
target_link_libraries(QtAndroidToolsDemo PRIVATE
    Qt6::Core
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::Svg
)
if(ANDROID)
    add_custom_target(Android SOURCES
        android/AndroidManifest.xml
        android/build.gradle
        android/gradle.properties
    )
    target_link_libraries(QtAndroidToolsDemo PRIVATE QtAndroidTools)
endif()

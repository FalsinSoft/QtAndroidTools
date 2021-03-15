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

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Quick QuickControls2 Svg REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Quick QuickControls2 Svg REQUIRED)
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
    set(QTAT_PLAY_STORE ON)
    find_package(Qt${QT_VERSION_MAJOR} COMPONENTS AndroidExtras REQUIRED)
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

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(QtAndroidToolsDemo
        ${PROJECT_SOURCES}
    )
else()
    if(ANDROID)
        add_library(QtAndroidToolsDemo SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(QtAndroidToolsDemo
          ${PROJECT_SOURCES}
        )
    endif()
endif()

target_compile_definitions(QtAndroidToolsDemo
  PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
target_link_libraries(QtAndroidToolsDemo
  PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Quick
          Qt${QT_VERSION_MAJOR}::QuickControls2 Qt${QT_VERSION_MAJOR}::Svg)
if(ANDROID)
    target_include_directories(QtAndroidToolsDemo
        PRIVATE ${Qt5AndroidExtras_INCLUDE_DIRS})
    target_link_libraries(QtAndroidToolsDemo
        PRIVATE Qt${QT_VERSION_MAJOR}::AndroidExtras
        QtAndroidTools)
endif()

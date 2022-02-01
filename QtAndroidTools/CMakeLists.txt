cmake_minimum_required(VERSION 3.14)

project(QtAndroidTools
        VERSION 1.4
        LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.2 COMPONENTS
        Core
        Quick
        Qml
        REQUIRED )

set(QTAT_JAVA_DIR src/com/falsinsoft/qtandroidtools)
set(QTAT_SOURCE_FILES QtAndroidTools.cpp)
set(QTAT_HEADER_FILES QtAndroidTools.h)
set(QTAT_JAVA_FILES ${QTAT_JAVA_DIR}/AndroidTools.java)

option(QTAT_APP_PERMISSIONS "Enable QtAndroidTools App permissions.")
if(QTAT_APP_PERMISSIONS)
	add_compile_definitions(QTAT_APP_PERMISSIONS)
	list(APPEND QTAT_SOURCE_FILES QAndroidAppPermissions.cpp)
	list(APPEND QTAT_HEADER_FILES QAndroidAppPermissions.h)
endif()

option(QTAT_APK_INFO "Enable QtAndroidTools Apk info.")
if(QTAT_APK_INFO)
	add_compile_definitions(QTAT_APK_INFO)
	list(APPEND QTAT_SOURCE_FILES QAndroidApkInfo.cpp)
	list(APPEND QTAT_HEADER_FILES QAndroidApkInfo.h)
endif()

option(QTAT_SCREEN "Enable QtAndroidTools Screen")
if(QTAT_SCREEN)
	add_compile_definitions(QTAT_SCREEN)
	list(APPEND QTAT_SOURCE_FILES QAndroidScreen.cpp)
	list(APPEND QTAT_HEADER_FILES QAndroidScreen.h)
	list(APPEND QTAT_JAVA_FILES ${QTAT_JAVA_DIR}/AndroidScreen.java)
endif()

option(QTAT_SYSTEM "Enable QtAndroidTools System")
if(QTAT_SYSTEM)
	add_compile_definitions(QTAT_SYSTEM)
	list(APPEND QTAT_SOURCE_FILES QAndroidSystem.cpp)
	list(APPEND QTAT_HEADER_FILES QAndroidSystem.h)
	list(APPEND QTAT_JAVA_FILES ${QTAT_JAVA_DIR}/AndroidSystem.java)
endif()

option(QTAT_APK_EXPANSION_FILES "Enable QtAndroidTools Apk Expansion.")
if(QTAT_APK_EXPANSION_FILES)
	add_compile_definitions(QTAT_APK_EXPANSION_FILES)
	list(APPEND QTAT_SOURCE_FILES QAndroidApkExpansionFiles.cpp)
	list(APPEND QTAT_HEADER_FILES QAndroidApkExpansionFiles.h)
	list(APPEND QTAT_JAVA_FILES ${QTAT_JAVA_DIR}/AndroidApkExpansionFiles.java)
	file(COPY src/com/google DESTINATION ${ANDROID_PACKAGE_SOURCE_DIR}/src/com/)
	file(COPY aidl DESTINATION ${ANDROID_PACKAGE_SOURCE_DIR}/)
endif()

option(QTAT_BATTERY_STATE "Enable QtAndroidTools Battery State.")
if(QTAT_BATTERY_STATE)
	add_compile_definitions(QTAT_BATTERY_STATE)
	list(APPEND QTAT_SOURCE_FILES QAndroidBatteryState.cpp)
	list(APPEND QTAT_HEADER_FILES QAndroidBatteryState.h)
	list(APPEND QTAT_JAVA_FILES ${QTAT_JAVA_DIR}/AndroidBatteryState.java)
endif()

option(QTAT_SIGNAL_STRENGTH "Enable QtAndroidTools Signal Strength.")
if(QTAT_SIGNAL_STRENGTH)
	add_compile_definitions(QTAT_SIGNAL_STRENGTH)
	list(APPEND QTAT_SOURCE_FILES QAndroidSignalStrength.cpp)
	list(APPEND QTAT_HEADER_FILES QAndroidSignalStrength.h)
	list(APPEND QTAT_JAVA_FILES ${QTAT_JAVA_DIR}/AndroidSignalStrength.java)
endif()

option(QTAT_ADMOB_BANNER "Enable QtAndroidTools AdMob banner.")
if(QTAT_ADMOB_BANNER)
	add_compile_definitions(QTAT_ADMOB_BANNER)
	list(APPEND QTAT_SOURCE_FILES QAndroidAdMobBanner.cpp)
	list(APPEND QTAT_HEADER_FILES QAndroidAdMobBanner.h)
	list(APPEND QTAT_JAVA_FILES
		${QTAT_JAVA_DIR}/AndroidAdMob.java
		${QTAT_JAVA_DIR}/AndroidAdMobBanner.java
		${QTAT_JAVA_DIR}/SyncRunOnUiThread.java)
endif()

option(QTAT_ADMOB_INTERSTITIAL "Enable QtAndroidTools AdMob Interstitial.")
if(QTAT_ADMOB_INTERSTITIAL)
	add_compile_definitions(QTAT_ADMOB_INTERSTITIAL)
	list(APPEND QTAT_SOURCE_FILES QAndroidAdMobInterstitial.cpp)
	list(APPEND QTAT_HEADER_FILES QAndroidAdMobInterstitial.h)
	list(APPEND QTAT_JAVA_FILES
		${QTAT_JAVA_DIR}/AndroidAdMob.java
		${QTAT_JAVA_DIR}/AndroidAdMobInterstitial.java
		${QTAT_JAVA_DIR}/SyncRunOnUiThread.java)
endif()

option(QTAT_ADMOB_REWAREDED_VIDEO "Enable QtAndroidTools AdMob Rewareded Video.")
if(QTAT_ADMOB_INTERSTITIAL)
	add_compile_definitions(QTAT_ADMOB_REWAREDED_VIDEO)
	list(APPEND QTAT_SOURCE_FILES QAndroidAdMobRewardedVideo.cpp)
	list(APPEND QTAT_HEADER_FILES QAndroidAdMobRewardedVideo.h)
	list(APPEND QTAT_JAVA_FILES
		${QTAT_JAVA_DIR}/AndroidAdMob.java
		${QTAT_JAVA_DIR}/AndroidAdMobRewardedVideo.java
		${QTAT_JAVA_DIR}/SyncRunOnUiThread.java)
endif()

option(QTAT_IMAGES "Enable QtAndroidTools Images.")
if(QTAT_IMAGES)
	add_compile_definitions(QTAT_IMAGES)
	list(APPEND QTAT_SOURCE_FILES QAndroidImages.cpp)
	list(APPEND QTAT_HEADER_FILES QAndroidImages.h)
	list(APPEND QTAT_JAVA_FILES ${QTAT_JAVA_DIR}/AndroidImages.java)
endif()

option(QTAT_NOTIFICATION "Enable QtAndroidTools Notifications.")
if(QTAT_NOTIFICATION)
	add_compile_definitions(QTAT_NOTIFICATION)
	list(APPEND QTAT_SOURCE_FILES QAndroidNotification.cpp)
	list(APPEND QTAT_HEADER_FILES QAndroidNotification.h)
	list(APPEND QTAT_JAVA_FILES ${QTAT_JAVA_DIR}/AndroidNotification.java)
endif()

option(QTAT_PLAY_STORE "Enable QtAndroidTools Play Store.")
if(QTAT_PLAY_STORE)
	add_compile_definitions(QTAT_PLAY_STORE)
	list(APPEND QTAT_SOURCE_FILES QAndroidPlayStore.cpp)
	list(APPEND QTAT_HEADER_FILES QAndroidPlayStore.h)
	list(APPEND QTAT_JAVA_FILES ${QTAT_JAVA_DIR}/AndroidPlayStore.java)
endif()

option(QTAT_GOOGLE_ACCOUNT "Enable QtAndroidTools Google Account.")
if(QTAT_GOOGLE_ACCOUNT)
	add_compile_definitions(QTAT_GOOGLE_ACCOUNT)
	list(APPEND QTAT_SOURCE_FILES QAndroidGoogleAccount.cpp)
	list(APPEND QTAT_HEADER_FILES QAndroidGoogleAccount.h)
	list(APPEND QTAT_JAVA_FILES ${QTAT_JAVA_DIR}/AndroidGoogleAccount.java)
endif()

option(QTAT_GOOGLE_DRIVE "Enable QtAndroidTools Google Drive.")
if(QTAT_GOOGLE_DRIVE)
	add_compile_definitions(QTAT_GOOGLE_DRIVE)
	list(APPEND QTAT_SOURCE_FILES QAndroidGoogleDrive.cpp)
	list(APPEND QTAT_HEADER_FILES QAndroidGoogleDrive.h)
	list(APPEND QTAT_JAVA_FILES ${QTAT_JAVA_DIR}/AndroidGoogleDrive.java)
endif()

option(QTAT_SHARING "Enable QtAndroidTools Sharing.")
if(QTAT_SHARING)
	add_compile_definitions(QTAT_SHARING)
	list(APPEND QTAT_SOURCE_FILES QAndroidSharing.cpp)
	list(APPEND QTAT_HEADER_FILES QAndroidSharing.h)
	list(APPEND QTAT_JAVA_FILES ${QTAT_JAVA_DIR}/AndroidSharing.java)
endif()

option(QTAT_USER_MESSAGING_PLATFORM "Enable QtAndroidTools User Messaging Platform.")
if(QTAT_USER_MESSAGING_PLATFORM)
	add_compile_definitions(QTAT_USER_MESSAGING_PLATFORM)
	list(APPEND QTAT_SOURCE_FILES QAndroidUserMessagingPlatform.cpp)
	list(APPEND QTAT_HEADER_FILES QAndroidUserMessagingPlatform.h)
	list(APPEND QTAT_JAVA_FILES ${QTAT_JAVA_DIR}/AndroidUserMessagingPlatform.java)
endif()

option(QTAT_AUDIO "Enable QtAndroidTools Audio.")
if(QTAT_AUDIO)
	add_compile_definitions(QTAT_AUDIO)
	list(APPEND QTAT_SOURCE_FILES QAndroidAudio.cpp)
	list(APPEND QTAT_HEADER_FILES QAndroidAudio.h)
	list(APPEND QTAT_JAVA_FILES ${QTAT_JAVA_DIR}/AndroidAudio.java)
endif()

add_library(QtAndroidTools
    ${QTAT_SOURCE_FILES}
    ${QTAT_HEADER_FILES})

target_link_libraries(QtAndroidTools Qt6::Quick Qt6::Core Qt6::Qml jnigraphics)

target_include_directories(QtAndroidTools PRIVATE ${Qt5Core_INCLUDE_DIRS})
target_include_directories(QtAndroidTools PRIVATE ${Qt5Quick_INCLUDE_DIRS})
target_include_directories(QtAndroidTools PRIVATE ${Qt5AndroidExtras_INCLUDE_DIRS})

file(MAKE_DIRECTORY ${ANDROID_PACKAGE_SOURCE_DIR}/${QTAT_JAVA_DIR}/)
foreach(JAVA_FILE ${QTAT_JAVA_FILES})
	configure_file(
		${CMAKE_CURRENT_SOURCE_DIR}/${JAVA_FILE}
		${ANDROID_PACKAGE_SOURCE_DIR}/${QTAT_JAVA_DIR}/
		COPYONLY)
endforeach(JAVA_FILE)

source_group("Source" FILES ${QTAT_SOURCE_FILES})
source_group("Headers" FILES ${QTAT_HEADER_FILES})
source_group("QTAT_JAVA" FILES ${QTAT_JAVA_FILES})

/*
 *	MIT License
 *
 *	Copyright (c) 2018 Fabio Falsini <falsinsoft@gmail.com>
 *
 *	Permission is hereby granted, free of charge, to any person obtaining a copy
 *	of this software and associated documentation files (the "Software"), to deal
 *	in the Software without restriction, including without limitation the rights
 *	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *	copies of the Software, and to permit persons to whom the Software is
 *	furnished to do so, subject to the following conditions:
 *
 *	The above copyright notice and this permission notice shall be included in all
 *	copies or substantial portions of the Software.
 *
 *	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *	SOFTWARE.
 */
#include <QQmlEngine>
#ifdef QTAT_APP_PERMISSIONS
#include "QAndroidAppPermissions.h"
#endif
#ifdef QTAT_APK_EXPANSION_FILES
#include "QAndroidApkExpansionFiles.h"
#endif
#ifdef QTAT_APK_INFO
#include "QAndroidApkInfo.h"
#endif
#ifdef QTAT_SCREEN
#include "QAndroidScreen.h"
#endif
#ifdef QTAT_BATTERY_STATE
#include "QAndroidBatteryState.h"
#endif
#ifdef QTAT_SIGNAL_STRENGTH
#include "QAndroidSignalStrength.h"
#endif
#ifdef QTAT_IMAGES
#include "QAndroidImages.h"
#endif
#ifdef QTAT_NOTIFICATION
#include "QAndroidNotification.h"
#endif
#ifdef QTAT_ADMOB_BANNER
#include "QAndroidAdMobBanner.h"
#endif
#ifdef QTAT_ADMOB_INTERSTITIAL
#include "QAndroidAdMobInterstitial.h"
#endif
#ifdef QTAT_ADMOB_REWARDED_VIDEO
#include "QAndroidAdMobRewardedVideo.h"
#endif
#ifdef QTAT_PLAY_STORE
#include "QAndroidPlayStore.h"
#endif
#include "QtAndroidTools.h"

QtAndroidTools::QtAndroidTools()
{
}

void QtAndroidTools::InitializeQmlTools()
{
#ifdef QTAT_APP_PERMISSIONS
    qmlRegisterSingletonType<QAndroidAppPermissions>("QtAndroidTools", 1, 0, "QtAndroidAppPermissions", &QAndroidAppPermissions::qmlInstance);
#endif
#ifdef QTAT_APK_EXPANSION_FILES
    qmlRegisterSingletonType<QAndroidApkExpansionFiles>("QtAndroidTools", 1, 0, "QtAndroidApkExpansionFiles", &QAndroidApkExpansionFiles::qmlInstance);
#endif
#ifdef QTAT_APK_INFO
    qmlRegisterSingletonType<QAndroidApkInfo>("QtAndroidTools", 1, 0, "QtAndroidApkInfo", &QAndroidApkInfo::qmlInstance);
#endif
#ifdef QTAT_SCREEN
    qmlRegisterSingletonType<QAndroidScreen>("QtAndroidTools", 1, 0, "QtAndroidScreen", &QAndroidScreen::qmlInstance);
#endif
#ifdef QTAT_BATTERY_STATE
    qmlRegisterSingletonType<QAndroidBatteryState>("QtAndroidTools", 1, 0, "QtAndroidBatteryState", &QAndroidBatteryState::qmlInstance);
#endif
#ifdef QTAT_SIGNAL_STRENGTH
    qmlRegisterSingletonType<QAndroidSignalStrength>("QtAndroidTools", 1, 0, "QtAndroidSignalStrength", &QAndroidSignalStrength::qmlInstance);
#endif
#ifdef QTAT_IMAGES
    qmlRegisterSingletonType<QAndroidImages>("QtAndroidTools", 1, 0, "QtAndroidImages", &QAndroidImages::qmlInstance);
#endif
#ifdef QTAT_NOTIFICATION
    qmlRegisterType<QAndroidNotification>("QtAndroidTools", 1, 0, "QtAndroidNotification");
#endif
#ifdef QTAT_ADMOB_BANNER
    qmlRegisterType<QAndroidAdMobBanner>("QtAndroidTools", 1, 0, "QtAndroidAdMobBanner");
#endif
#ifdef QTAT_ADMOB_INTERSTITIAL
    qmlRegisterType<QAndroidAdMobInterstitial>("QtAndroidTools", 1, 0, "QtAndroidAdMobInterstitial");
#endif
#ifdef QTAT_ADMOB_REWARDED_VIDEO
    qmlRegisterType<QAndroidAdMobRewardedVideo>("QtAndroidTools", 1, 0, "QtAndroidAdMobRewardedVideo");
#endif
#ifdef QTAT_PLAY_STORE
    qmlRegisterSingletonType<QAndroidPlayStore>("QtAndroidTools", 1, 0, "QtAndroidPlayStore", &QAndroidPlayStore::qmlInstance);
#endif
}

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
#include <QFile>
#include <QQmlEngine>
#include <QCoreApplication>
#ifdef QTAT_APP_PERMISSIONS
#include "QAndroidAppPermissions.h"
#endif
#ifdef QTAT_APK_INFO
#include "QAndroidApkInfo.h"
#endif
#ifdef QTAT_SCREEN
#include "QAndroidScreen.h"
#endif
#ifdef QTAT_SYSTEM
#include "QAndroidSystem.h"
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
#ifdef QTAT_ADMOB_REWARDED_AD
#include "QAndroidAdMobRewardedAd.h"
#endif
#ifdef QTAT_PLAY_STORE
#include "QAndroidPlayStore.h"
#endif
#ifdef QTAT_GOOGLE_ACCOUNT
#include "QAndroidGoogleAccount.h"
#endif
#ifdef QTAT_GOOGLE_DRIVE
#include "QAndroidGoogleDrive.h"
#endif
#ifdef QTAT_SHARING
#include "QAndroidSharing.h"
#endif
#ifdef QTAT_USER_MESSAGING_PLATFORM
#include "QAndroidUserMessagingPlatform.h"
#endif
#ifdef QTAT_AUDIO
#include "QAndroidAudio.h"
#endif
#include "QtAndroidTools.h"
#include <android/bitmap.h>

QtAndroidTools *QtAndroidTools::m_pInstance = nullptr;

QtAndroidTools::QtAndroidTools(QObject *parent) : QObject(parent),
                                                  m_javaTools("com/falsinsoft/qtandroidtools/AndroidTools",
                                                              QNativeInterface::QAndroidApplication::context())
{
    m_pInstance = this;
    getActivityData();
}

QtAndroidTools* QtAndroidTools::create(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine);
    Q_UNUSED(scriptEngine);

    QtAndroidTools *pAndroidTools = new QtAndroidTools();
    engine->addImageProvider("QtAndroidTools", new PhotoImageProvider(&pAndroidTools->m_photoMap));
    return pAndroidTools;
}

QtAndroidTools* QtAndroidTools::instance()
{
    return m_pInstance;
}

QtAndroidTools::ACTION_ID QtAndroidTools::getActivityAction() const
{
    return m_activityAction;
}

QString QtAndroidTools::getActivityMimeType() const
{
    return m_activityMimeType;
}

void QtAndroidTools::getActivityData()
{
    if(m_javaTools.isValid())
    {
        QJniObject mimeTypeObj;

        m_activityAction = static_cast<ACTION_ID>(m_javaTools.callMethod<jint>("getActivityAction", "()I"));

        mimeTypeObj = m_javaTools.callObjectMethod("getActivityMimeType", "()Ljava/lang/String;");
        if(mimeTypeObj.isValid())
        {
            m_activityMimeType = mimeTypeObj.toString();
        }
    }
}

bool QtAndroidTools::insertImage(const QString &name, const QByteArray &data)
{
    QPixmap dataImage;

    if(dataImage.loadFromData(data) == true)
    {
        m_photoMap.insert(name, dataImage);
        return true;
    }

    return false;
}

bool QtAndroidTools::removeImage(const QString &name)
{
    return (m_photoMap.remove(name) > 0) ? true : false;
}

bool QtAndroidTools::binaryDataToFile(const QByteArray &binaryData, const QString &filePath)
{
    QFile binaryFile(filePath);

    if(binaryFile.open(QIODevice::WriteOnly) == true)
    {
        if(binaryFile.write(binaryData) == binaryData.size())
        {
            return true;
        }
        binaryFile.remove();
    }

    return false;
}

QByteArray QtAndroidTools::fileToBinaryData(const QString &filePath)
{
    QFile binaryFile(filePath);
    QByteArray binaryData;

    if(binaryFile.open(QIODevice::ReadOnly) == true)
    {
        binaryData = binaryFile.readAll();
    }

    return binaryData;
}

// Function to convert Qt image to Android images is credits of KDAB
// https://www.kdab.com/qt-on-android-how-to-convert-qt-images-to-android-images-and-vice-versa-2/
QJniObject QtAndroidTools::imageToAndroidBitmap(const QImage &img)
{
    QImage image = img.format() == QImage::Format_RGBA8888 ? img : img.convertToFormat(QImage::Format_RGBA8888);
    QJniObject bitmap, config;
    QJniEnvironment env;
    AndroidBitmapInfo info;

    config = QJniObject::getStaticObjectField("android/graphics/Bitmap$Config",
                                              "ARGB_8888",
                                              "Landroid/graphics/Bitmap$Config;");

    bitmap = QJniObject::callStaticObjectMethod("android/graphics/Bitmap",
                                                "createBitmap",
                                                "(IILandroid/graphics/Bitmap$Config;)Landroid/graphics/Bitmap;",
                                                img.width(), img.height(), config.object());

    if (AndroidBitmap_getInfo(env.jniEnv(), bitmap.object(), &info) != ANDROID_BITMAP_RESULT_SUCCESS)
        return QJniObject();

    if (info.format!= ANDROID_BITMAP_FORMAT_RGBA_8888)
        return QJniObject();

    void *pixels;
    if (AndroidBitmap_lockPixels(env.jniEnv(), bitmap.object(), &pixels) != ANDROID_BITMAP_RESULT_SUCCESS)
        return QJniObject();

    if (info.stride == uint32_t(image.bytesPerLine())) {
        memcpy(pixels, image.constBits(), info.stride * info.height);
    } else {
        uchar *bmpPtr = static_cast<uchar *>(pixels);
        const unsigned width = std::min(info.width, (uint)image.width());
        const unsigned height = std::min(info.height, (uint)image.height());
        for (unsigned y = 0; y < height; y++, bmpPtr += info.stride)
            memcpy(bmpPtr, image.constScanLine(y), width);
    }

    if (AndroidBitmap_unlockPixels(env.jniEnv(), bitmap.object()) != ANDROID_BITMAP_RESULT_SUCCESS)
        return QJniObject();

    return bitmap;
}

// Copyright KDAB (BogDan Vatra)
// https://www.kdab.com/qt-on-android-how-to-convert-qt-images-to-android-images-and-vice-versa-2/
QImage QtAndroidTools::androidBitmapToImage(const QJniObject &jniBmp)
{
    QJniEnvironment env;
    AndroidBitmapInfo info;
    if (AndroidBitmap_getInfo(env.jniEnv(), jniBmp.object(), &info) != ANDROID_BITMAP_RESULT_SUCCESS)
        return QImage();

    QImage::Format format;
    switch (info.format) {
    case ANDROID_BITMAP_FORMAT_RGBA_8888:
        format = QImage::Format_RGBA8888;
        break;
    case ANDROID_BITMAP_FORMAT_RGB_565:
        format = QImage::Format_RGB16;
        break;
    case ANDROID_BITMAP_FORMAT_RGBA_4444:
        format = QImage::Format_ARGB4444_Premultiplied;
        break;
    case ANDROID_BITMAP_FORMAT_A_8:
        format = QImage::Format_Alpha8;
        break;
    default:
        return QImage();
    }

    void *pixels;
    if (AndroidBitmap_lockPixels(env.jniEnv(), jniBmp.object(), &pixels) != ANDROID_BITMAP_RESULT_SUCCESS)
        return QImage();

    QImage image(info.width, info.height, format);

    if (info.stride == uint32_t(image.bytesPerLine())) {
        memcpy((void*)image.constBits(), pixels, info.stride * info.height);
    } else {
        uchar *bmpPtr = static_cast<uchar *>(pixels);
        const unsigned width = std::min(info.width, (uint)image.width());
        const unsigned height = std::min(info.height, (uint)image.height());
        for (unsigned y = 0; y < height; y++, bmpPtr += info.stride)
            memcpy((void*)image.constScanLine(y), bmpPtr, width);
    }

    if (AndroidBitmap_unlockPixels(env.jniEnv(), jniBmp.object()) != ANDROID_BITMAP_RESULT_SUCCESS)
        return QImage();

    return image;
}

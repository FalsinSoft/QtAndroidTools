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
#include <QGuiApplication>
#include <QImage>
#include <android/bitmap.h>
#include "QAndroidNotification.h"

QMap<int, QAndroidNotification*> QAndroidNotification::m_pInstancesMap;
int QAndroidNotification::m_InstancesCounter = 0;

QAndroidNotification::QAndroidNotification(QQuickItem *parent) : QQuickItem(parent),
                                                                 m_JavaNotification("com/falsinsoft/qtandroidtools/AndroidNotification",
                                                                                    "(Landroid/app/Activity;I)V",
                                                                                    QtAndroid::androidActivity().object<jobject>(),
                                                                                    m_InstancesCounter),
                                                                 m_InstanceIndex(m_InstancesCounter++)
{
    m_pInstancesMap[m_InstanceIndex] = this;
}

QAndroidNotification::~QAndroidNotification()
{
    m_pInstancesMap.remove(m_InstanceIndex);
}

const QMap<int, QAndroidNotification*>& QAndroidNotification::Instances()
{
    return m_pInstancesMap;
}

void QAndroidNotification::show(const QString &title, const QString &content)
{
    if(m_JavaNotification.isValid() && m_SmallIconName.isEmpty() == false)
    {
        m_JavaNotification.callMethod<void>("show",
                                            "(Ljava/lang/String;Ljava/lang/String;)V",
                                            QAndroidJniObject::fromString(title).object<jstring>(),
                                            QAndroidJniObject::fromString(content).object<jstring>()
                                            );
    }
}

void QAndroidNotification::cancel()
{
    if(m_JavaNotification.isValid())
    {
        m_JavaNotification.callMethod<void>("cancel");
    }
}

const QString& QAndroidNotification::getChannelName() const
{
    return m_ChannelName;
}

void QAndroidNotification::setChannelName(const QString &ChannelName)
{
    if(m_JavaNotification.isValid() && m_ChannelName.isEmpty())
    {
        m_JavaNotification.callMethod<void>("createNotificationChannel",
                                            "(Ljava/lang/String;)V",
                                            QAndroidJniObject::fromString(ChannelName).object<jstring>()
                                            );
        m_ChannelName = ChannelName;
    }
}

const QString& QAndroidNotification::getLargeIconSource() const
{
    return m_LargeIconSource;
}

void QAndroidNotification::setLargeIconSource(const QString &LargeIconSource)
{
    const QImage LargeIcon((LargeIconSource.length() > 5 && LargeIconSource.left(5) == "qrc:/") ? LargeIconSource.right(LargeIconSource.length() - 3) : LargeIconSource);
    QAndroidJniObject AndroidBitmap;

    if(LargeIcon.isNull() == false)
    {
        AndroidBitmap = ImageToAndroidBitmap(LargeIcon);
    }

    if(m_JavaNotification.isValid() && AndroidBitmap.isValid())
    {
        m_JavaNotification.callMethod<void>("setLargeIcon",
                                            "(Landroid/graphics/Bitmap;)V",
                                            AndroidBitmap.object()
                                            );
        m_LargeIconSource = LargeIconSource;
    }
}

const QString& QAndroidNotification::getSmallIconName() const
{
    return m_SmallIconName;
}

void QAndroidNotification::setSmallIconName(const QString &SmallIconName)
{
    const QAndroidJniObject Activity = QtAndroid::androidActivity();
    QAndroidJniObject PackageName, PackageManager, Resources;
    int ResourceId;

    PackageManager = Activity.callObjectMethod("getPackageManager", "()Landroid/content/pm/PackageManager;");
    PackageName = Activity.callObjectMethod("getPackageName", "()Ljava/lang/String;");
    Resources = Activity.callObjectMethod("getResources", "()Landroid/content/res/Resources;");

    ResourceId = Resources.callMethod<jint>("getIdentifier",
                                            "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)I",
                                            QAndroidJniObject::fromString(SmallIconName).object<jstring>(),
                                            QAndroidJniObject::fromString("drawable").object<jstring>(),
                                            PackageName.object<jstring>()
                                            );

    if(m_JavaNotification.isValid() && ResourceId != 0)
    {
        m_JavaNotification.callMethod<void>("setSmallIconResourceId",
                                            "(I)V",
                                            ResourceId
                                            );
        m_SmallIconName = SmallIconName;
    }
}

// Function to convert Qt image to Android images is credits of KDAB
// https://www.kdab.com/qt-on-android-how-to-convert-qt-images-to-android-images-and-vice-versa-2/
QAndroidJniObject QAndroidNotification::ImageToAndroidBitmap(const QImage &img)
{
    QImage image = img.format() == QImage::Format_RGBA8888 ? img : img.convertToFormat(QImage::Format_RGBA8888);
    QAndroidJniObject bitmap, config;
    QAndroidJniEnvironment env;
    AndroidBitmapInfo info;

    config = QAndroidJniObject::getStaticObjectField("android/graphics/Bitmap$Config",
                                                     "ARGB_8888",
                                                     "Landroid/graphics/Bitmap$Config;");

    bitmap = QAndroidJniObject::callStaticObjectMethod("android/graphics/Bitmap",
                                                       "createBitmap",
                                                       "(IILandroid/graphics/Bitmap$Config;)Landroid/graphics/Bitmap;",
                                                       img.width(), img.height(), config.object());

    if (AndroidBitmap_getInfo(env, bitmap.object(), &info) != ANDROID_BITMAP_RESULT_SUCCESS)
        return QAndroidJniObject();

    if (info.format!= ANDROID_BITMAP_FORMAT_RGBA_8888)
        return QAndroidJniObject();

    void *pixels;
    if (AndroidBitmap_lockPixels(env, bitmap.object(), &pixels) != ANDROID_BITMAP_RESULT_SUCCESS)
        return QAndroidJniObject();

    if (info.stride == uint32_t(image.bytesPerLine())) {
        memcpy(pixels, image.constBits(), info.stride * info.height);
    } else {
        uchar *bmpPtr = static_cast<uchar *>(pixels);
        const unsigned width = std::min(info.width, (uint)image.width());
        const unsigned height = std::min(info.height, (uint)image.height());
        for (unsigned y = 0; y < height; y++, bmpPtr += info.stride)
            memcpy(bmpPtr, image.constScanLine(y), width);
    }

    if (AndroidBitmap_unlockPixels(env, bitmap.object()) != ANDROID_BITMAP_RESULT_SUCCESS)
        return QAndroidJniObject();

    return bitmap;
}

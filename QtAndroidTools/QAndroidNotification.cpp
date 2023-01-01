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
#include <QCoreApplication>
#include <QImage>
#include "QAndroidNotification.h"
#include "QtAndroidTools.h"

QMap<int, QAndroidNotification*> QAndroidNotification::m_pInstancesMap;
int QAndroidNotification::m_instancesCounter = 0;

QAndroidNotification::QAndroidNotification(QQuickItem *parent) : QQuickItem(parent),
                                                                 m_javaNotification("com/falsinsoft/qtandroidtools/AndroidNotification",
                                                                                    "(Landroid/app/Activity;I)V",
                                                                                    QNativeInterface::QAndroidApplication::context(),
                                                                                    m_instancesCounter),
                                                                 m_instanceIndex(m_instancesCounter++)
{
    m_pInstancesMap[m_instanceIndex] = this;
}

QAndroidNotification::~QAndroidNotification()
{
    m_pInstancesMap.remove(m_instanceIndex);
}

const QMap<int, QAndroidNotification*>& QAndroidNotification::instances()
{
    return m_pInstancesMap;
}

void QAndroidNotification::show()
{
    if(m_javaNotification.isValid() && m_smallIconName.isEmpty() == false && m_text.isEmpty() == false)
    {
        m_javaNotification.callMethod<void>("show");
    }
}

void QAndroidNotification::cancel()
{
    if(m_javaNotification.isValid())
    {
        m_javaNotification.callMethod<void>("cancel");
    }
}

const QString& QAndroidNotification::getTitle() const
{
    return m_title;
}

void QAndroidNotification::setTitle(const QString &title)
{
    if(m_javaNotification.isValid())
    {
        m_javaNotification.callMethod<void>("setTitle",
                                            "(Ljava/lang/String;)V",
                                            QJniObject::fromString(title).object<jstring>()
                                            );
        m_title = title;
    }
}

const QString& QAndroidNotification::getText() const
{
    return m_text;
}

void QAndroidNotification::setText(const QString &text)
{
    if(m_javaNotification.isValid())
    {
        m_javaNotification.callMethod<void>("setContent",
                                            "(Ljava/lang/String;)V",
                                            QJniObject::fromString(text).object<jstring>()
                                            );
        m_text = text;
    }
}

const QString& QAndroidNotification::getExpandableText() const
{
    return m_expandableText;
}

void QAndroidNotification::setExpandableText(const QString &expandableText)
{
    if(m_javaNotification.isValid())
    {
        m_javaNotification.callMethod<void>("setExpandableContent",
                                            "(Ljava/lang/String;)V",
                                            QJniObject::fromString(expandableText).object<jstring>()
                                            );
        m_expandableText = expandableText;
    }
}

const QString& QAndroidNotification::getChannelName() const
{
    return m_channelName;
}

void QAndroidNotification::setChannelName(const QString &channelName)
{
    if(m_javaNotification.isValid() && m_channelName.isEmpty())
    {
        m_javaNotification.callMethod<void>("createNotificationChannel",
                                            "(Ljava/lang/String;)V",
                                            QJniObject::fromString(channelName).object<jstring>()
                                            );
        m_channelName = channelName;
    }
}

const QString& QAndroidNotification::getLargeIconSource() const
{
    return m_largeIconSource;
}

void QAndroidNotification::setLargeIconSource(const QString &largeIconSource)
{
    const QImage largeIcon((largeIconSource.length() > 5 && largeIconSource.left(5) == "qrc:/") ? largeIconSource.right(largeIconSource.length() - 3) : largeIconSource);

    if(m_javaNotification.isValid() && largeIcon.isNull() == false)
    {
        const QJniObject androidBitmap = QtAndroidTools::imageToAndroidBitmap(largeIcon);

        m_javaNotification.callMethod<void>("setLargeIcon",
                                            "(Landroid/graphics/Bitmap;)V",
                                            androidBitmap.object()
                                            );

        m_largeIconSource = largeIconSource;
    }
}

const QString& QAndroidNotification::getSmallIconName() const
{
    return m_smallIconName;
}

void QAndroidNotification::setSmallIconName(const QString &smallIconName)
{
    const QJniObject activity = QNativeInterface::QAndroidApplication::context();
    QJniObject packageName, resources;
    int smallIconResourceId;

    packageName = activity.callObjectMethod("getPackageName", "()Ljava/lang/String;");
    resources = activity.callObjectMethod("getResources", "()Landroid/content/res/Resources;");

    smallIconResourceId = resources.callMethod<jint>("getIdentifier",
                                                     "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)I",
                                                     QJniObject::fromString(smallIconName).object<jstring>(),
                                                     QJniObject::fromString("drawable").object<jstring>(),
                                                     packageName.object<jstring>()
                                                     );

    if(m_javaNotification.isValid() && smallIconResourceId != 0)
    {
        m_javaNotification.callMethod<void>("setSmallIcon",
                                            "(I)V",
                                            smallIconResourceId
                                            );
        m_smallIconName = smallIconName;
    }
}

const QAndroidNotificationProgressBar& QAndroidNotification::getProgressBar()
{
    return m_progressBar;
}

void QAndroidNotification::setProgressBar(const QAndroidNotificationProgressBar &progressBar)
{
    if(m_javaNotification.isValid())
    {
        m_javaNotification.callMethod<void>("setProgressBar",
                                            "(IIZ)V",
                                            progressBar.max,
                                            progressBar.current,
                                            progressBar.indeterminate
                                            );
        m_progressBar = progressBar;
    }
}

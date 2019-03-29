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
#pragma once

#include <QtAndroidExtras>
#include <QQmlEngine>
#include <QQuickItem>

class QAndroidNotification : public QQuickItem
{
    Q_PROPERTY(QString channelName READ getChannelName WRITE setChannelName)
    Q_PROPERTY(QString largeIconSource READ getLargeIconSource WRITE setLargeIconSource)
    Q_PROPERTY(QString smallIconName READ getSmallIconName WRITE setSmallIconName)
    Q_OBJECT

public:
    QAndroidNotification(QQuickItem *parent = nullptr);
    ~QAndroidNotification();

    const QString& getChannelName() const;
    void setChannelName(const QString &ChannelName);
    const QString& getLargeIconSource() const;
    void setLargeIconSource(const QString &LargeIconSource);
    const QString& getSmallIconName() const;
    void setSmallIconName(const QString &SmallIconName);

    Q_INVOKABLE void show(const QString &title, const QString &content);
    Q_INVOKABLE void cancel();

    static const QMap<int, QAndroidNotification*>& Instances();

private:
    const QAndroidJniObject m_JavaNotification;
    static QMap<int, QAndroidNotification*> m_pInstancesMap;
    static int m_InstancesCounter;
    const int m_InstanceIndex;
    QString m_ChannelName;
    QString m_LargeIconSource, m_SmallIconName;

    QAndroidJniObject ImageToAndroidBitmap(const QImage &img);
};

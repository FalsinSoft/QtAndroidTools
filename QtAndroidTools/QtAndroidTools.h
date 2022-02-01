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
//#pragma once

#include <android/bitmap.h>
#include <QCoreApplication>
#include <QFile>
#include <QImage>
#include <QJniObject>
#include <QPixmap>
#include <QQmlEngine>
#include <QQuickImageProvider>
#include <qnativeinterface.h>

class QtAndroidTools : public QObject
{
    Q_PROPERTY(ACTION_ID activityAction READ getActivityAction CONSTANT)
    Q_PROPERTY(QString activityMimeType READ getActivityMimeType CONSTANT)
    Q_DISABLE_COPY(QtAndroidTools)
    Q_ENUMS(ACTION_ID)
    Q_OBJECT

    class PhotoImageProvider : public QQuickImageProvider
    {
    public:
        PhotoImageProvider(QMap<QString, QPixmap> *pPhotoMap) : QQuickImageProvider(QQuickImageProvider::Pixmap), m_pPhotoMap(pPhotoMap) {}

        QPixmap requestPixmap(const QString &id, QSize *size, const QSize &requestedSize)
        {
            QMap<QString, QPixmap>::const_iterator photoIterator = m_pPhotoMap->find(id);

            if(photoIterator != m_pPhotoMap->constEnd())
            {
                const QPixmap photo = photoIterator.value();

                if(size) *size = photo.size();

                if(requestedSize.width() > 0 && requestedSize.height() > 0)
                    return photo.scaled(requestedSize);
                else
                    return photo;
            }

            return QPixmap();
        }

    private:
        const QMap<QString, QPixmap> *const m_pPhotoMap;
    };

    QtAndroidTools() : QtAndroidTools(nullptr) {}

public:
    QtAndroidTools(QObject *parent);

    enum ACTION_ID
    {
        ACTION_NONE = 0,
        ACTION_SEND,
        ACTION_SEND_MULTIPLE,
        ACTION_PICK
    };

    static QObject* qmlInstance(QQmlEngine *engine, QJSEngine *scriptEngine);
    static QtAndroidTools *instance();
    static QJniObject imageToAndroidBitmap(const QImage &img);
    static QImage androidBitmapToImage(const QJniObject &jniBmp);
    static void initializeQmlTools();

    Q_INVOKABLE bool insertImage(const QString &name, const QByteArray &data);
    Q_INVOKABLE bool removeImage(const QString &name);
    Q_INVOKABLE bool binaryDataToFile(const QByteArray &binaryData, const QString &filePath);
    Q_INVOKABLE QByteArray fileToBinaryData(const QString &filePath);

    ACTION_ID getActivityAction() const;
    QString getActivityMimeType() const;

private:
    const QJniObject m_javaTools;
    static QtAndroidTools *m_pInstance;
    QMap<QString, QPixmap> m_photoMap;
    ACTION_ID m_activityAction = ACTION_NONE;
    QString m_activityMimeType;

    void getActivityData();
};

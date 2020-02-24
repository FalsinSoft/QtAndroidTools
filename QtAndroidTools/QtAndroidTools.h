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

#include <QQuickImageProvider>
#include <QtAndroidExtras>
#include <QQmlEngine>
#include <QPixmap>
#include <QImage>

class QtAndroidTools : public QObject
{
    Q_DISABLE_COPY(QtAndroidTools)
    Q_OBJECT

    class PhotoImageProvider : public QQuickImageProvider
    {
    public:
        PhotoImageProvider(QMap<QString, QPixmap> *pPhotoMap) : QQuickImageProvider(QQuickImageProvider::Pixmap), m_pPhotoMap(pPhotoMap) {}

        QPixmap requestPixmap(const QString &id, QSize *size, const QSize &requestedSize)
        {
            QMap<QString, QPixmap>::const_iterator PhotoIterator = m_pPhotoMap->find(id);

            if(PhotoIterator != m_pPhotoMap->constEnd())
            {
                const QPixmap Photo = PhotoIterator.value();

                if(size) *size = Photo.size();

                if(requestedSize.width() > 0 && requestedSize.height() > 0)
                    return Photo.scaled(requestedSize);
                else
                    return Photo;
            }

            return QPixmap();
        }

    private:
        const QMap<QString, QPixmap> *const m_pPhotoMap;
    };

    QtAndroidTools();

public:
    static QObject* qmlInstance(QQmlEngine *engine, QJSEngine *scriptEngine);
    static QtAndroidTools* instance();
    static void InitializeQmlTools();

    Q_INVOKABLE bool insertImage(const QString &Name, const QByteArray &Data);
    Q_INVOKABLE bool removeImage(const QString &Name);
    Q_INVOKABLE bool binaryDataToFile(const QByteArray &BinaryData, const QString &FilePath);
    Q_INVOKABLE QByteArray fileToBinaryData(const QString &FilePath);

private:
    static QtAndroidTools *m_pInstance;
    QMap<QString, QPixmap> m_PhotoMap;
};

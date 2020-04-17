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

class QAndroidSharing : public QObject, public QAndroidActivityResultReceiver
{
    Q_DISABLE_COPY(QAndroidSharing)
    Q_OBJECT

    QAndroidSharing();

public:
    static QObject* qmlInstance(QQmlEngine *engine, QJSEngine *scriptEngine);
    static QAndroidSharing* instance();

    Q_INVOKABLE bool shareText(const QString &Text);
    Q_INVOKABLE bool shareBinaryData(const QString &MimeType, const QString &DataFilePath);
    Q_INVOKABLE QString getReceivedSharedText();
    Q_INVOKABLE QByteArray getReceivedSharedBinaryData();
    Q_INVOKABLE QVariantList getReceivedMultipleSharedBinaryData();
    Q_INVOKABLE bool requestSharedFile(const QString &MimeType);
    Q_INVOKABLE bool saveRequestedSharedFile(const QString &FilePath);
    Q_INVOKABLE void closeRequestedSharedFile();
    Q_INVOKABLE bool shareFile(bool FileAvailable, const QString &MimeType = QString(), const QString &FilePath = QString());

signals:
    void requestedSharedFileReadyToSave(const QString &mimeType, const QString &name, long size);
    void requestedSharedFileNotAvailable();

private:
    const QAndroidJniObject m_JavaSharing;
    static QAndroidSharing *m_pInstance;
    const int m_SharedFileRequestId = 9002;

    void handleActivityResult(int receiverRequestCode, int resultCode, const QAndroidJniObject &data) override;

    static void RequestedSharedFileInfo(JNIEnv *env, jobject thiz, jstring mimeType, jstring name, jlong size);

    inline QByteArray ConvertByteArray(const QAndroidJniObject &JavaByteArray);
};

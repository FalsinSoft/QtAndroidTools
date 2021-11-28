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

    QAndroidSharing() : QAndroidSharing(nullptr) {}

public:
    QAndroidSharing(QObject *parent);

    static QObject* qmlInstance(QQmlEngine *engine, QJSEngine *scriptEngine);
    static QAndroidSharing* instance();

    Q_INVOKABLE bool shareText(const QString &text);
    Q_INVOKABLE bool shareBinaryData(const QString &mimeType, const QString &dataFilePath);
    Q_INVOKABLE QString getReceivedSharedText();
    Q_INVOKABLE QByteArray getReceivedSharedBinaryData();
    Q_INVOKABLE QVariantList getReceivedMultipleSharedBinaryData();
    Q_INVOKABLE bool requestSharedFile(const QString &mimeType);
    Q_INVOKABLE bool saveRequestedSharedFile(const QString &filePath);
    Q_INVOKABLE void closeRequestedSharedFile();
    Q_INVOKABLE bool shareFile(bool fileAvailable, const QString &mimeType = QString(), const QString &filePath = QString());

Q_SIGNALS:
    void requestedSharedFileReadyToSave(const QString &mimeType, const QString &name, long size);
    void requestedSharedFileNotAvailable();

private:
    const QAndroidJniObject m_javaSharing;
    static QAndroidSharing *m_pInstance;
    const int m_sharedFileRequestId = 9002;

    void handleActivityResult(int receiverRequestCode, int resultCode, const QAndroidJniObject &data) override;

    static void requestedSharedFileInfo(JNIEnv *env, jobject thiz, jstring mimeType, jstring name, jlong size);

    inline QByteArray convertByteArray(const QAndroidJniObject &javaByteArray);
};

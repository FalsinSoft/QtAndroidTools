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
    Q_PROPERTY(ACTION_ID action READ getAction CONSTANT)
    Q_PROPERTY(QString mimeType READ getMimeType CONSTANT)
    Q_DISABLE_COPY(QAndroidSharing)
    Q_ENUMS(ACTION_ID)
    Q_OBJECT

    QAndroidSharing();

public:

    enum ACTION_ID
    {
        ACTION_NONE = 0,
        ACTION_SEND,
        ACTION_SEND_MULTIPLE,
        ACTION_PICK
    };

    static QObject* qmlInstance(QQmlEngine *engine, QJSEngine *scriptEngine);
    static QAndroidSharing* instance();

    Q_INVOKABLE bool shareText(const QString &Text);
    Q_INVOKABLE bool shareData(const QString &MimeType, const QString &DataFilePath);
    Q_INVOKABLE QString getSharedText();
    Q_INVOKABLE QByteArray getSharedData();
    Q_INVOKABLE bool requestSharedFile(const QString &MimeType);
    Q_INVOKABLE QByteArray getRequestedSharedFile();
    Q_INVOKABLE void closeSharedFile();
    Q_INVOKABLE bool returnSharedFile(bool FileAvailable, const QString &MimeType = QString(), const QString &FilePath = QString());

    ACTION_ID getAction() const;
    QString getMimeType() const;

signals:
    void requestedSharedFileReadyToGet(const QString &mimeType, const QString &name, long size);
    void requestedSharedFileNotAvailable();

private:
    const QAndroidJniObject m_JavaSharing;
    static QAndroidSharing *m_pInstance;
    const int m_SharedFileRequestId = 9002;
    ACTION_ID m_Action = ACTION_NONE;
    QString m_MimeType;

    void handleActivityResult(int receiverRequestCode, int resultCode, const QAndroidJniObject &data) override;

    static void RequestedSharedFileInfo(JNIEnv *env, jobject thiz, jstring mimeType, jstring name, jlong size);

    inline QByteArray ConvertByteArray(const QAndroidJniObject &JavaByteArray);
    void CheckSharingRequest();
};

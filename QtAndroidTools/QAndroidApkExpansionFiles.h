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

struct QAndroidApkExpansionFileInfo
{
    Q_GADGET
    Q_PROPERTY(uint version MEMBER Version)
    Q_PROPERTY(uint size MEMBER Size)
public:
    uint Version = 0;
    uint Size = 0;
};
Q_DECLARE_METATYPE(QAndroidApkExpansionFileInfo)

class QAndroidApkExpansionFiles : public QObject
{
    Q_PROPERTY(QAndroidApkExpansionFileInfo main READ getMainExpansionFileInfo WRITE setMainExpansionFileInfo)
    Q_PROPERTY(QAndroidApkExpansionFileInfo patch READ getPatchExpansionFileInfo WRITE setPatchExpansionFileInfo)
    Q_PROPERTY(QString base64PublicKey READ getBase64PublicKey WRITE setBase64PublicKey)
    Q_PROPERTY(QVector<int> salt READ getSALT WRITE setSALT)
    Q_DISABLE_COPY(QAndroidApkExpansionFiles)
    Q_ENUMS(DOWNLOAD_STATE)
    Q_ENUMS(APKEF_STATE)
    Q_ENUMS(STRING_ID)
    Q_OBJECT

    QAndroidApkExpansionFiles();

public:
    ~QAndroidApkExpansionFiles();

    enum DOWNLOAD_STATE
    {
        STATE_IDLE = 1,
        STATE_FETCHING_URL,
        STATE_CONNECTING,
        STATE_DOWNLOADING,
        STATE_COMPLETED,
        STATE_PAUSED_NETWORK_UNAVAILABLE,
        STATE_PAUSED_BY_REQUEST,
        STATE_PAUSED_WIFI_DISABLED_NEED_CELLULAR_PERMISSION,
        STATE_PAUSED_NEED_CELLULAR_PERMISSION,
        STATE_PAUSED_WIFI_DISABLED,
        STATE_PAUSED_NEED_WIFI,
        STATE_PAUSED_ROAMING,
        STATE_PAUSED_NETWORK_SETUP_FAILURE,
        STATE_PAUSED_SDCARD_UNAVAILABLE,
        STATE_FAILED_UNLICENSED,
        STATE_FAILED_FETCHING_URL,
        STATE_FAILED_SDCARD_FULL,
        STATE_FAILED_CANCELED,
        STATE_FAILED
    };
    enum APKEF_STATE
    {
        APKEF_NO_DOWNLOAD_REQUIRED = 0,
        APKEF_STORAGE_READ_PERMISSION_REQUIRED,
        APKEF_STORAGE_WRITE_PERMISSION_REQUIRED,
        APKEF_LVL_CHECK_REQUIRED,
        APKEF_DOWNLOAD_STARTED,
        APKEF_INVALID_JAVA_CLASS,
        APKEF_INVALID_BASE64_PUBLIC_KEY,
        APKEF_INVALID_SALT,
        APKEF_UNKNOWN_ERROR
    };
    enum STRING_ID
    {
        STRING_IDLE = 0,
        STRING_FETCHING_URL,
        STRING_CONNECTING,
        STRING_DOWNLOADING,
        STRING_COMPLETED,
        STRING_PAUSED_NETWORK_UNAVAILABLE,
        STRING_PAUSED_BY_REQUEST,
        STRING_PAUSED_WIFI_DISABLED_NEED_CELLULAR_PERMISSION,
        STRING_PAUSED_NEED_CELLULAR_PERMISSION,
        STRING_PAUSED_WIFI_DISABLED,
        STRING_PAUSED_NEED_WIFI,
        STRING_PAUSED_ROAMING,
        STRING_PAUSED_NETWORK_SETUP_FAILURE,
        STRING_PAUSED_SDCARD_UNAVAILABLE,
        STRING_FAILED_UNLICENSED,
        STRING_FAILED_FETCHING_URL,
        STRING_FAILED_SDCARD_FULL,
        STRING_FAILED_CANCELED,
        STRING_FAILED,
        STRING_UNKNOWN,
        STRING_TIME_LEFT,
        STRING_NOTIFICATION_CHANNEL_NAME
    };

    static QObject* qmlInstance(QQmlEngine *engine, QJSEngine *scriptEngine);
    static QAndroidApkExpansionFiles* instance();

    Q_INVOKABLE QString mainFileName();
    Q_INVOKABLE QString patchFileName();
    Q_INVOKABLE APKEF_STATE startDownloadFiles();
    Q_INVOKABLE QString getString(int stringID);
    Q_INVOKABLE void abortDownload();
    Q_INVOKABLE void pauseDownload();
    Q_INVOKABLE void continueDownload();

    const QString& getBase64PublicKey() const;
    void setBase64PublicKey(const QString &base64PublicKey);
    const QVector<int>& getSALT() const;
    void setSALT(const QVector<int> &SALT);
    const QAndroidApkExpansionFileInfo& getMainExpansionFileInfo() const;
    void setMainExpansionFileInfo(const QAndroidApkExpansionFileInfo &mainExpansionFileInfo);
    const QAndroidApkExpansionFileInfo& getPatchExpansionFileInfo() const;
    void setPatchExpansionFileInfo(const QAndroidApkExpansionFileInfo &patchExpansionFileInfo);

Q_SIGNALS:
    void downloadStateChanged(int newState);
    void downloadProgress(qlonglong overallTotal, qlonglong overallProgress, qlonglong timeRemaining, float currentSpeed);

private Q_SLOTS:
    void applicationStateChanged(Qt::ApplicationState state);

private:
    const QAndroidJniObject m_javaApkExpansionFiles;
    static QAndroidApkExpansionFiles *m_pInstance;
    QAndroidApkExpansionFileInfo m_expansionsFileInfo[2];
    QStringList m_stringsList;
    QString m_base64PublicKey;
    QVector<int> m_SALT;

    static void downloaderStateChanged(JNIEnv *env, jobject thiz, jint newState);
    static void downloaderProgress(JNIEnv *env, jobject thiz, jlong overallTotal, jlong overallProgress, jlong timeRemaining, jfloat currentSpeed);

    enum APP_STATE
    {
        APP_STATE_CREATE = 0,
        APP_STATE_START,
        APP_STATE_STOP,
        APP_STATE_DESTROY
    };
    void setNewAppState(APP_STATE newState);
    enum REQUEST_ID
    {
        REQUEST_ABORT_DOWNLOAD = 0,
        REQUEST_PAUSE_DOWNLOAD,
        REQUEST_CONTINUE_DOWNLOAD,
        REQUEST_DOWNLOAD_STATUS
    };
    void sendRequest(REQUEST_ID requestID);
    void populateStringsList();
};

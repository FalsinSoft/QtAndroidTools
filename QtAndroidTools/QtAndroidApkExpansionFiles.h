#pragma once

#include <QtAndroidExtras>
#include <QQmlEngine>

struct ExpansionFileInfo
{
    Q_GADGET
    Q_PROPERTY(uint version MEMBER Version)
    Q_PROPERTY(uint size MEMBER Size)
public:
    ExpansionFileInfo() : Version(0), Size(0) {}
    uint Version;
    uint Size;
};
Q_DECLARE_METATYPE(ExpansionFileInfo)

class QtAndroidApkExpansionFiles : public QObject
{
    Q_PROPERTY(ExpansionFileInfo main READ getMainExpansionFileInfo WRITE setMainExpansionFileInfo)
    Q_PROPERTY(ExpansionFileInfo patch READ getPatchExpansionFileInfo WRITE setPatchExpansionFileInfo)
    Q_PROPERTY(QString base64PublicKey READ getBase64PublicKey WRITE setBase64PublicKey)
    Q_PROPERTY(QVector<int> salt READ getSALT WRITE setSALT)
    Q_DISABLE_COPY(QtAndroidApkExpansionFiles)
    Q_ENUMS(DOWNLOAD_STATE)
    Q_ENUMS(APKEF_STATE)
    Q_OBJECT

    QtAndroidApkExpansionFiles();

public:
    enum DOWNLOAD_STATE
    {
        STATE_IDLE = 1,
        STATE_FETCHING_URL = 2,
        STATE_CONNECTING = 3,
        STATE_DOWNLOADING = 4,
        STATE_COMPLETED = 5,
        STATE_PAUSED_NETWORK_UNAVAILABLE = 6,
        STATE_PAUSED_BY_REQUEST = 7,
        STATE_PAUSED_WIFI_DISABLED_NEED_CELLULAR_PERMISSION = 8,
        STATE_PAUSED_NEED_CELLULAR_PERMISSION = 9,
        STATE_PAUSED_WIFI_DISABLED = 10,
        STATE_PAUSED_NEED_WIFI = 11,
        STATE_PAUSED_ROAMING = 12,
        STATE_PAUSED_NETWORK_SETUP_FAILURE = 13,
        STATE_PAUSED_SDCARD_UNAVAILABLE = 14,
        STATE_FAILED_UNLICENSED = 15,
        STATE_FAILED_FETCHING_URL = 16,
        STATE_FAILED_SDCARD_FULL = 17,
        STATE_FAILED_CANCELED = 18,
        STATE_FAILED = 19,
        STATE_UNKNOWN = 20,
        STATE_DOWNLOADING_TIME_LEFT = 21
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

    static QObject* qmlInstance(QQmlEngine *engine, QJSEngine *scriptEngine);
    static QtAndroidApkExpansionFiles* instance();

    Q_INVOKABLE QString mainFileName();
    Q_INVOKABLE QString patchFileName();
    Q_INVOKABLE APKEF_STATE startDownloadFiles();
    Q_INVOKABLE QString stateTextInfo(int state);

    const QString& getBase64PublicKey() const;
    void setBase64PublicKey(const QString &Base64PublicKey);
    const QVector<int>& getSALT() const;
    void setSALT(const QVector<int> &SALT);
    const ExpansionFileInfo& getMainExpansionFileInfo() const;
    void setMainExpansionFileInfo(const ExpansionFileInfo &MainExpansionFileInfo);
    const ExpansionFileInfo& getPatchExpansionFileInfo() const;
    void setPatchExpansionFileInfo(const ExpansionFileInfo &PatchExpansionFileInfo);

signals:
    void downloadStateChanged(int newState);
    void downloadProgress(qlonglong overallTotal, qlonglong overallProgress, qlonglong timeRemaining, float currentSpeed);

private slots:
    void ApplicationStateChanged(Qt::ApplicationState State);

private:
    const QAndroidJniObject m_JavaApkExpansionDownloader;
    static QtAndroidApkExpansionFiles *m_pInstance;
    ExpansionFileInfo m_ExpansionsFileInfo[2];
    QString m_Base64PublicKey;
    QVector<int> m_SALT;
};


#include <QGuiApplication>
#include "QtAndroidApkExpansionFiles.h"

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jstring JNICALL Java_com_google_android_vending_expansion_downloader_Helpers_getDownloaderStringResourceFromState(JNIEnv *env, jobject thiz, jint state)
{
    QtAndroidApkExpansionFiles *pInstance = QtAndroidApkExpansionFiles::instance();
    QString TextInfo;

    Q_UNUSED(thiz)

    if(pInstance != nullptr)
    {
        TextInfo = pInstance->stateTextInfo(state);
    }

    return env->NewString(TextInfo.utf16(), TextInfo.length());
}

JNIEXPORT void JNICALL Java_com_falsinsoft_QtAndroidTools_ApkExpansionDownloader_downloadStateChanged(JNIEnv *env, jobject thiz, jint newState)
{
    QtAndroidApkExpansionFiles *pInstance = QtAndroidApkExpansionFiles::instance();

    Q_UNUSED(env)
    Q_UNUSED(thiz)

    if(pInstance != nullptr)
    {
        emit pInstance->downloadStateChanged(newState);
    }
}

JNIEXPORT void JNICALL Java_com_falsinsoft_QtAndroidTools_ApkExpansionDownloader_downloadProgress(JNIEnv *env, jobject thiz, jlong overallTotal, jlong overallProgress, jlong timeRemaining, jfloat currentSpeed)
{
    QtAndroidApkExpansionFiles *pInstance = QtAndroidApkExpansionFiles::instance();

    Q_UNUSED(env)
    Q_UNUSED(thiz)

    if(pInstance != nullptr)
    {
        emit pInstance->downloadProgress(overallTotal, overallProgress, timeRemaining, currentSpeed);
    }
}

#ifdef __cplusplus
}
#endif

QtAndroidApkExpansionFiles *QtAndroidApkExpansionFiles::m_pInstance = nullptr;

QtAndroidApkExpansionFiles::QtAndroidApkExpansionFiles() : m_JavaApkExpansionDownloader("com/falsinsoft/QtAndroidTools/ApkExpansionDownloader",
                                                                                        "(Landroid/app/Activity;)V",
                                                                                        QtAndroid::androidActivity().object<jobject>())
{
    connect(qGuiApp, &QGuiApplication::applicationStateChanged, this, &QtAndroidApkExpansionFiles::ApplicationStateChanged);
    qRegisterMetaType<ExpansionFileInfo>();
    m_pInstance = this;
}

QObject* QtAndroidApkExpansionFiles::qmlInstance(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine);
    Q_UNUSED(scriptEngine);

    return new QtAndroidApkExpansionFiles();
}

QtAndroidApkExpansionFiles* QtAndroidApkExpansionFiles::instance()
{
    return m_pInstance;
}

void QtAndroidApkExpansionFiles::ApplicationStateChanged(Qt::ApplicationState State)
{
    const bool StubConnect = (State == Qt::ApplicationActive) ? true : false;

    if(m_JavaApkExpansionDownloader.isValid())
    {
        m_JavaApkExpansionDownloader.callMethod<void>("enableClientStubConnection",
                                                      "(Z)V",
                                                      StubConnect
                                                      );
    }
}

QtAndroidApkExpansionFiles::APKEF_STATE QtAndroidApkExpansionFiles::startDownloadFiles()
{
    if(m_JavaApkExpansionDownloader.isValid() == false) return APKEF_INVALID_JAVA_CLASS;
    if(m_Base64PublicKey.count() == 0) return APKEF_INVALID_BASE64_PUBLIC_KEY;
    if(m_SALT.count() != 20) return APKEF_INVALID_SALT;
    if(QtAndroid::androidSdkVersion() >= 23)
    {
        if(QtAndroid::checkPermission("Manifest.permission.READ_EXTERNAL_STORAGE") != QtAndroid::PermissionResult::Granted) return APKEF_STORAGE_READ_PERMISSION_REQUIRED;
        if(QtAndroid::checkPermission("Manifest.permission.WRITE_EXTERNAL_STORAGE") != QtAndroid::PermissionResult::Granted) return APKEF_STORAGE_WRITE_PERMISSION_REQUIRED;
    }

    for(int i = 0; i < 2; i++)
    {
        if(m_ExpansionsFileInfo[i].Version > 0 && m_ExpansionsFileInfo[i].Size > 0)
        {
            if(!m_JavaApkExpansionDownloader.callMethod<jboolean>("isAPKFileDelivered",
                                                                  "(ZII)Z",
                                                                  (i == 0) ? true : false,
                                                                  m_ExpansionsFileInfo[i].Version,
                                                                  m_ExpansionsFileInfo[i].Size))
            {
                enum { NO_DOWNLOAD_REQUIRED = 0, LVL_CHECK_REQUIRED = 1, DOWNLOAD_REQUIRED = 2 };
                QAndroidJniEnvironment JniEnv;
                int DownloadResult;
                jbyte Buffer[20];
                jbyteArray SALT;

                for(int i = 0; i < 20; i++) Buffer[i] = static_cast<jbyte>(m_SALT[i]);

                SALT = JniEnv->NewByteArray(20);
                JniEnv->SetByteArrayRegion(SALT, 0, 20, Buffer);

                DownloadResult = m_JavaApkExpansionDownloader.callMethod<jint>("startDownloader",
                                                                               "(Ljava/lang/String;[B)I",
                                                                               QAndroidJniObject::fromString(m_Base64PublicKey).object<jstring>(),
                                                                               SALT
                                                                               );
                JniEnv->DeleteLocalRef(SALT);

                switch(DownloadResult)
                {
                    case NO_DOWNLOAD_REQUIRED:
                        return APKEF_NO_DOWNLOAD_REQUIRED;
                    case LVL_CHECK_REQUIRED:
                        return APKEF_LVL_CHECK_REQUIRED;
                    case DOWNLOAD_REQUIRED:
                        return APKEF_DOWNLOAD_STARTED;
                    default:
                        return APKEF_UNKNOWN_ERROR;
                }
            }
        }

        return APKEF_NO_DOWNLOAD_REQUIRED;
    }

    return APKEF_UNKNOWN_ERROR;
}

QString QtAndroidApkExpansionFiles::mainFileName()
{
    QString FileName;

    if(m_JavaApkExpansionDownloader.isValid() && m_ExpansionsFileInfo[0].Version > 0)
    {
        const QAndroidJniObject JavaFileName = m_JavaApkExpansionDownloader.callObjectMethod("getExpansionAPKFileName",
                                                                                             "(ZI)Ljava/lang/String;",
                                                                                             true,
                                                                                             m_ExpansionsFileInfo[0].Version
                                                                                             );
        FileName = JavaFileName.toString();
    }

    return FileName;
}

QString QtAndroidApkExpansionFiles::patchFileName()
{
    QString FileName;

    if(m_JavaApkExpansionDownloader.isValid() && m_ExpansionsFileInfo[1].Version > 0)
    {
        const QAndroidJniObject JavaFileName = m_JavaApkExpansionDownloader.callObjectMethod("getExpansionAPKFileName",
                                                                                             "(ZI)Ljava/lang/String;",
                                                                                             false,
                                                                                             m_ExpansionsFileInfo[1].Version
                                                                                             );
        FileName = JavaFileName.toString();
    }

    return FileName;
}

const QString& QtAndroidApkExpansionFiles::getBase64PublicKey() const
{
    return m_Base64PublicKey;
}

void QtAndroidApkExpansionFiles::setBase64PublicKey(const QString &Base64PublicKey)
{
    m_Base64PublicKey = Base64PublicKey;
}

const QVector<int>& QtAndroidApkExpansionFiles::getSALT() const
{
    return m_SALT;
}

void QtAndroidApkExpansionFiles::setSALT(const QVector<int> &SALT)
{
    m_SALT = SALT;
}

const ExpansionFileInfo& QtAndroidApkExpansionFiles::getMainExpansionFileInfo() const
{
    return m_ExpansionsFileInfo[0];
}

void QtAndroidApkExpansionFiles::setMainExpansionFileInfo(const ExpansionFileInfo &MainExpansionFileInfo)
{
    m_ExpansionsFileInfo[0] = MainExpansionFileInfo;
}

const ExpansionFileInfo& QtAndroidApkExpansionFiles::getPatchExpansionFileInfo() const
{
    return m_ExpansionsFileInfo[1];
}

void QtAndroidApkExpansionFiles::setPatchExpansionFileInfo(const ExpansionFileInfo &PatchExpansionFileInfo)
{
    m_ExpansionsFileInfo[1] = PatchExpansionFileInfo;
}

QString QtAndroidApkExpansionFiles::stateTextInfo(int state)
{
    QString TextInfo;

    switch(state)
    {
        case STATE_IDLE:
            TextInfo = tr("Waiting for download to start");
            break;
        case STATE_FETCHING_URL:
            TextInfo = tr("Looking for resources to download");
            break;
        case STATE_CONNECTING:
            TextInfo = tr("Connecting to the download server");
            break;
        case STATE_DOWNLOADING:
            TextInfo = tr("Downloading resources");
            break;
        case STATE_COMPLETED:
            TextInfo = tr("Download finished");
            break;
        case STATE_PAUSED_NETWORK_UNAVAILABLE:
            TextInfo = tr("Download paused because no network is available");
            break;
        case STATE_PAUSED_BY_REQUEST:
            TextInfo = tr("Download paused");
            break;
        case STATE_PAUSED_WIFI_DISABLED_NEED_CELLULAR_PERMISSION:
            TextInfo = tr("Download paused because wifi is disabled");
            break;
        case STATE_PAUSED_NEED_CELLULAR_PERMISSION:
        case STATE_PAUSED_NEED_WIFI:
            TextInfo = tr("Download paused because wifi is unavailable");
            break;
        case STATE_PAUSED_WIFI_DISABLED:
            TextInfo = tr("Download paused because wifi is disabled");
            break;
        case STATE_PAUSED_ROAMING:
            TextInfo = tr("Download paused because you are roaming");
            break;
        case STATE_PAUSED_NETWORK_SETUP_FAILURE:
            TextInfo = tr("Download paused. Test a website in browser");
            break;
        case STATE_PAUSED_SDCARD_UNAVAILABLE:
            TextInfo = tr("Download paused because the external storage is unavailable");
            break;
        case STATE_FAILED_UNLICENSED:
            TextInfo = tr("Download failed because you may not have purchased this app");
            break;
        case STATE_FAILED_FETCHING_URL:
            TextInfo = tr("Download failed because the resources could not be found");
            break;
        case STATE_FAILED_SDCARD_FULL:
            TextInfo = tr("Download failed because the external storage is full");
            break;
        case STATE_FAILED_CANCELED:
            TextInfo = tr("Download cancelled");
            break;
        case STATE_UNKNOWN:
            TextInfo = tr("Starting...");
            break;
        case STATE_DOWNLOADING_TIME_LEFT:
            TextInfo = tr("Time left");
            break;
    }

    return TextInfo;
}

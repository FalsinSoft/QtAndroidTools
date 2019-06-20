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
#include <QGuiApplication>
#include "QAndroidApkExpansionFiles.h"

QAndroidApkExpansionFiles *QAndroidApkExpansionFiles::m_pInstance = nullptr;

QAndroidApkExpansionFiles::QAndroidApkExpansionFiles() : m_JavaApkExpansionFiles("com/falsinsoft/qtandroidtools/AndroidApkExpansionFiles",
                                                                                 "(Landroid/app/Activity;)V",
                                                                                 QtAndroid::androidActivity().object<jobject>())
{
    m_pInstance = this;

    if(m_JavaApkExpansionFiles.isValid())
    {
        const JNINativeMethod JniMethod[] = {
            {"getString", "(I)Ljava/lang/String;", reinterpret_cast<void *>(&QAndroidApkExpansionFiles::DownloaderGetString)},
            {"downloadStateChanged", "(I)V", reinterpret_cast<void *>(&QAndroidApkExpansionFiles::DownloadStateChanged)},
            {"downloadProgress", "(JJJF)V", reinterpret_cast<void *>(&QAndroidApkExpansionFiles::DownloadProgress)}
        };
        QAndroidJniEnvironment JniEnv;
        jclass ObjectClass;

        ObjectClass = JniEnv->GetObjectClass(m_JavaApkExpansionFiles.object<jobject>());
        JniEnv->RegisterNatives(ObjectClass, JniMethod, sizeof(JniMethod)/sizeof(JNINativeMethod));
        JniEnv->DeleteLocalRef(ObjectClass);
    }
    connect(qGuiApp, &QGuiApplication::applicationStateChanged, this, &QAndroidApkExpansionFiles::ApplicationStateChanged);
    SetNewAppState(APP_STATE_CREATE);
}

QAndroidApkExpansionFiles::~QAndroidApkExpansionFiles()
{
    SetNewAppState(APP_STATE_DESTROY);
}

QObject* QAndroidApkExpansionFiles::qmlInstance(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine);
    Q_UNUSED(scriptEngine);

    return new QAndroidApkExpansionFiles();
}

QAndroidApkExpansionFiles* QAndroidApkExpansionFiles::instance()
{
    return m_pInstance;
}

void QAndroidApkExpansionFiles::ApplicationStateChanged(Qt::ApplicationState State)
{
    SetNewAppState((State == Qt::ApplicationActive) ? APP_STATE_START : APP_STATE_STOP);
}

void QAndroidApkExpansionFiles::SetNewAppState(APP_STATE NewState)
{
    if(m_JavaApkExpansionFiles.isValid())
    {
        m_JavaApkExpansionFiles.callMethod<void>("appStateChanged",
                                                 "(I)V",
                                                 NewState
                                                 );
    }
}

QAndroidApkExpansionFiles::APKEF_STATE QAndroidApkExpansionFiles::startDownloadFiles()
{
    if(m_JavaApkExpansionFiles.isValid() == false) return APKEF_INVALID_JAVA_CLASS;
    if(m_Base64PublicKey.count() == 0) return APKEF_INVALID_BASE64_PUBLIC_KEY;
    if(m_SALT.count() != 20) return APKEF_INVALID_SALT;
    if(QtAndroid::androidSdkVersion() >= 23)
    {
        if(QtAndroid::checkPermission("android.permission.READ_EXTERNAL_STORAGE") != QtAndroid::PermissionResult::Granted) return APKEF_STORAGE_READ_PERMISSION_REQUIRED;
        if(QtAndroid::checkPermission("android.permission.WRITE_EXTERNAL_STORAGE") != QtAndroid::PermissionResult::Granted) return APKEF_STORAGE_WRITE_PERMISSION_REQUIRED;
    }

    for(int i = 0; i < 2; i++)
    {
        if(m_ExpansionsFileInfo[i].Version > 0 && m_ExpansionsFileInfo[i].Size > 0)
        {
            if(!m_JavaApkExpansionFiles.callMethod<jboolean>("isAPKFileDelivered",
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

                DownloadResult = m_JavaApkExpansionFiles.callMethod<jint>("startDownloader",
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
    }

    return APKEF_NO_DOWNLOAD_REQUIRED;
}

QString QAndroidApkExpansionFiles::mainFileName()
{
    QString FileName;

    if(m_JavaApkExpansionFiles.isValid() && m_ExpansionsFileInfo[0].Version > 0)
    {
        const QAndroidJniObject JavaFileName = m_JavaApkExpansionFiles.callObjectMethod("getExpansionAPKFileName",
                                                                                        "(ZI)Ljava/lang/String;",
                                                                                        true,
                                                                                        m_ExpansionsFileInfo[0].Version
                                                                                        );
        FileName = JavaFileName.toString();
    }

    return FileName;
}

QString QAndroidApkExpansionFiles::patchFileName()
{
    QString FileName;

    if(m_JavaApkExpansionFiles.isValid() && m_ExpansionsFileInfo[1].Version > 0)
    {
        const QAndroidJniObject JavaFileName = m_JavaApkExpansionFiles.callObjectMethod("getExpansionAPKFileName",
                                                                                        "(ZI)Ljava/lang/String;",
                                                                                        false,
                                                                                        m_ExpansionsFileInfo[1].Version
                                                                                        );
        FileName = JavaFileName.toString();
    }

    return FileName;
}

void QAndroidApkExpansionFiles::abortDownload()
{
    SendRequest(REQUEST_ABORT_DOWNLOAD);
}

void QAndroidApkExpansionFiles::pauseDownload()
{
    SendRequest(REQUEST_PAUSE_DOWNLOAD);
}

void QAndroidApkExpansionFiles::continueDownload()
{
    SendRequest(REQUEST_CONTINUE_DOWNLOAD);
}

void QAndroidApkExpansionFiles::SendRequest(REQUEST_ID requestID)
{
    if(m_JavaApkExpansionFiles.isValid())
    {
        m_JavaApkExpansionFiles.callMethod<void>("sendRequest",
                                                 "(I)V",
                                                 requestID
                                                 );
    }
}

const QString& QAndroidApkExpansionFiles::getBase64PublicKey() const
{
    return m_Base64PublicKey;
}

void QAndroidApkExpansionFiles::setBase64PublicKey(const QString &Base64PublicKey)
{
    m_Base64PublicKey = Base64PublicKey;
}

const QVector<int>& QAndroidApkExpansionFiles::getSALT() const
{
    return m_SALT;
}

void QAndroidApkExpansionFiles::setSALT(const QVector<int> &SALT)
{
    m_SALT = SALT;
}

const QAndroidApkExpansionFileInfo& QAndroidApkExpansionFiles::getMainExpansionFileInfo() const
{
    return m_ExpansionsFileInfo[0];
}

void QAndroidApkExpansionFiles::setMainExpansionFileInfo(const QAndroidApkExpansionFileInfo &MainExpansionFileInfo)
{
    m_ExpansionsFileInfo[0] = MainExpansionFileInfo;
}

const QAndroidApkExpansionFileInfo& QAndroidApkExpansionFiles::getPatchExpansionFileInfo() const
{
    return m_ExpansionsFileInfo[1];
}

void QAndroidApkExpansionFiles::setPatchExpansionFileInfo(const QAndroidApkExpansionFileInfo &PatchExpansionFileInfo)
{
    m_ExpansionsFileInfo[1] = PatchExpansionFileInfo;
}

jstring QAndroidApkExpansionFiles::DownloaderGetString(JNIEnv *env, jobject thiz, jint StringID)
{
    QString TextString;

    Q_UNUSED(thiz)

    if(m_pInstance != nullptr)
    {
        TextString = m_pInstance->getString(StringID);
    }

    return env->NewString(TextString.utf16(), TextString.length());
}

void QAndroidApkExpansionFiles::DownloadStateChanged(JNIEnv *env, jobject thiz, jint NewState)
{
    Q_UNUSED(env)
    Q_UNUSED(thiz)

    if(m_pInstance != nullptr)
    {
        emit m_pInstance->downloadStateChanged(NewState);
    }
}

void QAndroidApkExpansionFiles::DownloadProgress(JNIEnv *env, jobject thiz, jlong OverallTotal, jlong OverallProgress, jlong TimeRemaining, jfloat CurrentSpeed)
{
    Q_UNUSED(env)
    Q_UNUSED(thiz)

    if(m_pInstance != nullptr)
    {
        emit m_pInstance->downloadProgress(OverallTotal, OverallProgress, TimeRemaining, CurrentSpeed);
    }
}

QString QAndroidApkExpansionFiles::getString(int stringID)
{
    QString TextString;

    switch(stringID)
    {
        case STRING_IDLE:
            TextString = tr("Waiting for download to start");
            break;
        case STRING_FETCHING_URL:
            TextString = tr("Looking for resources to download");
            break;
        case STRING_CONNECTING:
            TextString = tr("Connecting to the download server");
            break;
        case STRING_DOWNLOADING:
            TextString = tr("Downloading resources");
            break;
        case STRING_COMPLETED:
            TextString = tr("Download finished");
            break;
        case STRING_PAUSED_NETWORK_UNAVAILABLE:
            TextString = tr("Download paused because no network is available");
            break;
        case STRING_PAUSED_BY_REQUEST:
            TextString = tr("Download paused");
            break;
        case STRING_PAUSED_WIFI_DISABLED_NEED_CELLULAR_PERMISSION:
            TextString = tr("Download paused because wifi is disabled");
            break;
        case STRING_PAUSED_NEED_CELLULAR_PERMISSION:
        case STRING_PAUSED_NEED_WIFI:
            TextString = tr("Download paused because wifi is unavailable");
            break;
        case STRING_PAUSED_WIFI_DISABLED:
            TextString = tr("Download paused because wifi is disabled");
            break;
        case STRING_PAUSED_ROAMING:
            TextString = tr("Download paused because you are roaming");
            break;
        case STRING_PAUSED_NETWORK_SETUP_FAILURE:
            TextString = tr("Download paused. Test a website in browser");
            break;
        case STRING_PAUSED_SDCARD_UNAVAILABLE:
            TextString = tr("Download paused because the external storage is unavailable");
            break;
        case STRING_FAILED_UNLICENSED:
            TextString = tr("Download failed because you may not have purchased this app");
            break;
        case STRING_FAILED_FETCHING_URL:
            TextString = tr("Download failed because the resources could not be found");
            break;
        case STRING_FAILED_SDCARD_FULL:
            TextString = tr("Download failed because the external storage is full");
            break;
        case STRING_FAILED_CANCELED:
            TextString = tr("Download cancelled");
            break;
        case STRING_FAILED:
            TextString = tr("Download failed");
            break;
        case STRING_UNKNOWN:
            TextString = tr("Unknown error");
            break;
        case STRING_TIME_LEFT:
            TextString = tr("Time left");
            break;
        case STRING_NOTIFICATION_CHANNEL_NAME:
            TextString = tr("App data download");
            break;
    }

    return TextString;
}

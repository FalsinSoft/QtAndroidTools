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

QAndroidApkExpansionFiles::QAndroidApkExpansionFiles() : m_javaApkExpansionFiles("com/falsinsoft/qtandroidtools/AndroidApkExpansionFiles",
                                                                                 "(Landroid/app/Activity;)V",
                                                                                 QtAndroid::androidActivity().object<jobject>())
{
    m_pInstance = this;

    if(m_javaApkExpansionFiles.isValid())
    {
        const JNINativeMethod jniMethod[] = {
            {"getString", "(I)Ljava/lang/String;", reinterpret_cast<void *>(&QAndroidApkExpansionFiles::downloaderGetString)},
            {"downloadStateChanged", "(I)V", reinterpret_cast<void *>(&QAndroidApkExpansionFiles::downloaderStateChanged)},
            {"downloadProgress", "(JJJF)V", reinterpret_cast<void *>(&QAndroidApkExpansionFiles::downloaderProgress)}
        };
        QAndroidJniEnvironment jniEnv;
        jclass objectClass;

        objectClass = jniEnv->GetObjectClass(m_javaApkExpansionFiles.object<jobject>());
        jniEnv->RegisterNatives(objectClass, jniMethod, sizeof(jniMethod)/sizeof(JNINativeMethod));
        jniEnv->DeleteLocalRef(objectClass);
    }
    connect(qGuiApp, &QGuiApplication::applicationStateChanged, this, &QAndroidApkExpansionFiles::applicationStateChanged);
    setNewAppState(APP_STATE_CREATE);
}

QAndroidApkExpansionFiles::~QAndroidApkExpansionFiles()
{
    setNewAppState(APP_STATE_DESTROY);
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

void QAndroidApkExpansionFiles::applicationStateChanged(Qt::ApplicationState state)
{
    setNewAppState((state == Qt::ApplicationActive) ? APP_STATE_START : APP_STATE_STOP);
}

void QAndroidApkExpansionFiles::setNewAppState(APP_STATE newState)
{
    if(m_javaApkExpansionFiles.isValid())
    {
        m_javaApkExpansionFiles.callMethod<void>("appStateChanged",
                                                 "(I)V",
                                                 newState
                                                 );
    }
}

QAndroidApkExpansionFiles::APKEF_STATE QAndroidApkExpansionFiles::startDownloadFiles()
{
    if(m_javaApkExpansionFiles.isValid() == false) return APKEF_INVALID_JAVA_CLASS;
    if(m_base64PublicKey.count() == 0) return APKEF_INVALID_BASE64_PUBLIC_KEY;
    if(m_SALT.count() != 20) return APKEF_INVALID_SALT;
    if(QtAndroid::androidSdkVersion() >= 23)
    {
        if(QtAndroid::checkPermission("android.permission.READ_EXTERNAL_STORAGE") != QtAndroid::PermissionResult::Granted) return APKEF_STORAGE_READ_PERMISSION_REQUIRED;
        if(QtAndroid::checkPermission("android.permission.WRITE_EXTERNAL_STORAGE") != QtAndroid::PermissionResult::Granted) return APKEF_STORAGE_WRITE_PERMISSION_REQUIRED;
    }

    for(int i = 0; i < 2; i++)
    {
        if(m_expansionsFileInfo[i].Version > 0 && m_expansionsFileInfo[i].Size > 0)
        {
            if(!m_javaApkExpansionFiles.callMethod<jboolean>("isAPKFileDelivered",
                                                             "(ZII)Z",
                                                             (i == 0) ? true : false,
                                                             m_expansionsFileInfo[i].Version,
                                                             m_expansionsFileInfo[i].Size))
            {
                enum { NO_DOWNLOAD_REQUIRED = 0, LVL_CHECK_REQUIRED = 1, DOWNLOAD_REQUIRED = 2 };
                QAndroidJniEnvironment jniEnv;
                int downloadResult;
                jbyte buffer[20];
                jbyteArray SALT;

                for(int i = 0; i < 20; i++) buffer[i] = static_cast<jbyte>(m_SALT[i]);

                SALT = jniEnv->NewByteArray(20);
                jniEnv->SetByteArrayRegion(SALT, 0, 20, buffer);

                downloadResult = m_javaApkExpansionFiles.callMethod<jint>("startDownloader",
                                                                          "(Ljava/lang/String;[B)I",
                                                                          QAndroidJniObject::fromString(m_base64PublicKey).object<jstring>(),
                                                                          SALT
                                                                          );
                jniEnv->DeleteLocalRef(SALT);

                switch(downloadResult)
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
    QString fileName;

    if(m_javaApkExpansionFiles.isValid() && m_expansionsFileInfo[0].Version > 0)
    {
        const QAndroidJniObject JavaFileName = m_javaApkExpansionFiles.callObjectMethod("getExpansionAPKFileName",
                                                                                        "(ZI)Ljava/lang/String;",
                                                                                        true,
                                                                                        m_expansionsFileInfo[0].Version
                                                                                        );
        fileName = JavaFileName.toString();
    }

    return fileName;
}

QString QAndroidApkExpansionFiles::patchFileName()
{
    QString fileName;

    if(m_javaApkExpansionFiles.isValid() && m_expansionsFileInfo[1].Version > 0)
    {
        const QAndroidJniObject JavaFileName = m_javaApkExpansionFiles.callObjectMethod("getExpansionAPKFileName",
                                                                                        "(ZI)Ljava/lang/String;",
                                                                                        false,
                                                                                        m_expansionsFileInfo[1].Version
                                                                                        );
        fileName = JavaFileName.toString();
    }

    return fileName;
}

void QAndroidApkExpansionFiles::abortDownload()
{
    sendRequest(REQUEST_ABORT_DOWNLOAD);
}

void QAndroidApkExpansionFiles::pauseDownload()
{
    sendRequest(REQUEST_PAUSE_DOWNLOAD);
}

void QAndroidApkExpansionFiles::continueDownload()
{
    sendRequest(REQUEST_CONTINUE_DOWNLOAD);
}

void QAndroidApkExpansionFiles::sendRequest(REQUEST_ID requestID)
{
    if(m_javaApkExpansionFiles.isValid())
    {
        m_javaApkExpansionFiles.callMethod<void>("sendRequest",
                                                 "(I)V",
                                                 requestID
                                                 );
    }
}

const QString& QAndroidApkExpansionFiles::getBase64PublicKey() const
{
    return m_base64PublicKey;
}

void QAndroidApkExpansionFiles::setBase64PublicKey(const QString &base64PublicKey)
{
    m_base64PublicKey = base64PublicKey;
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
    return m_expansionsFileInfo[0];
}

void QAndroidApkExpansionFiles::setMainExpansionFileInfo(const QAndroidApkExpansionFileInfo &mainExpansionFileInfo)
{
    m_expansionsFileInfo[0] = mainExpansionFileInfo;
}

const QAndroidApkExpansionFileInfo& QAndroidApkExpansionFiles::getPatchExpansionFileInfo() const
{
    return m_expansionsFileInfo[1];
}

void QAndroidApkExpansionFiles::setPatchExpansionFileInfo(const QAndroidApkExpansionFileInfo &patchExpansionFileInfo)
{
    m_expansionsFileInfo[1] = patchExpansionFileInfo;
}

jstring QAndroidApkExpansionFiles::downloaderGetString(JNIEnv *env, jobject thiz, jint stringID)
{
    QString textString;

    Q_UNUSED(thiz)

    if(m_pInstance != nullptr)
    {
        textString = m_pInstance->getString(stringID);
    }

    return env->NewString(textString.utf16(), textString.length());
}

void QAndroidApkExpansionFiles::downloaderStateChanged(JNIEnv *env, jobject thiz, jint newState)
{
    Q_UNUSED(env)
    Q_UNUSED(thiz)

    if(m_pInstance != nullptr)
    {
        Q_EMIT m_pInstance->downloadStateChanged(newState);
    }
}

void QAndroidApkExpansionFiles::downloaderProgress(JNIEnv *env, jobject thiz, jlong overallTotal, jlong overallProgress, jlong timeRemaining, jfloat currentSpeed)
{
    Q_UNUSED(env)
    Q_UNUSED(thiz)

    if(m_pInstance != nullptr)
    {
        Q_EMIT m_pInstance->downloadProgress(overallTotal, overallProgress, timeRemaining, currentSpeed);
    }
}

QString QAndroidApkExpansionFiles::getString(int stringID)
{
    QString textString;

    switch(stringID)
    {
        case STRING_IDLE:
            textString = tr("Waiting for download to start");
            break;
        case STRING_FETCHING_URL:
            textString = tr("Looking for resources to download");
            break;
        case STRING_CONNECTING:
            textString = tr("Connecting to the download server");
            break;
        case STRING_DOWNLOADING:
            textString = tr("Downloading resources");
            break;
        case STRING_COMPLETED:
            textString = tr("Download finished");
            break;
        case STRING_PAUSED_NETWORK_UNAVAILABLE:
            textString = tr("Download paused because no network is available");
            break;
        case STRING_PAUSED_BY_REQUEST:
            textString = tr("Download paused");
            break;
        case STRING_PAUSED_WIFI_DISABLED_NEED_CELLULAR_PERMISSION:
            textString = tr("Download paused because wifi is disabled");
            break;
        case STRING_PAUSED_NEED_CELLULAR_PERMISSION:
        case STRING_PAUSED_NEED_WIFI:
            textString = tr("Download paused because wifi is unavailable");
            break;
        case STRING_PAUSED_WIFI_DISABLED:
            textString = tr("Download paused because wifi is disabled");
            break;
        case STRING_PAUSED_ROAMING:
            textString = tr("Download paused because you are roaming");
            break;
        case STRING_PAUSED_NETWORK_SETUP_FAILURE:
            textString = tr("Download paused. Test a website in browser");
            break;
        case STRING_PAUSED_SDCARD_UNAVAILABLE:
            textString = tr("Download paused because the external storage is unavailable");
            break;
        case STRING_FAILED_UNLICENSED:
            textString = tr("Download failed because you may not have purchased this app");
            break;
        case STRING_FAILED_FETCHING_URL:
            textString = tr("Download failed because the resources could not be found");
            break;
        case STRING_FAILED_SDCARD_FULL:
            textString = tr("Download failed because the external storage is full");
            break;
        case STRING_FAILED_CANCELED:
            textString = tr("Download cancelled");
            break;
        case STRING_FAILED:
            textString = tr("Download failed");
            break;
        case STRING_UNKNOWN:
            textString = tr("Unknown error");
            break;
        case STRING_TIME_LEFT:
            textString = tr("Time left");
            break;
        case STRING_NOTIFICATION_CHANNEL_NAME:
            textString = tr("App data download");
            break;
    }

    return textString;
}

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
#include "QAndroidSharing.h"

QAndroidSharing *QAndroidSharing::m_pInstance = nullptr;

QAndroidSharing::QAndroidSharing() : m_JavaSharing("com/falsinsoft/qtandroidtools/AndroidSharing",
                                                   "(Landroid/app/Activity;)V",
                                                   QtAndroid::androidActivity().object<jobject>())
{
    m_pInstance = this;

    if(m_JavaSharing.isValid())
    {
        const JNINativeMethod JniMethod[] = {
            {"requestedSharedFileInfo", "(Ljava/lang/String;Ljava/lang/String;J)V", reinterpret_cast<void*>(&QAndroidSharing::RequestedSharedFileInfo)}
        };
        QAndroidJniEnvironment JniEnv;
        jclass ObjectClass;

        ObjectClass = JniEnv->GetObjectClass(m_JavaSharing.object<jobject>());
        JniEnv->RegisterNatives(ObjectClass, JniMethod, sizeof(JniMethod)/sizeof(JNINativeMethod));
        JniEnv->DeleteLocalRef(ObjectClass);
    }

    CheckReceivedSharingRequest();
}

QObject* QAndroidSharing::qmlInstance(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine);
    Q_UNUSED(scriptEngine);

    return new QAndroidSharing();
}

QAndroidSharing* QAndroidSharing::instance()
{
    return m_pInstance;
}

QAndroidSharing::ACTION_ID QAndroidSharing::getReceivedSharingAction() const
{
    return m_ReceivedSharingAction;
}

QString QAndroidSharing::getReceivedSharingMimeType() const
{
    return m_ReceivedSharingMimeType;
}

void QAndroidSharing::CheckReceivedSharingRequest()
{
    if(m_JavaSharing.isValid())
    {
        QAndroidJniObject MimeTypeObj;

        m_ReceivedSharingAction = static_cast<ACTION_ID>(m_JavaSharing.callMethod<jint>("getReceivedSharingAction", "()I"));

        MimeTypeObj = m_JavaSharing.callObjectMethod("getReceivedSharingMimeType", "()Ljava/lang/String;");
        if(MimeTypeObj.isValid())
        {
            m_ReceivedSharingMimeType = MimeTypeObj.toString();
        }
    }
}

bool QAndroidSharing::shareText(const QString &Text)
{
    if(m_JavaSharing.isValid())
    {
        return m_JavaSharing.callMethod<jboolean>("shareText",
                                                  "(Ljava/lang/String;)Z",
                                                  QAndroidJniObject::fromString(Text).object<jstring>()
                                                  );
    }

    return false;
}

bool QAndroidSharing::shareBinaryData(const QString &MimeType, const QString &DataFilePath)
{
    if(m_JavaSharing.isValid())
    {
        return m_JavaSharing.callMethod<jboolean>("shareBinaryData",
                                                  "(Ljava/lang/String;Ljava/lang/String;)Z",
                                                  QAndroidJniObject::fromString(MimeType).object<jstring>(),
                                                  QAndroidJniObject::fromString(DataFilePath).object<jstring>()
                                                  );
    }

    return false;
}

QString QAndroidSharing::getReceivedSharedText()
{
    QString SharedText;

    if(m_JavaSharing.isValid())
    {
        const QAndroidJniObject SharedTextObj = m_JavaSharing.callObjectMethod("getReceivedSharedText", "()Ljava/lang/String;");

        if(SharedTextObj.isValid())
        {
            SharedText = SharedTextObj.toString();
        }
    }

    return SharedText;
}

QByteArray QAndroidSharing::getReceivedSharedBinaryData()
{
    QByteArray SharedData;

    if(m_JavaSharing.isValid())
    {
        const QAndroidJniObject SharedDataObj = m_JavaSharing.callObjectMethod("getReceivedSharedBinaryData", "()[B");

        if(SharedDataObj.isValid())
        {
            SharedData = ConvertByteArray(SharedDataObj);
        }
    }

    return SharedData;
}

QVariantList QAndroidSharing::getReceivedMultipleSharedBinaryData()
{
    QVariantList MultipleSharedData;

    if(m_JavaSharing.isValid())
    {
        const QAndroidJniObject MultipleSharedDataObj = m_JavaSharing.callObjectMethod("getReceivedMultipleSharedBinaryData", "()[[B");

        if(MultipleSharedDataObj.isValid())
        {
            const jobjectArray DataArray = MultipleSharedDataObj.object<jobjectArray>();
            QAndroidJniEnvironment pEnv;
            int ArraySize;

            ArraySize = pEnv->GetArrayLength(DataArray);
            for(int i = 0; i < ArraySize; i++)
            {
                MultipleSharedData << ConvertByteArray(QAndroidJniObject(pEnv->GetObjectArrayElement(DataArray, i)));
            }
        }
    }

    return MultipleSharedData;
}

bool QAndroidSharing::requestSharedFile(const QString &MimeType)
{
    if(m_JavaSharing.isValid())
    {
        const QAndroidJniObject RequestSharedFileIntent = m_JavaSharing.callObjectMethod("getRequestSharedFileIntent",
                                                                                         "(Ljava/lang/String;)Landroid/content/Intent;",
                                                                                         QAndroidJniObject::fromString(MimeType).object<jstring>()
                                                                                         );
        if(RequestSharedFileIntent.isValid())
        {
            QtAndroid::startActivity(RequestSharedFileIntent, m_SharedFileRequestId, this);
            return true;
        }
    }

    return false;
}

bool QAndroidSharing::saveRequestedSharedFile(const QString &FilePath)
{
    if(m_JavaSharing.isValid())
    {
        const QAndroidJniObject SharedFileDataObj = m_JavaSharing.callObjectMethod("getRequestedSharedFile", "()[B");

        if(SharedFileDataObj.isValid())
        {
            const QByteArray SharedFileData = ConvertByteArray(SharedFileDataObj);
            QFile SharedFile(FilePath);

            if(SharedFile.open(QIODevice::WriteOnly) == true)
            {
                if(SharedFile.write(SharedFileData) == SharedFileData.size())
                {
                    return true;
                }
                SharedFile.remove();
            }
        }
    }

    return false;
}

void QAndroidSharing::closeRequestedSharedFile()
{
    if(m_JavaSharing.isValid())
    {
        m_JavaSharing.callMethod<void>("closeRequestedSharedFile");
    }
}

bool QAndroidSharing::shareFile(bool FileAvailable, const QString &MimeType, const QString &FilePath)
{
    if(m_JavaSharing.isValid())
    {
        return m_JavaSharing.callMethod<jboolean>("shareFile",
                                                  "(ZLjava/lang/String;Ljava/lang/String;)Z",
                                                  FileAvailable,
                                                  QAndroidJniObject::fromString(MimeType).object<jstring>(),
                                                  QAndroidJniObject::fromString(FilePath).object<jstring>()
                                                  );
    }

    return false;
}

void QAndroidSharing::handleActivityResult(int receiverRequestCode, int resultCode, const QAndroidJniObject &data)
{
    Q_UNUSED(resultCode)

    if(receiverRequestCode == m_SharedFileRequestId)
    {
        const int RESULT_OK = QAndroidJniObject::getStaticField<jint>("android/app/Activity", "RESULT_OK");
        bool SharedFileAvailable = false;

        if(resultCode == RESULT_OK)
        {
            if(m_JavaSharing.isValid())
            {
                SharedFileAvailable = m_JavaSharing.callMethod<jboolean>("requestSharedFileIntentDataResult",
                                                                         "(Landroid/content/Intent;)Z",
                                                                         data.object<jobject>()
                                                                         );
            }
        }
        if(SharedFileAvailable == false)
        {
            emit requestedSharedFileNotAvailable();
        }
    }
}

void QAndroidSharing::RequestedSharedFileInfo(JNIEnv *env, jobject thiz, jstring mimeType, jstring name, jlong size)
{
    Q_UNUSED(env)
    Q_UNUSED(thiz)

    if(m_pInstance != nullptr)
    {
        emit m_pInstance->requestedSharedFileReadyToSave(QAndroidJniObject(mimeType).toString(), QAndroidJniObject(name).toString(), size);
    }
}

QByteArray QAndroidSharing::ConvertByteArray(const QAndroidJniObject &JavaByteArray)
{
    const jbyteArray DataArray = JavaByteArray.object<jbyteArray>();
    QAndroidJniEnvironment pEnv;
    QByteArray ByteArray;
    int ArraySize;
    jbyte *pData;

    ArraySize = pEnv->GetArrayLength(DataArray);
    pData = pEnv->GetByteArrayElements(DataArray, NULL);
    ByteArray = QByteArray(reinterpret_cast<const char*>(pData), ArraySize);
    pEnv->ReleaseByteArrayElements(DataArray, pData, JNI_ABORT);

    return ByteArray;
}

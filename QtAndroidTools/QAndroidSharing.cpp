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

QAndroidSharing::QAndroidSharing() : m_javaSharing("com/falsinsoft/qtandroidtools/AndroidSharing",
                                                   "(Landroid/app/Activity;)V",
                                                   QtAndroid::androidActivity().object<jobject>())
{
    m_pInstance = this;

    if(m_javaSharing.isValid())
    {
        const JNINativeMethod jniMethod[] = {
            {"requestedSharedFileInfo", "(Ljava/lang/String;Ljava/lang/String;J)V", reinterpret_cast<void*>(&QAndroidSharing::requestedSharedFileInfo)}
        };
        QAndroidJniEnvironment jniEnv;
        jclass objectClass;

        objectClass = jniEnv->GetObjectClass(m_javaSharing.object<jobject>());
        jniEnv->RegisterNatives(objectClass, jniMethod, sizeof(jniMethod)/sizeof(JNINativeMethod));
        jniEnv->DeleteLocalRef(objectClass);
    }
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

bool QAndroidSharing::shareText(const QString &text)
{
    if(m_javaSharing.isValid())
    {
        return m_javaSharing.callMethod<jboolean>("shareText",
                                                  "(Ljava/lang/String;)Z",
                                                  QAndroidJniObject::fromString(text).object<jstring>()
                                                  );
    }

    return false;
}

bool QAndroidSharing::shareBinaryData(const QString &mimeType, const QString &dataFilePath)
{
    if(m_javaSharing.isValid())
    {
        return m_javaSharing.callMethod<jboolean>("shareBinaryData",
                                                  "(Ljava/lang/String;Ljava/lang/String;)Z",
                                                  QAndroidJniObject::fromString(mimeType).object<jstring>(),
                                                  QAndroidJniObject::fromString(dataFilePath).object<jstring>()
                                                  );
    }

    return false;
}

QString QAndroidSharing::getReceivedSharedText()
{
    QString sharedText;

    if(m_javaSharing.isValid())
    {
        const QAndroidJniObject sharedTextObj = m_javaSharing.callObjectMethod("getReceivedSharedText", "()Ljava/lang/String;");

        if(sharedTextObj.isValid())
        {
            sharedText = sharedTextObj.toString();
        }
    }

    return sharedText;
}

QByteArray QAndroidSharing::getReceivedSharedBinaryData()
{
    QByteArray sharedData;

    if(m_javaSharing.isValid())
    {
        const QAndroidJniObject sharedDataObj = m_javaSharing.callObjectMethod("getReceivedSharedBinaryData", "()[B");

        if(sharedDataObj.isValid())
        {
            sharedData = convertByteArray(sharedDataObj);
        }
    }

    return sharedData;
}

QVariantList QAndroidSharing::getReceivedMultipleSharedBinaryData()
{
    QVariantList multipleSharedData;

    if(m_javaSharing.isValid())
    {
        const QAndroidJniObject multipleSharedDataObj = m_javaSharing.callObjectMethod("getReceivedMultipleSharedBinaryData", "()[[B");

        if(multipleSharedDataObj.isValid())
        {
            const jobjectArray dataArray = multipleSharedDataObj.object<jobjectArray>();
            QAndroidJniEnvironment jniEnv;
            int arraySize;

            arraySize = jniEnv->GetArrayLength(dataArray);
            for(int i = 0; i < arraySize; i++)
            {
                multipleSharedData << convertByteArray(QAndroidJniObject(jniEnv->GetObjectArrayElement(dataArray, i)));
            }
        }
    }

    return multipleSharedData;
}

bool QAndroidSharing::requestSharedFile(const QString &mimeType)
{
    if(m_javaSharing.isValid())
    {
        const QAndroidJniObject requestSharedFileIntent = m_javaSharing.callObjectMethod("getRequestSharedFileIntent",
                                                                                         "(Ljava/lang/String;)Landroid/content/Intent;",
                                                                                         QAndroidJniObject::fromString(mimeType).object<jstring>()
                                                                                         );
        if(requestSharedFileIntent.isValid())
        {
            QtAndroid::startActivity(requestSharedFileIntent, m_sharedFileRequestId, this);
            return true;
        }
    }

    return false;
}

bool QAndroidSharing::saveRequestedSharedFile(const QString &filePath)
{
    if(m_javaSharing.isValid())
    {
        const QAndroidJniObject sharedFileDataObj = m_javaSharing.callObjectMethod("getRequestedSharedFile", "()[B");

        if(sharedFileDataObj.isValid())
        {
            const QByteArray sharedFileData = convertByteArray(sharedFileDataObj);
            QFile sharedFile(filePath);

            if(sharedFile.open(QIODevice::WriteOnly) == true)
            {
                if(sharedFile.write(sharedFileData) == sharedFileData.size())
                {
                    return true;
                }
                sharedFile.remove();
            }
        }
    }

    return false;
}

void QAndroidSharing::closeRequestedSharedFile()
{
    if(m_javaSharing.isValid())
    {
        m_javaSharing.callMethod<void>("closeRequestedSharedFile");
    }
}

bool QAndroidSharing::shareFile(bool fileAvailable, const QString &mimeType, const QString &filePath)
{
    if(m_javaSharing.isValid())
    {
        return m_javaSharing.callMethod<jboolean>("shareFile",
                                                  "(ZLjava/lang/String;Ljava/lang/String;)Z",
                                                  fileAvailable,
                                                  QAndroidJniObject::fromString(mimeType).object<jstring>(),
                                                  QAndroidJniObject::fromString(filePath).object<jstring>()
                                                  );
    }

    return false;
}

void QAndroidSharing::handleActivityResult(int receiverRequestCode, int resultCode, const QAndroidJniObject &data)
{
    Q_UNUSED(resultCode)

    if(receiverRequestCode == m_sharedFileRequestId)
    {
        const int RESULT_OK = QAndroidJniObject::getStaticField<jint>("android/app/Activity", "RESULT_OK");
        bool sharedFileAvailable = false;

        if(resultCode == RESULT_OK)
        {
            if(m_javaSharing.isValid())
            {
                sharedFileAvailable = m_javaSharing.callMethod<jboolean>("requestSharedFileIntentDataResult",
                                                                         "(Landroid/content/Intent;)Z",
                                                                         data.object<jobject>()
                                                                         );
            }
        }
        if(sharedFileAvailable == false)
        {
            Q_EMIT requestedSharedFileNotAvailable();
        }
    }
}

void QAndroidSharing::requestedSharedFileInfo(JNIEnv *env, jobject thiz, jstring mimeType, jstring name, jlong size)
{
    Q_UNUSED(env)
    Q_UNUSED(thiz)

    if(m_pInstance != nullptr)
    {
        Q_EMIT m_pInstance->requestedSharedFileReadyToSave(QAndroidJniObject(mimeType).toString(), QAndroidJniObject(name).toString(), size);
    }
}

QByteArray QAndroidSharing::convertByteArray(const QAndroidJniObject &javaByteArray)
{
    const jbyteArray dataArray = javaByteArray.object<jbyteArray>();
    QAndroidJniEnvironment jniEnv;
    QByteArray byteArray;
    int arraySize;
    jbyte *pData;

    arraySize = jniEnv->GetArrayLength(dataArray);
    pData = jniEnv->GetByteArrayElements(dataArray, NULL);
    byteArray = QByteArray(reinterpret_cast<const char*>(pData), arraySize);
    jniEnv->ReleaseByteArrayElements(dataArray, pData, JNI_ABORT);

    return byteArray;
}

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
    CheckSharingRequest();
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

QAndroidSharing::ACTION_ID QAndroidSharing::getAction() const
{
    return m_Action;
}

QString QAndroidSharing::getMimeType() const
{
    return m_MimeType;
}

void QAndroidSharing::CheckSharingRequest()
{
    if(m_JavaSharing.isValid())
    {
        QAndroidJniObject MimeTypeObj;

        m_Action = static_cast<ACTION_ID>(m_JavaSharing.callMethod<jint>("getAction", "()I"));

        MimeTypeObj = m_JavaSharing.callObjectMethod("getMimeType", "()Ljava/lang/String;");
        if(MimeTypeObj.isValid())
        {
            m_MimeType = MimeTypeObj.toString();
        }
    }
}

void QAndroidSharing::shareText(const QString &Text)
{
    if(m_JavaSharing.isValid())
    {
        m_JavaSharing.callMethod<void>("shareText",
                                       "(Ljava/lang/String;)V",
                                       QAndroidJniObject::fromString(Text).object<jstring>()
                                       );
    }
}

QString QAndroidSharing::getSharedText()
{
    QString SharedText;

    if(m_JavaSharing.isValid())
    {
        const QAndroidJniObject SharedTextObj = m_JavaSharing.callObjectMethod("getSharedText", "()Ljava/lang/String;");

        if(SharedTextObj.isValid())
        {
            SharedText = SharedTextObj.toString();
        }
    }

    return SharedText;
}

QByteArray QAndroidSharing::getSharedData()
{
    QByteArray SharedData;

    if(m_JavaSharing.isValid())
    {
        const QAndroidJniObject SharedDataObj = m_JavaSharing.callObjectMethod("getSharedData", "()[B");

        if(SharedDataObj.isValid())
        {
            const jbyteArray DataArray = SharedDataObj.object<jbyteArray>();
            QAndroidJniEnvironment pEnv;
            int ArraySize;
            jbyte *pData;

            ArraySize = pEnv->GetArrayLength(DataArray);
            pData = pEnv->GetByteArrayElements(DataArray, NULL);
            SharedData = QByteArray(reinterpret_cast<const char*>(pData), ArraySize);
            pEnv->ReleaseByteArrayElements(DataArray, pData, JNI_ABORT);
        }
    }

    return SharedData;
}

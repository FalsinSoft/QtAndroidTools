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
#include "QAndroidUserMessagingPlatform.h"

QAndroidUserMessagingPlatform *QAndroidUserMessagingPlatform::m_pInstance = nullptr;

QAndroidUserMessagingPlatform::QAndroidUserMessagingPlatform(QObject *parent) : QObject(parent),
                                                                                m_javaUserMessagingPlatform("com/falsinsoft/qtandroidtools/AndroidUserMessagingPlatform",
                                                                                                            "(Landroid/app/Activity;)V",
                                                                                                            QtAndroid::androidActivity().object<jobject>())
{
    m_pInstance = this;

    if(m_javaUserMessagingPlatform.isValid())
    {
        const JNINativeMethod jniMethod[] = {
            {"consentFormRequestResult", "(I)V", reinterpret_cast<void*>(&QAndroidUserMessagingPlatform::deviceConsentFormRequestResult)},
            {"consentFormClosed", "()V", reinterpret_cast<void*>(&QAndroidUserMessagingPlatform::deviceConsentFormClosed)},
        };
        QAndroidJniEnvironment jniEnv;
        jclass objectClass;

        objectClass = jniEnv->GetObjectClass(m_javaUserMessagingPlatform.object<jobject>());
        jniEnv->RegisterNatives(objectClass, jniMethod, sizeof(jniMethod)/sizeof(JNINativeMethod));
        jniEnv->DeleteLocalRef(objectClass);
    }
}

QObject* QAndroidUserMessagingPlatform::qmlInstance(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine);
    Q_UNUSED(scriptEngine);

    return new QAndroidUserMessagingPlatform();
}

QAndroidUserMessagingPlatform* QAndroidUserMessagingPlatform::instance()
{
    return m_pInstance;
}

void QAndroidUserMessagingPlatform::deviceConsentFormRequestResult(JNIEnv *env, jobject thiz, int eventId)
{
    Q_UNUSED(env)
    Q_UNUSED(thiz)

    if(m_pInstance != nullptr)
    {
        Q_EMIT m_pInstance->consentFormRequestResult(eventId);
    }
}

void QAndroidUserMessagingPlatform::deviceConsentFormClosed(JNIEnv *env, jobject thiz)
{
    Q_UNUSED(env)
    Q_UNUSED(thiz)

    if(m_pInstance != nullptr)
    {
        Q_EMIT m_pInstance->consentFormClosed();
    }
}

void QAndroidUserMessagingPlatform::requestConsentForm()
{
    if(m_javaUserMessagingPlatform.isValid())
    {
        m_javaUserMessagingPlatform.callMethod<void>("requestConsentForm");
    }
}

int QAndroidUserMessagingPlatform::consentStatus()
{
    if(m_javaUserMessagingPlatform.isValid())
    {
        return m_javaUserMessagingPlatform.callMethod<jint>("consentStatus");
    }
    return CONSENT_FORM_STATUS_UNKNOWN;
}

bool QAndroidUserMessagingPlatform::showConsentForm()
{
    if(m_javaUserMessagingPlatform.isValid())
    {
        return m_javaUserMessagingPlatform.callMethod<jboolean>("showConsentForm");
    }
    return false;
}

void QAndroidUserMessagingPlatform::resetConsentInformation()
{
    if(m_javaUserMessagingPlatform.isValid())
    {
        m_javaUserMessagingPlatform.callMethod<void>("resetConsentInformation");
    }
}

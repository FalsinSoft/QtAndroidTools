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
#include <QCoreApplication>
#include "QAndroidUserMessagingPlatform.h"

QAndroidUserMessagingPlatform *QAndroidUserMessagingPlatform::m_pInstance = nullptr;

QAndroidUserMessagingPlatform::QAndroidUserMessagingPlatform(QObject *parent) : QObject(parent),
                                                                                m_javaUserMessagingPlatform("com/falsinsoft/qtandroidtools/AndroidUserMessagingPlatform",
                                                                                                            "(Landroid/app/Activity;)V",
                                                                                                            QNativeInterface::QAndroidApplication::context())
{
    m_pInstance = this;

    if(m_javaUserMessagingPlatform.isValid())
    {
        const JNINativeMethod jniMethod[] = {
            {"consentFormRequestFailure", "(Ljava/lang/String;)V", reinterpret_cast<void*>(&QAndroidUserMessagingPlatform::deviceConsentFormRequestFailure)},
            {"consentFormDismissed", "(ZZ)V", reinterpret_cast<void*>(&QAndroidUserMessagingPlatform::deviceConsentFormDismissed)},
        };
        QJniEnvironment jniEnv;
        jclass objectClass;

        objectClass = jniEnv->GetObjectClass(m_javaUserMessagingPlatform.object<jobject>());
        jniEnv->RegisterNatives(objectClass, jniMethod, sizeof(jniMethod)/sizeof(JNINativeMethod));
        jniEnv->DeleteLocalRef(objectClass);
    }
}

QAndroidUserMessagingPlatform* QAndroidUserMessagingPlatform::create(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine);
    Q_UNUSED(scriptEngine);

    return new QAndroidUserMessagingPlatform();
}

QAndroidUserMessagingPlatform* QAndroidUserMessagingPlatform::instance()
{
    return m_pInstance;
}

void QAndroidUserMessagingPlatform::deviceConsentFormRequestFailure(JNIEnv *env, jobject thiz, jstring errorMessage)
{
    Q_UNUSED(env)
    Q_UNUSED(thiz)

    if(m_pInstance != nullptr)
    {
        QJniEnvironment jniEnv;
        Q_EMIT m_pInstance->consentFormRequestFailure(QString(jniEnv->GetStringUTFChars(errorMessage, NULL)));
    }
}

void QAndroidUserMessagingPlatform::deviceConsentFormDismissed(JNIEnv *env, jobject thiz, jboolean consentGathered, jboolean privacyOptionsRequired)
{
    Q_UNUSED(env)
    Q_UNUSED(thiz)

    if(m_pInstance != nullptr)
    {
        Q_EMIT m_pInstance->consentFormDismissed(consentGathered, privacyOptionsRequired);
    }
}

void QAndroidUserMessagingPlatform::loadAndShowConsentFormIfRequired(bool underAgeOfConsent)
{
    if(m_javaUserMessagingPlatform.isValid())
    {
        m_javaUserMessagingPlatform.callMethod<void>("loadAndShowConsentFormIfRequired",
                                                     "(Z)V",
                                                     underAgeOfConsent);
    }
}

void QAndroidUserMessagingPlatform::showPrivacyOptionsForm()
{
    if(m_javaUserMessagingPlatform.isValid())
    {
        m_javaUserMessagingPlatform.callMethod<void>("showPrivacyOptionsForm");
    }
}

bool QAndroidUserMessagingPlatform::canRequestAds()
{
    if(m_javaUserMessagingPlatform.isValid())
    {
        return m_javaUserMessagingPlatform.callMethod<jboolean>("canRequestAds");
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

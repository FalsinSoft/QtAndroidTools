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
#include "QAndroidAuthentication.h"

QAndroidAuthentication *QAndroidAuthentication::m_pInstance = nullptr;

QAndroidAuthentication::QAndroidAuthentication(QObject *parent) : QObject(parent),
                                                                  m_javaAuthentication("com/falsinsoft/qtandroidtools/AndroidAuthentication",
                                                                                       QNativeInterface::QAndroidApplication::context()),
                                                                  m_authenticators(0)
{
    m_pInstance = this;

    if(m_javaAuthentication.isValid())
    {
        const JNINativeMethod jniMethod[] = {
            {"authenticationError", "(ILjava/lang/String;)V", reinterpret_cast<void*>(&QAndroidAuthentication::authenticationError)},
            {"authenticationSucceeded", "()V", reinterpret_cast<void*>(&QAndroidAuthentication::authenticationSucceeded)},
            {"authenticationFailed", "()V", reinterpret_cast<void*>(&QAndroidAuthentication::authenticationFailed)},
            {"authenticationCancelled", "()V", reinterpret_cast<void*>(&QAndroidAuthentication::authenticationCancelled)}
        };
        QJniEnvironment jniEnv;
        jclass objectClass;

        objectClass = jniEnv->GetObjectClass(m_javaAuthentication.object<jobject>());
        jniEnv->RegisterNatives(objectClass, jniMethod, sizeof(jniMethod)/sizeof(JNINativeMethod));
        jniEnv->DeleteLocalRef(objectClass);
    }
}

QAndroidAuthentication* QAndroidAuthentication::create(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine);
    Q_UNUSED(scriptEngine);

    return new QAndroidAuthentication();
}

QAndroidAuthentication* QAndroidAuthentication::instance()
{
    return m_pInstance;
}

int QAndroidAuthentication::getAuthenticators() const
{
    return m_authenticators;
}

void QAndroidAuthentication::setAuthenticators(int authenticators)
{
    if(m_javaAuthentication.isValid())
    {
        m_javaAuthentication.callMethod<void>("setAuthenticators",
                                              "(I)V",
                                              authenticators
                                              );
        m_authenticators = authenticators;
    }
}

QString QAndroidAuthentication::getTitle() const
{
    return m_title;
}

void QAndroidAuthentication::setTitle(const QString &title)
{
    if(m_javaAuthentication.isValid())
    {
        m_javaAuthentication.callMethod<void>("setTitle",
                                              "(Ljava/lang/String;)V",
                                              QJniObject::fromString(title).object<jstring>()
                                              );
        m_title = title;
    }
}

QString QAndroidAuthentication::getDescription() const
{
    return m_description;
}

void QAndroidAuthentication::setDescription(const QString &description)
{
    if(m_javaAuthentication.isValid())
    {
        m_javaAuthentication.callMethod<void>("setDescription",
                                              "(Ljava/lang/String;)V",
                                              QJniObject::fromString(description).object<jstring>()
                                              );
        m_description = description;
    }
}

QString QAndroidAuthentication::getNegativeButton() const
{
    return m_negativeButton;
}

void QAndroidAuthentication::setNegativeButton(const QString &negativeButton)
{
    if(m_javaAuthentication.isValid())
    {
        m_javaAuthentication.callMethod<void>("setNegativeButton",
                                              "(Ljava/lang/String;)V",
                                              QJniObject::fromString(negativeButton).object<jstring>()
                                              );
        m_negativeButton = negativeButton;
    }
}

int QAndroidAuthentication::canAuthenticate()
{
    if(m_javaAuthentication.isValid())
    {
        return m_javaAuthentication.callMethod<jint>("canAuthenticate");
    }

    return BIOMETRIC_STATUS_UNKNOWN;
}

bool QAndroidAuthentication::requestBiometricEnroll()
{
    if(m_javaAuthentication.isValid())
    {
        return m_javaAuthentication.callMethod<jboolean>("requestBiometricEnroll");
    }

    return false;
}

bool QAndroidAuthentication::authenticate()
{
    if(m_javaAuthentication.isValid())
    {
        return m_javaAuthentication.callMethod<jboolean>("authenticate");
    }

    return false;
}

void QAndroidAuthentication::cancel()
{
    if(m_javaAuthentication.isValid())
    {
        m_javaAuthentication.callMethod<void>("cancel");
    }
}

void QAndroidAuthentication::authenticationError(JNIEnv *env, jobject thiz, jint errorCode, jstring errString)
{
    Q_UNUSED(env)
    Q_UNUSED(thiz)

    if(m_pInstance != nullptr)
    {
        Q_EMIT m_pInstance->error(errorCode, QJniObject(errString).toString());
    }
}

void QAndroidAuthentication::authenticationSucceeded(JNIEnv *env, jobject thiz)
{
    Q_UNUSED(env)
    Q_UNUSED(thiz)

    if(m_pInstance != nullptr)
    {
        Q_EMIT m_pInstance->succeeded();
    }
}

void QAndroidAuthentication::authenticationFailed(JNIEnv *env, jobject thiz)
{
    Q_UNUSED(env)
    Q_UNUSED(thiz)

    if(m_pInstance != nullptr)
    {
        Q_EMIT m_pInstance->failed();
    }
}

void QAndroidAuthentication::authenticationCancelled(JNIEnv *env, jobject thiz)
{
    Q_UNUSED(env)
    Q_UNUSED(thiz)

    if(m_pInstance != nullptr)
    {
        Q_EMIT m_pInstance->cancelled();
    }
}

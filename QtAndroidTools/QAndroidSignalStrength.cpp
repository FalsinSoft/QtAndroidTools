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
#include "QAndroidSignalStrength.h"

QAndroidSignalStrength *QAndroidSignalStrength::m_pInstance = nullptr;

QAndroidSignalStrength::QAndroidSignalStrength(QObject *parent) : QObject(parent),
                                                                  m_javaSignalStrength("com/falsinsoft/qtandroidtools/AndroidSignalStrength",
                                                                                       "(Landroid/app/Activity;)V",
                                                                                       QNativeInterface::QAndroidApplication::context())
{
    m_pInstance = this;

    if(m_javaSignalStrength.isValid())
    {
        const JNINativeMethod jniMethod[] = {
            {"signalStrengthChanged", "()V", reinterpret_cast<void *>(&QAndroidSignalStrength::deviceSignalStrengthChanged)},
        };
        QJniEnvironment jniEnv;
        jclass objectClass;

        objectClass = jniEnv->GetObjectClass(m_javaSignalStrength.object<jobject>());
        jniEnv->RegisterNatives(objectClass, jniMethod, sizeof(jniMethod)/sizeof(JNINativeMethod));
        jniEnv->DeleteLocalRef(objectClass);
    }
    connect(qGuiApp, &QGuiApplication::applicationStateChanged, this, &QAndroidSignalStrength::applicationStateChanged);
    setNewAppState(APP_STATE_CREATE);
}

QAndroidSignalStrength::~QAndroidSignalStrength()
{
    setNewAppState(APP_STATE_DESTROY);
}

QObject* QAndroidSignalStrength::qmlInstance(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine);
    Q_UNUSED(scriptEngine);

    return new QAndroidSignalStrength();
}

QAndroidSignalStrength* QAndroidSignalStrength::instance()
{
    return m_pInstance;
}

void QAndroidSignalStrength::deviceSignalStrengthChanged(JNIEnv *env, jobject thiz)
{
    Q_UNUSED(env)
    Q_UNUSED(thiz)

    if(m_pInstance != nullptr)
    {
        Q_EMIT m_pInstance->signalStrengthChanged();
        Q_EMIT m_pInstance->signalLevelChanged();
    }
}

int QAndroidSignalStrength::getSignalStrength()
{
    if(m_javaSignalStrength.isValid())
    {
        return m_javaSignalStrength.callMethod<jint>("getSignalStrength");
    }
    return 0;
}

QAndroidSignalStrength::SIGNAL_LEVEL QAndroidSignalStrength::getSignalLevel()
{
    const int signalStrength = getSignalStrength();
    SIGNAL_LEVEL signalLevel;

    if(signalStrength >= 30)
        signalLevel = LEVEL_GREAT;
    else if(signalStrength >= 20)
        signalLevel = LEVEL_GOOD;
    else if(signalStrength >= 12)
        signalLevel = LEVEL_MODERATE;
    else if(signalStrength >= 5)
        signalLevel = LEVEL_POOR;
    else
        signalLevel = LEVEL_NONE;

    return signalLevel;
}

void QAndroidSignalStrength::applicationStateChanged(Qt::ApplicationState state)
{
    setNewAppState((state == Qt::ApplicationActive) ? APP_STATE_START : APP_STATE_STOP);
}

void QAndroidSignalStrength::setNewAppState(APP_STATE newState)
{
    if(m_javaSignalStrength.isValid())
    {
        m_javaSignalStrength.callMethod<void>("appStateChanged",
                                              "(I)V",
                                              newState
                                              );
    }
}

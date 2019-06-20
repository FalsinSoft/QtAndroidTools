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

QAndroidSignalStrength::QAndroidSignalStrength() : m_JavaSignalStrength("com/falsinsoft/qtandroidtools/AndroidSignalStrength",
                                                                        "(Landroid/app/Activity;)V",
                                                                        QtAndroid::androidActivity().object<jobject>())
{
    m_pInstance = this;

    if(m_JavaSignalStrength.isValid())
    {
        const JNINativeMethod JniMethod[] = {
            {"signalStrengthChanged", "()V", reinterpret_cast<void *>(&QAndroidSignalStrength::SignalStrengthChanged)},
        };
        QAndroidJniEnvironment JniEnv;
        jclass ObjectClass;

        ObjectClass = JniEnv->GetObjectClass(m_JavaSignalStrength.object<jobject>());
        JniEnv->RegisterNatives(ObjectClass, JniMethod, sizeof(JniMethod)/sizeof(JNINativeMethod));
        JniEnv->DeleteLocalRef(ObjectClass);
    }
    connect(qGuiApp, &QGuiApplication::applicationStateChanged, this, &QAndroidSignalStrength::ApplicationStateChanged);
    SetNewAppState(APP_STATE_CREATE);
}

QAndroidSignalStrength::~QAndroidSignalStrength()
{
    SetNewAppState(APP_STATE_DESTROY);
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

void QAndroidSignalStrength::SignalStrengthChanged(JNIEnv *env, jobject thiz)
{
    Q_UNUSED(env)
    Q_UNUSED(thiz)

    if(m_pInstance != nullptr)
    {
        emit m_pInstance->signalStrengthChanged();
        emit m_pInstance->signalLevelChanged();
    }
}

int QAndroidSignalStrength::getSignalStrength()
{
    if(m_JavaSignalStrength.isValid())
    {
        return m_JavaSignalStrength.callMethod<jint>("getSignalStrength");
    }
    return 0;
}

QAndroidSignalStrength::SIGNAL_LEVEL QAndroidSignalStrength::getSignalLevel()
{
    const int SignalStrength = getSignalStrength();
    SIGNAL_LEVEL SignalLevel;

    if(SignalStrength >= 30)
        SignalLevel = LEVEL_GREAT;
    else if(SignalStrength >= 20)
        SignalLevel = LEVEL_GOOD;
    else if(SignalStrength >= 12)
        SignalLevel = LEVEL_MODERATE;
    else if(SignalStrength >= 5)
        SignalLevel = LEVEL_POOR;
    else
        SignalLevel = LEVEL_NONE;

    return SignalLevel;
}

void QAndroidSignalStrength::ApplicationStateChanged(Qt::ApplicationState State)
{
    SetNewAppState((State == Qt::ApplicationActive) ? APP_STATE_START : APP_STATE_STOP);
}

void QAndroidSignalStrength::SetNewAppState(APP_STATE NewState)
{
    if(m_JavaSignalStrength.isValid())
    {
        m_JavaSignalStrength.callMethod<void>("appStateChanged",
                                              "(I)V",
                                              NewState
                                              );
    }
}

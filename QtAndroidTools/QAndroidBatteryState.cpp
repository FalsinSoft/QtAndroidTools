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
#include "QAndroidBatteryState.h"

QAndroidBatteryState *QAndroidBatteryState::m_pInstance = nullptr;

QAndroidBatteryState::QAndroidBatteryState() : m_JavaBatteryState("com/falsinsoft/qtandroidtools/AndroidBatteryState",
                                                                  "(Landroid/app/Activity;)V",
                                                                  QtAndroid::androidActivity().object<jobject>())
{
    m_pInstance = this;

    if(m_JavaBatteryState.isValid())
    {
        const JNINativeMethod JniMethod[] = {
            {"batteryLevelChanged", "()V", reinterpret_cast<void *>(&QAndroidBatteryState::BatteryLevelChanged)},
            {"batteryOnChargeChanged", "()V", reinterpret_cast<void *>(&QAndroidBatteryState::BatteryOnChargeChanged)}
        };
        QAndroidJniEnvironment JniEnv;
        jclass ObjectClass;

        ObjectClass = JniEnv->GetObjectClass(m_JavaBatteryState.object<jobject>());
        JniEnv->RegisterNatives(ObjectClass, JniMethod, sizeof(JniMethod)/sizeof(JNINativeMethod));
        JniEnv->DeleteLocalRef(ObjectClass);
    }
    connect(qGuiApp, &QGuiApplication::applicationStateChanged, this, &QAndroidBatteryState::ApplicationStateChanged);
    SetNewAppState(APP_STATE_CREATE);
}

QAndroidBatteryState::~QAndroidBatteryState()
{
    SetNewAppState(APP_STATE_DESTROY);
}

QObject* QAndroidBatteryState::qmlInstance(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine);
    Q_UNUSED(scriptEngine);

    return new QAndroidBatteryState();
}

QAndroidBatteryState* QAndroidBatteryState::instance()
{
    return m_pInstance;
}

void QAndroidBatteryState::BatteryLevelChanged(JNIEnv *env, jobject thiz)
{
    Q_UNUSED(env)
    Q_UNUSED(thiz)

    if(m_pInstance != nullptr)
    {
        emit m_pInstance->levelChanged();
    }
}

void QAndroidBatteryState::BatteryOnChargeChanged(JNIEnv *env, jobject thiz)
{
    Q_UNUSED(env)
    Q_UNUSED(thiz)

    if(m_pInstance != nullptr)
    {
        emit m_pInstance->onChargeChanged();
    }
}

int QAndroidBatteryState::getLevel()
{
    if(m_JavaBatteryState.isValid())
    {
        return m_JavaBatteryState.callMethod<jint>("getLevel");
    }
    return 0;
}

bool QAndroidBatteryState::isOnCharge()
{
    if(m_JavaBatteryState.isValid())
    {
        return m_JavaBatteryState.callMethod<jboolean>("isOnCharge");
    }
    return false;
}

void QAndroidBatteryState::ApplicationStateChanged(Qt::ApplicationState State)
{
    SetNewAppState((State == Qt::ApplicationActive) ? APP_STATE_START : APP_STATE_STOP);
}

void QAndroidBatteryState::SetNewAppState(APP_STATE NewState)
{
    if(m_JavaBatteryState.isValid())
    {
        m_JavaBatteryState.callMethod<void>("appStateChanged",
                                            "(I)V",
                                            NewState
                                            );
    }
}

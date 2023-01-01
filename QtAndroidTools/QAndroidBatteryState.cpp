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

QAndroidBatteryState::QAndroidBatteryState(QObject *parent) : QObject(parent),
                                                              m_javaBatteryState("com/falsinsoft/qtandroidtools/AndroidBatteryState",
                                                                                 "(Landroid/app/Activity;)V",
                                                                                 QNativeInterface::QAndroidApplication::context())
{
    m_pInstance = this;

    if(m_javaBatteryState.isValid())
    {
        const JNINativeMethod jniMethod[] = {
            {"batteryLevelChanged", "()V", reinterpret_cast<void *>(&QAndroidBatteryState::batteryLevelChanged)},
            {"batteryOnChargeChanged", "()V", reinterpret_cast<void *>(&QAndroidBatteryState::batteryOnChargeChanged)}
        };
        QJniEnvironment jniEnv;
        jclass objectClass;

        objectClass = jniEnv->GetObjectClass(m_javaBatteryState.object<jobject>());
        jniEnv->RegisterNatives(objectClass, jniMethod, sizeof(jniMethod)/sizeof(JNINativeMethod));
        jniEnv->DeleteLocalRef(objectClass);
    }
    connect(qGuiApp, &QGuiApplication::applicationStateChanged, this, &QAndroidBatteryState::applicationStateChanged);
    setNewAppState(APP_STATE_CREATE);
}

QAndroidBatteryState::~QAndroidBatteryState()
{
    setNewAppState(APP_STATE_DESTROY);
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

void QAndroidBatteryState::batteryLevelChanged(JNIEnv *env, jobject thiz)
{
    Q_UNUSED(env)
    Q_UNUSED(thiz)

    if(m_pInstance != nullptr)
    {
        Q_EMIT m_pInstance->levelChanged();
    }
}

void QAndroidBatteryState::batteryOnChargeChanged(JNIEnv *env, jobject thiz)
{
    Q_UNUSED(env)
    Q_UNUSED(thiz)

    if(m_pInstance != nullptr)
    {
        Q_EMIT m_pInstance->onChargeChanged();
    }
}

int QAndroidBatteryState::getLevel()
{
    if(m_javaBatteryState.isValid())
    {
        return m_javaBatteryState.callMethod<jint>("getLevel");
    }
    return 0;
}

bool QAndroidBatteryState::isOnCharge()
{
    if(m_javaBatteryState.isValid())
    {
        return m_javaBatteryState.callMethod<jboolean>("isOnCharge");
    }
    return false;
}

void QAndroidBatteryState::applicationStateChanged(Qt::ApplicationState state)
{
    setNewAppState((state == Qt::ApplicationActive) ? APP_STATE_START : APP_STATE_STOP);
}

void QAndroidBatteryState::setNewAppState(APP_STATE newState)
{
    if(m_javaBatteryState.isValid())
    {
        m_javaBatteryState.callMethod<void>("appStateChanged",
                                            "(I)V",
                                            newState
                                            );
    }
}

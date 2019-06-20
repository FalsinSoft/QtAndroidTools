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
#include "QAndroidAdMobInterstitial.h"

QMap<int, QAndroidAdMobInterstitial*> QAndroidAdMobInterstitial::m_pInstancesMap;
int QAndroidAdMobInterstitial::m_InstancesCounter = 0;

QAndroidAdMobInterstitial::QAndroidAdMobInterstitial(QQuickItem *parent) : QQuickItem(parent),
                                                                           m_JavaAdMobInterstitial("com/falsinsoft/qtandroidtools/AndroidAdMobInterstitial",
                                                                                                   "(Landroid/app/Activity;)V",
                                                                                                   QtAndroid::androidActivity().object<jobject>()),
                                                                           m_InstanceIndex(m_InstancesCounter++)
{
    m_pInstancesMap[m_InstanceIndex] = this;

    if(m_InstanceIndex == 0 && m_JavaAdMobInterstitial.isValid())
    {
        const JNINativeMethod JniMethod[] = {
            {"interstitialEvent", "(I)V", reinterpret_cast<void *>(&QAndroidAdMobInterstitial::InterstitialEvent)},
            {"interstitialError", "(I)V", reinterpret_cast<void *>(&QAndroidAdMobInterstitial::InterstitialError)}
        };
        QAndroidJniEnvironment JniEnv;
        jclass ObjectClass;

        ObjectClass = JniEnv->GetObjectClass(m_JavaAdMobInterstitial.object<jobject>());
        JniEnv->RegisterNatives(ObjectClass, JniMethod, sizeof(JniMethod)/sizeof(JNINativeMethod));
        JniEnv->DeleteLocalRef(ObjectClass);
    }
    SetNewAppState(APP_STATE_CREATE);
}

QAndroidAdMobInterstitial::~QAndroidAdMobInterstitial()
{
    m_pInstancesMap.remove(m_InstanceIndex);
    SetNewAppState(APP_STATE_DESTROY);
}

const QMap<int, QAndroidAdMobInterstitial*>& QAndroidAdMobInterstitial::Instances()
{
    return m_pInstancesMap;
}

bool QAndroidAdMobInterstitial::show()
{
    if(m_JavaAdMobInterstitial.isValid() && m_UnitId.isEmpty() == false)
    {
        m_JavaAdMobInterstitial.callMethod<void>("show");
        return true;
    }

    return false;
}

bool QAndroidAdMobInterstitial::load()
{
    if(m_JavaAdMobInterstitial.isValid() && m_UnitId.isEmpty() == false)
    {
        m_JavaAdMobInterstitial.callMethod<void>("load");
        return true;
    }

    return false;
}

const QString& QAndroidAdMobInterstitial::getUnitId() const
{
    return m_UnitId;
}

void QAndroidAdMobInterstitial::setUnitId(const QString &UnitId)
{
    if(m_JavaAdMobInterstitial.isValid())
    {
        m_JavaAdMobInterstitial.callMethod<void>("setUnitId",
                                                 "(Ljava/lang/String;)V",
                                                 QAndroidJniObject::fromString(UnitId).object<jstring>()
                                                 );
        m_UnitId = UnitId;
    }
}

void QAndroidAdMobInterstitial::InterstitialEvent(JNIEnv *env, jobject thiz, jint eventId)
{
    QMapIterator<int, QAndroidAdMobInterstitial*> Instance(m_pInstancesMap);

    Q_UNUSED(env)
    Q_UNUSED(thiz)

    while(Instance.hasNext())
    {
        Instance.next();
        switch(eventId)
        {
            case EVENT_LOADING:
                emit Instance.value()->loading();
                break;
            case EVENT_LOADED:
                emit Instance.value()->loaded();
                break;
            case EVENT_CLOSED:
                emit Instance.value()->closed();
                break;
            case EVENT_CLICKED:
                emit Instance.value()->clicked();
                break;
        }
    }
}

void QAndroidAdMobInterstitial::InterstitialError(JNIEnv *env, jobject thiz, jint errorId)
{
    QMapIterator<int, QAndroidAdMobInterstitial*> Instance(m_pInstancesMap);

    Q_UNUSED(env)
    Q_UNUSED(thiz)

    while(Instance.hasNext())
    {
        Instance.next();
        emit Instance.value()->loadError(errorId);
    }
}

void QAndroidAdMobInterstitial::SetNewAppState(APP_STATE NewState)
{
    if(m_JavaAdMobInterstitial.isValid())
    {
        m_JavaAdMobInterstitial.callMethod<void>("appStateChanged",
                                                 "(I)V",
                                                 NewState
                                                 );
    }
}

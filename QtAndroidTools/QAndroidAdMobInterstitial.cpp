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
int QAndroidAdMobInterstitial::m_instancesCounter = 0;

QAndroidAdMobInterstitial::QAndroidAdMobInterstitial(QQuickItem *parent) : QQuickItem(parent),
                                                                           m_javaAdMobInterstitial("com/falsinsoft/qtandroidtools/AndroidAdMobInterstitial",
                                                                                                   "(Landroid/app/Activity;)V",
                                                                                                   QtAndroid::androidActivity().object<jobject>()),
                                                                           m_instanceIndex(m_instancesCounter++),
                                                                           m_nonPersonalizedAds(false)
{
    m_pInstancesMap[m_instanceIndex] = this;

    if(m_instanceIndex == 0 && m_javaAdMobInterstitial.isValid())
    {
        const JNINativeMethod jniMethod[] = {
            {"interstitialEvent", "(I)V", reinterpret_cast<void *>(&QAndroidAdMobInterstitial::interstitialEvent)},
            {"interstitialError", "(I)V", reinterpret_cast<void *>(&QAndroidAdMobInterstitial::interstitialError)}
        };
        QAndroidJniEnvironment jniEnv;
        jclass objectClass;

        objectClass = jniEnv->GetObjectClass(m_javaAdMobInterstitial.object<jobject>());
        jniEnv->RegisterNatives(objectClass, jniMethod, sizeof(jniMethod)/sizeof(JNINativeMethod));
        jniEnv->DeleteLocalRef(objectClass);
    }
    setNewAppState(APP_STATE_CREATE);
}

QAndroidAdMobInterstitial::~QAndroidAdMobInterstitial()
{
    m_pInstancesMap.remove(m_instanceIndex);
    setNewAppState(APP_STATE_DESTROY);
}

const QMap<int, QAndroidAdMobInterstitial*>& QAndroidAdMobInterstitial::instances()
{
    return m_pInstancesMap;
}

bool QAndroidAdMobInterstitial::show()
{
    if(m_javaAdMobInterstitial.isValid() && m_unitId.isEmpty() == false)
    {
        m_javaAdMobInterstitial.callMethod<void>("show");
        return true;
    }

    return false;
}

bool QAndroidAdMobInterstitial::load()
{
    if(m_javaAdMobInterstitial.isValid() && m_unitId.isEmpty() == false)
    {
        m_javaAdMobInterstitial.callMethod<void>("load");
        return true;
    }

    return false;
}

const QString& QAndroidAdMobInterstitial::getUnitId() const
{
    return m_unitId;
}

void QAndroidAdMobInterstitial::setUnitId(const QString &unitId)
{
    if(m_javaAdMobInterstitial.isValid())
    {
        m_javaAdMobInterstitial.callMethod<void>("setUnitId",
                                                 "(Ljava/lang/String;)V",
                                                 QAndroidJniObject::fromString(unitId).object<jstring>()
                                                 );
        m_unitId = unitId;
    }
}

bool QAndroidAdMobInterstitial::getNonPersonalizedAds() const
{
    return m_nonPersonalizedAds;
}

void QAndroidAdMobInterstitial::setNonPersonalizedAds(bool npa)
{
    if(m_javaAdMobInterstitial.isValid())
    {
        m_javaAdMobInterstitial.callMethod<void>("setNonPersonalizedAds",
                                                 "(Z)V",
                                                 npa
                                                 );
        m_nonPersonalizedAds = npa;
    }
}

void QAndroidAdMobInterstitial::interstitialEvent(JNIEnv *env, jobject thiz, jint eventId)
{
    QMapIterator<int, QAndroidAdMobInterstitial*> instance(m_pInstancesMap);

    Q_UNUSED(env)
    Q_UNUSED(thiz)

    while(instance.hasNext())
    {
        instance.next();
        switch(eventId)
        {
            case EVENT_LOADING:
                Q_EMIT instance.value()->loading();
                break;
            case EVENT_LOADED:
                Q_EMIT instance.value()->loaded();
                break;
            case EVENT_CLOSED:
                Q_EMIT instance.value()->closed();
                break;
            case EVENT_CLICKED:
                Q_EMIT instance.value()->clicked();
                break;
        }
    }
}

void QAndroidAdMobInterstitial::interstitialError(JNIEnv *env, jobject thiz, jint errorId)
{
    QMapIterator<int, QAndroidAdMobInterstitial*> instance(m_pInstancesMap);

    Q_UNUSED(env)
    Q_UNUSED(thiz)

    while(instance.hasNext())
    {
        instance.next();
        Q_EMIT instance.value()->loadError(errorId);
    }
}

void QAndroidAdMobInterstitial::setNewAppState(APP_STATE newState)
{
    if(m_javaAdMobInterstitial.isValid())
    {
        m_javaAdMobInterstitial.callMethod<void>("appStateChanged",
                                                 "(I)V",
                                                 newState
                                                 );
    }
}

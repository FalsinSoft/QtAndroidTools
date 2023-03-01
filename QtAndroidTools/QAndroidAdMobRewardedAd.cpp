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
#include "QAndroidAdMobRewardedAd.h"

QMap<int, QAndroidAdMobRewardedAd*> QAndroidAdMobRewardedAd::m_pInstancesMap;
int QAndroidAdMobRewardedAd::m_instancesCounter = 0;

QAndroidAdMobRewardedAd::QAndroidAdMobRewardedAd(QQuickItem *parent) : QQuickItem(parent),
                                                                       m_javaAdMobRewardedAd("com/falsinsoft/qtandroidtools/AndroidAdMobRewardedAd",
                                                                                             "(Landroid/app/Activity;)V",
                                                                                             QNativeInterface::QAndroidApplication::context()),
                                                                       m_instanceIndex(m_instancesCounter++),
                                                                       m_nonPersonalizedAds(false)
{
    m_pInstancesMap[m_instanceIndex] = this;

    if(m_instanceIndex == 0 && m_javaAdMobRewardedAd.isValid())
    {
        const JNINativeMethod jniMethod[] = {
            {"rewardedAdReward", "(Ljava/lang/String;I)V", reinterpret_cast<void *>(&QAndroidAdMobRewardedAd::rewardedAdReward)},
            {"rewardedAdEvent", "(I)V", reinterpret_cast<void *>(&QAndroidAdMobRewardedAd::rewardedAdEvent)}
        };
        QJniEnvironment jniEnv;
        jclass objectClass;

        objectClass = jniEnv->GetObjectClass(m_javaAdMobRewardedAd.object<jobject>());
        jniEnv->RegisterNatives(objectClass, jniMethod, sizeof(jniMethod)/sizeof(JNINativeMethod));
        jniEnv->DeleteLocalRef(objectClass);
    }
}

QAndroidAdMobRewardedAd::~QAndroidAdMobRewardedAd()
{
    m_pInstancesMap.remove(m_instanceIndex);
}

const QMap<int, QAndroidAdMobRewardedAd*>& QAndroidAdMobRewardedAd::instances()
{
    return m_pInstancesMap;
}

bool QAndroidAdMobRewardedAd::show()
{
    if(m_javaAdMobRewardedAd.isValid() && m_unitId.isEmpty() == false)
    {
        m_javaAdMobRewardedAd.callMethod<void>("show");
        return true;
    }

    return false;
}

bool QAndroidAdMobRewardedAd::load()
{
    if(m_javaAdMobRewardedAd.isValid() && m_unitId.isEmpty() == false)
    {
        m_javaAdMobRewardedAd.callMethod<void>("load");
        return true;
    }

    return false;
}

const QString& QAndroidAdMobRewardedAd::getUnitId() const
{
    return m_unitId;
}

void QAndroidAdMobRewardedAd::setUnitId(const QString &unitId)
{
    if(m_javaAdMobRewardedAd.isValid())
    {
        m_javaAdMobRewardedAd.callMethod<void>("setUnitId",
                                               "(Ljava/lang/String;)V",
                                               QJniObject::fromString(unitId).object<jstring>()
                                               );
        m_unitId = unitId;
    }
}

const QString& QAndroidAdMobRewardedAd::getCustomData() const
{
    return m_customData;
}

void QAndroidAdMobRewardedAd::setCustomData(const QString &customData)
{
    if(m_javaAdMobRewardedAd.isValid())
    {
        m_javaAdMobRewardedAd.callMethod<void>("setCustomData",
                                               "(Ljava/lang/String;)V",
                                               QJniObject::fromString(customData).object<jstring>()
                                               );
        m_customData = customData;
    }
}

bool QAndroidAdMobRewardedAd::getNonPersonalizedAds() const
{
    return m_nonPersonalizedAds;
}

void QAndroidAdMobRewardedAd::setNonPersonalizedAds(bool npa)
{
    if(m_javaAdMobRewardedAd.isValid())
    {
        m_javaAdMobRewardedAd.callMethod<void>("setNonPersonalizedAds",
                                               "(Z)V",
                                               npa
                                               );
        m_nonPersonalizedAds = npa;
    }
}

void QAndroidAdMobRewardedAd::rewardedAdReward(JNIEnv *env, jobject thiz, jstring type, jint amount)
{
    QMapIterator<int, QAndroidAdMobRewardedAd*> instance(m_pInstancesMap);
    QJniEnvironment jniEnv;

    Q_UNUSED(env)
    Q_UNUSED(thiz)

    while(instance.hasNext())
    {
        instance.next();
        Q_EMIT instance.value()->rewarded(QString(jniEnv->GetStringUTFChars(type, NULL)), amount);
    }
}

void QAndroidAdMobRewardedAd::rewardedAdEvent(JNIEnv *env, jobject thiz, jint eventId)
{
    QMapIterator<int, QAndroidAdMobRewardedAd*> instance(m_pInstancesMap);

    Q_UNUSED(env)
    Q_UNUSED(thiz)

    while(instance.hasNext())
    {
        instance.next();
        switch(eventId)
        {
            case EVENT_LOAD_ERROR:
                Q_EMIT instance.value()->loadError();
                break;
            case EVENT_LOADING:
                Q_EMIT instance.value()->loading();
                break;
            case EVENT_LOADED:
                Q_EMIT instance.value()->loaded();
                break;
            case EVENT_CLICKED:
                Q_EMIT instance.value()->clicked();
                break;
            case EVENT_DISMISSED:
                Q_EMIT instance.value()->dismissed();
                break;
            case EVENT_SHOW_FAILED:
                Q_EMIT instance.value()->showFailed();
                break;
            case EVENT_IMPRESSION:
                Q_EMIT instance.value()->impression();
                break;
            case EVENT_SHOWED:
                Q_EMIT instance.value()->showed();
                break;
        }
    }
}

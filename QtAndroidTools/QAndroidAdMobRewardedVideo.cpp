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
#include "QAndroidAdMobRewardedVideo.h"

QMap<int, QAndroidAdMobRewardedVideo*> QAndroidAdMobRewardedVideo::m_pInstancesMap;
int QAndroidAdMobRewardedVideo::m_instancesCounter = 0;

QAndroidAdMobRewardedVideo::QAndroidAdMobRewardedVideo(QQuickItem *parent) : QQuickItem(parent),
                                                                             m_javaAdMobRewardedVideo("com/falsinsoft/qtandroidtools/AndroidAdMobRewardedVideo",
                                                                                                      "(Landroid/app/Activity;)V",
                                                                                                      QtAndroid::androidActivity().object<jobject>()),
                                                                             m_instanceIndex(m_instancesCounter++),
                                                                             m_nonPersonalizedAds(false)
{
    m_pInstancesMap[m_instanceIndex] = this;

    if(m_instanceIndex == 0 && m_javaAdMobRewardedVideo.isValid())
    {
        const JNINativeMethod jniMethod[] = {
            {"rewardedVideoReward", "(Ljava/lang/String;I)V", reinterpret_cast<void *>(&QAndroidAdMobRewardedVideo::rewardedVideoReward)},
            {"rewardedVideoEvent", "(I)V", reinterpret_cast<void *>(&QAndroidAdMobRewardedVideo::rewardedVideoEvent)},
            {"rewardedVideoError", "(I)V", reinterpret_cast<void *>(&QAndroidAdMobRewardedVideo::rewardedVideoError)}
        };
        QAndroidJniEnvironment jniEnv;
        jclass objectClass;

        objectClass = jniEnv->GetObjectClass(m_javaAdMobRewardedVideo.object<jobject>());
        jniEnv->RegisterNatives(objectClass, jniMethod, sizeof(jniMethod)/sizeof(JNINativeMethod));
        jniEnv->DeleteLocalRef(objectClass);
    }
    setNewAppState(APP_STATE_CREATE);
}

QAndroidAdMobRewardedVideo::~QAndroidAdMobRewardedVideo()
{
    m_pInstancesMap.remove(m_instanceIndex);
    setNewAppState(APP_STATE_DESTROY);
}

const QMap<int, QAndroidAdMobRewardedVideo*>& QAndroidAdMobRewardedVideo::instances()
{
    return m_pInstancesMap;
}

bool QAndroidAdMobRewardedVideo::show()
{
    if(m_javaAdMobRewardedVideo.isValid() && m_unitId.isEmpty() == false)
    {
        m_javaAdMobRewardedVideo.callMethod<void>("show");
        return true;
    }

    return false;
}

bool QAndroidAdMobRewardedVideo::load()
{
    if(m_javaAdMobRewardedVideo.isValid() && m_unitId.isEmpty() == false)
    {
        m_javaAdMobRewardedVideo.callMethod<void>("load",
                                                  "(Ljava/lang/String;)V",
                                                  QAndroidJniObject::fromString(m_unitId).object<jstring>()
                                                  );
        return true;
    }

    return false;
}

const QString& QAndroidAdMobRewardedVideo::getUnitId() const
{
    return m_unitId;
}

void QAndroidAdMobRewardedVideo::setUnitId(const QString &unitId)
{
    m_unitId = unitId;
}

bool QAndroidAdMobRewardedVideo::getNonPersonalizedAds() const
{
    return m_nonPersonalizedAds;
}

void QAndroidAdMobRewardedVideo::setNonPersonalizedAds(bool npa)
{
    if(m_javaAdMobRewardedVideo.isValid())
    {
        m_javaAdMobRewardedVideo.callMethod<void>("setNonPersonalizedAds",
                                                  "(Z)V",
                                                  npa
                                                  );
        m_nonPersonalizedAds = npa;
    }
}

void QAndroidAdMobRewardedVideo::rewardedVideoReward(JNIEnv *env, jobject thiz, jstring type, jint amount)
{
    QMapIterator<int, QAndroidAdMobRewardedVideo*> instance(m_pInstancesMap);
    QAndroidJniEnvironment jniEnv;

    Q_UNUSED(env)
    Q_UNUSED(thiz)

    while(instance.hasNext())
    {
        instance.next();
        Q_EMIT instance.value()->rewarded(QString(jniEnv->GetStringUTFChars(type, NULL)), amount);
    }
}

void QAndroidAdMobRewardedVideo::rewardedVideoEvent(JNIEnv *env, jobject thiz, jint eventId)
{
    QMapIterator<int, QAndroidAdMobRewardedVideo*> instance(m_pInstancesMap);

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
            case EVENT_OPENED:
                Q_EMIT instance.value()->opened();
                break;
            case EVENT_CLOSED:
                Q_EMIT instance.value()->closed();
                break;
            case EVENT_STARTED:
                Q_EMIT instance.value()->started();
                break;
            case EVENT_COMPLETED:
                Q_EMIT instance.value()->completed();
                break;
            case EVENT_LEFT_APPLICATION:
                Q_EMIT instance.value()->leftApplication();
                break;
        }
    }
}

void QAndroidAdMobRewardedVideo::rewardedVideoError(JNIEnv *env, jobject thiz, jint errorId)
{
    QMapIterator<int, QAndroidAdMobRewardedVideo*> instance(m_pInstancesMap);

    Q_UNUSED(env)
    Q_UNUSED(thiz)

    while(instance.hasNext())
    {
        instance.next();
        Q_EMIT instance.value()->loadError(errorId);
    }
}

void QAndroidAdMobRewardedVideo::setNewAppState(APP_STATE newState)
{
    if(m_javaAdMobRewardedVideo.isValid())
    {
        m_javaAdMobRewardedVideo.callMethod<void>("appStateChanged",
                                                  "(I)V",
                                                  newState
                                                  );
    }
}

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
int QAndroidAdMobRewardedVideo::m_InstancesCounter = 0;

QAndroidAdMobRewardedVideo::QAndroidAdMobRewardedVideo(QQuickItem *parent) : QQuickItem(parent),
                                                                             m_JavaAdMobRewardedVideo("com/falsinsoft/qtandroidtools/AndroidAdMobRewardedVideo",
                                                                                                      "(Landroid/app/Activity;)V",
                                                                                                      QtAndroid::androidActivity().object<jobject>()),
                                                                             m_InstanceIndex(m_InstancesCounter++)
{
    m_pInstancesMap[m_InstanceIndex] = this;

    if(m_InstanceIndex == 0 && m_JavaAdMobRewardedVideo.isValid())
    {
        const JNINativeMethod JniMethod[] = {
            {"rewardedVideoReward", "(Ljava/lang/String;I)V", reinterpret_cast<void *>(&QAndroidAdMobRewardedVideo::RewardedVideoReward)},
            {"rewardedVideoEvent", "(I)V", reinterpret_cast<void *>(&QAndroidAdMobRewardedVideo::RewardedVideoEvent)},
            {"rewardedVideoError", "(I)V", reinterpret_cast<void *>(&QAndroidAdMobRewardedVideo::RewardedVideoError)}
        };
        QAndroidJniEnvironment JniEnv;
        jclass ObjectClass;

        ObjectClass = JniEnv->GetObjectClass(m_JavaAdMobRewardedVideo.object<jobject>());
        JniEnv->RegisterNatives(ObjectClass, JniMethod, sizeof(JniMethod)/sizeof(JNINativeMethod));
        JniEnv->DeleteLocalRef(ObjectClass);
    }
    SetNewAppState(APP_STATE_CREATE);
}

QAndroidAdMobRewardedVideo::~QAndroidAdMobRewardedVideo()
{
    m_pInstancesMap.remove(m_InstanceIndex);
    SetNewAppState(APP_STATE_DESTROY);
}

const QMap<int, QAndroidAdMobRewardedVideo*>& QAndroidAdMobRewardedVideo::Instances()
{
    return m_pInstancesMap;
}

bool QAndroidAdMobRewardedVideo::show()
{
    if(m_JavaAdMobRewardedVideo.isValid() && m_UnitId.isEmpty() == false)
    {
        m_JavaAdMobRewardedVideo.callMethod<void>("show");
        return true;
    }

    return false;
}

bool QAndroidAdMobRewardedVideo::load()
{
    if(m_JavaAdMobRewardedVideo.isValid() && m_UnitId.isEmpty() == false)
    {
        m_JavaAdMobRewardedVideo.callMethod<void>("load",
                                                  "(Ljava/lang/String;)V",
                                                  QAndroidJniObject::fromString(m_UnitId).object<jstring>()
                                                  );
        return true;
    }

    return false;
}

const QString& QAndroidAdMobRewardedVideo::getUnitId() const
{
    return m_UnitId;
}

void QAndroidAdMobRewardedVideo::setUnitId(const QString &UnitId)
{
    m_UnitId = UnitId;
}

void QAndroidAdMobRewardedVideo::RewardedVideoReward(JNIEnv *env, jobject thiz, jstring type, jint amount)
{
    QMapIterator<int, QAndroidAdMobRewardedVideo*> Instance(m_pInstancesMap);
    QAndroidJniEnvironment JniEnv;

    Q_UNUSED(env)
    Q_UNUSED(thiz)

    while(Instance.hasNext())
    {
        Instance.next();
        emit Instance.value()->rewarded(QString(JniEnv->GetStringUTFChars(type, NULL)), amount);
    }
}

void QAndroidAdMobRewardedVideo::RewardedVideoEvent(JNIEnv *env, jobject thiz, jint eventId)
{
    QMapIterator<int, QAndroidAdMobRewardedVideo*> Instance(m_pInstancesMap);

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
            case EVENT_OPENED:
                emit Instance.value()->opened();
                break;
            case EVENT_CLOSED:
                emit Instance.value()->closed();
                break;
            case EVENT_STARTED:
                emit Instance.value()->started();
                break;
            case EVENT_COMPLETED:
                emit Instance.value()->completed();
                break;
            case EVENT_LEFT_APPLICATION:
                emit Instance.value()->leftApplication();
                break;
        }
    }
}

void QAndroidAdMobRewardedVideo::RewardedVideoError(JNIEnv *env, jobject thiz, jint errorId)
{
    QMapIterator<int, QAndroidAdMobRewardedVideo*> Instance(m_pInstancesMap);

    Q_UNUSED(env)
    Q_UNUSED(thiz)

    while(Instance.hasNext())
    {
        Instance.next();
        emit Instance.value()->loadError(errorId);
    }
}

void QAndroidAdMobRewardedVideo::SetNewAppState(APP_STATE NewState)
{
    if(m_JavaAdMobRewardedVideo.isValid())
    {
        m_JavaAdMobRewardedVideo.callMethod<void>("appStateChanged",
                                                  "(I)V",
                                                  NewState
                                                  );
    }
}

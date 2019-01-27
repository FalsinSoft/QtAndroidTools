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
#include "QAndroidAdMobBanner.h"

QMap<int, QAndroidAdMobBanner*> QAndroidAdMobBanner::m_pInstancesMap;
int QAndroidAdMobBanner::m_InstancesCounter = 0;

QAndroidAdMobBanner::QAndroidAdMobBanner() : m_JavaAdMobBanner("com/falsinsoft/qtandroidtools/AdMobBanner",
                                                               "(Landroid/app/Activity;)V",
                                                               QtAndroid::androidActivity().object<jobject>()),
                                             m_InstanceIndex(m_InstancesCounter++),
                                             m_BannerType(TYPE_NO_BANNER),
                                             m_BannerPos(0,0)
{
    m_pInstancesMap[m_InstanceIndex] = this;

    if(m_InstanceIndex == 0 && m_JavaAdMobBanner.isValid())
    {
        JNINativeMethod JniMethod[] = {{"bannerEvent", "(I)V", reinterpret_cast<void *>(&QAndroidAdMobBanner::BannerEvent)},
                                       {"bannerError", "(I)V", reinterpret_cast<void *>(&QAndroidAdMobBanner::BannerError)}
                                      };
        QAndroidJniEnvironment JniEnv;
        jclass ObjectClass;

        ObjectClass = JniEnv->GetObjectClass(m_JavaAdMobBanner.object<jobject>());
        JniEnv->RegisterNatives(ObjectClass, JniMethod, 2);
        JniEnv->DeleteLocalRef(ObjectClass);
    }
    connect(qGuiApp, &QGuiApplication::applicationStateChanged, this, &QAndroidAdMobBanner::ApplicationStateChanged);
    SetNewAppState(APP_STATE_CREATE);
}

QAndroidAdMobBanner::~QAndroidAdMobBanner()
{
    m_pInstancesMap.remove(m_InstanceIndex);
    SetNewAppState(APP_STATE_DESTROY);
}

const QMap<int, QAndroidAdMobBanner*>& QAndroidAdMobBanner::Instances()
{
    return m_pInstancesMap;
}

bool QAndroidAdMobBanner::show()
{
    if(m_JavaAdMobBanner.isValid() && m_BannerType != TYPE_NO_BANNER && m_UnitId.isEmpty() == false)
    {
        m_JavaAdMobBanner.callMethod<void>("show");
        return true;
    }

    return false;
}

void QAndroidAdMobBanner::hide()
{
    if(m_JavaAdMobBanner.isValid())
    {
        m_JavaAdMobBanner.callMethod<void>("hide");
    }
}

const QString& QAndroidAdMobBanner::getUnitId() const
{
    return m_UnitId;
}

void QAndroidAdMobBanner::setUnitId(const QString &UnitId)
{
    if(m_JavaAdMobBanner.isValid())
    {
        m_JavaAdMobBanner.callMethod<void>("setUnitId",
                                           "(Ljava/lang/String;)V",
                                           QAndroidJniObject::fromString(UnitId).object<jstring>()
                                           );
        m_UnitId = UnitId;
    }
}

QAndroidAdMobBanner::BANNER_TYPE QAndroidAdMobBanner::getType() const
{
    return m_BannerType;
}

void QAndroidAdMobBanner::setType(BANNER_TYPE Type)
{
    if(m_JavaAdMobBanner.isValid() && Type != TYPE_NO_BANNER)
    {
        m_JavaAdMobBanner.callMethod<void>("setType",
                                           "(I)V",
                                           Type
                                           );
        m_BannerType = Type;
    }
}

int QAndroidAdMobBanner::getXPos() const
{
    return m_BannerPos.x();
}

void QAndroidAdMobBanner::setXPos(int XPos)
{
    if(m_JavaAdMobBanner.isValid())
    {
        m_JavaAdMobBanner.callMethod<void>("setXPos",
                                           "(I)V",
                                           XPos
                                           );
        m_BannerPos.setX(XPos);
    }
}

int QAndroidAdMobBanner::getYPos() const
{
    return m_BannerPos.y();
}

void QAndroidAdMobBanner::setYPos(int YPos)
{
    if(m_JavaAdMobBanner.isValid())
    {
        m_JavaAdMobBanner.callMethod<void>("setYPos",
                                           "(I)V",
                                           YPos
                                           );
        m_BannerPos.setY(YPos);
    }
}

void QAndroidAdMobBanner::BannerEvent(JNIEnv *env, jobject thiz, jint eventId)
{
    QMapIterator<int, QAndroidAdMobBanner*> Instance(m_pInstancesMap);

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

void QAndroidAdMobBanner::BannerError(JNIEnv *env, jobject thiz, jint errorId)
{
    QMapIterator<int, QAndroidAdMobBanner*> Instance(m_pInstancesMap);

    Q_UNUSED(env)
    Q_UNUSED(thiz)

    while(Instance.hasNext())
    {
        Instance.next();
        emit Instance.value()->loadError(errorId);
    }
}

void QAndroidAdMobBanner::ApplicationStateChanged(Qt::ApplicationState State)
{
    SetNewAppState((State == Qt::ApplicationActive) ? APP_STATE_START : APP_STATE_STOP);
}

void QAndroidAdMobBanner::SetNewAppState(APP_STATE NewState)
{
    if(m_JavaAdMobBanner.isValid())
    {
        m_JavaAdMobBanner.callMethod<void>("appStateChanged",
                                           "(I)V",
                                           NewState
                                           );
    }
}

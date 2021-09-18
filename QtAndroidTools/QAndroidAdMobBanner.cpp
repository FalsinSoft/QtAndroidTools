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
#include <QScreen>
#include "QAndroidAdMobBanner.h"

QMap<int, QAndroidAdMobBanner*> QAndroidAdMobBanner::m_pInstancesMap;
int QAndroidAdMobBanner::m_instancesCounter = 0;

QAndroidAdMobBanner::QAndroidAdMobBanner(QQuickItem *parent) : QQuickItem(parent),
                                                               m_javaAdMobBanner("com/falsinsoft/qtandroidtools/AndroidAdMobBanner",
                                                                                 "(Landroid/app/Activity;)V",
                                                                                 QtAndroid::androidActivity().object<jobject>()),
                                                               m_instanceIndex(m_instancesCounter++),
                                                               m_bannerType(TYPE_NO_BANNER),
                                                               m_nonPersonalizedAds(false),
                                                               m_bannerShowed(false)
{
    m_pInstancesMap[m_instanceIndex] = this;

    if(m_instanceIndex == 0 && m_javaAdMobBanner.isValid())
    {
        const JNINativeMethod jniMethod[] = {
            {"bannerEvent", "(I)V", reinterpret_cast<void *>(&QAndroidAdMobBanner::bannerEvent)},
            {"bannerError", "(I)V", reinterpret_cast<void *>(&QAndroidAdMobBanner::bannerError)}
        };
        QAndroidJniEnvironment jniEnv;
        jclass objectClass;

        objectClass = jniEnv->GetObjectClass(m_javaAdMobBanner.object<jobject>());
        jniEnv->RegisterNatives(objectClass, jniMethod, sizeof(jniMethod)/sizeof(JNINativeMethod));
        jniEnv->DeleteLocalRef(objectClass);
    }
    connect(qGuiApp, &QGuiApplication::applicationStateChanged, this, &QAndroidAdMobBanner::applicationStateChanged);
    connect(qGuiApp->primaryScreen(), &QScreen::geometryChanged, this, &QAndroidAdMobBanner::screenGeometryChanged);
    connect(this, &QQuickItem::xChanged, [this]() { updatePosition(); });
    connect(this, &QQuickItem::yChanged, [this]() { updatePosition(); });
    setNewAppState(APP_STATE_CREATE);
}

QAndroidAdMobBanner::~QAndroidAdMobBanner()
{
    m_pInstancesMap.remove(m_instanceIndex);
    setNewAppState(APP_STATE_DESTROY);
}

const QMap<int, QAndroidAdMobBanner*>& QAndroidAdMobBanner::instances()
{
    return m_pInstancesMap;
}

bool QAndroidAdMobBanner::show()
{
    if(m_javaAdMobBanner.isValid() && m_bannerType != TYPE_NO_BANNER && m_unitId.isEmpty() == false)
    {
        updatePosition();
        m_javaAdMobBanner.callMethod<void>("show");
        m_bannerShowed = true;
        return true;
    }

    return false;
}

bool QAndroidAdMobBanner::hide()
{
    if(m_javaAdMobBanner.isValid())
    {
        m_javaAdMobBanner.callMethod<void>("hide");
        m_bannerShowed = false;
        return true;
    }

    return false;
}

bool QAndroidAdMobBanner::reload()
{
    if(m_javaAdMobBanner.isValid() && m_bannerType != TYPE_NO_BANNER && m_unitId.isEmpty() == false)
    {
        const bool bannerShowed = m_bannerShowed;

        if(bannerShowed) hide();
        m_javaAdMobBanner.callMethod<void>("reload");
        setType(m_bannerType);
        setUnitId(m_unitId);
        if(bannerShowed) show();

        return true;
    }

    return false;
}

const QString& QAndroidAdMobBanner::getUnitId() const
{
    return m_unitId;
}

void QAndroidAdMobBanner::setUnitId(const QString &unitId)
{
    if(m_javaAdMobBanner.isValid())
    {
        m_javaAdMobBanner.callMethod<void>("setUnitId",
                                           "(Ljava/lang/String;)V",
                                           QAndroidJniObject::fromString(unitId).object<jstring>()
                                           );
        m_unitId = unitId;
    }
}

const QStringList& QAndroidAdMobBanner::getKeywords() const
{
    return m_keywordsList;
}

void QAndroidAdMobBanner::setKeywords(const QStringList &keywordsList)
{
    if(m_javaAdMobBanner.isValid())
    {
        const QAndroidJniObject stringObj("java/lang/String");
        QAndroidJniObject stringArrayObj;
        QAndroidJniEnvironment jniEnv;

        stringArrayObj = QAndroidJniObject::fromLocalRef(jniEnv->NewObjectArray(keywordsList.count(), jniEnv->GetObjectClass(stringObj.object()), NULL));

        for(int i = 0; i < keywordsList.count(); i++)
        {
            jniEnv->SetObjectArrayElement(stringArrayObj.object<jobjectArray>(), i, QAndroidJniObject::fromString(keywordsList[i]).object<jstring>());
        }

        m_javaAdMobBanner.callMethod<void>("setKeywords",
                                           "([Ljava/lang/String;)V",
                                           stringArrayObj.object<jobjectArray>()
                                           );
        m_keywordsList = keywordsList;
    }
}

QAndroidAdMobBanner::BANNER_TYPE QAndroidAdMobBanner::getType() const
{
    return m_bannerType;
}

void QAndroidAdMobBanner::setType(BANNER_TYPE type)
{
    if(m_javaAdMobBanner.isValid() && type != TYPE_NO_BANNER)
    {
        const qreal pixelRatio = qApp->primaryScreen()->devicePixelRatio();
        QAndroidJniObject bannerPixelsSizeObj;

        m_javaAdMobBanner.callMethod<void>("setType",
                                           "(I)V",
                                           type
                                           );        
        m_bannerType = type;

        bannerPixelsSizeObj = m_javaAdMobBanner.callObjectMethod("getPixelsSize",
                                                                 "()Lcom/falsinsoft/qtandroidtools/AndroidAdMobBanner$BannerSize;"
                                                                 );
        setWidth(bannerPixelsSizeObj.getField<jint>("width") / pixelRatio);
        setHeight(bannerPixelsSizeObj.getField<jint>("height") / pixelRatio);
    }
}

bool QAndroidAdMobBanner::getNonPersonalizedAds() const
{
    return m_nonPersonalizedAds;
}

void QAndroidAdMobBanner::setNonPersonalizedAds(bool npa)
{
    if(m_javaAdMobBanner.isValid())
    {
        m_javaAdMobBanner.callMethod<void>("setNonPersonalizedAds",
                                           "(Z)V",
                                           npa
                                           );
        m_nonPersonalizedAds = npa;
    }
}

void QAndroidAdMobBanner::screenGeometryChanged(const QRect &geometry)
{
    Q_UNUSED(geometry)

    if(m_bannerShowed == true)
    {
        reload();
    }
}

void QAndroidAdMobBanner::updatePosition()
{
    if(m_javaAdMobBanner.isValid())
    {
        const qreal pixelRatio = qApp->primaryScreen()->devicePixelRatio();
        const QPointF screenPos = mapToGlobal(QPointF(0,0));

        m_javaAdMobBanner.callMethod<void>("setPos",
                                           "(II)V",
                                           static_cast<int>(screenPos.x() * pixelRatio),
                                           static_cast<int>(screenPos.y() * pixelRatio)
                                           );
    }
}

void QAndroidAdMobBanner::bannerEvent(JNIEnv *env, jobject thiz, jint eventId)
{
    QMapIterator<int, QAndroidAdMobBanner*> instance(m_pInstancesMap);

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

void QAndroidAdMobBanner::bannerError(JNIEnv *env, jobject thiz, jint errorId)
{
    QMapIterator<int, QAndroidAdMobBanner*> instance(m_pInstancesMap);

    Q_UNUSED(env)
    Q_UNUSED(thiz)

    while(instance.hasNext())
    {
        instance.next();
        Q_EMIT instance.value()->loadError(errorId);
    }
}

void QAndroidAdMobBanner::applicationStateChanged(Qt::ApplicationState state)
{
    setNewAppState((state == Qt::ApplicationActive) ? APP_STATE_START : APP_STATE_STOP);
}

void QAndroidAdMobBanner::setNewAppState(APP_STATE newState)
{
    if(m_javaAdMobBanner.isValid())
    {
        m_javaAdMobBanner.callMethod<void>("appStateChanged",
                                           "(I)V",
                                           newState
                                           );
    }
}

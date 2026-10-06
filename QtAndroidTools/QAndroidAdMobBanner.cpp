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
                                                                                 QNativeInterface::QAndroidApplication::context()),
                                                               m_instanceIndex(m_instancesCounter++),
                                                               m_bannerType(TYPE_NO_BANNER),
                                                               m_inlineAdaptiveBannerMaxHeight(0),
                                                               m_nonPersonalizedAds(false),
                                                               m_trackMovement(false),
                                                               m_bannerShowed(false)
{
    m_pInstancesMap[m_instanceIndex] = this;

    if(m_instanceIndex == 0 && m_javaAdMobBanner.isValid())
    {
        const JNINativeMethod jniMethod[] = {
            {"bannerEvent", "(I)V", reinterpret_cast<void *>(&QAndroidAdMobBanner::bannerEvent)},
            {"bannerError", "(I)V", reinterpret_cast<void *>(&QAndroidAdMobBanner::bannerError)}
        };
        QJniEnvironment jniEnv;
        jclass objectClass;

        objectClass = jniEnv->GetObjectClass(m_javaAdMobBanner.object<jobject>());
        jniEnv->RegisterNatives(objectClass, jniMethod, sizeof(jniMethod)/sizeof(JNINativeMethod));
        jniEnv->DeleteLocalRef(objectClass);
    }
    connect(qGuiApp, &QGuiApplication::applicationStateChanged, this, &QAndroidAdMobBanner::applicationStateChanged);
    connect(qGuiApp->primaryScreen(), &QScreen::geometryChanged, this, &QAndroidAdMobBanner::screenGeometryChanged);
    connect(this, &QQuickItem::windowChanged, this, &QAndroidAdMobBanner::windowChanged);
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
                                           QJniObject::fromString(unitId).object<jstring>()
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
        const QJniObject stringObj("java/lang/String");
        QJniObject stringArrayObj;
        QJniEnvironment jniEnv;

        stringArrayObj = QJniObject::fromLocalRef(jniEnv->NewObjectArray(keywordsList.count(), jniEnv->GetObjectClass(stringObj.object()), NULL));

        for(int i = 0; i < keywordsList.count(); i++)
        {
            jniEnv->SetObjectArrayElement(stringArrayObj.object<jobjectArray>(), i, QJniObject::fromString(keywordsList[i]).object<jstring>());
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
        m_javaAdMobBanner.callMethod<void>("setType",
                                           "(I)V",
                                           type
                                           );
        m_bannerType = type;

        if(m_bannerType != TYPE_INLINE_ADAPTIVE_BANNER) updateSize();
    }
}

int QAndroidAdMobBanner::getInlineAdaptiveBannerMaxHeight() const
{
    return m_inlineAdaptiveBannerMaxHeight;
}

void QAndroidAdMobBanner::setInlineAdaptiveBannerMaxHeight(int maxHeight)
{
    if(m_javaAdMobBanner.isValid() && maxHeight > 0)
    {
        m_javaAdMobBanner.callMethod<void>("setInlineAdaptiveBannerMaxHeight",
                                           "(I)V",
                                           maxHeight
                                           );
        m_inlineAdaptiveBannerMaxHeight = maxHeight;

        if(m_bannerType == TYPE_INLINE_ADAPTIVE_BANNER) setType(m_bannerType);
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

bool QAndroidAdMobBanner::getTrackMovement() const
{
    return m_trackMovement;
}

void QAndroidAdMobBanner::setTrackMovement(bool trackEnabled)
{
    if(trackEnabled != m_trackMovement)
    {
        QQuickWindow *win = window();

        if(win)
        {
            if(trackEnabled)
                connect(win, &QQuickWindow::afterAnimating, this, &QAndroidAdMobBanner::updatePosition, Qt::UniqueConnection);
            else
                disconnect(win, &QQuickWindow::afterAnimating, this, &QAndroidAdMobBanner::updatePosition);
        }

        m_trackMovement = trackEnabled;
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
    const QPointF screenPos = mapToGlobal(QPointF(0,0));

    if(m_javaAdMobBanner.isValid() && screenPos != m_lastScreenPos)
    {
        m_javaAdMobBanner.callMethod<void>("setPos",
                                           "(II)V",
                                           static_cast<int>(screenPos.x()),
                                           static_cast<int>(screenPos.y())
                                           );
    }
}

void QAndroidAdMobBanner::updateSize()
{
    if(m_javaAdMobBanner.isValid())
    {
        const QJniObject bannerPixelsSizeObj = m_javaAdMobBanner.callObjectMethod("getPixelsSize",
                                                                                  "()Lcom/falsinsoft/qtandroidtools/AndroidAdMobBanner$BannerSize;"
                                                                                  );
        setWidth(bannerPixelsSizeObj.getField<jint>("width"));
        setHeight(bannerPixelsSizeObj.getField<jint>("height"));
    }
}

void QAndroidAdMobBanner::bannerLoaded()
{
    if(m_bannerType == TYPE_INLINE_ADAPTIVE_BANNER)
    {
        QMetaObject::invokeMethod(this,
                                  &QAndroidAdMobBanner::updateSize,
                                  Qt::QueuedConnection);
        QMetaObject::invokeMethod(this,
                                  &QAndroidAdMobBanner::updatePosition,
                                  Qt::QueuedConnection);
    }
    Q_EMIT loaded();
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
                instance.value()->bannerLoaded();
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

void QAndroidAdMobBanner::windowChanged(QQuickWindow *win)
{
    if(win && m_trackMovement)
    {
        connect(win, &QQuickWindow::afterAnimating, this, &QAndroidAdMobBanner::updatePosition, Qt::UniqueConnection);
    }
    updatePosition();
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

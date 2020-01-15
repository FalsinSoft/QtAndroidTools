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
#pragma once

#include <QtAndroidExtras>
#include <QQmlEngine>
#include <QQuickItem>

class QAndroidAdMobBanner : public QQuickItem
{
    Q_PROPERTY(QString unitId READ getUnitId WRITE setUnitId)
    Q_PROPERTY(BANNER_TYPE type READ getType WRITE setType)
    Q_PROPERTY(QStringList keywords READ getKeywords WRITE setKeywords)
    Q_ENUMS(BANNER_TYPE)
    Q_ENUMS(ERROR_TYPE)
    Q_OBJECT

public:
    QAndroidAdMobBanner(QQuickItem *parent = nullptr);
    ~QAndroidAdMobBanner();

    enum BANNER_TYPE
    {
        TYPE_NO_BANNER = -1,
        TYPE_BANNER = 0,
        TYPE_FULL_BANNER,
        TYPE_LARGE_BANNER,
        TYPE_MEDIUM_RECTANGLE,
        TYPE_SMART_BANNER,
        TYPE_WIDE_SKYSCRAPER
    };
    enum ERROR_TYPE
    {
        ERROR_INTERNAL = 0,
        ERROR_NETWORK,
        ERROR_INVALID_REQUEST,
        ERROR_NO_FILL
    };

    Q_INVOKABLE bool show();
    Q_INVOKABLE bool hide();
    Q_INVOKABLE bool reload();

    const QString& getUnitId() const;
    void setUnitId(const QString &UnitId);
    BANNER_TYPE getType() const;
    void setType(BANNER_TYPE Type);
    const QStringList& getKeywords() const;
    void setKeywords(const QStringList &KeywordsList);

    static const QMap<int, QAndroidAdMobBanner*>& Instances();

signals:
    void loadError(int errorId);
    void loading();
    void loaded();
    void closed();
    void clicked();

private slots:
    void ApplicationStateChanged(Qt::ApplicationState State);
    void ScreenGeometryChanged(const QRect &Geometry);
    void ItemPosChanged();

private:
    const QAndroidJniObject m_JavaAdMobBanner;
    static QMap<int, QAndroidAdMobBanner*> m_pInstancesMap;
    static int m_InstancesCounter;
    const int m_InstanceIndex;
    BANNER_TYPE m_BannerType;
    bool m_BannerShowed;
    QString m_UnitId;
    QStringList m_KeywordsList;

    enum EVENT_TYPE
    {
        EVENT_LOADING = 0,
        EVENT_LOADED,
        EVENT_CLOSED,
        EVENT_CLICKED
    };

    static void BannerEvent(JNIEnv *env, jobject thiz, jint eventId);
    static void BannerError(JNIEnv *env, jobject thiz, jint errorId);

    enum APP_STATE
    {
        APP_STATE_CREATE = 0,
        APP_STATE_START,
        APP_STATE_STOP,
        APP_STATE_DESTROY
    };
    void SetNewAppState(APP_STATE NewState);
};

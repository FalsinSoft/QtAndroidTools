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

#include <QJniObject>
#include <QQmlEngine>
#include <QQuickItem>

class QAndroidAdMobInterstitial : public QQuickItem
{
    Q_PROPERTY(QString unitId READ getUnitId WRITE setUnitId)
    Q_PROPERTY(bool nonPersonalizedAds READ getNonPersonalizedAds WRITE setNonPersonalizedAds)
    Q_ENUMS(ERROR_TYPE)
    Q_OBJECT

public:
    QAndroidAdMobInterstitial(QQuickItem *parent = nullptr);
    ~QAndroidAdMobInterstitial();

    Q_INVOKABLE bool show();
    Q_INVOKABLE bool load();

    const QString& getUnitId() const;
    void setUnitId(const QString &unitId);
    bool getNonPersonalizedAds() const;
    void setNonPersonalizedAds(bool npa);

    static const QMap<int, QAndroidAdMobInterstitial*>& instances();

Q_SIGNALS:
    void loadError();
    void loading();
    void loaded();
    void clicked();
    void dismissed();
    void showFailed();
    void impression();
    void showed();

private:
    const QJniObject m_javaAdMobInterstitial;
    static QMap<int, QAndroidAdMobInterstitial*> m_pInstancesMap;
    static int m_instancesCounter;
    const int m_instanceIndex;
    bool m_nonPersonalizedAds;
    QString m_unitId;

    enum EVENT_TYPE
    {
        EVENT_LOAD_ERROR = 0,
        EVENT_LOADING,
        EVENT_LOADED,
        EVENT_CLICKED,
        EVENT_DISMISSED,
        EVENT_SHOW_FAILED,
        EVENT_IMPRESSION,
        EVENT_SHOWED
    };

    static void interstitialEvent(JNIEnv *env, jobject thiz, jint eventId);
};

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

class QAndroidAdMobRewardedVideo : public QQuickItem
{
    Q_PROPERTY(QString unitId READ getUnitId WRITE setUnitId)
    Q_PROPERTY(bool nonPersonalizedAds READ getNonPersonalizedAds WRITE setNonPersonalizedAds)
    Q_ENUMS(ERROR_TYPE)
    Q_OBJECT

public:
    QAndroidAdMobRewardedVideo(QQuickItem *parent = nullptr);
    ~QAndroidAdMobRewardedVideo();

    enum ERROR_TYPE
    {
        ERROR_INTERNAL = 0,
        ERROR_NETWORK,
        ERROR_INVALID_REQUEST,
        ERROR_NO_FILL
    };

    Q_INVOKABLE bool show();
    Q_INVOKABLE bool load();

    const QString& getUnitId() const;
    void setUnitId(const QString &unitId);
    bool getNonPersonalizedAds() const;
    void setNonPersonalizedAds(bool npa);

    static const QMap<int, QAndroidAdMobRewardedVideo*>& instances();

Q_SIGNALS:
    void rewarded(const QString &type, int amount);
    void loadError(int errorId);
    void loading();
    void loaded();
    void opened();
    void closed();
    void started();
    void completed();
    void leftApplication();

private:
    const QJniObject m_javaAdMobRewardedVideo;
    static QMap<int, QAndroidAdMobRewardedVideo*> m_pInstancesMap;
    static int m_instancesCounter;
    const int m_instanceIndex;
    bool m_nonPersonalizedAds;
    QString m_unitId;

    enum EVENT_TYPE
    {
        EVENT_LOADING = 0,
        EVENT_LOADED,
        EVENT_OPENED,
        EVENT_CLOSED,
        EVENT_STARTED,
        EVENT_COMPLETED,
        EVENT_LEFT_APPLICATION
    };

    static void rewardedVideoReward(JNIEnv *env, jobject thiz, jstring type, jint amount);
    static void rewardedVideoEvent(JNIEnv *env, jobject thiz, jint eventId);
    static void rewardedVideoError(JNIEnv *env, jobject thiz, jint errorId);

    enum APP_STATE
    {
        APP_STATE_CREATE = 0,
        APP_STATE_START,
        APP_STATE_STOP,
        APP_STATE_DESTROY
    };
    void setNewAppState(APP_STATE newState);
};

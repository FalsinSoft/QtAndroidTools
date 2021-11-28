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

class QAndroidBatteryState : public QObject
{
    Q_PROPERTY(int level READ getLevel NOTIFY levelChanged)
    Q_PROPERTY(bool onCharge READ isOnCharge NOTIFY onChargeChanged)
	Q_DISABLE_COPY(QAndroidBatteryState)
    Q_OBJECT

    QAndroidBatteryState() : QAndroidBatteryState(nullptr) {}

public:
    QAndroidBatteryState(QObject *parent);
    ~QAndroidBatteryState();

    static QObject* qmlInstance(QQmlEngine *engine, QJSEngine *scriptEngine);
    static QAndroidBatteryState* instance();

    int getLevel();
    bool isOnCharge();

Q_SIGNALS:
    void levelChanged();
    void onChargeChanged();

private Q_SLOTS:
    void applicationStateChanged(Qt::ApplicationState state);

private:
    const QAndroidJniObject m_javaBatteryState;
    static QAndroidBatteryState *m_pInstance;

    static void batteryLevelChanged(JNIEnv *env, jobject thiz);
    static void batteryOnChargeChanged(JNIEnv *env, jobject thiz);

    enum APP_STATE
    {
        APP_STATE_CREATE = 0,
        APP_STATE_START,
        APP_STATE_STOP,
        APP_STATE_DESTROY
    };
    void setNewAppState(APP_STATE newState);
};

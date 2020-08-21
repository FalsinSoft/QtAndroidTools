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

class QAndroidSignalStrength : public QObject
{
    Q_PROPERTY(int signalStrength READ getSignalStrength NOTIFY signalStrengthChanged)
    Q_PROPERTY(int signalLevel READ getSignalLevel NOTIFY signalLevelChanged)
    Q_DISABLE_COPY(QAndroidSignalStrength)
    Q_ENUMS(SIGNAL_LEVEL)
    Q_OBJECT

    QAndroidSignalStrength();

public:
    ~QAndroidSignalStrength();

    enum SIGNAL_LEVEL
    {
        LEVEL_GREAT,
        LEVEL_GOOD,
        LEVEL_MODERATE,
        LEVEL_POOR,
        LEVEL_NONE
    };

    static QObject* qmlInstance(QQmlEngine *engine, QJSEngine *scriptEngine);
    static QAndroidSignalStrength* instance();

    int getSignalStrength();
    SIGNAL_LEVEL getSignalLevel();

Q_SIGNALS:
    void signalStrengthChanged();
    void signalLevelChanged();

private Q_SLOTS:
    void applicationStateChanged(Qt::ApplicationState state);

private:
    const QAndroidJniObject m_javaSignalStrength;
    static QAndroidSignalStrength *m_pInstance;

    static void deviceSignalStrengthChanged(JNIEnv *env, jobject thiz);

    enum APP_STATE
    {
        APP_STATE_CREATE = 0,
        APP_STATE_START,
        APP_STATE_STOP,
        APP_STATE_DESTROY
    };
    void setNewAppState(APP_STATE newState);
};

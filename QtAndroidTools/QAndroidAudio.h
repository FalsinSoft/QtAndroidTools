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

class QAndroidAudio : public QObject
{
    Q_PROPERTY(int focus READ hasFocus NOTIFY focusChanged)
    Q_DISABLE_COPY(QAndroidAudio)
    Q_OBJECT

    QAndroidAudio();

public:
    ~QAndroidAudio();

    static QObject* qmlInstance(QQmlEngine *engine, QJSEngine *scriptEngine);
    static QAndroidAudio* instance();

    Q_INVOKABLE bool requestFocus();
    Q_INVOKABLE void abandonFocus();

    bool hasFocus();

Q_SIGNALS:
    void focusChanged();

private:
    const QAndroidJniObject m_javaAudio;
    static QAndroidAudio *m_pInstance;
    bool m_focus;

    void setFocus(bool focus);

    static void deviceFocusChanged(JNIEnv *env, jobject thiz, jboolean focus);
};

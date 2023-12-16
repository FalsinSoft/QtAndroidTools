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

class QAndroidUserMessagingPlatform : public QObject
{
    Q_DISABLE_COPY(QAndroidUserMessagingPlatform)
    QML_NAMED_ELEMENT(QtAndroidUserMessagingPlatform)
    QML_SINGLETON
    Q_OBJECT

    QAndroidUserMessagingPlatform() : QAndroidUserMessagingPlatform(nullptr) {}

public:
    QAndroidUserMessagingPlatform(QObject *parent);

    static QAndroidUserMessagingPlatform* create(QQmlEngine *engine, QJSEngine *scriptEngine);
    static QAndroidUserMessagingPlatform* instance();

    Q_INVOKABLE void loadAndShowConsentFormIfRequired(bool underAgeOfConsent);
    Q_INVOKABLE void showPrivacyOptionsForm();
    Q_INVOKABLE bool canRequestAds();
    Q_INVOKABLE void resetConsentInformation();

Q_SIGNALS:
    void consentFormRequestFailure(const QString &errorMessage);
    void consentFormDismissed(bool consentGathered, bool privacyOptionsRequired);

private:
    const QJniObject m_javaUserMessagingPlatform;
    static QAndroidUserMessagingPlatform *m_pInstance;

    static void deviceConsentFormRequestFailure(JNIEnv *env, jobject thiz, jstring errorMessage);
    static void deviceConsentFormDismissed(JNIEnv *env, jobject thiz, jboolean consentGathered, jboolean privacyOptionsRequired);
};

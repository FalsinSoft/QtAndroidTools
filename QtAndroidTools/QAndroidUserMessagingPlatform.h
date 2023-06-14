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

    enum CONSENT_STATUS
    {
        CONSENT_FORM_STATUS_UNKNOWN = 0,
        CONSENT_FORM_STATUS_REQUIRED = 1,
        CONSENT_FORM_STATUS_NOT_REQUIRED = 2,
        CONSENT_FORM_STATUS_OBTAINED = 3,
    };
    Q_ENUM(CONSENT_STATUS)
    enum REQUEST_RESULT
    {
        CONSENT_FORM_INFO_UPDATE_FAILURE = 0,
        CONSENT_FORM_NOT_AVAILABLE = 1,
        CONSENT_FORM_LOAD_SUCCESS = 2,
        CONSENT_FORM_LOAD_FAILURE = 3
    };
    Q_ENUM(REQUEST_RESULT)

    static QAndroidUserMessagingPlatform* create(QQmlEngine *engine, QJSEngine *scriptEngine);
    static QAndroidUserMessagingPlatform* instance();

    Q_INVOKABLE void requestConsentForm();
    Q_INVOKABLE int consentStatus();
    Q_INVOKABLE bool showConsentForm();
    Q_INVOKABLE void resetConsentInformation();

Q_SIGNALS:
    void consentFormRequestResult(int eventId);
    void consentFormClosed();

private:
    const QJniObject m_javaUserMessagingPlatform;
    static QAndroidUserMessagingPlatform *m_pInstance;

    static void deviceConsentFormRequestResult(JNIEnv *env, jobject thiz, int eventId);
    static void deviceConsentFormClosed(JNIEnv *env, jobject thiz);
};

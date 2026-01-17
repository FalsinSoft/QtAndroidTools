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

class QAndroidAuthentication : public QObject
{
    Q_PROPERTY(int authenticators READ getAuthenticators WRITE setAuthenticators)
    Q_PROPERTY(QString title READ getTitle WRITE setTitle)
    Q_PROPERTY(QString description READ getDescription WRITE setDescription)
    Q_PROPERTY(QString negativeButton READ getNegativeButton WRITE setNegativeButton)
    Q_DISABLE_COPY(QAndroidAuthentication)
    QML_NAMED_ELEMENT(QtAndroidAuthentication)
    QML_SINGLETON
    Q_OBJECT

    QAndroidAuthentication() : QAndroidAuthentication(nullptr) {}

public:
    QAndroidAuthentication(QObject *parent);

    enum AUTHENTICATOR_TYPE
    {
        BIOMETRIC_STRONG = 0x01,
        BIOMETRIC_WEAK = 0x02,
        DEVICE_CREDENTIAL = 0x04
    };
    Q_ENUM(AUTHENTICATOR_TYPE)
    enum BIOMETRIC_STATUS
    {
        BIOMETRIC_STATUS_UNKNOWN = 0,
        BIOMETRIC_SUCCESS,
        BIOMETRIC_ERROR_NO_HARDWARE,
        BIOMETRIC_ERROR_HW_UNAVAILABLE,
        BIOMETRIC_ERROR_NONE_ENROLLED,
        BIOMETRIC_ERROR_SECURITY_UPDATE_REQUIRED,
        BIOMETRIC_ERROR_UNSUPPORTED
    };
    Q_ENUM(BIOMETRIC_STATUS)

    static QAndroidAuthentication* create(QQmlEngine *engine, QJSEngine *scriptEngine);
    static QAndroidAuthentication* instance();

    Q_INVOKABLE int canAuthenticate();
    Q_INVOKABLE bool requestBiometricEnroll();
    Q_INVOKABLE bool authenticate();
    Q_INVOKABLE bool authenticateAndEncrypt(const QString &plainText);
    Q_INVOKABLE bool authenticateAndDecrypt(const QString &encryptedText, const QString &initializationVector);
    Q_INVOKABLE void cancel();

    int getAuthenticators() const;
    void setAuthenticators(int authenticators);
    QString getTitle() const;
    void setTitle(const QString &title);
    QString getDescription() const;
    void setDescription(const QString &description);
    QString getNegativeButton() const;
    void setNegativeButton(const QString &negativeButton);

Q_SIGNALS:
    void error(const QString &error);
    void succeeded();
    void succeededAndEncrypted(const QString &encryptedText, const QString &initializationVector);
    void succeededAndDecrypted(const QString &decryptedText);
    void failed();
    void cancelled();

private:
    const QJniObject m_javaAuthentication;
    static QAndroidAuthentication *m_pInstance;
    QString m_title, m_description, m_negativeButton;
    int m_authenticators;

    static void authenticationError(JNIEnv *env, jobject thiz, jstring error);
    static void authenticationSucceeded(JNIEnv *env, jobject thiz);
    static void authenticationAndEncryptionSucceeded(JNIEnv *env, jobject thiz, jstring encryptedText, jstring initializationVector);
    static void authenticationAndDecryptionSucceeded(JNIEnv *env, jobject thiz, jstring decryptedText);
    static void authenticationFailed(JNIEnv *env, jobject thiz);
    static void authenticationCancelled(JNIEnv *env, jobject thiz);
};

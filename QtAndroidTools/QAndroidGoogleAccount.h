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

#include <QQuickImageProvider>
#include <QtAndroidExtras>
#include <QQmlEngine>
#include <QPixmap>
#include <QImage>

struct QAndroidGoogleAccountInfo
{
    Q_GADGET
    Q_PROPERTY(QString id MEMBER id)
    Q_PROPERTY(QString displayName MEMBER displayName)
    Q_PROPERTY(QString email MEMBER email)
    Q_PROPERTY(QString familyName MEMBER familyName)
    Q_PROPERTY(QString givenName MEMBER givenName)
    Q_PROPERTY(QByteArray photo MEMBER photo)
public:
    QString id;
    QString displayName;
    QString email;
    QString familyName;
    QString givenName;
    QByteArray photo;
};
Q_DECLARE_METATYPE(QAndroidGoogleAccountInfo)

class QAndroidGoogleAccount : public QObject, public QAndroidActivityResultReceiver
{
    Q_PROPERTY(QAndroidGoogleAccountInfo signedInAccountInfo READ getSignedInAccountInfo NOTIFY signedInAccountInfoChanged)
    Q_DISABLE_COPY(QAndroidGoogleAccount)
    Q_OBJECT

    QAndroidGoogleAccount() : QAndroidGoogleAccount(nullptr) {}

public:
    QAndroidGoogleAccount(QObject *parent);

    static QObject* qmlInstance(QQmlEngine *engine, QJSEngine *scriptEngine);
    static QAndroidGoogleAccount* instance();

    Q_INVOKABLE bool signIn(const QString &scopeName = QString());
    Q_INVOKABLE bool signInSelectAccount(const QString &scopeName = QString());
    Q_INVOKABLE bool signOut();
    Q_INVOKABLE bool revokeAccess();

    const QAndroidGoogleAccountInfo& getSignedInAccountInfo() const;

Q_SIGNALS:
    void signedInAccountInfoChanged();
    void signedIn(bool signInSuccessfully);
    void signedOut();

private:
    const QAndroidJniObject m_javaGoogleAccount;
    static QAndroidGoogleAccount *m_pInstance;
    const int m_signInId = 9001;
    QAndroidGoogleAccountInfo m_signedInAccountInfo;

    void handleActivityResult(int receiverRequestCode, int resultCode, const QAndroidJniObject &data) override;

    static void updateSignedInAccountInfo(JNIEnv *env, jobject thiz, jobject accountInfo);
    static void signedInAccount(JNIEnv *env, jobject thiz, jboolean signInSuccessfully);
    static void signedOutAccount(JNIEnv *env, jobject thiz);

    void setSignedInAccountInfo(const QAndroidJniObject &accountInfoObj);
};

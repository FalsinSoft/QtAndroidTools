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
    Q_PROPERTY(QString id MEMBER Id)
    Q_PROPERTY(QString displayName MEMBER DisplayName)
    Q_PROPERTY(QString email MEMBER Email)
    Q_PROPERTY(QString familyName MEMBER FamilyName)
    Q_PROPERTY(QString givenName MEMBER GivenName)
    Q_PROPERTY(QByteArray photo MEMBER Photo)
public:
    QString Id;
    QString DisplayName;
    QString Email;
    QString FamilyName;
    QString GivenName;
    QByteArray Photo;
};
Q_DECLARE_METATYPE(QAndroidGoogleAccountInfo)

class QAndroidGoogleAccount : public QObject, public QAndroidActivityResultReceiver
{
    Q_PROPERTY(QAndroidGoogleAccountInfo signedInAccount READ getSignedInAccountInfo NOTIFY signedInAccountInfoChanged)
    Q_DISABLE_COPY(QAndroidGoogleAccount)
    Q_OBJECT

    QAndroidGoogleAccount();

public:
    static QObject* qmlInstance(QQmlEngine *engine, QJSEngine *scriptEngine);
    static QAndroidGoogleAccount* instance();

    Q_INVOKABLE bool signIn(const QString &ScopeName = QString());
    Q_INVOKABLE bool signInSelectAccount(const QString &ScopeName = QString());
    Q_INVOKABLE bool signOut();
    Q_INVOKABLE bool revokeAccess();

    const QAndroidGoogleAccountInfo& getSignedInAccountInfo() const;

signals:
    void signedInAccountInfoChanged();
    void signedIn(bool signInSuccessfully);
    void signedOut();

private:
    const QAndroidJniObject m_JavaGoogleAccount;
    static QAndroidGoogleAccount *m_pInstance;
    const int m_SignInId = 9001;
    QAndroidGoogleAccountInfo m_SignedInAccountInfo;

    void handleActivityResult(int receiverRequestCode, int resultCode, const QAndroidJniObject &data) override;

    static void UpdateSignedInAccountInfo(JNIEnv *env, jobject thiz, jobject accountInfo);
    static void SignedIn(JNIEnv *env, jobject thiz, jboolean signInSuccessfully);
    static void SignedOut(JNIEnv *env, jobject thiz);

    void SetSignedInAccountInfo(const QAndroidJniObject &AccountInfoObj);
    QImage AndroidBitmapToImage(const QAndroidJniObject &JniBmp);
};

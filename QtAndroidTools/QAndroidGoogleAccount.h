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
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
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
public:
    QString Id;
    QString DisplayName;
    QString Email;
    QString FamilyName;
    QString GivenName;
};
Q_DECLARE_METATYPE(QAndroidGoogleAccountInfo)

class QAndroidGoogleAccount : public QObject
{
    Q_PROPERTY(QAndroidGoogleAccountInfo lastSignedInAccount READ getLastSignedInAccountInfo NOTIFY lastSignedInAccountInfoChanged)
    Q_DISABLE_COPY(QAndroidGoogleAccount)
    Q_OBJECT

    class AccountPhotoImageProvider : public QQuickImageProvider
    {
    public:
        AccountPhotoImageProvider(QAndroidGoogleAccount *pAccount) : QQuickImageProvider(QQuickImageProvider::Pixmap), m_pAccount(pAccount) {}

        QPixmap requestPixmap(const QString &id, QSize *size, const QSize &requestedSize)
        {
            Q_UNUSED(id)
            Q_UNUSED(size)
            return m_pAccount->GetAccountPhoto().scaled(requestedSize);
        }

    private:
        const QAndroidGoogleAccount const *m_pAccount;
    };

    QAndroidGoogleAccount();

public:

    static QObject* qmlInstance(QQmlEngine *engine, QJSEngine *scriptEngine);
    static QAndroidGoogleAccount* instance();

    Q_INVOKABLE bool signIn();

    const QAndroidGoogleAccountInfo& getLastSignedInAccountInfo() const;

signals:
    void lastSignedInAccountInfoChanged();

private slots:
    void AccountPhotoDownloaded(QNetworkReply *pReply);

private:
    const QAndroidJniObject m_JavaGoogleAccount;
    static QAndroidGoogleAccount *m_pInstance;
    const int m_SignInId = 9001;
    QAndroidGoogleAccountInfo m_LastSignedInAccountInfo;
    QNetworkAccessManager m_NetworkAccessManager;
    QPixmap m_LastSignedInAccountPhoto;

    void ActivityResult(int RequestCode, int ResultCode, const QAndroidJniObject &Data);
    void LoadLastSignedInAccountInfo();
    QPixmap GetAccountPhoto() const;
};

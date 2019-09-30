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
    Q_ENUMS(SCOPES)
    Q_OBJECT

    class AccountPhotoImageProvider : public QQuickImageProvider
    {
    public:
        AccountPhotoImageProvider(QAndroidGoogleAccount *pAccount) : QQuickImageProvider(QQuickImageProvider::Pixmap), m_pAccount(pAccount) {}

        QPixmap requestPixmap(const QString &id, QSize *size, const QSize &requestedSize)
        {
            const QPixmap LastSignedInAccountPhoto = m_pAccount->GetAccountPhoto();
            Q_UNUSED(id)

            if(size) *size = LastSignedInAccountPhoto.size();

            if(requestedSize.width() > 0 && requestedSize.height() > 0)
                return LastSignedInAccountPhoto.scaled(requestedSize);
            else
                return LastSignedInAccountPhoto;
        }

    private:
        const QAndroidGoogleAccount const *m_pAccount;
    };

    QAndroidGoogleAccount();

public:

    enum SCOPES
    {
        SCOPE_NULL = 0,
        SCOPE_APP_STATE,
        SCOPE_CLOUD_SAVE,
        SCOPE_DRIVE_APPFOLDER,
        SCOPE_DRIVE_FILE,
        SCOPE_EMAIL,
        SCOPE_FITNESS_ACTIVITY_READ,
        SCOPE_FITNESS_ACTIVITY_READ_WRITE,
        SCOPE_FITNESS_BODY_READ,
        SCOPE_FITNESS_BODY_READ_WRITE,
        SCOPE_FITNESS_LOCATION_READ,
        SCOPE_FITNESS_LOCATION_READ_WRITE,
        SCOPE_FITNESS_NUTRITION_READ,
        SCOPE_FITNESS_NUTRITION_READ_WRITE,
        SCOPE_GAMES,
        SCOPE_PLUS_ME,
        SCOPE_PROFILE
    };

    static QObject* qmlInstance(QQmlEngine *engine, QJSEngine *scriptEngine);
    static QAndroidGoogleAccount* instance();

    Q_INVOKABLE bool signIn();
    Q_INVOKABLE bool signInNewAccount(int scope = SCOPE_NULL);
    Q_INVOKABLE void signOut();
    Q_INVOKABLE void revokeAccess();

    const QAndroidGoogleAccountInfo& getLastSignedInAccountInfo() const;

signals:
    void lastSignedInAccountInfoChanged();
    void signedIn(bool signInSuccessfully);
    void signedOut();

private:
    const QAndroidJniObject m_JavaGoogleAccount;
    static QAndroidGoogleAccount *m_pInstance;
    const int m_SignInId = 9001;
    QAndroidGoogleAccountInfo m_LastSignedInAccountInfo;
    QPixmap m_LastSignedInAccountPhoto;

    static void UpdateLastSignedInAccountInfo(JNIEnv *env, jobject thiz, jobject accountInfo);

    void ActivityResult(int RequestCode, int ResultCode, const QAndroidJniObject &Data);
    void SetLastSignedInAccountInfo(const QAndroidJniObject &AccountInfoObj);
    QImage AndroidBitmapToImage(const QAndroidJniObject &JniBmp);
    QPixmap GetAccountPhoto() const;
};

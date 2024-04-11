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
#include <QBuffer>
#include <QCoreApplication>
#include "QAndroidGoogleAccount.h"
#include "QtAndroidTools.h"

QAndroidGoogleAccount *QAndroidGoogleAccount::m_pInstance = nullptr;

QAndroidGoogleAccount::QAndroidGoogleAccount(QObject *parent) : QObject(parent),
                                                                m_javaGoogleAccount("com/falsinsoft/qtandroidtools/AndroidGoogleAccount",
                                                                                    QNativeInterface::QAndroidApplication::context())
{
    m_pInstance = this;

    if(m_javaGoogleAccount.isValid())
    {
        const JNINativeMethod jniMethod[] = {
            {"updateSignedInAccountInfo", "(Lcom/falsinsoft/qtandroidtools/AndroidGoogleAccount$AccountInfo;)V", reinterpret_cast<void*>(&QAndroidGoogleAccount::updateSignedInAccountInfo)},
            {"signedIn", "(Z)V", reinterpret_cast<void*>(&QAndroidGoogleAccount::signedInAccount)},
            {"signedOut", "()V", reinterpret_cast<void*>(&QAndroidGoogleAccount::signedOutAccount)}
        };
        QJniEnvironment JniEnv;
        jclass objectClass;

        objectClass = JniEnv->GetObjectClass(m_javaGoogleAccount.object<jobject>());
        JniEnv->RegisterNatives(objectClass, jniMethod, sizeof(jniMethod)/sizeof(JNINativeMethod));
        JniEnv->DeleteLocalRef(objectClass);
    }
}

QAndroidGoogleAccount* QAndroidGoogleAccount::create(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    return new QAndroidGoogleAccount();
}

QAndroidGoogleAccount* QAndroidGoogleAccount::instance()
{
    return m_pInstance;
}

bool QAndroidGoogleAccount::signIn(const QString &scopeName)
{
    if(m_javaGoogleAccount.isValid())
    {
        return m_javaGoogleAccount.callMethod<jboolean>("signIn",
                                                        "(Ljava/lang/String;)Z",
                                                        QJniObject::fromString(scopeName).object<jstring>()
                                                        );
    }
    return false;
}

bool QAndroidGoogleAccount::signInSelectAccount(const QString &scopeName)
{
    if(m_javaGoogleAccount.isValid())
    {
        const QJniObject signInIntent = m_javaGoogleAccount.callObjectMethod("getSignInIntent",
                                                                             "(Ljava/lang/String;)Landroid/content/Intent;",
                                                                             QJniObject::fromString(scopeName).object<jstring>()
                                                                             );
        if(signInIntent.isValid())
        {
            QtAndroidPrivate::startActivity(signInIntent, m_signInId, this);
            return true;
        }
    }
    return false;
}

bool QAndroidGoogleAccount::signOut()
{
    if(m_javaGoogleAccount.isValid())
    {
        return m_javaGoogleAccount.callMethod<jboolean>("signOut", "()Z");
    }
    return false;
}

bool QAndroidGoogleAccount::revokeAccess()
{
    if(m_javaGoogleAccount.isValid())
    {
        return m_javaGoogleAccount.callMethod<jboolean>("revokeAccess", "()Z");
    }
    return false;
}

void QAndroidGoogleAccount::setSignedInAccountInfo(const QJniObject &accountInfoObj)
{
    if(accountInfoObj.isValid())
    {
        const QJniObject photoObj = accountInfoObj.getObjectField("photo", "Landroid/graphics/Bitmap;");

        m_signedInAccountInfo.id = accountInfoObj.getObjectField<jstring>("id").toString();
        m_signedInAccountInfo.displayName = accountInfoObj.getObjectField<jstring>("displayName").toString();
        m_signedInAccountInfo.email = accountInfoObj.getObjectField<jstring>("email").toString();
        m_signedInAccountInfo.familyName = accountInfoObj.getObjectField<jstring>("familyName").toString();
        m_signedInAccountInfo.givenName = accountInfoObj.getObjectField<jstring>("givenName").toString();

        m_signedInAccountInfo.photo.clear();
        if(photoObj.isValid())
        {
            const QImage photo = QtAndroidTools::androidBitmapToImage(photoObj);
            QBuffer photoBuffer(&m_signedInAccountInfo.photo);

            photoBuffer.open(QIODevice::WriteOnly);
            photo.save(&photoBuffer, "PNG");
        }
    }
    else
    {
        m_signedInAccountInfo = QAndroidGoogleAccountInfo();
    }

    Q_EMIT signedInAccountInfoChanged();
}

const QAndroidGoogleAccountInfo& QAndroidGoogleAccount::getSignedInAccountInfo() const
{
    return m_signedInAccountInfo;
}

void QAndroidGoogleAccount::handleActivityResult(int receiverRequestCode, int resultCode, const QJniObject &data)
{
    Q_UNUSED(resultCode)

    if(receiverRequestCode == m_signInId)
    {
        if(m_javaGoogleAccount.isValid())
        {
            m_javaGoogleAccount.callMethod<void>("signInIntentDataResult", "(Landroid/content/Intent;)V", data.object<jobject>());
        }
    }
}

void QAndroidGoogleAccount::updateSignedInAccountInfo(JNIEnv *env, jobject thiz, jobject accountInfo)
{
    Q_UNUSED(env)
    Q_UNUSED(thiz)

    if(m_pInstance != nullptr)
    {
        m_pInstance->setSignedInAccountInfo(QJniObject(accountInfo));
    }
}

void QAndroidGoogleAccount::signedInAccount(JNIEnv *env, jobject thiz, jboolean signInSuccessfully)
{
    Q_UNUSED(env)
    Q_UNUSED(thiz)

    if(m_pInstance != nullptr)
    {
        Q_EMIT m_pInstance->signedIn(signInSuccessfully);
    }
}

void QAndroidGoogleAccount::signedOutAccount(JNIEnv *env, jobject thiz)
{
    Q_UNUSED(env)
    Q_UNUSED(thiz)

    if(m_pInstance != nullptr)
    {
        Q_EMIT m_pInstance->signedOut();
    }
}

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
#include "QAndroidGoogleAccount.h"

QAndroidGoogleAccount *QAndroidGoogleAccount::m_pInstance = nullptr;

QAndroidGoogleAccount::QAndroidGoogleAccount() : m_JavaGoogleAccount("com/falsinsoft/qtandroidtools/AndroidGoogleAccount",
                                                                     "(Landroid/app/Activity;)V",
                                                                     QtAndroid::androidActivity().object<jobject>())
{
    m_pInstance = this;

    LoadLastSignedInAccountInfo();
}

QObject* QAndroidGoogleAccount::qmlInstance(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine);
    Q_UNUSED(scriptEngine);

    return new QAndroidGoogleAccount();
}

QAndroidGoogleAccount* QAndroidGoogleAccount::instance()
{
    return m_pInstance;
}

bool QAndroidGoogleAccount::signIn()
{
    if(m_JavaGoogleAccount.isValid())
    {
        const std::function<void (int, int, const QAndroidJniObject &)> ActivityResultFunc = std::bind(&QAndroidGoogleAccount::ActivityResult, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);

        QtAndroid::startActivity(m_JavaGoogleAccount.callObjectMethod("getSignInIntent", "()Landroid/content/Intent;"),
                                 m_SignInId,
                                 ActivityResultFunc
                                 );
        return true;
    }

    return false;
}

void QAndroidGoogleAccount::LoadLastSignedInAccountInfo()
{
    if(m_JavaGoogleAccount.isValid())
    {
        const QAndroidJniObject AccountInfoObj = m_JavaGoogleAccount.callObjectMethod("getLastSignedInAccountInfo",
                                                                                      "()Lcom/falsinsoft/qtandroidtools/AndroidGoogleAccount$AccountInfo;"
                                                                                      );
        if(AccountInfoObj.isValid())
        {
            m_LastSignedInAccountInfo.Id = AccountInfoObj.getObjectField<jstring>("id").toString();
            m_LastSignedInAccountInfo.DisplayName = AccountInfoObj.getObjectField<jstring>("displayName").toString();
            m_LastSignedInAccountInfo.Email = AccountInfoObj.getObjectField<jstring>("email").toString();
            m_LastSignedInAccountInfo.FamilyName = AccountInfoObj.getObjectField<jstring>("familyName").toString();
            m_LastSignedInAccountInfo.GivenName = AccountInfoObj.getObjectField<jstring>("givenName").toString();
        }
    }
}

const QAndroidGoogleLastSignedInAccountInfo& QAndroidGoogleAccount::getLastSignedInAccountInfo() const
{
    return m_LastSignedInAccountInfo;
}

void QAndroidGoogleAccount::ActivityResult(int RequestCode, int ResultCode, const QAndroidJniObject &Data)
{
    Q_UNUSED(ResultCode);

    if(RequestCode == m_SignInId)
    {
        if(m_JavaGoogleAccount.isValid())
        {
            m_JavaGoogleAccount.callMethod<void>("signInIntentDataResult",
                                                 "(Landroid/content/Intent;)V",
                                                 Data.object()
                                                 );
            LoadLastSignedInAccountInfo();
        }
    }
}

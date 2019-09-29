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
#include <android/bitmap.h>

QAndroidGoogleAccount *QAndroidGoogleAccount::m_pInstance = nullptr;

QAndroidGoogleAccount::QAndroidGoogleAccount() : m_JavaGoogleAccount("com/falsinsoft/qtandroidtools/AndroidGoogleAccount",
                                                                     "(Landroid/app/Activity;)V",
                                                                     QtAndroid::androidActivity().object<jobject>())
{
    m_pInstance = this;

    if(m_JavaGoogleAccount.isValid())
    {
        const JNINativeMethod JniMethod[] = {
            {"loadedLastSignedInAccountInfo", "(Lcom/falsinsoft/qtandroidtools/AndroidGoogleAccount$AccountInfo;)V", reinterpret_cast<void *>(&QAndroidGoogleAccount::LoadedLastSignedInAccountInfo)}
        };
        QAndroidJniEnvironment JniEnv;
        jclass ObjectClass;

        ObjectClass = JniEnv->GetObjectClass(m_JavaGoogleAccount.object<jobject>());
        JniEnv->RegisterNatives(ObjectClass, JniMethod, sizeof(JniMethod)/sizeof(JNINativeMethod));
        JniEnv->DeleteLocalRef(ObjectClass);
    }
}

QObject* QAndroidGoogleAccount::qmlInstance(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(scriptEngine)

    QAndroidGoogleAccount *pAndroidGoogleAccount = new QAndroidGoogleAccount();
    engine->addImageProvider("LastSignedInAccountPhoto", new AccountPhotoImageProvider(pAndroidGoogleAccount));
    return pAndroidGoogleAccount;
}

QAndroidGoogleAccount* QAndroidGoogleAccount::instance()
{
    return m_pInstance;
}

bool QAndroidGoogleAccount::signIn(bool lastSignedIn)
{
    bool SignInSuccessfully;

    if(lastSignedIn == true)
    {
        SignInSuccessfully = SignInToLastSignedInAccount();

        if(SignInSuccessfully == false)
        {
            SignInSuccessfully = SelectSignInAccount();
        }
    }
    else
    {
        SignInSuccessfully = SelectSignInAccount();
    }

    return SignInSuccessfully;
}

void QAndroidGoogleAccount::UpdateLastSignedInAccountInfo(const QAndroidJniObject &AccountInfoObj)
{
    if(AccountInfoObj.isValid())
    {
        const QAndroidJniObject PhotoObj = AccountInfoObj.getObjectField("photo", "Landroid/graphics/Bitmap;");

        m_LastSignedInAccountInfo.Id = AccountInfoObj.getObjectField<jstring>("id").toString();
        m_LastSignedInAccountInfo.DisplayName = AccountInfoObj.getObjectField<jstring>("displayName").toString();
        m_LastSignedInAccountInfo.Email = AccountInfoObj.getObjectField<jstring>("email").toString();
        m_LastSignedInAccountInfo.FamilyName = AccountInfoObj.getObjectField<jstring>("familyName").toString();
        m_LastSignedInAccountInfo.GivenName = AccountInfoObj.getObjectField<jstring>("givenName").toString();

        if(PhotoObj.isValid())
        {
            const QImage PhotoImage = AndroidBitmapToImage(PhotoObj);

            if(PhotoImage.isNull() == false)
            {
                m_LastSignedInAccountPhoto = QPixmap::fromImage(PhotoImage);
            }
        }

        emit lastSignedInAccountInfoChanged();
    }
}

const QAndroidGoogleAccountInfo& QAndroidGoogleAccount::getLastSignedInAccountInfo() const
{
    return m_LastSignedInAccountInfo;
}

QPixmap QAndroidGoogleAccount::GetAccountPhoto() const
{
    return m_LastSignedInAccountPhoto;
}

bool QAndroidGoogleAccount::SignInToLastSignedInAccount()
{
    if(m_JavaGoogleAccount.isValid())
    {
        return m_JavaGoogleAccount.callMethod<jboolean>("loadLastSignedInAccountInfo", "()Z");
    }

    return false;
}

bool QAndroidGoogleAccount::SelectSignInAccount()
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

void QAndroidGoogleAccount::ActivityResult(int RequestCode, int ResultCode, const QAndroidJniObject &Data)
{
    Q_UNUSED(ResultCode)

    if(RequestCode == m_SignInId)
    {
        if(m_JavaGoogleAccount.isValid())
        {
            const bool SignInSuccessfully = m_JavaGoogleAccount.callMethod<jboolean>("signInIntentDataResult",
                                                                                     "(Landroid/content/Intent;)Z",
                                                                                     Data.object()
                                                                                     );
            if(SignInSuccessfully == false)
            {
                emit signedIn(false);
            }
        }
    }
}

void QAndroidGoogleAccount::LoadedLastSignedInAccountInfo(JNIEnv *env, jobject thiz, jobject accountInfo)
{
    Q_UNUSED(env)
    Q_UNUSED(thiz)

    if(m_pInstance != nullptr)
    {
        m_pInstance->UpdateLastSignedInAccountInfo(QAndroidJniObject(accountInfo));
        emit m_pInstance->signedIn(true);
    }
}

// Copyright KDAB (BogDan Vatra)
// https://www.kdab.com/qt-on-android-how-to-convert-qt-images-to-android-images-and-vice-versa-2/
QImage QAndroidGoogleAccount::AndroidBitmapToImage(const QAndroidJniObject &JniBmp)
{
    QAndroidJniEnvironment env;
    AndroidBitmapInfo info;
    if (AndroidBitmap_getInfo(env, JniBmp.object(), &info) != ANDROID_BITMAP_RESULT_SUCCESS)
        return QImage();

    QImage::Format format;
    switch (info.format) {
    case ANDROID_BITMAP_FORMAT_RGBA_8888:
        format = QImage::Format_RGBA8888;
        break;
    case ANDROID_BITMAP_FORMAT_RGB_565:
        format = QImage::Format_RGB16;
        break;
    case ANDROID_BITMAP_FORMAT_RGBA_4444:
        format = QImage::Format_ARGB4444_Premultiplied;
        break;
    case ANDROID_BITMAP_FORMAT_A_8:
        format = QImage::Format_Alpha8;
        break;
    default:
        return QImage();
    }

    void *pixels;
    if (AndroidBitmap_lockPixels(env, JniBmp.object(), &pixels) != ANDROID_BITMAP_RESULT_SUCCESS)
        return QImage();

    QImage image(info.width, info.height, format);

    if (info.stride == uint32_t(image.bytesPerLine())) {
        memcpy((void*)image.constBits(), pixels, info.stride * info.height);
    } else {
        uchar *bmpPtr = static_cast<uchar *>(pixels);
        const unsigned width = std::min(info.width, (uint)image.width());
        const unsigned height = std::min(info.height, (uint)image.height());
        for (unsigned y = 0; y < height; y++, bmpPtr += info.stride)
            memcpy((void*)image.constScanLine(y), bmpPtr, width);
    }

    if (AndroidBitmap_unlockPixels(env, JniBmp.object()) != ANDROID_BITMAP_RESULT_SUCCESS)
        return QImage();

    return image;
}

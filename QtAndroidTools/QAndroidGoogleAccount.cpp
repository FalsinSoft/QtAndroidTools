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
            {"updateSignedInAccountInfo", "(Lcom/falsinsoft/qtandroidtools/AndroidGoogleAccount$AccountInfo;)V", reinterpret_cast<void*>(&QAndroidGoogleAccount::UpdateSignedInAccountInfo)},
            {"signedIn", "(Z)V", reinterpret_cast<void*>(&QAndroidGoogleAccount::SignedIn)},
            {"signedOut", "()V", reinterpret_cast<void*>(&QAndroidGoogleAccount::SignedOut)}
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
    engine->addImageProvider("SignedInAccountPhoto", new AccountPhotoImageProvider(pAndroidGoogleAccount));
    return pAndroidGoogleAccount;
}

QAndroidGoogleAccount* QAndroidGoogleAccount::instance()
{
    return m_pInstance;
}

bool QAndroidGoogleAccount::signIn(int scope)
{
    if(m_JavaGoogleAccount.isValid())
    {
        return m_JavaGoogleAccount.callMethod<jboolean>("signIn", "(I)Z", scope);
    }
    return false;
}

bool QAndroidGoogleAccount::signInSelectAccount(int scope)
{
    if(m_JavaGoogleAccount.isValid())
    {
        const QAndroidJniObject SignInIntent = m_JavaGoogleAccount.callObjectMethod("getSignInIntent", "(I)Landroid/content/Intent;", scope);

        if(SignInIntent.isValid())
        {
            QtAndroid::startActivity(SignInIntent, m_SignInId, this);
            return true;
        }
    }
    return false;
}

bool QAndroidGoogleAccount::signOut()
{
    if(m_JavaGoogleAccount.isValid())
    {
        return m_JavaGoogleAccount.callMethod<jboolean>("signOut", "()Z");
    }
    return false;
}

bool QAndroidGoogleAccount::revokeAccess()
{
    if(m_JavaGoogleAccount.isValid())
    {
        return m_JavaGoogleAccount.callMethod<jboolean>("revokeAccess", "()Z");
    }
    return false;
}

void QAndroidGoogleAccount::SetSignedInAccountInfo(const QAndroidJniObject &AccountInfoObj)
{
    if(AccountInfoObj.isValid())
    {
        const QAndroidJniObject PhotoObj = AccountInfoObj.getObjectField("photo", "Landroid/graphics/Bitmap;");

        m_SignedInAccountInfo.Id = AccountInfoObj.getObjectField<jstring>("id").toString();
        m_SignedInAccountInfo.DisplayName = AccountInfoObj.getObjectField<jstring>("displayName").toString();
        m_SignedInAccountInfo.Email = AccountInfoObj.getObjectField<jstring>("email").toString();
        m_SignedInAccountInfo.FamilyName = AccountInfoObj.getObjectField<jstring>("familyName").toString();
        m_SignedInAccountInfo.GivenName = AccountInfoObj.getObjectField<jstring>("givenName").toString();

        if(PhotoObj.isValid())
            m_SignedInAccountPhoto = QPixmap::fromImage(AndroidBitmapToImage(PhotoObj));
        else
            m_SignedInAccountPhoto = QPixmap();
    }
    else
    {
        m_SignedInAccountInfo = QAndroidGoogleAccountInfo();
        m_SignedInAccountPhoto = QPixmap();
    }

    emit signedInAccountInfoChanged();
}

const QAndroidGoogleAccountInfo& QAndroidGoogleAccount::getSignedInAccountInfo() const
{
    return m_SignedInAccountInfo;
}

QPixmap QAndroidGoogleAccount::GetAccountPhoto() const
{
    return m_SignedInAccountPhoto;
}

void QAndroidGoogleAccount::handleActivityResult(int receiverRequestCode, int resultCode, const QAndroidJniObject &data)
{
    Q_UNUSED(resultCode)

    if(receiverRequestCode == m_SignInId)
    {
        if(m_JavaGoogleAccount.isValid())
        {
            m_JavaGoogleAccount.callMethod<void>("signInIntentDataResult", "(Landroid/content/Intent;)V", data.object<jobject>());
        }
    }
}

void QAndroidGoogleAccount::UpdateSignedInAccountInfo(JNIEnv *env, jobject thiz, jobject accountInfo)
{
    Q_UNUSED(env)
    Q_UNUSED(thiz)

    if(m_pInstance != nullptr)
    {
        m_pInstance->SetSignedInAccountInfo(QAndroidJniObject(accountInfo));
    }
}

void QAndroidGoogleAccount::SignedIn(JNIEnv *env, jobject thiz, jboolean signInSuccessfully)
{
    Q_UNUSED(env)
    Q_UNUSED(thiz)

    if(m_pInstance != nullptr)
    {
        emit m_pInstance->signedIn(signInSuccessfully);
    }
}

void QAndroidGoogleAccount::SignedOut(JNIEnv *env, jobject thiz)
{
    Q_UNUSED(env)
    Q_UNUSED(thiz)

    if(m_pInstance != nullptr)
    {
        emit m_pInstance->signedOut();
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

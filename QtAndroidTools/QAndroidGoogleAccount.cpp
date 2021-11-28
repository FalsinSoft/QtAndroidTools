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

QAndroidGoogleAccount::QAndroidGoogleAccount(QObject *parent) : QObject(parent),
                                                                m_javaGoogleAccount("com/falsinsoft/qtandroidtools/AndroidGoogleAccount",
                                                                                    "(Landroid/app/Activity;)V",
                                                                                    QtAndroid::androidActivity().object<jobject>())
{
    m_pInstance = this;

    if(m_javaGoogleAccount.isValid())
    {
        const JNINativeMethod jniMethod[] = {
            {"updateSignedInAccountInfo", "(Lcom/falsinsoft/qtandroidtools/AndroidGoogleAccount$AccountInfo;)V", reinterpret_cast<void*>(&QAndroidGoogleAccount::updateSignedInAccountInfo)},
            {"signedIn", "(Z)V", reinterpret_cast<void*>(&QAndroidGoogleAccount::signedInAccount)},
            {"signedOut", "()V", reinterpret_cast<void*>(&QAndroidGoogleAccount::signedOutAccount)}
        };
        QAndroidJniEnvironment JniEnv;
        jclass objectClass;

        objectClass = JniEnv->GetObjectClass(m_javaGoogleAccount.object<jobject>());
        JniEnv->RegisterNatives(objectClass, jniMethod, sizeof(jniMethod)/sizeof(JNINativeMethod));
        JniEnv->DeleteLocalRef(objectClass);
    }
}

QObject* QAndroidGoogleAccount::qmlInstance(QQmlEngine *engine, QJSEngine *scriptEngine)
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
                                                        QAndroidJniObject::fromString(scopeName).object<jstring>()
                                                        );
    }
    return false;
}

bool QAndroidGoogleAccount::signInSelectAccount(const QString &scopeName)
{
    if(m_javaGoogleAccount.isValid())
    {
        const QAndroidJniObject signInIntent = m_javaGoogleAccount.callObjectMethod("getSignInIntent",
                                                                                    "(Ljava/lang/String;)Landroid/content/Intent;",
                                                                                    QAndroidJniObject::fromString(scopeName).object<jstring>()
                                                                                    );
        if(signInIntent.isValid())
        {
            QtAndroid::startActivity(signInIntent, m_signInId, this);
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

void QAndroidGoogleAccount::setSignedInAccountInfo(const QAndroidJniObject &accountInfoObj)
{
    if(accountInfoObj.isValid())
    {
        const QAndroidJniObject photoObj = accountInfoObj.getObjectField("photo", "Landroid/graphics/Bitmap;");

        m_signedInAccountInfo.id = accountInfoObj.getObjectField<jstring>("id").toString();
        m_signedInAccountInfo.displayName = accountInfoObj.getObjectField<jstring>("displayName").toString();
        m_signedInAccountInfo.email = accountInfoObj.getObjectField<jstring>("email").toString();
        m_signedInAccountInfo.familyName = accountInfoObj.getObjectField<jstring>("familyName").toString();
        m_signedInAccountInfo.givenName = accountInfoObj.getObjectField<jstring>("givenName").toString();

        m_signedInAccountInfo.photo.clear();
        if(photoObj.isValid())
        {
            const QImage photo = androidBitmapToImage(photoObj);
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

void QAndroidGoogleAccount::handleActivityResult(int receiverRequestCode, int resultCode, const QAndroidJniObject &data)
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
        m_pInstance->setSignedInAccountInfo(QAndroidJniObject(accountInfo));
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

// Copyright KDAB (BogDan Vatra)
// https://www.kdab.com/qt-on-android-how-to-convert-qt-images-to-android-images-and-vice-versa-2/
QImage QAndroidGoogleAccount::androidBitmapToImage(const QAndroidJniObject &jniBmp)
{
    QAndroidJniEnvironment env;
    AndroidBitmapInfo info;
    if (AndroidBitmap_getInfo(env, jniBmp.object(), &info) != ANDROID_BITMAP_RESULT_SUCCESS)
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
    if (AndroidBitmap_lockPixels(env, jniBmp.object(), &pixels) != ANDROID_BITMAP_RESULT_SUCCESS)
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

    if (AndroidBitmap_unlockPixels(env, jniBmp.object()) != ANDROID_BITMAP_RESULT_SUCCESS)
        return QImage();

    return image;
}

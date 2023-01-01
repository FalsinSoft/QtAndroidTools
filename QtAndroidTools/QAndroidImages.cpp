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
#include <QCoreApplication>
#include "QAndroidImages.h"
#include "QtAndroidTools.h"

QAndroidImages *QAndroidImages::m_pInstance = nullptr;

QAndroidImages::QAndroidImages(QObject *parent) : QObject(parent),
                                                  m_javaImages("com/falsinsoft/qtandroidtools/AndroidImages",
                                                               "(Landroid/app/Activity;)V",
                                                               QNativeInterface::QAndroidApplication::context())
{
    m_pInstance = this;
}

QObject* QAndroidImages::qmlInstance(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine);
    Q_UNUSED(scriptEngine);

    return new QAndroidImages();
}

QAndroidImages* QAndroidImages::instance()
{
    return m_pInstance;
}

QStringList QAndroidImages::getAlbumsList()
{
    QJniEnvironment jniEnv;
    QStringList albumsList;

    if(QNativeInterface::QAndroidApplication::sdkVersion() >= 23)
    {
        if(QtAndroidPrivate::checkPermission("android.permission.READ_EXTERNAL_STORAGE").result() != QtAndroidPrivate::PermissionResult::Authorized) return albumsList;
    }

    if(m_javaImages.isValid())
    {
        const QJniObject albumsListObj = m_javaImages.callObjectMethod("getAlbumsList",
                                                                       "()[Ljava/lang/String;"
                                                                       );
        if(albumsListObj.isValid())
        {
            const jobjectArray albumsListObjArray = albumsListObj.object<jobjectArray>();
            const int albumsNum = jniEnv->GetArrayLength(albumsListObjArray);

            for(int i = 0; i < albumsNum; i++)
            {
                const QJniObject albumNameObj = QJniObject::fromLocalRef(jniEnv->GetObjectArrayElement(albumsListObjArray, i));
                albumsList << albumNameObj.toString();
            }
        }
    }

    return albumsList;
}

QStringList QAndroidImages::getAlbumImagesList(const QString &name)
{
    QJniEnvironment jniEnv;
    QStringList imagesList;

    if(QNativeInterface::QAndroidApplication::sdkVersion() >= 23)
    {
        if(QtAndroidPrivate::checkPermission("android.permission.READ_EXTERNAL_STORAGE").result() != QtAndroidPrivate::PermissionResult::Authorized) return imagesList;
    }

    if(m_javaImages.isValid())
    {
        const QJniObject imagesListObj = m_javaImages.callObjectMethod("getAlbumImagesList",
                                                                       "(Ljava/lang/String;)[Ljava/lang/String;",
                                                                       QJniObject::fromString(name).object<jstring>()
                                                                       );
        if(imagesListObj.isValid())
        {
            const jobjectArray imagesListObjArray = imagesListObj.object<jobjectArray>();
            const int imagesNum = jniEnv->GetArrayLength(imagesListObjArray);

            for(int i = 0; i < imagesNum; i++)
            {
                const QJniObject imagePathObj = QJniObject::fromLocalRef(jniEnv->GetObjectArrayElement(imagesListObjArray, i));
                imagesList << imagePathObj.toString();
            }
        }
    }

    return imagesList;
}

void QAndroidImages::addImageToGallery(const QString &imagePath)
{
    if(m_javaImages.isValid())
    {
        m_javaImages.callMethod<void>("addImageToGallery",
                                      "(Ljava/lang/String;)V",
                                      QJniObject::fromString(imagePath).object<jstring>()
                                      );
    }
}

bool QAndroidImages::saveImageToGallery(const QString &name, const QImage &image)
{
    if(m_javaImages.isValid())
    {
        const QJniObject androidBitmap = QtAndroidTools::imageToAndroidBitmap(image);

        return m_javaImages.callMethod<jboolean>("saveImageToGallery",
                                                 "(Ljava/lang/String;Landroid/graphics/Bitmap;)Z",
                                                 QJniObject::fromString(name).object<jstring>(),
                                                 androidBitmap.object()
                                                 );
    }

    return false;
}

bool QAndroidImages::imageFileExist(const QString &name)
{
    if(m_javaImages.isValid())
    {
        return m_javaImages.callMethod<jboolean>("imageFileExist",
                                                 "(Ljava/lang/String;)Z",
                                                 QJniObject::fromString(name).object<jstring>()
                                                 );
    }

    return false;
}

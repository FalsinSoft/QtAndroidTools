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
#include "QAndroidImages.h"

QAndroidImages *QAndroidImages::m_pInstance = nullptr;

QAndroidImages::QAndroidImages() : m_javaImages("com/falsinsoft/qtandroidtools/AndroidImages",
                                                "(Landroid/app/Activity;)V",
                                                QtAndroid::androidActivity().object<jobject>())
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

QVariantList QAndroidImages::getAlbumsList()
{
    QAndroidJniEnvironment jniEnv;
    QVariantList albumsList;

    if(QtAndroid::androidSdkVersion() >= 23)
    {
        if(QtAndroid::checkPermission("android.permission.READ_EXTERNAL_STORAGE") != QtAndroid::PermissionResult::Granted) return albumsList;
    }

    if(m_javaImages.isValid())
    {
        const QAndroidJniObject albumsListObj = m_javaImages.callObjectMethod("getAlbumsList",
                                                                              "()[Lcom/falsinsoft/qtandroidtools/AndroidImages$AlbumInfo;"
                                                                              );
        if(albumsListObj.isValid())
        {
            const jobjectArray albumsListObjArray = albumsListObj.object<jobjectArray>();
            const int albumsNum = jniEnv->GetArrayLength(albumsListObjArray);

            for(int i = 0; i < albumsNum; i++)
            {
                const QAndroidJniObject albumInfoObj = QAndroidJniObject::fromLocalRef(jniEnv->GetObjectArrayElement(albumsListObjArray, i));
                QVariantMap albumInfo;

                albumInfo["id"] = albumInfoObj.getField<jint>("id");
                albumInfo["name"] = albumInfoObj.getObjectField<jstring>("name").toString();

                albumsList << albumInfo;
            }
        }
    }

    return albumsList;
}

QStringList QAndroidImages::getAlbumImagesList(int albumId)
{
    QAndroidJniEnvironment jniEnv;
    QStringList imagesList;

    if(QtAndroid::androidSdkVersion() >= 23)
    {
        if(QtAndroid::checkPermission("android.permission.READ_EXTERNAL_STORAGE") != QtAndroid::PermissionResult::Granted) return imagesList;
    }

    if(m_javaImages.isValid())
    {
        const QAndroidJniObject imagesListObj = m_javaImages.callObjectMethod("getAlbumImagesList",
                                                                              "(I)[Ljava/lang/String;",
                                                                              albumId
                                                                              );
        if(imagesListObj.isValid())
        {
            const jobjectArray imagesListObjArray = imagesListObj.object<jobjectArray>();
            const int imagesNum = jniEnv->GetArrayLength(imagesListObjArray);

            for(int i = 0; i < imagesNum; i++)
            {
                const QAndroidJniObject imagePathObj = QAndroidJniObject::fromLocalRef(jniEnv->GetObjectArrayElement(imagesListObjArray, i));
                imagesList << imagePathObj.toString();
            }
        }
    }

    return imagesList;
}

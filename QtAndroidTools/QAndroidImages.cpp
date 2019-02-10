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

QAndroidImages::QAndroidImages() : m_JavaImages("com/falsinsoft/qtandroidtools/Images",
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
    QAndroidJniEnvironment qJniEnv;
    QVariantList AlbumsList;

    if(m_JavaImages.isValid())
    {
        const QAndroidJniObject AlbumsListObj = m_JavaImages.callObjectMethod("getAlbumsList",
                                                                              "()[Lcom/falsinsoft/qtandroidtools/Images$AlbumInfo;"
                                                                              );
        if(AlbumsListObj.isValid())
        {
            const jobjectArray AlbumsListObjArray = AlbumsListObj.object<jobjectArray>();
            const int AlbumsNum = qJniEnv->GetArrayLength(AlbumsListObjArray);

            for(int i = 0; i < AlbumsNum; i++)
            {
                const QAndroidJniObject AlbumInfoObj = QAndroidJniObject::fromLocalRef(qJniEnv->GetObjectArrayElement(AlbumsListObjArray, i));
                QVariantMap AlbumInfo;

                AlbumInfo["id"] = AlbumInfoObj.getField<jint>("id");
                AlbumInfo["name"] = AlbumInfoObj.getObjectField<jstring>("name").toString();

                AlbumsList << AlbumInfo;
            }
        }
    }

    return AlbumsList;
}

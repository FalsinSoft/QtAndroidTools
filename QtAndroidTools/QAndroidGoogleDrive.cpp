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
#include "QAndroidGoogleDrive.h"

QAndroidGoogleDrive *QAndroidGoogleDrive::m_pInstance = nullptr;

QAndroidGoogleDrive::QAndroidGoogleDrive() : m_javaGoogleDrive("com/falsinsoft/qtandroidtools/AndroidGoogleDrive",
                                                               "(Landroid/app/Activity;)V",
                                                               QtAndroid::androidActivity().object<jobject>())
{
    m_pInstance = this;

    if(m_javaGoogleDrive.isValid())
    {
        const JNINativeMethod jniMethod[] = {
            {"downloadProgressChanged", "(ID)V", reinterpret_cast<void *>(&QAndroidGoogleDrive::downloadDriveProgressChanged)},
            {"uploadProgressChanged", "(ID)V", reinterpret_cast<void *>(&QAndroidGoogleDrive::uploadDriveProgressChanged)}
        };
        QAndroidJniEnvironment jniEnv;
        jclass objectClass;

        objectClass = jniEnv->GetObjectClass(m_javaGoogleDrive.object<jobject>());
        jniEnv->RegisterNatives(objectClass, jniMethod, sizeof(jniMethod)/sizeof(JNINativeMethod));
        jniEnv->DeleteLocalRef(objectClass);
    }
    loadScopeDefinitions();
}

QObject* QAndroidGoogleDrive::qmlInstance(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine);
    Q_UNUSED(scriptEngine);

    return new QAndroidGoogleDrive();
}

QAndroidGoogleDrive* QAndroidGoogleDrive::instance()
{
    return m_pInstance;
}

bool QAndroidGoogleDrive::authenticate(const QString &appName, const QString &scopeName)
{
    if(m_javaGoogleDrive.isValid())
    {
        m_isAuthenticated = m_javaGoogleDrive.callMethod<jboolean>("authenticate",
                                                                   "(Ljava/lang/String;Ljava/lang/String;)Z",
                                                                   QAndroidJniObject::fromString(appName).object<jstring>(),
                                                                   QAndroidJniObject::fromString(scopeName).object<jstring>()
                                                                   );
        Q_EMIT isAuthenticatedChanged();
        return m_isAuthenticated;
    }
    return false;
}

QVariantList QAndroidGoogleDrive::getFilesList(const QString &query)
{
    QAndroidJniEnvironment jniEnv;
    QVariantList filesList;

    if(m_javaGoogleDrive.isValid())
    {
        const QAndroidJniObject filesListObj = m_javaGoogleDrive.callObjectMethod("listFiles",
                                                                                  "(Ljava/lang/String;)[Lcom/falsinsoft/qtandroidtools/AndroidGoogleDrive$DriveFile;",
                                                                                  QAndroidJniObject::fromString(query).object<jstring>()
                                                                                  );
        if(filesListObj.isValid())
        {
            const jobjectArray filesListJObjArray = filesListObj.object<jobjectArray>();
            const int filesNum = jniEnv->GetArrayLength(filesListJObjArray);

            for(int i = 0; i < filesNum; i++)
            {
                const QAndroidJniObject jniObject = jniEnv->GetObjectArrayElement(filesListJObjArray, i);
                QAndroidJniObject parentsListObj;
                QVariantList fileParents;
                QVariantMap fileInfo;

                fileInfo["id"] = jniObject.getObjectField<jstring>("id").toString();
                fileInfo["name"] = jniObject.getObjectField<jstring>("name").toString();
                fileInfo["mimeType"] = jniObject.getObjectField<jstring>("mimeType").toString();
                parentsListObj = jniObject.getObjectField("parents", "[Ljava/lang/String;");
                if(parentsListObj.isValid())
                {
                    const jobjectArray parentsListJObjArray = parentsListObj.object<jobjectArray>();
                    const int parentsNum = jniEnv->GetArrayLength(parentsListJObjArray);

                    for(int p = 0; p < parentsNum; p++)
                    {
                        fileParents << QAndroidJniObject(jniEnv->GetObjectArrayElement(parentsListJObjArray, p)).toString();
                    }
                }
                fileInfo["parents"] = fileParents;

                filesList << fileInfo;
            }
        }
    }

    return filesList;
}

QString QAndroidGoogleDrive::getRootId()
{
    QString rootId;

    if(m_javaGoogleDrive.isValid())
    {
        const QAndroidJniObject rootIdObj = m_javaGoogleDrive.callObjectMethod("getRootId",
                                                                               "()Ljava/lang/String;"
                                                                               );
        if(rootIdObj.isValid())
        {
            rootId = rootIdObj.toString();
        }
    }

    return rootId;
}

bool QAndroidGoogleDrive::createFolder(const QString &name, const QString &parentFolderId)
{
    if(m_javaGoogleDrive.isValid())
    {
        return m_javaGoogleDrive.callMethod<jboolean>("createFolder",
                                                      "(Ljava/lang/String;Ljava/lang/String;)Z",
                                                      QAndroidJniObject::fromString(name).object<jstring>(),
                                                      QAndroidJniObject::fromString(parentFolderId).object<jstring>()
                                                      );
    }
    return false;
}

bool QAndroidGoogleDrive::isFolder(const QString &fileId)
{
    const FILE_METADATA fileMetadata = getFileMetadata(fileId);
    return (fileMetadata.id.isEmpty() == false && fileMetadata.mimeType == "application/vnd.google-apps.folder") ? true : false;
}

bool QAndroidGoogleDrive::downloadFile(const QString &fileId, const QString &localFilePath)
{
    if(m_javaGoogleDrive.isValid())
    {
        return m_javaGoogleDrive.callMethod<jboolean>("downloadFile",
                                                      "(Ljava/lang/String;Ljava/lang/String;)Z",
                                                      QAndroidJniObject::fromString(fileId).object<jstring>(),
                                                      QAndroidJniObject::fromString(localFilePath).object<jstring>()
                                                      );
    }
    return false;
}

QString QAndroidGoogleDrive::uploadFile(const QString &localFilePath, const QString &mimeType, const QString &parentFolderId)
{
    const QFileInfo fileInfo(localFilePath);
    QString uploadedFileId;

    if(m_javaGoogleDrive.isValid())
    {
        const QAndroidJniObject uploadedFileIdObj = m_javaGoogleDrive.callObjectMethod("uploadFile",
                                                                                       "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;",
                                                                                       QAndroidJniObject::fromString(localFilePath).object<jstring>(),
                                                                                       QAndroidJniObject::fromString(fileInfo.fileName()).object<jstring>(),
                                                                                       QAndroidJniObject::fromString(mimeType).object<jstring>(),
                                                                                       QAndroidJniObject::fromString(parentFolderId).object<jstring>()
                                                                                       );
        if(uploadedFileIdObj.isValid())
        {
            uploadedFileId = uploadedFileIdObj.toString();
        }
    }

    return uploadedFileId;
}

bool QAndroidGoogleDrive::moveFile(const QString &fileId, const QString &folderId)
{
    if(m_javaGoogleDrive.isValid())
    {
        return m_javaGoogleDrive.callMethod<jboolean>("moveFile",
                                                      "(Ljava/lang/String;Ljava/lang/String;)Z",
                                                      QAndroidJniObject::fromString(fileId).object<jstring>(),
                                                      QAndroidJniObject::fromString(folderId).object<jstring>()
                                                      );
    }
    return false;
}

bool QAndroidGoogleDrive::deleteFile(const QString &fileId)
{
    if(m_javaGoogleDrive.isValid())
    {
        return m_javaGoogleDrive.callMethod<jboolean>("deleteFile",
                                                      "(Ljava/lang/String;)Z",
                                                      QAndroidJniObject::fromString(fileId).object<jstring>()
                                                      );
    }
    return false;
}

QAndroidGoogleDrive::FILE_METADATA QAndroidGoogleDrive::getFileMetadata(const QString &fileId)
{
    const QString fields("id, mimeType");
    FILE_METADATA fileMetadata;

    if(m_javaGoogleDrive.isValid())
    {
        const QAndroidJniObject fileMetadataObj = m_javaGoogleDrive.callObjectMethod("getFileMetadata",
                                                                                     "(Ljava/lang/String;Ljava/lang/String;)Lcom/google/api/services/drive/model/File;",
                                                                                     QAndroidJniObject::fromString(fileId).object<jstring>(),
                                                                                     QAndroidJniObject::fromString(fields).object<jstring>()
                                                                                     );
        if(fileMetadataObj.isValid())
        {
            fileMetadata.id = fileMetadataObj.callObjectMethod<jstring>("getId").toString();
            fileMetadata.mimeType = fileMetadataObj.callObjectMethod<jstring>("getMimeType").toString();
        }
    }

    return fileMetadata;
}

void QAndroidGoogleDrive::downloadDriveProgressChanged(JNIEnv *env, jobject thiz, jint state, jdouble progress)
{
    Q_UNUSED(env)
    Q_UNUSED(thiz)

    if(m_pInstance != nullptr)
    {
        Q_EMIT m_pInstance->downloadProgressChanged(state, progress);
    }
}

void QAndroidGoogleDrive::uploadDriveProgressChanged(JNIEnv *env, jobject thiz, jint state, jdouble progress)
{
    Q_UNUSED(env)
    Q_UNUSED(thiz)

    if(m_pInstance != nullptr)
    {
        Q_EMIT m_pInstance->uploadProgressChanged(state, progress);
    }
}

void QAndroidGoogleDrive::loadScopeDefinitions()
{
    const char scopesClass[] = "com/google/api/services/drive/DriveScopes";

    m_scopeList[0] = QAndroidJniObject::getStaticObjectField<jstring>(scopesClass, "DRIVE").toString();
    m_scopeList[1] = QAndroidJniObject::getStaticObjectField<jstring>(scopesClass, "DRIVE_APPDATA").toString();
    m_scopeList[2] = QAndroidJniObject::getStaticObjectField<jstring>(scopesClass, "DRIVE_FILE").toString();
    m_scopeList[3] = QAndroidJniObject::getStaticObjectField<jstring>(scopesClass, "DRIVE_METADATA").toString();
    m_scopeList[4] = QAndroidJniObject::getStaticObjectField<jstring>(scopesClass, "DRIVE_METADATA_READONLY").toString();
    m_scopeList[5] = QAndroidJniObject::getStaticObjectField<jstring>(scopesClass, "DRIVE_PHOTOS_READONLY").toString();
    m_scopeList[6] = QAndroidJniObject::getStaticObjectField<jstring>(scopesClass, "DRIVE_READONLY").toString();
    m_scopeList[7] = QAndroidJniObject::getStaticObjectField<jstring>(scopesClass, "DRIVE_SCRIPTS").toString();
}

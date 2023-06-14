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
#include <QFileInfo>
#include <QCoreApplication>
#include "QAndroidGoogleDrive.h"

QAndroidGoogleDrive *QAndroidGoogleDrive::m_pInstance = nullptr;

QAndroidGoogleDrive::QAndroidGoogleDrive(QObject *parent) : QObject(parent),
                                                            m_javaGoogleDrive("com/falsinsoft/qtandroidtools/AndroidGoogleDrive",
                                                                              "(Landroid/app/Activity;)V",
                                                                              QNativeInterface::QAndroidApplication::context())
{
    m_pInstance = this;

    if(m_javaGoogleDrive.isValid())
    {
        const JNINativeMethod jniMethod[] = {
            {"downloadProgressChanged", "(ID)V", reinterpret_cast<void *>(&QAndroidGoogleDrive::downloadDriveProgressChanged)},
            {"uploadProgressChanged", "(ID)V", reinterpret_cast<void *>(&QAndroidGoogleDrive::uploadDriveProgressChanged)}
        };
        QJniEnvironment jniEnv;
        jclass objectClass;

        objectClass = jniEnv->GetObjectClass(m_javaGoogleDrive.object<jobject>());
        jniEnv->RegisterNatives(objectClass, jniMethod, sizeof(jniMethod)/sizeof(JNINativeMethod));
        jniEnv->DeleteLocalRef(objectClass);
    }
    loadScopeDefinitions();
}

QAndroidGoogleDrive* QAndroidGoogleDrive::create(QQmlEngine *engine, QJSEngine *scriptEngine)
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
                                                                   QJniObject::fromString(appName).object<jstring>(),
                                                                   QJniObject::fromString(scopeName).object<jstring>()
                                                                   );
        Q_EMIT isAuthenticatedChanged();
        return m_isAuthenticated;
    }
    return false;
}

QVariantList QAndroidGoogleDrive::getFilesList(const QString &query)
{
    QJniEnvironment jniEnv;
    QVariantList filesList;

    if(m_javaGoogleDrive.isValid())
    {
        const QJniObject filesListObj = m_javaGoogleDrive.callObjectMethod("listFiles",
                                                                           "(Ljava/lang/String;)[Lcom/falsinsoft/qtandroidtools/AndroidGoogleDrive$DriveFile;",
                                                                           QJniObject::fromString(query).object<jstring>()
                                                                           );
        if(filesListObj.isValid())
        {
            const jobjectArray filesListJObjArray = filesListObj.object<jobjectArray>();
            const int filesNum = jniEnv->GetArrayLength(filesListJObjArray);

            for(int i = 0; i < filesNum; i++)
            {
                const QJniObject jniObject = jniEnv->GetObjectArrayElement(filesListJObjArray, i);
                QJniObject parentsListObj;
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
                        fileParents << QJniObject(jniEnv->GetObjectArrayElement(parentsListJObjArray, p)).toString();
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
        const QJniObject rootIdObj = m_javaGoogleDrive.callObjectMethod("getRootId",
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
                                                      QJniObject::fromString(name).object<jstring>(),
                                                      QJniObject::fromString(parentFolderId).object<jstring>()
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
                                                      QJniObject::fromString(fileId).object<jstring>(),
                                                      QJniObject::fromString(localFilePath).object<jstring>()
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
        const QJniObject uploadedFileIdObj = m_javaGoogleDrive.callObjectMethod("uploadFile",
                                                                                "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;",
                                                                                QJniObject::fromString(localFilePath).object<jstring>(),
                                                                                QJniObject::fromString(fileInfo.fileName()).object<jstring>(),
                                                                                QJniObject::fromString(mimeType).object<jstring>(),
                                                                                QJniObject::fromString(parentFolderId).object<jstring>()
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
                                                      QJniObject::fromString(fileId).object<jstring>(),
                                                      QJniObject::fromString(folderId).object<jstring>()
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
                                                      QJniObject::fromString(fileId).object<jstring>()
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
        const QJniObject fileMetadataObj = m_javaGoogleDrive.callObjectMethod("getFileMetadata",
                                                                              "(Ljava/lang/String;Ljava/lang/String;)Lcom/google/api/services/drive/model/File;",
                                                                              QJniObject::fromString(fileId).object<jstring>(),
                                                                              QJniObject::fromString(fields).object<jstring>()
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

    m_scopeList[0] = QJniObject::getStaticObjectField<jstring>(scopesClass, "DRIVE").toString();
    m_scopeList[1] = QJniObject::getStaticObjectField<jstring>(scopesClass, "DRIVE_APPDATA").toString();
    m_scopeList[2] = QJniObject::getStaticObjectField<jstring>(scopesClass, "DRIVE_FILE").toString();
    m_scopeList[3] = QJniObject::getStaticObjectField<jstring>(scopesClass, "DRIVE_METADATA").toString();
    m_scopeList[4] = QJniObject::getStaticObjectField<jstring>(scopesClass, "DRIVE_METADATA_READONLY").toString();
    m_scopeList[5] = QJniObject::getStaticObjectField<jstring>(scopesClass, "DRIVE_PHOTOS_READONLY").toString();
    m_scopeList[6] = QJniObject::getStaticObjectField<jstring>(scopesClass, "DRIVE_READONLY").toString();
    m_scopeList[7] = QJniObject::getStaticObjectField<jstring>(scopesClass, "DRIVE_SCRIPTS").toString();
}

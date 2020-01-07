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

QAndroidGoogleDrive::QAndroidGoogleDrive() : m_JavaGoogleDrive("com/falsinsoft/qtandroidtools/AndroidGoogleDrive",
                                                               "(Landroid/app/Activity;)V",
                                                               QtAndroid::androidActivity().object<jobject>())
{
    m_pInstance = this;
    LoadScopeDefinitions();
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

bool QAndroidGoogleDrive::authenticate(const QString &AppName, const QString &ScopeName)
{
    if(m_JavaGoogleDrive.isValid())
    {
        m_isAuthenticated = m_JavaGoogleDrive.callMethod<jboolean>("authenticate",
                                                                   "(Ljava/lang/String;Ljava/lang/String;)Z",
                                                                   QAndroidJniObject::fromString(AppName).object<jstring>(),
                                                                   QAndroidJniObject::fromString(ScopeName).object<jstring>()
                                                                   );
        emit isAuthenticatedChanged();
        return m_isAuthenticated;
    }
    return false;
}

QVariantList QAndroidGoogleDrive::getFilesList(const QString &Query)
{
    QAndroidJniEnvironment JniEnv;
    QVariantList FilesList;

    if(m_JavaGoogleDrive.isValid())
    {
        const QAndroidJniObject FilesListObj = m_JavaGoogleDrive.callObjectMethod("listFiles",
                                                                                  "(Ljava/lang/String;)[Lcom/falsinsoft/qtandroidtools/AndroidGoogleDrive$DriveFile;",
                                                                                  QAndroidJniObject::fromString(Query).object<jstring>()
                                                                                  );
        if(FilesListObj.isValid())
        {
            const jobjectArray FilesListJObjArray = FilesListObj.object<jobjectArray>();
            const int FilesNum = JniEnv->GetArrayLength(FilesListJObjArray);

            for(int i = 0; i < FilesNum; i++)
            {
                const QAndroidJniObject JniObject = JniEnv->GetObjectArrayElement(FilesListJObjArray, i);
                QAndroidJniObject ParentsListObj;
                QVariantList FileParents;
                QVariantMap FileInfo;

                FileInfo["id"] = JniObject.getObjectField<jstring>("id").toString();
                FileInfo["name"] = JniObject.getObjectField<jstring>("name").toString();
                FileInfo["mimeType"] = JniObject.getObjectField<jstring>("mimeType").toString();
                ParentsListObj = JniObject.getObjectField("parents", "[Ljava/lang/String;");
                if(ParentsListObj.isValid())
                {
                    const jobjectArray ParentsListJObjArray = ParentsListObj.object<jobjectArray>();
                    const int ParentsNum = JniEnv->GetArrayLength(ParentsListJObjArray);

                    for(int p = 0; p < ParentsNum; p++)
                    {
                        FileParents << QAndroidJniObject(JniEnv->GetObjectArrayElement(ParentsListJObjArray, p)).toString();
                    }
                }
                FileInfo["parents"] = FileParents;

                FilesList << FileInfo;
            }
        }
    }

    return FilesList;
}

QString QAndroidGoogleDrive::getRootId()
{
    QString RootId;

    if(m_JavaGoogleDrive.isValid())
    {
        const QAndroidJniObject RootIdObj = m_JavaGoogleDrive.callObjectMethod("getRootId",
                                                                               "()Ljava/lang/String;"
                                                                               );
        if(RootIdObj.isValid())
        {
            RootId = RootIdObj.toString();
        }
    }

    return RootId;
}

void QAndroidGoogleDrive::LoadScopeDefinitions()
{
    const char ScopesClass[] = "com/google/api/services/drive/DriveScopes";

    m_ScopeList[0] = QAndroidJniObject::getStaticObjectField<jstring>(ScopesClass, "DRIVE").toString();
    m_ScopeList[1] = QAndroidJniObject::getStaticObjectField<jstring>(ScopesClass, "DRIVE_APPDATA").toString();
    m_ScopeList[2] = QAndroidJniObject::getStaticObjectField<jstring>(ScopesClass, "DRIVE_FILE").toString();
    m_ScopeList[3] = QAndroidJniObject::getStaticObjectField<jstring>(ScopesClass, "DRIVE_METADATA").toString();
    m_ScopeList[4] = QAndroidJniObject::getStaticObjectField<jstring>(ScopesClass, "DRIVE_METADATA_READONLY").toString();
    m_ScopeList[5] = QAndroidJniObject::getStaticObjectField<jstring>(ScopesClass, "DRIVE_PHOTOS_READONLY").toString();
    m_ScopeList[6] = QAndroidJniObject::getStaticObjectField<jstring>(ScopesClass, "DRIVE_READONLY").toString();
    m_ScopeList[7] = QAndroidJniObject::getStaticObjectField<jstring>(ScopesClass, "DRIVE_SCRIPTS").toString();
}

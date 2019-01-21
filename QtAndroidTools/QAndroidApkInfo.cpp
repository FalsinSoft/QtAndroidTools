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
#include "QAndroidApkInfo.h"

QAndroidApkInfo *QAndroidApkInfo::m_pInstance = nullptr;

QAndroidApkInfo::QAndroidApkInfo()
{
    m_pInstance = this;
    LoadApkPackageInfo();
}

QObject* QAndroidApkInfo::qmlInstance(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine);
    Q_UNUSED(scriptEngine);

    return new QAndroidApkInfo();
}

QAndroidApkInfo* QAndroidApkInfo::instance()
{
    return m_pInstance;
}

qlonglong QAndroidApkInfo::getFirstInstallTime() const
{
    return m_ApkInfo.FirstInstallTime;
}

qlonglong QAndroidApkInfo::getLastUpdateTime() const
{
    return m_ApkInfo.LastUpdateTime;
}

const QString& QAndroidApkInfo::getPackageName() const
{
    return m_ApkInfo.PackageName;
}

const QStringList& QAndroidApkInfo::getRequestedPermissions() const
{
    return m_ApkInfo.RequestedPermissions;
}

int QAndroidApkInfo::getVersionCode() const
{
    return m_ApkInfo.VersionCode;
}

const QString& QAndroidApkInfo::getVersionName() const
{
    return m_ApkInfo.VersionName;
}

void QAndroidApkInfo::LoadApkPackageInfo()
{
    const int InfoFlags = QAndroidJniObject::getStaticField<jint>("android/content/pm/PackageManager", "GET_PERMISSIONS");
    const QAndroidJniObject Activity = QtAndroid::androidActivity();
    QAndroidJniObject PackageName, PackageManager, PackageInfo;
    QAndroidJniEnvironment JniEnv;

    PackageManager = Activity.callObjectMethod("getPackageManager", "()Landroid/content/pm/PackageManager;");
    PackageName = Activity.callObjectMethod("getPackageName", "()Ljava/lang/String;");

    PackageInfo = PackageManager.callObjectMethod("getPackageInfo",
                                                  "(Ljava/lang/String;I)Landroid/content/pm/PackageInfo;",
                                                  PackageName.object<jstring>(),
                                                  InfoFlags
                                                  );
    if(!JniEnv->ExceptionCheck())
    {
        m_ApkInfo.FirstInstallTime = PackageInfo.getField<jlong>("firstInstallTime");
        m_ApkInfo.LastUpdateTime = PackageInfo.getField<jlong>("lastUpdateTime");
        m_ApkInfo.PackageName = PackageInfo.getObjectField<jstring>("packageName").toString();
        m_ApkInfo.RequestedPermissions = GetStringListField(PackageInfo, "requestedPermissions");
        m_ApkInfo.VersionCode = PackageInfo.getField<jint>("versionCode");
        m_ApkInfo.VersionName = PackageInfo.getObjectField<jstring>("versionName").toString();
    }
    else
    {
        JniEnv->ExceptionClear();
    }
}

QStringList QAndroidApkInfo::GetStringListField(const QAndroidJniObject &JniObject, const QString &FieldName) const
{
    const QAndroidJniObject StringArrayObj = JniObject.getObjectField(FieldName.toStdString().c_str(), "[Ljava/lang/String;");
    QAndroidJniEnvironment JniEnv;
    QStringList StringList;

    if(StringArrayObj.isValid())
    {
        const jobjectArray StringJObjArray = StringArrayObj.object<jobjectArray>();
        const int StringsNum = JniEnv->GetArrayLength(StringJObjArray);

        for(int i = 0; i < StringsNum; i++)
        {
            StringList << QAndroidJniObject::fromLocalRef(JniEnv->GetObjectArrayElement(StringJObjArray, i)).toString();
        }
    }

    return StringList;
}

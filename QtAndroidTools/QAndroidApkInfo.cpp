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

QAndroidApkInfo::QAndroidApkInfo(QObject *parent) : QObject(parent)
{
    m_pInstance = this;
    loadApkPackageInfo();
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
    return m_apkInfo.firstInstallTime;
}

qlonglong QAndroidApkInfo::getLastUpdateTime() const
{
    return m_apkInfo.lastUpdateTime;
}

const QString& QAndroidApkInfo::getPackageName() const
{
    return m_apkInfo.packageName;
}

const QStringList& QAndroidApkInfo::getRequestedPermissions() const
{
    return m_apkInfo.requestedPermissions;
}

int QAndroidApkInfo::getVersionCode() const
{
    return m_apkInfo.versionCode;
}

const QString& QAndroidApkInfo::getVersionName() const
{
    return m_apkInfo.versionName;
}

void QAndroidApkInfo::loadApkPackageInfo()
{
    const int infoFlags = QAndroidJniObject::getStaticField<jint>("android/content/pm/PackageManager", "GET_PERMISSIONS");
    const QAndroidJniObject activity = QtAndroid::androidActivity();
    QAndroidJniObject packageName, packageManager, packageInfo;
    QAndroidJniEnvironment jniEnv;

    packageManager = activity.callObjectMethod("getPackageManager", "()Landroid/content/pm/PackageManager;");
    packageName = activity.callObjectMethod("getPackageName", "()Ljava/lang/String;");

    packageInfo = packageManager.callObjectMethod("getPackageInfo",
                                                  "(Ljava/lang/String;I)Landroid/content/pm/PackageInfo;",
                                                  packageName.object<jstring>(),
                                                  infoFlags
                                                  );
    if(!jniEnv->ExceptionCheck())
    {
        m_apkInfo.firstInstallTime = packageInfo.getField<jlong>("firstInstallTime");
        m_apkInfo.lastUpdateTime = packageInfo.getField<jlong>("lastUpdateTime");
        m_apkInfo.packageName = packageInfo.getObjectField<jstring>("packageName").toString();
        m_apkInfo.requestedPermissions = getStringListField(packageInfo, "requestedPermissions");
        m_apkInfo.versionCode = packageInfo.getField<jint>("versionCode");
        m_apkInfo.versionName = packageInfo.getObjectField<jstring>("versionName").toString();
    }
    else
    {
        jniEnv->ExceptionClear();
    }
}

QStringList QAndroidApkInfo::getStringListField(const QAndroidJniObject &jniObject, const QString &fieldName) const
{
    const QAndroidJniObject stringArrayObj = jniObject.getObjectField(fieldName.toStdString().c_str(), "[Ljava/lang/String;");
    QAndroidJniEnvironment jniEnv;
    QStringList stringList;

    if(stringArrayObj.isValid())
    {
        const jobjectArray stringJObjArray = stringArrayObj.object<jobjectArray>();
        const int StringsNum = jniEnv->GetArrayLength(stringJObjArray);

        for(int i = 0; i < StringsNum; i++)
        {
            stringList << QAndroidJniObject::fromLocalRef(jniEnv->GetObjectArrayElement(stringJObjArray, i)).toString();
        }
    }

    return stringList;
}

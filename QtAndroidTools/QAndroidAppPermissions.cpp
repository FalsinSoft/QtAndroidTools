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
#include "QAndroidAppPermissions.h"

QAndroidAppPermissions *QAndroidAppPermissions::m_pInstance = nullptr;

QAndroidAppPermissions::QAndroidAppPermissions()
{
    m_pInstance = this;
}

QObject* QAndroidAppPermissions::qmlInstance(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine);
    Q_UNUSED(scriptEngine);

    return new QAndroidAppPermissions();
}

QAndroidAppPermissions* QAndroidAppPermissions::instance()
{
    return m_pInstance;
}

void QAndroidAppPermissions::requestPermissions(const QStringList &permissionsNameList)
{
    QtAndroid::PermissionResultMap resultMap;

    if(QtAndroid::androidSdkVersion() >= 23)
    {
        QStringList permissionsNotGrantedList;

        for(int i = 0; i < permissionsNameList.count(); i++)
        {
            if(QtAndroid::checkPermission(permissionsNameList[i]) != QtAndroid::PermissionResult::Granted)
            {
                permissionsNotGrantedList << permissionsNameList[i];
            }
            else
            {
                resultMap[permissionsNameList[i]] = QtAndroid::PermissionResult::Granted;
            }
        }

        if(permissionsNotGrantedList.count() > 0)
        {
            QtAndroid::requestPermissions(permissionsNotGrantedList, std::bind(&QAndroidAppPermissions::requestPermissionResults, this, std::placeholders::_1));
        }
    }
    else
    {
        for(int i = 0; i < permissionsNameList.count(); i++)
        {
            resultMap[permissionsNameList[i]] = QtAndroid::PermissionResult::Granted;
        }
    }

    if(resultMap.size() > 0)
    {
        Q_EMIT requestPermissionsResults(convertToVariantList(resultMap));
    }
}

void QAndroidAppPermissions::requestPermission(const QString &permissionName)
{
    if(QtAndroid::androidSdkVersion() >= 23 && QtAndroid::checkPermission(permissionName) != QtAndroid::PermissionResult::Granted)
    {
        QtAndroid::requestPermissions(QStringList() << permissionName, std::bind(&QAndroidAppPermissions::requestPermissionResults, this, std::placeholders::_1));
    }
    else
    {
        QtAndroid::PermissionResultMap resultMap;
        resultMap[permissionName] = QtAndroid::PermissionResult::Granted;
        Q_EMIT requestPermissionsResults(convertToVariantList(resultMap));
    }
}

bool QAndroidAppPermissions::shouldShowRequestPermissionInfo(const QString &permissionName)
{
    if(QtAndroid::androidSdkVersion() >= 23)
    {
        return QtAndroid::shouldShowRequestPermissionRationale(permissionName);
    }

    return false;
}

bool QAndroidAppPermissions::isPermissionGranted(const QString &permissionName)
{
    return (QtAndroid::checkPermission(permissionName) == QtAndroid::PermissionResult::Granted) ? true : false;
}

void QAndroidAppPermissions::requestPermissionResults(const QtAndroid::PermissionResultMap &resultMap)
{
    Q_EMIT requestPermissionsResults(convertToVariantList(resultMap));
}

QVariantList QAndroidAppPermissions::convertToVariantList(const QtAndroid::PermissionResultMap &resultMap)
{
    QtAndroid::PermissionResultMap::const_iterator permissionItem = resultMap.constBegin();
    QVariantList permissionsList;

    while(permissionItem != resultMap.constEnd())
    {
        QVariantMap permissionResult;

        permissionResult["name"] = permissionItem.key();
        permissionResult["granted"] = (permissionItem.value() == QtAndroid::PermissionResult::Granted) ? true : false;
        permissionsList << permissionResult;

        ++permissionItem;
    }

    return permissionsList;
}

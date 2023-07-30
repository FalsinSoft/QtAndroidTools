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

QAndroidAppPermissions::QAndroidAppPermissions(QQuickItem *parent) : QQuickItem(parent)
{
}

void QAndroidAppPermissions::requestPermissions(const QStringList &permissionsNameList)
{
    PermissionResultMap resultMap;

    if(QNativeInterface::QAndroidApplication::sdkVersion() >= 23)
    {
        QStringList permissionsNotGrantedList;

        for(int i = 0; i < permissionsNameList.count(); i++)
        {
            if(isPermissionGranted(permissionsNameList[i]) == false)
            {
                resultMap[permissionsNameList[i]] = QtAndroidPrivate::requestPermission(permissionsNameList[i]).result();
            }
            else
            {
                resultMap[permissionsNameList[i]] = QtAndroidPrivate::PermissionResult::Authorized;
            }
        }
    }
    else
    {
        for(int i = 0; i < permissionsNameList.count(); i++)
        {
            resultMap[permissionsNameList[i]] = QtAndroidPrivate::PermissionResult::Authorized;
        }
    }

    if(resultMap.size() > 0)
    {
        Q_EMIT requestPermissionsResults(convertToVariantList(resultMap));
    }
}

void QAndroidAppPermissions::requestPermission(const QString &permissionName)
{
    PermissionResultMap resultMap;

    if(QNativeInterface::QAndroidApplication::sdkVersion() >= 23
    && isPermissionGranted(permissionName) == false)
    {
        resultMap[permissionName] = QtAndroidPrivate::requestPermission(permissionName).result();
    }
    else
    {
        resultMap[permissionName] = QtAndroidPrivate::PermissionResult::Authorized;
    }

    Q_EMIT requestPermissionsResults(convertToVariantList(resultMap));
}

bool QAndroidAppPermissions::shouldShowRequestPermissionInfo(const QString &permissionName)
{
    if(QNativeInterface::QAndroidApplication::sdkVersion() >= 23)
    {
        const QJniObject activity = QNativeInterface::QAndroidApplication::context();

        return activity.callMethod<jboolean>("shouldShowRequestPermissionRationale",
                                             "(Ljava/lang/String;)Z",
                                             QJniObject::fromString(permissionName).object<jstring>()
                                             );
    }

    return false;
}

bool QAndroidAppPermissions::isPermissionGranted(const QString &permissionName)
{
    return (QtAndroidPrivate::checkPermission(permissionName).result() == QtAndroidPrivate::PermissionResult::Authorized) ? true : false;
}

QVariantList QAndroidAppPermissions::convertToVariantList(const PermissionResultMap &resultMap) const
{
    auto permissionItem = resultMap.constBegin();
    QVariantList permissionsList;

    while(permissionItem != resultMap.constEnd())
    {
        QVariantMap permissionResult;

        permissionResult["name"] = permissionItem.key();
        permissionResult["granted"] = (permissionItem.value() == QtAndroidPrivate::PermissionResult::Authorized) ? true : false;
        permissionsList << permissionResult;

        ++permissionItem;
    }

    return permissionsList;
}

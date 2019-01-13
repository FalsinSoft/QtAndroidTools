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

QAndroidAppPermissions::QAndroidAppPermissions()
{
}

QObject* QAndroidAppPermissions::qmlInstance(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine);
    Q_UNUSED(scriptEngine);

    return new QAndroidAppPermissions();
}

void QAndroidAppPermissions::requestPermissions(const QStringList &permissionsNameList)
{
    QtAndroid::PermissionResultMap ResultMap;

    if(QtAndroid::androidSdkVersion() >= 23)
    {
        QStringList PermissionsNotGrantedList;

        for(int i = 0; i < permissionsNameList.count(); i++)
        {
            if(QtAndroid::checkPermission(permissionsNameList[i]) != QtAndroid::PermissionResult::Granted)
            {
                PermissionsNotGrantedList << permissionsNameList[i];
            }
            else
            {
                ResultMap[permissionsNameList[i]] = QtAndroid::PermissionResult::Granted;
            }
        }

        if(PermissionsNotGrantedList.count() > 0)
        {
            QtAndroid::requestPermissions(PermissionsNotGrantedList, std::bind(&QAndroidAppPermissions::RequestPermissionResults, this, std::placeholders::_1));
        }
    }
    else
    {
        for(int i = 0; i < permissionsNameList.count(); i++)
        {
            ResultMap[permissionsNameList[i]] = QtAndroid::PermissionResult::Granted;
        }
    }

    if(ResultMap.size() > 0)
    {
        emit requestPermissionsResults(ConvertToVariantList(ResultMap));
    }
}

void QAndroidAppPermissions::requestPermission(const QString &permissionName)
{
    if(QtAndroid::androidSdkVersion() >= 23 && QtAndroid::checkPermission(permissionName) != QtAndroid::PermissionResult::Granted)
    {
        QtAndroid::requestPermissions(QStringList() << permissionName, std::bind(&QAndroidAppPermissions::RequestPermissionResults, this, std::placeholders::_1));
    }
    else
    {
        QtAndroid::PermissionResultMap ResultMap;
        ResultMap[permissionName] = QtAndroid::PermissionResult::Granted;
        emit requestPermissionsResults(ConvertToVariantList(ResultMap));
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

void QAndroidAppPermissions::RequestPermissionResults(const QtAndroid::PermissionResultMap &ResultMap)
{
    emit requestPermissionsResults(ConvertToVariantList(ResultMap));
}

QVariantList QAndroidAppPermissions::ConvertToVariantList(const QtAndroid::PermissionResultMap &ResultMap)
{
    QtAndroid::PermissionResultMap::const_iterator PermissionItem = ResultMap.constBegin();
    QVariantList PermissionsList;

    while(PermissionItem != ResultMap.constEnd())
    {
        QVariantMap PermissionResult;

        PermissionResult["name"] = PermissionItem.key();
        PermissionResult["granted"] = (PermissionItem.value() == QtAndroid::PermissionResult::Granted) ? true : false;
        PermissionsList << PermissionResult;

        ++PermissionItem;
    }

    return PermissionsList;
}

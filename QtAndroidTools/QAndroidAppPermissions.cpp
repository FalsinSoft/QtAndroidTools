
#include "QtAndroidAppPermissions.h"

QtAndroidAppPermissions::QtAndroidAppPermissions()
{
}

QObject* QtAndroidAppPermissions::qmlInstance(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine);
    Q_UNUSED(scriptEngine);

    return new QtAndroidAppPermissions();
}

void QtAndroidAppPermissions::requestPermissions(const QStringList &permissionsNameList)
{
    for(int i = 0; i < permissionsNameList.count(); i++)
    {
        requestPermission(permissionsNameList[i]);
    }
}

void QtAndroidAppPermissions::requestPermission(const QString &permissionName)
{
    if(QtAndroid::androidSdkVersion() >= 23)
    {
        if(QtAndroid::checkPermission(permissionName) != QtAndroid::PermissionResult::Granted)
        {
            QtAndroid::requestPermissions(QStringList() << permissionName, std::bind(&QtAndroidAppPermissions::RequestPermissionResults, this, std::placeholders::_1));
            return;
        }
    }

    emit requestPermissionsResults(permissionName, true);
}

bool QtAndroidAppPermissions::shouldShowRequestPermissionInfo(const QString &permissionName)
{
    if(QtAndroid::androidSdkVersion() >= 23)
    {
        return QtAndroid::shouldShowRequestPermissionRationale(permissionName);
    }

    return false;
}

void QtAndroidAppPermissions::RequestPermissionResults(const QtAndroid::PermissionResultMap &ResultMap)
{
    if(ResultMap.count() > 0)
    {
        QtAndroid::PermissionResultMap::const_iterator PermissionItem = ResultMap.constBegin();

        while(PermissionItem != ResultMap.constEnd())
        {
            const bool Granted = (PermissionItem.value() == QtAndroid::PermissionResult::Granted) ? true : false;
            emit requestPermissionsResults(PermissionItem.key(), Granted);
            ++PermissionItem;
        }
    }
}

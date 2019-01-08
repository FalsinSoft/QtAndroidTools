#pragma once

#include <QtAndroidExtras>
#include <QQmlEngine>

class QtAndroidAppPermissions : public QObject
{
	Q_OBJECT

    QtAndroidAppPermissions();

public:
    static QObject* qmlInstance(QQmlEngine *engine, QJSEngine *scriptEngine);

    Q_INVOKABLE void requestPermissions(const QStringList &permissionsNameList);
    Q_INVOKABLE void requestPermission(const QString &permissionName);
    Q_INVOKABLE bool shouldShowRequestPermissionInfo(const QString &permissionName);

signals:
    void requestPermissionsResults(const QString &permissionName, bool granted);

private:
    void RequestPermissionResults(const QtAndroid::PermissionResultMap &ResultMap);
};

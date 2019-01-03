
#include <QQmlEngine>
#include "QtAndroidAppPermissions.h"
#include "QtAndroidApkExpansionFiles.h"
#include "QtAndroidTools.h"

QtAndroidTools::QtAndroidTools()
{
}

void QtAndroidTools::InitializeQmlTools()
{
    qmlRegisterSingletonType<QtAndroidAppPermissions>("AndroidTools.AppPermissions", 1, 0, "QtAndroidAppPermissions", &QtAndroidAppPermissions::qmlInstance);
    qmlRegisterSingletonType<QtAndroidApkExpansionFiles>("AndroidTools.ApkExpansionFiles", 1, 0, "QtAndroidApkExpansionFiles", &QtAndroidApkExpansionFiles::qmlInstance);
}

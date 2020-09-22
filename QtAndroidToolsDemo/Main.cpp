#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>
#include <QIcon>
#include <QDir>
#include <QFile>
#include <QStandardPaths>
#include "QtAndroidTools.h"

void prepareSharedFiles(const QString &sharedFolderName)
{
    const QDir sharedFilesFolder(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));

    if(sharedFilesFolder.mkdir(sharedFolderName) == true)
    {
        const QString sharedFilesPath = (sharedFilesFolder.path() + "/" + sharedFolderName);

        QFile::copy(":/images/logo_falsinsoft.jpg", sharedFilesPath + "/logo_falsinsoft.jpg");
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QQuickStyle::setStyle("Material");
    QIcon::setThemeName("tools");
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    QtAndroidTools::initializeQmlTools();

    QObject::connect(&engine, SIGNAL(quit()), &app, SLOT(quit()));
    engine.load(QUrl(QStringLiteral("qrc:/Main.qml")));
    if(engine.rootObjects().isEmpty()) return -1;
    prepareSharedFiles("sharedfiles");

    QtAndroid::hideSplashScreen();

    return app.exec();
}

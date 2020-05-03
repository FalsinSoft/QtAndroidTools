#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>
#include <QIcon>
#include "QtAndroidTools.h"

void PrepareSharedFiles(const QString &SharedFolderName)
{
    const QDir SharedFilesFolder(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));

    if(SharedFilesFolder.mkdir(SharedFolderName) == true)
    {
        const QString SharedFilesPath = (SharedFilesFolder.path() + "/" + SharedFolderName);

        QFile::copy(":/images/logo_falsinsoft.jpg", SharedFilesPath + "/logo_falsinsoft.jpg");
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QQuickStyle::setStyle("Material");
    QIcon::setThemeName("tools");
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    QtAndroidTools::InitializeQmlTools();

    QObject::connect(&engine, SIGNAL(quit()), &app, SLOT(quit()));
    engine.load(QUrl(QStringLiteral("qrc:/Main.qml")));
    if(engine.rootObjects().isEmpty()) return -1;
    PrepareSharedFiles("sharedfiles");

    QtAndroid::hideSplashScreen();

    return app.exec();
}

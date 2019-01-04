#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>
#include <QIcon>
#include "../QtAndroidTools/QtAndroidTools.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QQuickStyle::setStyle("Material");
    QIcon::setThemeName("tools");
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    QtAndroidTools::InitializeQmlTools();

    engine.load(QUrl(QStringLiteral("qrc:/Main.qml")));
    if(engine.rootObjects().isEmpty()) return -1;

    return app.exec();
}

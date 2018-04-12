#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "pyloncamera.h"

#include "controller.h"
#include "logging.h"
#include "state.h"

void registerQMLTypes()
{
  qmlRegisterType<app::State>();
  qmlRegisterType<app::Controller>();
  qmlRegisterType<PylonCamera>();
}

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);

    initLogging();
    registerQMLTypes();
    app::Controller controller;
    controller.start();

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("controller", &controller);

    engine.load(QUrl(QLatin1String("qrc:/qml/main.qml")));

    return app.exec();
}

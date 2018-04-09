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
}

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);

    initLogging();
    registerQMLTypes();
    app::Controller controller;
    controller.start();

    core::PylonCamera camera(&app);

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("controller", &controller);
    engine.rootContext()->setContextProperty("pylonCamera", &camera);

    engine.load(QUrl(QLatin1String("qrc:/qml/main.qml")));

    return app.exec();
}

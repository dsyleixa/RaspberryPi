// main.cpp


#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <gameoflifemodel.h>

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    qmlRegisterType<GameOfLifeModel>("GameOfLifeModel", 1, 0, "GameOfLifeModel");

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}

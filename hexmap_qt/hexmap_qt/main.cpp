#include <QApplication>
#include <QQmlApplicationEngine>

#include <QtQuick/QQuickView>

//#include "hexmap_qt_renderer.h"
#include "hexmap_framebuffer.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    //qmlRegisterType<hexmap_qt_item>("Hexmap", 1, 0, "HexmapView");  // old method using QOpenGLFunctions
    qmlRegisterType<hexmap_item>("Hexmap", 1, 0, "HexmapView"); //new method using FBO

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/hexmap/main.qml")));

//    QQuickView view;
//    view.setResizeMode(QQuickView::SizeRootObjectToView);
//    view.setSource(QUrl(QStringLiteral("qrc:/hexmap/main.qml")));
//    view.show();

    return app.exec();
}

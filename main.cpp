#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QQmlContext>
#include <QSurfaceFormat>
#include <body.h>
#include <paintnode.h>
#include <paintrelation.h>
#include <paintstructural.h>
#include <paintline.h>
#include <painttriangle.h>
#include <expandicon.h>
#include <paintnodearea.h>
#include <math.h>
#include <file.h>
#include <pavementfile.h>



extern QQmlApplicationEngine * enginePtr;
extern QQuickWindow * windowPtr;

QQmlApplicationEngine * enginePtr;
QQuickWindow * windowPtr;

Body * Body::instance = nullptr;
int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    enginePtr = new QQmlApplicationEngine;
    windowPtr = new QQuickWindow;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(enginePtr, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);



    qmlRegisterType<PaintNode>("Paint",1,0,"PaintNode");
    qmlRegisterType<PaintRelation>("Paint",1,0,"PaintRelation");
    qmlRegisterType<PaintStructural>("Paint",1,0,"PaintStructural");
    qmlRegisterType<PaintLine>("Paint",1,0,"PaintLine");
    qmlRegisterType<PaintTriangle>("Paint",1,0,"PaintTriangle");
    qmlRegisterType<ExpandIcon>("Paint",1,0,"ExpandIcon");
    qmlRegisterType<PaintNodeArea>("Paint",1,0,"PaintNodeArea");

    Body * body = Body::getInstance();
    qDebug()<<"initial instance"<<body;





    /*
    qmlRegisterType<paintRe connect() connect()gular>("Paint",1,0,"PaintRegular");
    qmlRegisterType<Gradient>("Paint",1,0,"Gradient");
    qmlRegisterType<Bridge>("Bridge",1,0,"Bridge");
    */

    enginePtr->load(url);
    enginePtr->rootContext()->setContextObject(body);
    windowPtr = qobject_cast<QQuickWindow*>(enginePtr->rootObjects().at(0));
    body->initialize();

    windowPtr->show();
    windowPtr->showFullScreen();
    windowPtr->showNormal();





    return app.exec();
}

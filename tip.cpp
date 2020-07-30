#include "tip.h"
#include <body.h>

tip::tip()
{
}

void tip::setText(QString text)
{
    m_obj->findChild<QObject*>("tip")->setProperty("text",text);
}

void tip::initializeObj(QString text)
{
    Body * body = Body::getInstance();
    QQuickItem * object = nullptr;

    QQmlComponent component(body->engine(),QUrl("qrc:/tips.qml"));
    object = qobject_cast<QQuickItem*>(component.create());
    object->setParentItem(body->getRoot());
    object->setX(10);
    object->setY(body->getRoot()->property("height").toInt() - 50);

    connect(object,SIGNAL(close()),this,SLOT(destroy()));

    m_obj = object;
    setText(text);

}

void tip::destroy()
{
    qDebug()<<"hey";
}

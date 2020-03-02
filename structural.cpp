#include "structural.h"
#include <node.h>

structural::structural(QObject * parent):
    QObject(parent)
{

}

void structural::setChildNode(Node * n)
{
    m_childNode = n;
}

void structural::setParentNode(Node *n)
{
    m_parentNode = n;
}


void structural::initializeObj()
{
    Body * body = Body::getInstance();
    QQuickItem * object;

    QQmlComponent component(body->engine(),QUrl("qrc:/structural.qml"));
    object = qobject_cast<QQuickItem*>(component.create());
    QQuickItem * item = body->getRoot();
    object->setParentItem(item->findChild<QQuickItem*>("layer"));
    object->setParent(body->engine());
    m_obj = object;
}

void structural::update()
{


    if(hovering()){
        Body * b = Body::getInstance();
        setOrigin(b->mousePosition());
        setDestination(childNode()->centerPosition());
        setStructuralCutoff();
    }else{
        setOrigin(parentNode()->centerPosition());
        setDestination(childNode()->centerPosition());
        setStructuralCutoff();
    }
}

void structural::setOrigin(Body::coordinate c)
{
    obj()->setX(c.x);
    obj()->setY(c.y);
    m_origin = c;
}

void structural::setDestination(Body::coordinate c)
{
    m_destination = c;
    c = c.subtract(origin());
    obj()->setProperty("width",c.x);
    obj()->setProperty("height",c.y);


}

void structural::setStructuralCutoff()
{
    Body::coordinate Vector = origin().subtract(destination());
    double radius;
    if(childNode()->width() > childNode()->height()){
        radius = childNode()->width();
    }else{
        radius = childNode()->height();
    }
    radius -= 13;

    double angle = Vector.getAngle();


    double intersectionX = cos(angle) * radius;
    double intersectionY = sin(angle) * radius;

    intersectionX += destination().subtract(origin()).x;
    intersectionY += destination().subtract(origin()).y;

    obj()->findChild<QObject*>("line")->setProperty("p1",QPointF(intersectionX,intersectionY));

}

void structural::setHovering(bool b)
{
    m_hovering = b;
}

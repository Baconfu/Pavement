#include "structural.h"
#include <node.h>

structural::structural(QObject * parent):
    QObject(parent)
{

}

void structural::setChildNode(Node * n)
{
    if(!m_childNode){
        setDisplayChildNode(n);
    }
    m_childNode = n;
    connect(n,SIGNAL(updateStructural()),this,SLOT(update()),Qt::UniqueConnection);
}

void structural::setParentNode(Node *n)
{
    if(!m_parentNode){
        setDisplayParentNode(n);
    }
    m_parentNode = n;
    connect(n,SIGNAL(updateStructural()),this,SLOT(update()),Qt::UniqueConnection);
}

void structural::setDisplayChildNode(Node *n)
{
    m_displayChildNode = n;
    connect(n,SIGNAL(updateStructural()),this,SLOT(update()),Qt::UniqueConnection);
}

void structural::setDisplayParentNode(Node *n)
{
    m_displayParentNode = n;
    connect(n,SIGNAL(updateStructural()),this,SLOT(update()),Qt::UniqueConnection);
}

Node *structural::findDisplayParentNode()
{
    QVector<Node*> path = parentNode()->getIncludes();

    if(path.isEmpty()){
        Node * n = parentNode();
        return n;
    }
    int i=0;
    while(!path[i]->isVisible()){
        i+=1;
    }

    return path[i];

}

Node *structural::findDisplayChildNode()
{
    //invalid concept for function
    QVector<Node*> path = childNode()->getIncludes();
    if(path.isEmpty()){
        Node * n = childNode();
        return n;
    }
    int i=0;
    while(!path[i]->isVisible()){
        i+=1;
    }
    return path[i];
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
        setDestination(displayChildNode()->centerPosition());
        setStructuralCutoff();
    }else{

        if(childNode()->isVisible()){
            setVisibility(true);
            setDisplayChildNode(childNode());
        }else{
            setVisibility(false);

        }
        setDisplayParentNode(findDisplayParentNode());

        setOrigin(displayParentNode()->centerPosition());
        setDestination(displayChildNode()->centerPosition());
        setStructuralCutoff();
    }
}

void structural::setOrigin(Body::coordinate c)
{
    obj()->setProperty("x",c.x);
    obj()->setProperty("y",c.y);
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

void structural::setVisibility(bool b)
{
    obj()->setProperty("visible",b);
    m_visible = b;

}

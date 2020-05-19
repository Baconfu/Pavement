#include "structural.h"
#include <node.h>

structural::structural(QObject * parent):
    QObject(parent)
{

}

void structural::setChildNode(Node * n)
{
    setDisplayChildNode(n);
    setDestination(n->getCenterPosition());
    m_childNode = n;
    connect(n,SIGNAL(updateStructural()),this,SLOT(update()),Qt::UniqueConnection);

}

void structural::setParentNode(Node *n)
{

    setDisplayParentNode(n);
    setOrigin(n->getCenterPosition());
    m_parentNode = n;
    connect(n,SIGNAL(updateStructural()),this,SLOT(update()),Qt::UniqueConnection);
    connect(n,SIGNAL(terminate()),this,SLOT(destroy()));
}

void structural::setDisplayChildNode(Node *n)
{
    m_displayChildNode = n;

    disconnect();
    connect(n,SIGNAL(updateStructural()),this,SLOT(update()),Qt::UniqueConnection);
}

void structural::setDisplayParentNode(Node *n)
{
    m_displayParentNode = n;
    disconnect();
    connect(n,SIGNAL(updateStructural()),this,SLOT(update()),Qt::UniqueConnection);
}

Node *structural::findDisplayParentNode()
{
    /*
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
    */
    return parentNode();

}

Node *structural::findDisplayChildNode()
{
    /*
    //invalid concept for function
    QVector<Node*> path = childNode()->getIncludes();
    if(path.isEmpty()){
        Node * n = childNode();
        return n;
    }
    int i=0;
    while(!path[i]->isVisible()){
        i+=1;qDebug()<<i;
    }
    return path[i];
    */
    return childNode();
}

bool structural::isInside(int x, int y)
{

    int tbx = m_obj->findChild<QObject*>("textBox")->property("x").toInt() + m_obj->property("x").toInt();
    int tby = m_obj->findChild<QObject*>("textBox")->property("y").toInt() + m_obj->property("y").toInt();
    int tbwidth = m_obj->findChild<QObject*>("textBox")->property("width").toInt();
    int tbheight = m_obj->findChild<QObject*>("textBox")->property("height").toInt();



    int give = 5;
    if(tbwidth<4){
        give = 10;
    }



    if(x > tbx - give && x < tbx + tbwidth + give && y > tby - 2 && y < tby + tbheight + 2){

        tally+=1;
        return true;
    }
    return false;
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

    setHighlighted(false);
    if(hovering()){
        Body * b = Body::getInstance();
        setOrigin(b->mousePosition());
        setDestination(displayChildNode()->getCenterPosition());
        setStructuralCutoff();
    }else{

        if(childNode()->isVisible()){
            setVisibility(true);
            setDisplayChildNode(childNode());
        }else{
            setVisibility(false);

        }
        setDisplayParentNode(findDisplayParentNode());

        setOrigin(displayParentNode()->getCenterPosition());
        setDestination(displayChildNode()->getCenterPosition());
        setStructuralCutoff();
    }
}

void structural::destroy()
{
    m_parentNode->removeChild(m_childNode);
    m_childNode->removeParent(m_parentNode);
    Body * b = Body::getInstance();
    m_obj->deleteLater();
    m_obj = nullptr;
    b->removeStructural(this);
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
    if(hovering()){
        return;
    }
    Body::coordinate localVector = destination().subtract(origin());
    Body::coordinate inverseVector = origin().subtract(destination());

    double angle = inverseVector.getAngle();


    if(displayChildNode()){
        Node * destinationNode = displayChildNode();
        Node * originNode = displayParentNode();
        double pi = 3.1415926;
        double a1 = atan(double(destinationNode->height()) / double(destinationNode->width()));

        int targetEdge_gradient = 0;
        double targetEdge_b = 0;

        if(angle>=a1 && angle < pi - a1){
            targetEdge_gradient = 0;
            targetEdge_b = destinationNode->getY() + destinationNode->height() - originNode->getCenterPosition().y;

        }
        if(angle >= pi - a1 && angle < pi + a1){
            targetEdge_gradient = 1;
            targetEdge_b = destinationNode->getX() - originNode->getCenterPosition().x - 5;

        }
        if(angle >= pi + a1 && angle < 2 * pi - a1){
            targetEdge_gradient = 0;
            targetEdge_b = destinationNode->getY() - originNode->getCenterPosition().y;

        }
        if(angle >= 2 * pi - a1 || angle < a1){
            targetEdge_gradient = 1;
            targetEdge_b = destinationNode->getX() + destinationNode->width() - originNode->getCenterPosition().x + 5;

        }

        int intersectionY = 0;
        int intersectionX = 0;
        if(targetEdge_gradient){
            intersectionX = int(targetEdge_b);
            intersectionY = int((double(localVector.y) / double(localVector.x)) * targetEdge_b);

        }else{
            intersectionX = int(targetEdge_b / (double(localVector.y) / double(localVector.x)));
            intersectionY = int(targetEdge_b);
        }

        m_obj->findChild<QObject*>("textInput")->setProperty("xMod",intersectionX);
        m_obj->findChild<QObject*>("textInput")->setProperty("yMod",intersectionY);

        obj()->findChild<QObject*>("line")->setProperty("p1",QPointF(intersectionX,intersectionY));
    }

}

void structural::setHovering(bool b)
{
    m_hovering = b;
}

void structural::setHighlighted(bool b)
{
    if(b != m_highlighted){
        m_highlighted = b;

        if(b){

            obj()->setProperty("focus",true);
            obj()->setProperty("highlighted",true);
        }else{
            obj()->setProperty("focus",true);
            obj()->setProperty("highlighted",false);
        }
    }
}

void structural::setSelected(bool b)
{
    if(b != m_selected){
        m_selected = b;
        if(b){

            m_obj->findChild<QObject*>("textInput")->setProperty("focus",true);
        }
        else{
            Body * b = Body::getInstance();
            b->setFocusWindow();
        }
    }
}

void structural::setVisibility(bool b)
{
    obj()->setProperty("visible",b);
    m_visible = b;

}

#include "relation.h"
#include "node.h"
#include <ghostnode.h>

Relation::Relation(QObject * parent,QString type):
    QObject (parent)
{
    m_type = type;
}

QString Relation::getName()
{
    return m_obj->findChild<QObject*>("textInput")->property("text").toString();
}

void Relation::setName(QString s)
{
    m_obj->findChild<QObject*>("textInput")->setProperty("text",s);
}

bool Relation::isInside(int x, int y)
{
    int tbx = m_obj->findChild<QObject*>("textBox")->property("x").toInt() + origin().x ;
    int tby = m_obj->findChild<QObject*>("textBox")->property("y").toInt() + origin().y;
    int tbwidth = m_obj->findChild<QObject*>("textBox")->property("width").toInt();
    int tbheight = m_obj->findChild<QObject*>("textBox")->property("height").toInt();

    int give = 5;
    if(tbwidth<4){
        give = 10;
    }
    if(x > tbx - give && x < tbx + tbwidth + give && y > tby - 2 && y < tby + tbheight + 2){
        return true;
    }
    return false;
}

Body::coordinate Relation::calculateRelationCutoff(BaseNode * originNode, BaseNode * destinationNode)
{

    if(hovering()){
        Body::coordinate c;
        c.x=0;
        c.y=0;
        return c;
    }

    Body::coordinate destination = destinationNode->getCenterAbsolutePosition();
    Body::coordinate origin = originNode->getCenterAbsolutePosition();

    Body::coordinate localVector = destination.subtract(origin);
    Body::coordinate inverseVector = origin.subtract(destination);

    double angle = inverseVector.getAngle();



    if(true){


        double pi = 3.1415926;
        double a1 = atan(double(destinationNode->height()) / double(destinationNode->width()));

        int targetEdge_gradient = 0;
        double targetEdge_b = 0;

        int destinationY = destinationNode->getAbsolutePosition().y;
        int destinationX = destinationNode->getAbsolutePosition().x;


        if(angle>=a1 && angle < pi - a1){
            targetEdge_gradient = 0;
            targetEdge_b = destinationY + destinationNode->height() - originNode->getCenterAbsolutePosition().y;

        }
        if(angle >= pi - a1 && angle < pi + a1){
            targetEdge_gradient = 1;
            targetEdge_b = destinationX - originNode->getCenterAbsolutePosition().x - 5;

        }
        if(angle >= pi + a1 && angle < 2 * pi - a1){
            targetEdge_gradient = 0;
            targetEdge_b = destinationY - originNode->getCenterAbsolutePosition().y;

        }
        if(angle >= 2 * pi - a1 || angle < a1){
            targetEdge_gradient = 1;
            targetEdge_b = destinationX + destinationNode->width() - originNode->getCenterAbsolutePosition().x + 5;

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



        Body::coordinate c;
        c.x = intersectionX;
        c.y = intersectionY;

        return c;

    }

}

void Relation::setDestinationCutoff(Body::coordinate c)
{

    obj()->findChild<QObject*>("textBox")->setProperty("xModd",c.x);
    obj()->findChild<QObject*>("textBox")->setProperty("yModd",c.y);

    obj()->findChild<QObject*>("line")->setProperty("p1",QPointF(c.x,c.y));
}

void Relation::setOriginCutoff(Body::coordinate c)
{
    obj()->findChild<QObject*>("textBox")->setProperty("xModo",c.x);
    obj()->findChild<QObject*>("textBox")->setProperty("yModo",c.y);

    obj()->findChild<QObject*>("line")->setProperty("p0",QPointF(c.x,c.y));
}

void Relation::setOriginObject(BaseNode *n){
    m_origin_node = n;
    Body::coordinate c;
    if(n->getAbstraction()){
        c = n->getCenterPosition().add(n->getAbstraction()->getAbsolutePosition());
    }else{
        c = n->getCenterPosition();
    }


    determineTypes();

    setOrigin(c);
}

void Relation::setOriginObject(Relation * r){
    m_origin_relation = r;
    determineTypes();
    Body::coordinate c = r->worldMidPoint();
    setOrigin(c);
}

void Relation::setDestinationObject(BaseNode *n){
    m_destination_node = n;
    Body::coordinate c;
    if(n->getAbstraction()){
        Body::coordinate c2;
        c2.x = n->width();
        c2.y = n->height();

        c = n->getAbsolutePosition().add(c2);
    }else{
        c = n->getCenterPosition();
    }


    determineTypes();
    if(hovering()){
        setHovering(false);
    }
    setDestination(c);

    updateSelf();
}
void Relation::setDestinationObject(Relation *r){
    m_destination_relation = r;
    Body::coordinate c = r->worldMidPoint();
    if(hovering()){
        setHovering(false);
    }
    determineTypes();
    setDestination(c);
    updateSelf();
}
void Relation::clearDestinationObject()
{
    m_destination_node = nullptr;
    m_destination_relation = nullptr;
}
void Relation::setHovering(bool b)
{
    m_hovering = b;
    if(b){
        clearDestinationObject();
    }
    \
}

void Relation::determineOriginType()
{
    if(m_origin_node){
        originType = node;
        return;
    }
    if(m_origin_relation){
        originType = relation;
        return;
    }

}

void Relation::determineDestinationType()
{
    if(m_destination_node){
        destinationType = node;
        return;
    }
    if(m_destination_relation){
        destinationType = relation;
        return;
    }

}

void Relation::determineTypes()
{
    determineOriginType();
    determineDestinationType();
}
void Relation::setOrigin(Body::coordinate c){
    m_origin = c;
    m_width = m_destination.x - m_origin.x;
    m_height = m_destination.y - m_origin.y;
    m_obj->setProperty("x",m_origin.x);
    m_obj->setProperty("y",m_origin.y);
    updateRelations();
}
void Relation::setOrigin(int x,int y){
    Body::coordinate c;
    c.x = x;
    c.y = y;
    setOrigin(c);
}
void Relation::setDestination(Body::coordinate c){
    m_destination = c;
    m_width = m_destination.x - m_origin.x;
    m_height = m_destination.y - m_origin.y;
    m_obj->setProperty("width",m_width);
    m_obj->setProperty("height",m_height);


    updateRelations();
}
void Relation::setDestination(int x, int y){
    Body::coordinate c;
    c.x = x;
    c.y = y;
    setDestination(c);
}

void Relation::syncCutoff()
{
    setOriginCutoff(calculateRelationCutoff(m_destination_node,m_origin_node).add(destination().subtract(origin())));
    setDestinationCutoff(calculateRelationCutoff(m_origin_node,m_destination_node));
}

Body::coordinate Relation::worldMidPoint(){
    Body::coordinate p;
    p.x = origin().x + width() / 2;
    p.y = origin().y + height() / 2;
    return p;
}
Body::coordinate Relation::localMidPoint(){
    Body::coordinate p;
    p.x = width()/2;
    p.y = height()/2;
    return p;
}

void Relation::updateSelf()
{



    if(originType == node){
        if(!originNode()){
            destroy();
        }
        if(m_origin_node->isVisible()){
            setVisibility(true);

            setOrigin(m_origin_node->getCenterAbsolutePosition());
        }else{
            setVisibility(false);
            return;
        }
    }

    if(originType == relation){
        setOrigin(m_origin_relation->worldMidPoint());

    }
    if(hovering()){
        Body * b = Body::getInstance();
        setDestination(b->mousePosition());

        return;
    }


    if(destinationType == node){
        if(!destinationNode()){
            destroy();

        }
        if(m_destination_node->isVisible()){
            setDestination(m_destination_node->getCenterAbsolutePosition());
            syncCutoff();

            setVisibility(true);
        }else{
            setVisibility(false);
            return;
        }


    }

    if(destinationType == relation){
        setDestination(m_destination_relation->worldMidPoint());
    }

}
void Relation::setVisibility(bool visibility)
{
    m_obj->setProperty("visible",visibility);
    m_visible = visibility;
}
void Relation::updateRelations(){

}
/*
void Relation::registerRelation(Relation *r)
{
    if(r->destinationObjectType() == "node"){
        qDebug()<<"error: invalid relation";
    }
    if(r->destinationObjectType() == "relation"){
        toRelation.append(r);
        toRelation_relation.append(r->destinationRelation());
    }
}

void Relation::registerIncomingRelation(Relation *r)
{
    if(r->originObjectType() == "node"){
        fromNode.append(r);
        fromNode_node.append(r->originNode());
    }
    if(r->originObjectType() == "relation"){
        fromRelation.append(r);
        fromRelation_relation.append(r->originRelation());
    }
}
*/
void Relation::setHighlighted(bool b)
{

    if(m_highlighted == b){
        return;
    }
    m_highlighted = b;
    if(b){
        obj()->setProperty("highlighted",true);
    }else{
        obj()->setProperty("highlighted",false);
    }

}

void Relation::setSelected(bool b)
{
    if(m_selected!=b){
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

void Relation::finalizeSelf()
{
    if(originNode()){
        //originNode()->registerRelation(this);


        connect(originNode(),SIGNAL(updateRelation()),this,SLOT(updateSelf()),Qt::UniqueConnection);
        connect(originNode(),SIGNAL(terminate()),this,SLOT(destroy()));


    }
    if(originRelation()){
        //originRelation()->registerRelation(this);
    }
    if(destinationNode()){
        //destinationNode()->registerIncomingRelation(this);
        connect(destinationNode(),SIGNAL(updateRelation()),this,SLOT(updateSelf()),Qt::UniqueConnection);
        connect(destinationNode(),SIGNAL(terminate()),this,SLOT(destroy()));
    }
    if(destinationRelation()){
        //destinationRelation()->registerIncomingRelation(this);
    }
    if(originNode() && destinationNode()){
        if(originNode()->getAbstraction() == destinationNode()->getAbstraction()){
            BaseNode * b = originNode()->getAbstraction();
            if(typeid (*b) == typeid (Node)){
                b->getNodePointer()->registerUnderRelation(this);
                setAbstraction(b);
            }
        }
    }
}

void Relation::initializeObj()
{
    Body * body = Body::getInstance();
    QQuickItem * object = nullptr;

    if(m_type == "arrow"){
        QQmlComponent component(body->engine(),QUrl("qrc:/relation.qml"));
        object = qobject_cast<QQuickItem*>(component.create());
    }
    if(m_type == "line"){
        QQmlComponent component(body->engine(),QUrl("qrc:/line.qml"));
        object = qobject_cast<QQuickItem*>(component.create());
    }
    if(m_type == "triangle"){
        QQmlComponent component(body->engine(),QUrl("qrc:/triangle.qml"));
        object = qobject_cast<QQuickItem*>(component.create());
    }

    if(!object){
        qDebug()<<"error: no relation type";
        return;
    }
    QQuickItem * item = body->getRoot();
    object->setParentItem(item->findChild<QQuickItem*>("layer"));
    object->setParent(body->engine());

    m_obj = object;
}





void Relation::destroy(){
    m_obj->deleteLater();
    delete(m_obj);
    m_obj = nullptr;
    Body * b = Body::getInstance();
    b->removeRelation(this);
}


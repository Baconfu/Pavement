#include "relation.h"
#include "node.h"
#include <ghostnode.h>

Relation::Relation(QObject * parent):
    QObject (parent)
{

}

bool Relation::isInside(int x, int y)
{
    if(x > worldMidPoint().x - 20 && x < worldMidPoint().x + 20 && y > worldMidPoint().y - 20 && y < worldMidPoint().y + 20){
        return true;

    }
    return false;
}

int Relation::setRelationCutoff()
{
    if(hovering()){
        return 0;
    }
    Body::coordinate localVector = destination().subtract(origin());
    Body::coordinate inverseVector = origin().subtract(destination());

    double angle = inverseVector.getAngle();


    if(true){

        Node * destinationNode = m_destination_node;
        Node * originNode = m_origin_node;
        double pi = 3.1415926;
        double a1 = atan(double(destinationNode->height()) / double(destinationNode->width()));

        int targetEdge_gradient = 0;
        double targetEdge_b = 0;

        if(angle>=a1 && angle < pi - a1){
            targetEdge_gradient = 0;
            targetEdge_b = destinationNode->absY() + destinationNode->height() - originNode->centerPosition().y;

        }
        if(angle >= pi - a1 && angle < pi + a1){
            targetEdge_gradient = 1;
            targetEdge_b = destinationNode->absX() - originNode->centerPosition().x - 5;

        }
        if(angle >= pi + a1 && angle < 2 * pi - a1){
            targetEdge_gradient = 0;
            targetEdge_b = destinationNode->absY() - originNode->centerPosition().y;

        }
        if(angle >= 2 * pi - a1 || angle < a1){
            targetEdge_gradient = 1;
            targetEdge_b = destinationNode->absX() + destinationNode->width() - originNode->centerPosition().x + 5;

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


        obj()->findChild<QObject*>("line")->setProperty("p1",QPointF(intersectionX,intersectionY));

    }
    return 0;
}

void Relation::setOriginObject(Node *n){
    m_origin_node = n;
    Body::coordinate c = n->centerPosition();
    setOrigin(c);
}

void Relation::setOriginObject(Relation * r){
    m_origin_relation = r;
    Body::coordinate c = r->worldMidPoint();
    setOrigin(c);
}

void Relation::setDestinationObject(Node *n){
    m_destination_node = n;

    Body::coordinate c = n->centerPosition();
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
    if(m_origin_ghost){
        originType = ghost;
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
    if(m_destination_ghost){
        destinationType = ghost;
        return;
    }
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
    if(hovering()){
        Body * b = Body::getInstance();
        setDestination(b->mousePosition());

    }

    if(originType == node){
        setOrigin(m_origin_node->centerPosition());

    }
    if(originType == ghost){
        setOrigin(m_origin_ghost->getCenterPosition());
    }
    if(originType == relation){
        setOrigin(m_origin_relation->worldMidPoint());
    }


    if(destinationType == node){
        qDebug()<<m_destination_node;
        setDestination(m_destination_node->centerPosition());
        setRelationCutoff();
    }
    if(destinationType == ghost){
        setDestination(m_destination_ghost->getCenterPosition());
        setRelationCutoff();
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

void Relation::finalizeSelf()
{
    if(originNode()){
        originNode()->registerRelation(this);
        connect(originNode(),SIGNAL(updateRelation()),this,SLOT(updateSelf()));
    }
    if(originRelation()){
        //originRelation()->registerRelation(this);
    }
    if(destinationNode()){
        destinationNode()->registerIncomingRelation(this);
        connect(destinationNode(),SIGNAL(updateRelation()),this,SLOT(updateSelf()));
    }
    if(destinationRelation()){
        //destinationRelation()->registerIncomingRelation(this);
    }
}

void Relation::initializeObj()
{
    Body * body = Body::getInstance();
    QQuickItem * object;

    QQmlComponent component(body->engine(),QUrl("qrc:/relation.qml"));
    object = qobject_cast<QQuickItem*>(component.create());
    QQuickItem * item = body->getRoot();
    object->setParentItem(item->findChild<QQuickItem*>("layer"));
    object->setParent(body->engine());

    m_obj = object;
}

void Relation::createObj()
{

}



void Relation::deleteObj(){
    m_obj->deleteLater();
    m_obj = nullptr;
}


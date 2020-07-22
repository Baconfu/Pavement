#include "basenode.h"

BaseNode::BaseNode(QObject * parent, int id):
    QObject(parent)
{
    m_id = id;
}

Body::coordinate BaseNode::getPosition()
{
    Body::coordinate c;
    c.x=0;
    c.y=0;
    return c;
}

Body::coordinate BaseNode::getCenterPosition()
{
    Body::coordinate c;
    c.x=0;
    c.y=0;
    return c;
}

Body::coordinate BaseNode::getCenterAbsolutePosition()
{
    Body::coordinate c;
    c.x=0;
    c.y=0;
    return c;
}

Body::coordinate BaseNode::getLocalCenterPosition()
{
    Body::coordinate c;
    c.x=0;
    c.y=0;
    return c;
}

int BaseNode::getX()
{
    return 0;
}

int BaseNode::getY()
{
    return 0;
}

int BaseNode::width()
{
    return 0;
}

int BaseNode::height()
{
    return 0;
}

void BaseNode::registerRelation(Relation *r)
{
    qDebug()<<"BaseNode virtual function called. "<<r;
}

void BaseNode::registerIncomingRelation(Relation *r)
{
    qDebug()<<"BaseNode virtual function called. "<<r;
}

void BaseNode::dissolve()
{

}

void BaseNode::setID(int id)
{
    qDebug()<<"BaseNode virtual function called. "<<id;
}

int BaseNode::getID()
{
    return 0;
}

BaseNode *BaseNode::getAbstraction()
{
    return nullptr;
}

void BaseNode::setVisibility(bool visibility)
{
    qDebug()<<"BaseNode virtual function called. "<<visibility;
}





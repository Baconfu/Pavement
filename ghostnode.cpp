#include "ghostnode.h"



GhostNode::GhostNode(Node * original,QObject * parent):
    QObject(parent)
{
    m_original = original;
}

void GhostNode::setX(int x)
{
    m_x = x;
    m_obj->setProperty("x",x);
}

void GhostNode::setY(int y)
{
    m_y = y;
    m_obj->setProperty("y",y);
}


Body::coordinate GhostNode::getCenterPosition()
{
    Body::coordinate c;
    c.x = m_x + m_width;
    c.y = m_y + m_height;
    return c;
}







void GhostNode::initializeObj()
{
    Body * body = Body::getInstance();
    QQuickItem * object;

    QQmlComponent component(body->engine(),QUrl("qrc:/node.qml"));
    object = qobject_cast<QQuickItem*>(component.create());
    QQuickItem * item = body->getRoot();
    object->setParentItem(item->findChild<QQuickItem*>("layer"));
    object->setParent(body->engine());
    connect(object,SIGNAL(widthChanged()),this,SLOT(getWidth()));
    connect(object,SIGNAL(heightChanged()),this,SLOT(getHeight()));
    //QObject * textObj = object->findChild<QObject*>("textInput");
    //connect(textObj,SIGNAL(accepted()),this,SLOT(inputAccepted()));
    //connect(object,SIGNAL(typeAccepted(QString)),this,SLOT(typeInputAccepted(QString)));

    //connect(object,SIGNAL(update()),this,SLOT(updateRelations()));

    m_obj = object;
    getWidth();
    getHeight();
}

void GhostNode::adoptOriginal()
{
    m_obj->findChild<QObject*>("textInput")->setProperty("text",m_original->name());
    m_obj->findChild<QObject*>("typeName")->setProperty("text",m_original->typeName());
    m_obj->setProperty("ghost",true);
}


bool GhostNode::isInside(int x, int y)
{
    if(x>m_x && x<m_x + m_width && y > m_y && y < m_y + m_height){
        return true;
    }else{
        return false;
    }
}

void GhostNode::highlight(bool b)
{
    m_obj->setProperty("highlighted",b);
}

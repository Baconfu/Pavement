#include "ghostnode.h"



GhostNode::GhostNode(Node * original,QObject * parent)
{
    m_original = original;
}

void GhostNode::setPosition(Body::coordinate c)
{
    m_position = c;
    m_obj->setProperty("x",c.x);
    m_obj->setProperty("y",c.y);
}

Body::coordinate GhostNode::getPosition()
{
    Body::coordinate c;
    c.x = m_obj->property("x").toInt();
    c.y = m_obj->property("y").toInt();
    m_position = c;
    return m_position;
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
    m_obj->findChild<QObject*>("textInput")->setProperty("text",m_original->getName());
    m_obj->findChild<QObject*>("typeName")->setProperty("text",m_original->typeName());
    m_obj->setProperty("ghost",true);

}


bool GhostNode::isInside(int x, int y)
{
    if(x>m_position.x && x<m_position.x + m_width && y > m_position.y && y < m_position.y + m_height){
        return true;
    }else{
        return false;
    }
}

void GhostNode::setSelected(bool b)
{
    if(b){
        m_batchSelected = true;
        highlight(true);
        preventFocus(true);
    }else{
        m_batchSelected = false;
        if(!hoverSelected()){
            highlight(false);
        }
    }
}

void GhostNode::setHover(bool b)
{

    if(b){
        m_hoverSelected = true;
        highlight(true);
    }else{
        m_hoverSelected = false;
        if(!selected()){
            highlight(false);
        }
    }
}

void GhostNode::highlight(bool b)
{
    m_obj->setProperty("highlighted",b);
}

void GhostNode::setVisibility(bool b)
{
    if(b!=m_visible){
        m_visible = b;
        m_obj->setProperty("visible",b);

        updateRelation();
    }
}

void GhostNode::setName(QString s)
{
    m_obj->findChild<QObject*>("textInput")->setProperty("text",s);
    m_name = s;
}

Body::coordinate GhostNode::getCenterPosition()
{
    Body::coordinate c;

    getPosition();
    getWidth();
    getHeight();
    c.x = m_position.x + m_width/2;
    c.y = m_position.y + m_height/2;
    m_centerPosition = c;
    return c;
}

void GhostNode::calculateLocalVector()
{
    Body::coordinate self = getPosition();
    Body::coordinate origin = m_abstraction->getPosition();
    m_localVector = self.subtract(origin);
}

Body::coordinate GhostNode::getLocalVector()
{
    return m_localVector;
}

void GhostNode::setPositionByLocalVector()
{

}

void GhostNode::imposeLocalVector(Body::coordinate c)
{
    m_localVector = c;
    Body::coordinate origin = m_abstraction->getPosition();
    setPosition(origin.add(m_localVector));
}

void GhostNode::abstract()
{

}

void GhostNode::expand()
{
    QVector<BaseNode*> subMap = m_original->getUnderMap();

    for(int i=0; i<subMap.length(); i++){
        BaseNode * b = subMap[i];
        if(typeid (*b) == typeid (GhostNode)){
            GhostNode * g = b->getGhostPointer();
            GhostNode * clone = g->getOriginal()->newGhostNode();
            clone->setAbstraction(this);
            m_underMap.append(clone);

            clone->imposeLocalVector(g->getLocalVector());
        }
    }
}

void GhostNode::setAbstraction(BaseNode *n)
{
    m_abstraction = n;
}

#include "nodearea.h"
#include <ghostnode.h>

NodeArea::NodeArea(QObject * parent)
{

}

Body::coordinate NodeArea::getPosition()
{
    m_position.x = m_obj->property("x").toInt();
    m_position.y = m_obj->property("y").toInt();
    return m_position;
}

Body::coordinate NodeArea::getCenterPosition()
{
    getPosition();
    Body::coordinate c;
    c.x = m_position.x + m_width/2;
    c.y = m_position.y + m_height/2;
    return c;
}

void NodeArea::updateAbsolutePosition()
{
    if(m_abstraction){

        m_absolutePosition = m_abstraction->getAbsolutePosition().add(getPosition());
    }
    else{
        m_absolutePosition = m_position;
    }

}

BaseNode *NodeArea::isInside(Body::coordinate c)
{
    int x = c.x;
    int y = c.y;
    int width = m_width;
    int height = m_height;
    Body::coordinate pos = m_position;

    BaseNode * moving = nullptr;
    for(int i=0; i<m_underMap.length(); i++){
        BaseNode * b = m_underMap[i]->isInside(c.subtract(m_position));
        if(b){
            hover(false,Body::coordinate());
            if(b->isMoving()){
                moving  = b;
            }else{
                return b;
            }
        }
    }
    if(moving){
        return moving;
    }



    int padding = 15;
    bool horizontalbound = ((x>pos.x-padding && x<pos.x) || (x>pos.x+width && x<pos.x + width + padding)) &&
            (y>pos.y-padding && y<pos.y+height+padding);
    bool verticalbound = ((y>pos.y - padding && y<pos.y) || (y>pos.y+height && y<pos.y + height + padding)) &&
            (x > pos.x-padding && x<pos.x + width + padding);
    if(horizontalbound || verticalbound){
        hover(true,c);
        return this;
    }else{
        removeUnderMapFocus();
        return nullptr;
    }
}

void NodeArea::transform(Body::coordinate c)
{
    setPosition(getPosition().add(c));
}

void NodeArea::transformSubMap(Body::coordinate vector)
{
    for(int i=0; i<m_underMap.length(); i++){

        m_underMap[i]->transform(vector);
    }
}

void NodeArea::transformIgnoreSubMap(Body::coordinate c)
{
    transform(c);
    transformSubMap(c.invert());
}

void NodeArea::setPosition(Body::coordinate c)
{
    m_position = c;
    m_obj->setProperty("x",c.x);
    m_obj->setProperty("y",c.y);
    updateAbsolutePosition();
    updateRelation();
}

void NodeArea::setPositionByCenter(Body::coordinate c)
{
    Body::coordinate m;

    m.x = c.x - m_width/2;
    m.y = c.y - m_height/2;
    setPosition(m);
}

void NodeArea::setUnderMap(QVector<BaseNode *> nodes)
{
    m_underMap = nodes;
    for(int i=0; i<nodes.length(); i++){
        BaseNode * b = nodes[i];
        Body::coordinate c = b->getPosition().subtract(this->getPosition());
        b->obj()->setParentItem(this->obj());
        b->setPosition(c);
        b->setAbstraction(this);

    }
    if(!nodes.isEmpty()){
        reFormatExpandedForm();
        updateAbsolutePosition();
    }
}

void NodeArea::underMapAppendNode(BaseNode *b)
{
    m_underMap.append(b);
    Body::coordinate c = b->getAbsolutePosition().subtract(this->getAbsolutePosition());
    b->obj()->setParentItem(this->obj());
    b->setPosition(c);
    b->setAbstraction(this);
    if(!m_underMap.isEmpty()){
        reFormatExpandedForm();
        updateAbsolutePosition();
    }
}

bool NodeArea::underMapContains(BaseNode *b)
{
    for(int i=0; i<m_underMap.length(); i++){
        if(m_underMap[i] == b){
            return true;
        }
        if(m_underMap[i]->underMapContains(b)){
            return true;
        }
    }
    return false;
}

void NodeArea::cloneSubMap(BaseNode *b)
{
    QVector<BaseNode*> subMap = b->getUnderMap();
    if(!subMap.isEmpty()){
        QVector<BaseNode*> mySubMap;
        for(int i=0; i<subMap.length(); i++){
            BaseNode * b = subMap[i];
            if(typeid (*b) == typeid (GhostNode)){

                GhostNode * g = b->getGhostPointer();
                GhostNode * clone = g->getOriginal()->newGhostNode();
                Body::coordinate geometry;
                geometry.x = g->getAbstraction()->width()/2;
                geometry.y = g->getAbstraction()->height()/2;
                Body::coordinate vector = g->getCenterPosition();
                clone->setPositionByCenter(getAbsolutePosition().add(vector));

                mySubMap.append(clone);

            }
        }
        setUnderMap(mySubMap);
    }
}

void NodeArea::removeUnderMapFocus()
{
    hover(false,Body::coordinate());
    for(int i=0; i<m_underMap.length(); i++){
        m_underMap[i]->removeUnderMapFocus();
    }
}

void NodeArea::subNodeMoved()
{
    reFormatExpandedForm();
    if(m_abstraction){
        m_abstraction->subNodeMoved();
    }
}

void NodeArea::reFormatExpandedForm()
{
    if(m_underMap.isEmpty()){
        qDebug()<<"Error: node area is empty";
    }
    if(!m_underMap.isEmpty()){
        int leftMost = 10000000;
        int rightMost = -1000000;
        int topMost = 10000000;
        int botMost = -1000000;

        for(int i=0; i<m_underMap.length(); i++){
            BaseNode * b = m_underMap[i];

            int padding = 5;
            int x = b->getPosition().x;
            if(x < leftMost){

                leftMost = x - padding;
            }
            x = b->getPosition().x + b->width();
            if(x > rightMost){

                rightMost = x + padding;
            }
            int y = b->getPosition().y;
            if(y < topMost){

                topMost = y - padding;
            }
            y = b->getPosition().y + b->height();
            if(y > botMost){

                botMost = y + padding;
            }

        }
        Body::coordinate median;
        median.x = (leftMost + rightMost) / 2;
        median.y = (topMost + botMost) / 2;

        m_obj->setProperty("width",rightMost - leftMost);
        m_obj->setProperty("height",botMost - topMost);
        Body::coordinate geometry;

        geometry.x = width()/2;
        geometry.y = height()/2;


        transformIgnoreSubMap(median.subtract(geometry));

        if(m_abstraction){
            m_abstraction->reFormatExpandedForm();
        }
    }
    updateRelation();

}

void NodeArea::extract()
{
    Body * b = Body::getInstance();
    Body::coordinate c = getAbsolutePosition();
    m_obj->setParentItem(b->getRoot()->findChild<QQuickItem*>("layer"));
    setPosition(c);

}

void NodeArea::exude(BaseNode * b)
{
    if(m_underMap.contains(b)){
        b->extract();
        m_underMap.removeOne(b);
    }
    reFormatExpandedForm();
}

void NodeArea::hover(bool b,Body::coordinate c)
{
    if(m_batchSelected){
        return;
    }else{
        highlight(b);
    }

}

void NodeArea::select(bool b,Body::coordinate c)
{
    m_batchSelected = b;
    highlight(b);

}

void NodeArea::highlight(bool b)
{
    m_obj->setProperty("highlighted",b);
}

void NodeArea::moving(bool b)
{
    if(m_moving!=b){
        m_moving = b;
        if(b){
            m_obj->setProperty("z",1);
            m_positionBeforeDragged = getPosition();
        }else{
            m_obj->setProperty("z",0);
        }
    }
}

void NodeArea::geometryChanged()
{
    updateRelation();

    for(int i=0; i<m_underMap.length(); i++){
        m_underMap[i]->geometryChanged();
    }
}

void NodeArea::initializeObj()
{
    Body * body = Body::getInstance();

    QQuickItem * object;

    QQmlComponent component(body->engine(),QUrl("qrc:/nodeArea.qml"));
    object = qobject_cast<QQuickItem*>(component.create());
    QQuickItem * item = body->getRoot();
    object->setParentItem(item->findChild<QQuickItem*>("layer"));
    object->setParent(body->engine());
    connect(object,SIGNAL(widthChanged()),this,SLOT(widthChanged()));
    connect(object,SIGNAL(heightChanged()),this,SLOT(heightChanged()));
    connect(object,SIGNAL(update()),this,SLOT(geometryChanged()));
    setID(body->allocateNewID("node"));

    m_obj = object;
    widthChanged();
    heightChanged();
}


void NodeArea::widthChanged()
{
    m_width = m_obj->property("width").toInt();

}

void NodeArea::heightChanged()
{
    m_height = m_obj->property("height").toInt();
}


#include "nodearea.h"

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

BaseNode *NodeArea::isInside(int x, int y)
{

    Body::coordinate position = getPosition();
    for(int i=0; i<m_underMap.length(); i++){
        BaseNode * b = m_underMap[i]->isInside(x-position.x,y-position.y);
        if(b){

            hover(false);
            return b;
        }
    }
    int width = m_width;
    int height = m_height;

    int s = 15;
    bool horizontalbound = ((x>position.x-s && x<position.x) || (x>position.x+width && x<position.x + width + s)) &&
            (y>position.y-s && y<position.y+height+s);
    bool verticalbound = ((y>position.y - s && y<position.y) || (y>position.y+height && y<position.y + height + s)) &&
            (x > position.x-s && x<position.x + width + s);
    if(horizontalbound || verticalbound){
        hover(true);

        return this;
    }else{

        hover(false);
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
    Body::coordinate c = b->getPosition().subtract(this->getPosition());
    b->obj()->setParentItem(this->obj());
    b->setPosition(c);
    b->setAbstraction(this);
    if(!m_underMap.isEmpty()){
        reFormatExpandedForm();
        updateAbsolutePosition();
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


            int x = b->getPosition().x;
            if(x < leftMost){

                leftMost = x - 3;
            }
            x = b->getPosition().x + b->width();
            if(x > rightMost){

                rightMost = x + 3;
            }
            int y = b->getPosition().y;
            if(y < topMost){

                topMost = y - 3;
            }
            y = b->getPosition().y + b->height();
            if(y > botMost){

                botMost = y + 3;
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

void NodeArea::hover(bool b)
{
    if(b){
        highlight(b);
    }else{
        if(!m_batchSelected){
            highlight(b);
        }
    }
}

void NodeArea::select(bool b)
{
    m_batchSelected = b;
    highlight(b);

}

void NodeArea::highlight(bool b)
{
    m_obj->setProperty("highlighted",b);
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


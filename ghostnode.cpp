#include "ghostnode.h"
#include <iostream>

using namespace std;

GhostNode::GhostNode(Node * original,QObject * parent)
{
    m_original = original;
}

void GhostNode::transform(Body::coordinate c)
{
    setPosition(getPosition().add(c));

}

void GhostNode::transformSubMap(Body::coordinate vector)
{
    for(int i=0; i<m_underMap.length(); i++){
        m_underMap[i]->transform(vector);
    }
}

void GhostNode::transformIgnoreSubMap(Body::coordinate c)
{
    setPosition(getPosition().add(c));
    transformSubMap(c.invert());
}

void GhostNode::setPosition(Body::coordinate c)
{
    m_position = c;
    m_obj->setProperty("x",c.x);
    m_obj->setProperty("y",c.y);

    updateAbsolutePosition();

    updateRelation();
}


void GhostNode::setPositionByCenter(Body::coordinate c)
{
    Body::coordinate m;
    m.x = c.x - m_width/2;
    m.y = c.y - m_height/2;
    setPosition(m);
}
void GhostNode::setPositionByCenterIgnoreSubMap(Body::coordinate c)
{
    Body::coordinate m;
    m.x = c.x - m_width/2;
    m.y = c.y - m_height/2;

    shiftSubMap(getPosition().subtract(m));
    setPosition(m);

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
    connect(object,SIGNAL(widthChanged()),this,SLOT(widthChanged()));
    connect(object,SIGNAL(heightChanged()),this,SLOT(heightChanged()));
    //QObject * textObj = object->findChild<QObject*>("textInput");
    //connect(textObj,SIGNAL(accepted()),this,SLOT(inputAccepted()));
    //connect(object,SIGNAL(typeAccepted(QString)),this,SLOT(typeInputAccepted(QString)));

    connect(object,SIGNAL(update()),this,SLOT(geometryChanged()));

    m_obj = object;
    widthChanged();
    heightChanged();
}

void GhostNode::adoptOriginal()
{
    m_obj->findChild<QObject*>("textInput")->setProperty("text",m_original->getName());
    m_obj->findChild<QObject*>("typeName")->setProperty("text",m_original->typeName());
    m_obj->setProperty("ghost",true);
    setName(m_original->getName());
    heightChanged();


}


BaseNode * GhostNode::isInside(int x, int y)
{
    Body::coordinate position = m_position;

    if(m_obj->property("expanded").toBool()){
        for(int i=0; i<m_underMap.length(); i++){
            BaseNode * b = m_underMap[i]->isInside(x-position.x,y-position.y);
            if(b){
                hover(false);
                return b;
            }
        }
    }
    int width = m_width;
    int height = m_height;

    if(x>position.x && x<position.x + width && y > position.y && y < position.y + height){
        hover(true);
        return this;
    }else{
        hover(false);

        return nullptr;
    }
}

void GhostNode::select(bool b)
{
    if(b){
        m_batchSelected = true;
        highlight(true);
        preventFocus(true);
    }else{
        m_batchSelected = false;

        highlight(false);

    }
}

void GhostNode::hover(bool b)
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

void GhostNode::mouseClicked()
{

}

void GhostNode::mousePressed()
{

}

void GhostNode::mouseReleased()
{

}

void GhostNode::geometryChanged()
{
    updateRelation();
    for(int i=0; i<m_underMap.length(); i++){
        m_underMap[i]->geometryChanged();
    }

}

void GhostNode::widthChanged()
{
    m_width = m_obj->property("width").toInt();

}

void GhostNode::heightChanged()
{
    if(m_obj->findChild<QObject*>("typeNameContainer")->property("width").toInt() == 0){
        m_height = m_obj->property("height").toInt() - m_obj->findChild<QObject*>("typeNameContainer")->property("height").toInt();
    }else{
        m_height = m_obj->property("height").toInt();
    }

}

void GhostNode::updateAbsolutePosition()
{


    if(m_abstraction){

        m_absolutePosition = m_abstraction->getAbsolutePosition().add(getPosition());
    }
    else{

        m_absolutePosition = m_position;
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
    widthChanged();
    heightChanged();
    c.x = m_position.x + m_width/2;
    c.y = m_position.y + m_height/2;
    m_centerPosition = c;
    return c;
}

void GhostNode::setUnderMap(QVector<BaseNode *> subMap)
{
    updateAbsolutePosition();
    m_underMap = subMap;
    for(int i=0; i<subMap.length(); i++){
        BaseNode * b = subMap[i];
        Body::coordinate c = b->getAbsolutePosition().subtract(this->getAbsolutePosition());
        b->obj()->setParentItem(this->obj()->findChild<QQuickItem*>("expandedArea"));
        b->setPosition(c);

        b->setVisibility(false);
    }
    if(!m_underMap.isEmpty()){
        reFormatExpandedForm();
        updateAbsolutePosition();
    }
}

void GhostNode::underMapAppendNode(BaseNode *b)
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

void GhostNode::subNodeMoved()
{
    reFormatExpandedForm();
    if(m_abstraction){
        m_abstraction->subNodeMoved();
    }
}

void GhostNode::abstract()
{
    for(int i=0; i<m_underMap.length(); i++){
        m_underMap[i]->abstract();
        m_underMap[i]->setVisibility(false);
    }
    Body::coordinate center = getCenterPosition();
    m_obj->setProperty("expanded",false);
    m_expanded = false;
    setPositionByCenterIgnoreSubMap(center);

}

void GhostNode::expand()
{
    if(m_underMap.isEmpty()){
        QVector<BaseNode*> subMap = m_original->getUnderMap();

        QVector<BaseNode*> mySubMap;
        for(int i=0; i<subMap.length(); i++){
            BaseNode * b = subMap[i];
            if(typeid (*b) == typeid (GhostNode)){

                GhostNode * g = b->getGhostPointer();
                GhostNode * clone = g->getOriginal()->newGhostNode();
                clone->setAbstraction(this);
                Body::coordinate geometry;
                geometry.x = g->getAbstraction()->width()/2;
                geometry.y = g->getAbstraction()->height()/2;
                Body::coordinate vector = g->getPosition().subtract(geometry);

                clone->setPosition(vector.add(this->getCenterAbsolutePosition()));

                mySubMap.append(clone);

            }
        }
        setUnderMap(mySubMap);


    }

    for(int i=0; i<m_underMap.length(); i++){
        BaseNode * b = m_underMap[i];
        b->setVisibility(true);
        //b->setPositionByLocalVector();

    }
    //Body::coordinate center = getCenterPosition();
    m_obj->setProperty("expanded",true);
    m_expanded = true;

    //setPositionByCenterIgnoreSubMap(center);
    reFormatExpandedForm();



}

void GhostNode::reFormatExpandedForm()
{
    if(!m_underMap.isEmpty()){

        int leftMost = 10000000;
        int rightMost = -1000000;
        int topMost = 10000000;
        int botMost = -1000000;

        for(int i=0; i<m_underMap.length(); i++){
            BaseNode * b = m_underMap[i];

            int padding = 5;
            int x;
            if(b->obj()->findChild<QObject*>("typeNameContainer")->property("width").toInt() > b->width()){
                x = b->getPosition().x + b->obj()->findChild<QObject*>("typeNameContainer")->property("x").toInt();
            }else{
                x = b->getPosition().x;
            }

            if(x - padding< leftMost){

                leftMost = x - padding;
            }

            if(b->obj()->findChild<QObject*>("typeNameContainer")->property("width").toInt() > b->width()){
                x += b->obj()->findChild<QObject*>("typeNameContainer")->property("width").toInt();
            }else{
                x += b->width();
            }
            if(x + padding> rightMost){

                rightMost = x + padding;
            }
            int y = b->getPosition().y;
            if(y - padding< topMost){

                topMost = y - padding;
            }
            y = b->getPosition().y + b->height();
            if(y + padding> botMost){

                botMost = y + padding;
            }

        }
        Body::coordinate median;
        median.x = (leftMost + rightMost) / 2;
        median.y = (topMost + botMost) / 2;


        int displace = m_obj->findChild<QQuickItem*>("nameContainer")->property("height").toInt();

        QObject * area = m_obj->findChild<QObject*>("expandedArea");
        area->setProperty("width",rightMost - leftMost);
        area->setProperty("height",botMost - topMost + displace);
        Body::coordinate geometry;
        geometry.x = width()/2;

        if(m_obj->property("expanded").toBool()){
            geometry.y = displace+area->findChild<QObject*>("expandedRectangle")->property("height").toInt()/2;
        }else{
            geometry.y = height()/2;
        }


        transformIgnoreSubMap(median.subtract(geometry));
        if(m_abstraction){
            m_abstraction->reFormatExpandedForm();
        }
    }
    updateRelation();

}

void GhostNode::setAbstraction(BaseNode *n)
{
    m_abstraction = n;
}

void GhostNode::shiftSubMap(Body::coordinate vector)
{
    for(int i=0; i<m_underMap.length(); i++){
        BaseNode * b = m_underMap[i];
        if(typeid (*b) == typeid (GhostNode)){
            GhostNode * g = b->getGhostPointer();
            g->setPosition(g->getPosition().add(vector));
        }

    }
}
void GhostNode::destroy()
{
    Body * b = Body::getInstance();
    QVector<BaseNode*> temp = m_underMap;
    for(int i=0; i<temp.length(); i++){

        temp[i]->destroy();

    }
    if(m_abstraction){

        m_abstraction->removeSubNode(this);

    }
    b->removeGhost(this);

    terminate();
    m_original->unregisterGhost(this);
    m_obj->deleteLater();
    m_obj = nullptr;


}

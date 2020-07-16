#include "ghostnode.h"
#include <iostream>

using namespace std;

GhostNode::GhostNode(Node * original,QObject * parent)
{
    m_original = original;
}

void GhostNode::qDebugSpecs()
{
    qDebug()<<"GhostNode: "<<getName();
    qDebug()<<"coordinates: "<<getPosition().x<<getPosition().y;
    qDebug()<<"type: "<<m_obj->findChild<QObject*>("typeName")->property("text").toString();
    qDebug()<<"undermap: ";
    for(int i=0; i<m_underMap.length(); i++){
        qDebug()<<m_underMap[i]->getName();
    }
    qDebug()<<"expanded: "<<isExpanded();
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
void GhostNode::selectTextBox()
{
    m_obj->findChild<QObject*>("expandedText")->setProperty("focus",true);
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
    if(getName() == "sperm head"){

    }
    if(m_obj->property("expanded").toBool()){
        BaseNode * move = nullptr;
        for(int i=0; i<m_underMap.length(); i++){
            BaseNode * b = m_underMap[i]->isInside(x-position.x,y-position.y);
            if(b){
                if(!b->isMoving()){
                    hover(false);
                    return b;
                }else{
                    move = b;
                }
            }
        }
        if(move){
            return move;
        }
    }
    int width = m_width;
    int height = m_height;

    if(x>position.x && x<position.x + width && y > position.y && y < position.y + height){


        QObject * rect = m_obj->findChild<QObject*>("expandedTextBox");
        if(rect->property("visible").toBool()){
            y-=m_position.y;
            if(y > rect->property("y").toInt() && y < rect->property("height").toInt() + rect->property("y").toInt()){
                selectTextBox();
                return nullptr;
            }
        }

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

void GhostNode::selectTextBox(bool b)
{
    m_obj->findChild<QObject*>("expandedText")->setProperty("focus",b);
    if(!b){
        Body * b = Body::getInstance();
        b->setFocusWindow();
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


QString GhostNode::getText()
{
    return m_obj->findChild<QObject*>("expandedText")->property("text").toString();
}

void GhostNode::setText(QString s)
{
    m_obj->findChild<QObject*>("expandedText")->setProperty("text",s);
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

int GhostNode::displayHeight()
{
    int out = m_height;
    if(typeVisible()){
        out += m_obj->findChild<QObject*>("typeName")->property("height").toInt();
    }
    return out;
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
    if(m_underMap.contains(b)){
        return;
    }

    if(!m_obj->property("expanded").toBool()){
        b->setVisibility(false);

    }
    appendToUnderMap(b);

    syncOriginal(b);

}

void GhostNode::appendToUnderMap(BaseNode *b)
{
    m_underMap.append(b);
    Body::coordinate c = b->getAbsolutePosition().subtract(this->getAbsolutePosition());
    b->obj()->setParentItem(m_obj->findChild<QQuickItem*>("expandedArea"));
    b->setPosition(c);
    b->setAbstraction(this);

    if(!m_underMap.isEmpty()){
        reFormatExpandedForm();
        updateAbsolutePosition();
    }
}

void GhostNode::syncOriginal(BaseNode * b)
{

    if(typeid (*b) == typeid (GhostNode)){
        Body::coordinate vector = b->getPosition();
        BaseNode * g = b->getGhostPointer()->getOriginal()->newGhostNode();



        g->setPosition(m_original->getAbsolutePosition().add(vector));

        m_original->appendToUnderMap(g,this);

    }

}
bool GhostNode::underMapContains(BaseNode *b)
{
    if(m_underMap.contains(b)){
        return true;
    }
    for(int i=0; i<m_underMap.length(); i++){

        if(m_underMap[i]->underMapContains(b)){
            return true;
        }
    }
    return false;
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
        //m_underMap[i]->abstract();
        m_underMap[i]->setVisibility(false);
    }
    Body::coordinate center = getCenterPosition();
    m_obj->setProperty("expanded",false);
    m_expanded = false;
    setPositionByCenterIgnoreSubMap(center);

}



void GhostNode::cloneSubMap(BaseNode *b)
{
    QVector<Relation*> buffer;
    if(typeid (*b) == typeid (Node)){
        QVector<Relation*> subRelations = b->getNodePointer()->getUnderRelation();
        for(int i=0; i<subRelations.length(); i++){
            Body * body = Body::getInstance();
            Relation * r = subRelations[i];
            Relation * clone;
            if(r->getType() == "arrow"){
                clone = body->newRelation(body->allocateNewID("relation"),r->originNode(),r->destinationNode());
            }
            if(r->getType() == "line"){
                clone = body->newLine(body->allocateNewID("relation"),r->originNode(),r->destinationNode());
            }
            if(r->getType() == "triangle"){
                clone = body->newTriangle(body->allocateNewID("relation"),r->originNode(),r->destinationNode());
            }
            clone->setName(r->getName());
            buffer.append(clone);
            buffer[i]->disconnect();
        }
    }
    QVector<BaseNode*> subMap = b->getUnderMap();


    if(!subMap.isEmpty()){

        QVector<BaseNode*> mySubMap;
        for(int i=0; i<subMap.length(); i++){
            BaseNode * n = subMap[i];
            if(typeid (*n) == typeid (GhostNode)){
                GhostNode * g = n->getGhostPointer();
                GhostNode * clone = g->getOriginal()->newGhostNode();
                clone->setAbstraction(this);
                Body::coordinate geometry;
                geometry.x = g->getAbstraction()->width()/2;
                geometry.y = g->getAbstraction()->height()/2;
                Body::coordinate vector = g->getPosition();



                clone->setPosition(vector);


                mySubMap.append(clone);
                for(int j=0; j<buffer.length(); j++){

                    if(buffer[j]->originNode() == n){
                        buffer[j]->setOriginObject(clone);
                        buffer[j]->updateSelf();
                    }
                    if(buffer[j]->destinationNode() == n){
                        buffer[j]->setDestinationObject(clone);
                        buffer[j]->updateSelf();
                    }
                }
                for(int j=0; j<buffer.length(); j++){
                    buffer[j]->finalizeSelf();
                }

            }
        }

        setUnderMap(mySubMap);
    }


}



void GhostNode::expand()
{
    m_obj->setProperty("expanded",true);
    m_expanded = true;
    if(m_expandState == 0 || m_expandState == -1){
        expandMap();
    }
    if(m_expandState == 1){
        expandTree();
    }
    if(m_expandState == 2){
        expandText();
    }


}

void GhostNode::expandMap()
{
    if(m_underMap.isEmpty()){
        if(m_original){

            cloneSubMap(m_original);

        }

    }
    m_obj->findChild<QObject*>("expandedRectangle")->setProperty("visible",true);

    if(!m_underMap.isEmpty()){
        for(int i=0; i<m_underMap.length(); i++){
            m_underMap[i]->setVisibility(true);
        }
        //Body::coordinate center = getCenterPosition();
        m_obj->setProperty("expanded",true);
        //setPositionByCenterIgnoreSubMap(center);

        reFormatExpandedForm();

    }else{
        m_expanded = false;
        m_obj->setProperty("expanded",false);

    }


}

void GhostNode::expandTree()
{
    if(!m_underMap.isEmpty()){
        for(int i=0; i<m_underMap.length(); i++){
            m_underMap[i]->setVisibility(true);

        }
    }
}

void GhostNode::expandImage()
{

}


void GhostNode::expandText()
{
    m_obj->findChild<QObject*>("expandedTextBox")->setProperty("visible",true);
}

bool GhostNode::clickAction()
{
    if(textBoxSelected()){
        return false;
    }
    return true;
}

void GhostNode::cycleExpandState(int state)
{
    m_expandState = state;
    abstract();
    expand();
}


void GhostNode::extract()
{
    Body * b = Body::getInstance();
    Body::coordinate c = getAbsolutePosition();
    m_obj->setParentItem(b->getRoot()->findChild<QQuickItem*>("layer"));
    setPosition(c);
    setAbstraction(nullptr);
    b->registerGhost(this);

}

void GhostNode::exude(BaseNode * b)
{
    if(m_underMap.contains(b)){
        b->extract();
        m_underMap.removeOne(b);
    }
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
            y = b->getPosition().y + b->displayHeight();

            if(y + padding> botMost){

                botMost = y + padding;
            }

        }

        Body::coordinate median;

        median.x = (leftMost + rightMost) / 2;
        median.y = (topMost + botMost) / 2;
        //qDebug()<<"Leftmost: "<<leftMost<<"| rightmost: "<<rightMost<<"| topmost: "<<topMost<<"| botmost: "<<botMost;

        int displace = m_obj->findChild<QQuickItem*>("nameContainer")->property("height").toInt();

        QObject * area = m_obj->findChild<QObject*>("expandedArea");
        area->setProperty("width",rightMost - leftMost);
        area->setProperty("height",botMost - topMost + displace);

        Body::coordinate geometry;
        geometry.x = m_obj->property("width").toInt()/2;

        if(m_obj->property("expanded").toBool()){
            geometry.y = displace+area->findChild<QObject*>("expandedRectangle")->property("height").toInt()/2;
        }else{
            geometry.y = height()/2;
        }
        //qDebug()<<"median: "<<median;
        //qDebug()<<"Transformed: "<<median.subtract(geometry);
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


    terminate();
    m_original->unregisterGhost(this);
    m_obj->deleteLater();
    m_obj = nullptr;
    b->removeGhost(this);

}

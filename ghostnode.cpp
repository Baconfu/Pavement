#include "ghostnode.h"
#include <note.h>
#include <iostream>
#include <nodearea.h>

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
    /*
    if(getName() == "sub2"){
        if(getID() == 20){
            qDebug()<<"sub2:"<<c;
            qDebug()<<"check";
        }

    }
    if(getName() == "sub3"){
        if(getID() == 22){
            qDebug()<<"sub3:"<<c;

        }
    }*/
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

BaseNode * GhostNode::isInside(Body::coordinate c)
{
    int x = c.x;
    int y = c.y;
    int width = m_width;
    int height = m_height;
    if(preventingFocus()){
        hover(false,Body::coordinate());
        return nullptr;
    }
    if(x>m_position.x && x<m_position.x + width && y > m_position.y && y < m_position.y + height){

        if(isExpanded() && expandState() == 0){
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
        }


        return this;
    }else{
        removeUnderMapFocus();
        return nullptr;
    }
}

void GhostNode::select(bool b,Body::coordinate c)
{
    if(b){
        Body::coordinate local = c.subtract(m_position);

        int divider1 = m_obj->findChild<QObject*>("nameContainer")->property("height").toInt();
        int divider2 = m_obj->findChild<QObject*>("typeNameContainer")->property("y").toInt();

        if(isExpanded() && expandState() == 2){
            if(local.y > divider1 && local.y < divider2){
                return;
            }else{
                m_batchSelected = b;
                highlight(true);
            }
        }else{
            m_batchSelected = b;
            highlight(true);
        }


    }
}

void GhostNode::select(bool b)
{
    highlight(b);
    m_batchSelected = b;
}

void GhostNode::hover(bool b,Body::coordinate c)
{
    Body::coordinate local = c.subtract(getAbsolutePosition());
    Body * body = Body::getInstance();
    if(selected()){
        return;
    }else{
        if(b){

            int divider1 = m_obj->findChild<QObject*>("nameContainer")->property("height").toInt();
            int divider2 = m_obj->findChild<QObject*>("typeNameContainer")->property("y").toInt();

            if(local.y < divider1){
                highlight(true);
                body->setFocusWindow();
            }
            if(local.y > divider1 && local.y < divider2){
                if(expandState() == 2){
                    m_obj->findChild<QObject*>("expandedText")->setProperty("focus",true);

                }
                highlight(false);
            }
            if(local.y > divider2){
                body->setFocusWindow();
                highlight(true);
            }
        }else{
            highlight(false);
        }
    }

}

void GhostNode::highlight(bool b)
{
    if(b){
        obj()->setProperty("highlighted",b);
    }else{
        if(!m_batchSelected){
            obj()->setProperty("highlighted",b);
        }
    }
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
int GhostNode::displayX()
{
    int x = getX();
    int w = width();
    if(obj()->findChild<QObject*>("typeNameContainer")->property("width").toInt() > w){
        x = getPosition().x + obj()->findChild<QObject*>("typeNameContainer")->property("x").toInt();
        w = obj()->findChild<QObject*>("typeNameContainer")->property("width").toInt();
    }
    if(obj()->findChild<QObject*>("nameContainer")->property("width").toInt() > w){
        x = getPosition().x + obj()->findChild<QObject*>("nameContainer")->property("x").toInt();
    }
    return x;
}

int GhostNode::displayY()
{
    int y = getY();
    return y;
}

int GhostNode::displayWidth()
{
    int out = width();
    if(m_obj->findChild<QObject*>("typeNameContainer")->property("width").toInt() > width()){
        out = m_obj->findChild<QObject*>("typeNameContainer")->property("width").toInt();
    }
    if(m_obj->findChild<QObject*>("textInput")->property("contentWidth").toInt()+10 > out){
        out = m_obj->findChild<QObject*>("textInput")->property("contentWidth").toInt()+10 ;
    }

    return out;
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
        if(b->getName() == "microtubule"){
            qDebug()<<"definitive";
        }
        b->setPosition(c);
        if(b->getName() == "microtubule"){
            qDebug()<<"definitive end";
        }
        b->setAbstraction(this);

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
    BaseNode * append = b;
    if(typeid (*b) == typeid (Note)){
        return;
    }
    if(typeid (*b) == typeid (Node)){
        Node * n = b->getNodePointer();
        GhostNode * g = n->newGhostNode();
        append = g;


    }
    if(!m_obj->property("expanded").toBool()){
        append->setVisibility(false);

    }
    appendToUnderMap(append);

    syncOriginal(append);
}

void GhostNode::appendToUnderMap(BaseNode *b)
{
    m_underMap.append(b);
    Body::coordinate c = b->getAbsolutePosition().subtract(this->getAbsolutePosition());
    b->obj()->setParentItem(m_obj->findChild<QQuickItem*>("expandedArea"));
    b->setPosition(c);
    b->setAbstraction(this);

    reFormatExpandedForm();
    updateAbsolutePosition();


}

void GhostNode::syncOriginal(BaseNode * b)
{

    if(typeid (*b) == typeid (GhostNode)){
        Body::coordinate vector = b->getPosition();
        BaseNode * g = b->getGhostPointer()->getOriginal()->newGhostNode();



        g->setPosition(m_original->getAbsolutePosition().add(vector));

        m_original->appendToUnderMap(g);
        m_original->syncGhosts(g,this);
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

void GhostNode::clearUnderMap()
{
    QVector<BaseNode*> temp = m_underMap;
    for(int i=0; i<temp.length(); i++){

        temp[i]->destroy();

    }

}

void GhostNode::removeUnderMapFocus()
{
    hover(false,Body::coordinate());
    for(int i=0; i<m_underMap.length(); i++){
        m_underMap[i]->removeUnderMapFocus();
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
    if(m_obj->property("expanded").toBool()){

        Body::coordinate center = getCenterPosition();

        m_obj->setProperty("expanded",false);
        m_expanded = false;
        setPositionByCenter(center);

    }


    reFormatExpandedForm();


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
            Body::coordinate geometry;
            geometry.x = n->getAbstraction()->width()/2;
            geometry.y = n->getAbstraction()->height()/2;
            if(typeid (*n) == typeid (GhostNode)){
                GhostNode * g = n->getGhostPointer();
                GhostNode * clone = g->getOriginal()->newGhostNode();


                Body::coordinate vector = g->getCenterPosition();

                clone->setPositionByCenter(getAbsolutePosition().subtract(geometry).add(vector));


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

            }
            if(typeid (*n) == typeid (NodeArea)){
                Body * body = Body::getInstance();
                NodeArea * clone = body->newNodeArea(n);
                Body::coordinate vector = n->getPosition();
                clone->setPosition(getAbsolutePosition().subtract(geometry).add(vector));
                mySubMap.append(clone);
            }
            for(int j=0; j<buffer.length(); j++){
                buffer[j]->finalizeSelf();
            }

        }
        qDebug()<<"should have worked";
        setUnderMap(mySubMap);
    }


}



void GhostNode::expand()
{
    if(m_expandState == -1){
        return;
    }
    //m_obj->setProperty("expanded",true);
    m_expanded = true;
    if(m_expandState == 0){

        expandMap();
    }
    if(m_expandState == 1){
        expandTree();
    }
    if(m_expandState == 2){
        expandText();
    }
    for(int i=0; i<m_underMap.length(); i++){
        m_underMap[i]->expand();
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
    QObject * textBox = m_obj->findChild<QObject*>("expandedTextBox");
    QObject * text = textBox->findChild<QObject*>("expandedText");
    int width = text->property("contentWidth").toInt() + 20;
    if(width < textBox->property("minWidth").toInt()){
        width = textBox->property("minWidth").toInt();
    }
    int height = text->property("contentHeight").toInt() + 40;
    if(height < textBox->property("minHeight").toInt()){
        height = textBox->property("minHeight").toInt();
    }
    m_obj->findChild<QObject*>("expandedTextBox")->setProperty("visible",true);
    m_obj->findChild<QObject*>("expandedArea")->setProperty("width",width);
    m_obj->findChild<QObject*>("expandedArea")->setProperty("height",height);
}

bool GhostNode::clickShouldSelect()
{
    if(m_obj->findChild<QObject*>("expandedText")->property("focus").toBool()){
        return false;
    }
    return true;
}

void GhostNode::cycleExpandState(int state)
{
    setExpandState(state);
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
    reFormatExpandedForm();
}

void GhostNode::reFormatExpandedForm()
{
    Body::coordinate pos = getPosition();
    if(!m_underMap.isEmpty()){

        int leftMost = 10000000;
        int rightMost = -1000000;
        int topMost = 10000000;
        int botMost = -1000000;

        for(int i=0; i<m_underMap.length(); i++){
            BaseNode * b = m_underMap[i];

            int padding = 5;
            int x;
            int width = b->displayWidth();
            x = b->displayX();

            if(x - padding< leftMost){

                leftMost = x - padding;
            }
            if(x + width + padding> rightMost){

                rightMost = x + width + padding;
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
    }
    if(!isExpanded()){
        setPosition(pos);
    }
    if(m_abstraction){
        m_abstraction->reFormatExpandedForm();
    }

    updateRelation();

}

void GhostNode::setAbstraction(BaseNode *n)
{
    m_abstraction = n;
}

bool GhostNode::abstractionExists(BaseNode * b)
{
    BaseNode * abstraction = m_abstraction;
    while(abstraction){
        if(abstraction == b){
            return true;
        }else{
            abstraction = abstraction->getAbstraction();
        }
    }
    return false;
}

BaseNode *GhostNode::getHighestAbstraction()
{
    BaseNode * abstraction = m_abstraction;
    while(abstraction){
        if(!abstraction->getAbstraction()){
            return abstraction;
        }
        abstraction = abstraction->getAbstraction();
    }
    return this;
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

void GhostNode::moving(bool b)
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
void GhostNode::destroy()
{
    Body * b = Body::getInstance();
    clearUnderMap();
    if(m_abstraction){

        m_abstraction->removeSubNode(this);

    }


    terminate();
    m_original->unregisterGhost(this);
    m_obj->deleteLater();
    m_obj = nullptr;
    b->removeGhost(this);

}

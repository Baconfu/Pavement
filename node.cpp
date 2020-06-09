#include "node.h"
#include <ghostnode.h>
#include <QMetaObject>
#include <QGenericArgument>

Node::Node(QObject * parent, int id)
{
    m_id = id;
}

QString Node::getName()
{
    return m_name;
}

void Node::inputAccepted()
{

    preventFocus(true);
    Body * b = Body::getInstance();
    b->setFocusWindow();
    if(b->latestContext() == Body::node_selected){
        b->contextResolved();
    }

    m_name = m_obj->findChild<QObject*>("textInput")->property("text").toString();
}

void Node::inputPassivelyAccepted()
{
    m_name = m_obj->findChild<QObject*>("textInput")->property("text").toString();
}

void Node::typeInputAccepted(QString s)
{
    Body * b = Body::getInstance();
    setType(b->getNodeByName(s));
    for(int i=0; i<m_ghosts.length(); i++){
        m_ghosts[i]->adoptOriginal();
    }
    b->setFocusWindow();
    m_type = s;
}

void Node::typeInputPassivelyAccepted(QString s)
{
    Body * b = Body::getInstance();
    setType(b->getNodeByName(s));
    m_type = s;

}

void Node::highlight(bool visible)
{
    obj()->setProperty("highlighted",visible);

}

void Node::hover(bool b)
{
    if(!m_batchSelected){
        highlight(b);
    }
}

void Node::select(bool b)
{
    highlight(b);
    m_batchSelected = b;
}

void Node::preventFocus(bool b)
{
    m_preventFocus = b;
}


void Node::setName(QString name)
{
    m_name = name;
    m_obj->setProperty("name",name);
}

void Node::transform(Body::coordinate c)
{
    setPosition(getPosition().add(c));

}

void Node::transformIgnoreSubMap(Body::coordinate c)
{
    transform(c);
    transformSubMap(c.invert());
}

void Node::setPosition(Body::coordinate c)
{
    m_position = c;
    m_obj->setProperty("x",c.x);
    m_obj->setProperty("y",c.y);
    updateAbsolutePosition();
    updateRelation();

}

void Node::setPositionIgnoreSubMap(Body::coordinate c)
{
    Body::coordinate vector = m_position.subtract(c);
    setPosition(c);
    transformSubMap(vector);
}

void Node::setPositionByCenter(Body::coordinate c)
{
    Body::coordinate m;

    m.x = c.x - m_width/2;
    m.y = c.y - m_height/2;
    setPosition(m);
}

void Node::setPositionByCenterIgnoreSubMap(Body::coordinate c)
{
    Body::coordinate m;

    m.x = c.x - m_width/2;
    m.y = c.y - m_height/2;
    setPositionIgnoreSubMap(m);

}

Body::coordinate Node::getPosition()
{
    Body::coordinate c;

    c.x = m_obj->property("x").toInt();
    c.y = m_obj->property("y").toInt();
    m_position = c;
    return c;
}

Body::coordinate Node::getCenterPosition()
{
    Body::coordinate c;

    getPosition();
    c.x = m_position.x + m_width/2;
    c.y = m_position.y + m_height/2;
    m_centerPosition = c;
    return c;
}

int Node::allocateGhostID()
{
    Body * b = Body::getInstance();

    return b->allocateNewID("node");
}

GhostNode *Node::getGhostByID(int id)
{
    for(int i=0; i<m_ghosts.length(); i++){
        if(m_ghosts[i]->getID() == id){
            return m_ghosts[i];
        }
    }
    return nullptr;

}

GhostNode *Node::newGhostNode()
{
    GhostNode * n = new GhostNode(this);
    registerGhost(n);
    n->setID(allocateGhostID());
    n->initializeObj();
    n->adoptOriginal();
    Body * body = Body::getInstance();
    body->registerGhost(n);

    return n;
}


void Node::setType(Node *n)
{
    if(n){
        m_typeNode = n;
        m_type = n->getName();

        obj()->setProperty("type",m_type);
        obj()->findChild<QObject*>("typeName")->setProperty("italic",true);


        n->registerMember(this);
    }else{
        Body * b = Body::getInstance();

        m_typeNode = nullptr;
        m_type = "";
        m_style = b->defaultStyle();
        obj()->setProperty("type",m_type);
        obj()->findChild<QObject*>("typeName")->setProperty("italic",false);
    }

}

int Node::registerMember(Node *n)
{
    if(!n){
        return 1;
    }
    if(memberExists(n)){
        return 1;
    }
    m_members.append(n);
    n->setType(this);
    return 0;
}

void Node::removeMember(Node *n)
{
    for(int i=0; i<members().length(); i++){
        if(n->getID() == members()[i]->getID()){
            Node * nll = nullptr;
            n->setType(nll);
            members().removeAt(i);
        }
    }
}

bool Node::memberExists(Node *n)
{
    for(int i=0; i<m_members.length(); i++){
        if(n == m_members[i]){
            return true;
        }
    }
    return false;
}

void Node::setUnderMap(QVector<BaseNode *> nodes)
{
    m_underMap = nodes;
    for(int i=0; i<nodes.length(); i++){
        BaseNode * b = nodes[i];
        Body::coordinate c = b->getAbsolutePosition().subtract(this->getPosition());
        b->obj()->setParentItem(this->obj()->findChild<QQuickItem*>("expandedArea"));
        b->setPosition(c);
        b->setVisibility(false);

    }
    if(!nodes.isEmpty()){
        reFormatExpandedForm();
        updateAbsolutePosition();
    }


}

void Node::underMapAppendNode(BaseNode *node)
{
    if(typeid (*node) == typeid (GhostNode)){
        if(node->getGhostPointer()->getOriginal() == this){
            return;
        }
    }
    m_underMap.append(node);

    Body::coordinate c = node->getPosition().subtract(this->getPosition());
    node->obj()->setParentItem(this->obj()->findChild<QQuickItem*>("expandedArea"));
    node->setPosition(c);
    //node->setVisibility(false);
    node->setAbstraction(this);
    for(int i=0; i<m_ghosts.length(); i++){
        if(typeid (*node) == typeid (GhostNode)){
            BaseNode * b = node->getGhostPointer()->getOriginal()->newGhostNode();
            Body::coordinate d;
            d.x = 5;
            d.y = 30;
            b->setPosition(m_ghosts[i]->getAbsolutePosition().add(d));
            m_ghosts[i]->underMapAppendNode(b);
        }

    }
    reFormatExpandedForm();

}

void Node::removeSubNode(BaseNode *b)
{
    m_underMap.removeOne(b);
}

void Node::transformSubMap(Body::coordinate vector)
{
    for(int i=0; i<m_underMap.length(); i++){
        m_underMap[i]->transform(vector);
    }
}

void Node::subNodeMoved()
{
    reFormatExpandedForm();
    if(m_abstraction){
        m_abstraction->subNodeMoved();
    }
}

void Node::reFormatExpandedForm()
{
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


    }
    updateRelation();

}

void Node::setAbstraction(BaseNode *n)
{
    m_abstraction = n;
}

void Node::cloneSubMap(BaseNode *b)
{
    QVector<BaseNode*> subMap = b->getUnderMap();
    if(!subMap.isEmpty()){
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
}



void Node::expand()
{
    Body::coordinate pos = getCenterPosition();
    m_obj->setProperty("expanded",true);
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
    setPositionByCenter(pos);

}

void Node::expandMap()
{
    if(m_underMap.isEmpty()){
        if(m_typeNode){
            cloneSubMap(m_typeNode);

        }

    }
    m_obj->findChild<QObject*>("expandedRectangle")->setProperty("visible",true);

    if(!m_underMap.isEmpty()){
        m_expanded = true;
        for(int i=0; i<m_underMap.length(); i++){
            m_underMap[i]->setVisibility(true);
        }
        Body::coordinate center = getCenterPosition();
        m_obj->setProperty("expanded",true);


        setPositionByCenterIgnoreSubMap(center);

        reFormatExpandedForm();
    }else{
        m_expanded = true;
        m_obj->setProperty("expanded",true);

    }


}

void Node::expandTree()
{

}

void Node::expandImage()
{

}

void Node::expandText()
{
    m_obj->findChild<QObject*>("expandedTextBox")->setProperty("visible",true);
}

bool Node::clickAction()
{
    if(textBoxSelected()){

        return false;
    }
    return true;
}

void Node::cycleExpandState(int state)
{
    m_expandState = state;
    abstract();
    expand();
}

void Node::abstract()
{

    if(m_expanded){

        m_expanded = false;

        for(int i=0; i<m_underMap.length(); i++){
            m_underMap[i]->abstract();
            m_underMap[i]->setVisibility(false);

        }
        Body::coordinate center = getCenterPosition();
        m_obj->setProperty("expanded",false);

        setPositionByCenterIgnoreSubMap(center);

    }
}

void Node::exude(BaseNode * b)
{
    if(m_underMap.contains(b)){
        b->extract();
        m_underMap.removeOne(b);
    }
    reFormatExpandedForm();

}

void Node::deleteObj()
{
    m_obj->deleteLater();
    m_obj = nullptr;
}

void Node::destroy()
{
    disconnect();
    Body * b = Body::getInstance();



    QVector<BaseNode*> temp = m_underMap;
    for(int i=0; i<temp.length(); i++){
        temp[i]->destroy();

    }
    if(m_abstraction){
        if(typeid (*m_abstraction) == typeid (GhostNode)){
            m_abstraction->getGhostPointer()->removeSubNode(this);
        }
        if(typeid (*m_abstraction) == typeid (Node)){
            m_abstraction->getNodePointer()->removeSubNode(this);
        }
    }

    QVector<GhostNode*> temp2 = m_ghosts;
    for(int i=0; i<temp2.length(); i++){

        temp2[i]->destroy();



    }
    for(int i=0; i<m_members.length(); i++){
        m_members[i]->setType(nullptr);
    }

    terminate();


    m_obj->deleteLater();
    delete(m_obj);
    m_obj = nullptr;
    b->removeNode(this);

}

void Node::selectTextBox(bool b)
{
    m_obj->findChild<QObject*>("expandedText")->setProperty("focus",b);
}

void Node::setVisibility(bool visibility)
{
    m_visible = visibility;
    obj()->setProperty("visible",visibility);

}



void Node::setHidden(bool b)
{
    m_hidden = b;


}

void Node::dissolve()
{
    m_dissolve = true;
    setVisibility(false);
    updateRelations();

}

void Node::distill()
{
    m_dissolve = false;
    setVisibility(true);
    updateRelations();
}

QString Node::getText()
{
    return m_obj->findChild<QObject*>("expandedText")->property("text").toString();
}

void Node::setText(QString s)
{
    m_obj->findChild<QObject*>("expandedText")->setProperty("text",s);
}


void Node::updateRelations()
{

}

void Node::setStyle()
{

}

void Node::giveInputFocus()
{
    if(!preventingFocus()){
        obj()->findChild<QObject*>("textInput")->setProperty("focus",true);
    }
}
void Node::giveTypeInputFocus()
{
    if(!preventingFocus()){
        obj()->findChild<QObject*>("typeName")->setProperty("focus",true);
    }
}

void Node::moving(bool b)
{
    m_moving = b;
}

void Node::initializeObj()
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
    QObject * textObj = object->findChild<QObject*>("textInput");
    connect(textObj,SIGNAL(accepted()),this,SLOT(inputAccepted()));
    connect(object,SIGNAL(passivelyAccepted()),this,SLOT(inputPassivelyAccepted()));
    connect(object,SIGNAL(typeAccepted(QString)),this,SLOT(typeInputAccepted(QString)));
    connect(object,SIGNAL(typePassivelyAccepted(QString)),this,SLOT(typeInputPassivelyAccepted(QString)));
    connect(object,SIGNAL(update()),this,SLOT(updateRelations()));
    connect(object,SIGNAL(update()),this,SLOT(geometryChanged()));

    m_obj = object;
    widthChanged();
    heightChanged();
}



BaseNode * Node::isInside(int x, int y)
{
    Body::coordinate position = m_position;
    heightChanged();
    if(m_obj->property("expanded").toBool()){
        for(int i=0; i<m_underMap.length(); i++){
            BaseNode * b = m_underMap[i]->isInside(x-position.x,y-position.y);
            if(b){
                preventFocus(false);
                hover(false);
                return b;
            }
        }
    }

    int width = m_width;
    int height = m_height;

    if(m_obj->findChild<QObject*>("typeNameContainer")->property("width").toInt() == 0){
        height+=m_obj->findChild<QObject*>("typeNameContainer")->property("height").toInt();
    }


    if(x>position.x && x<position.x + width && y > position.y && y < position.y + height){

        QObject * rect = m_obj->findChild<QObject*>("expandedTextBox");
        if(rect->property("visible").toBool()){
            y-=m_position.y;
            if(y > rect->property("y").toInt() && y < rect->property("height").toInt() + rect->property("y").toInt()){

                selectTextBox(true);
            }else{
                selectTextBox(false);
            }
        }
        return this;
    }else{
        if(textBoxSelected()){
            selectTextBox(false);

            hoverSelect(10);
        }
        preventFocus(false);
        hover(false);
        return nullptr;
    }
}


void Node::updateAbsolutePosition()
{

    if(m_abstraction){
        m_absolutePosition = m_abstraction->getAbsolutePosition().add(getPosition());
    }
    else{
        m_absolutePosition = m_position;
    }

}


void Node::hoverSelect(int y)
{
    int divider = obj()->findChild<QObject*>("typeNameContainer")->property("y").toInt();

    qDebug()<<tally<<"wow";
    tally+=1;
    int localY = y - m_position.y;
    if(localY<=divider){
        qDebug()<<"selected";


        giveInputFocus();
    }else{
        giveTypeInputFocus();
    }

}

void Node::widthChanged()
{
    m_width = m_obj->property("width").toInt();

}
void Node::heightChanged()
{
    if(m_obj->findChild<QObject*>("typeName")->property("contentWidth").toInt() == 0){
        m_height = m_obj->property("height").toInt() - m_obj->findChild<QObject*>("typeNameContainer")->property("height").toInt();
    }else{
        m_height = m_obj->property("height").toInt();
    }


}

void Node::mouseClicked()
{

}

void Node::mousePressed()
{

}

void Node::mouseReleased()
{

}

void Node::geometryChanged()
{

    updateRelation();

    updateStructural();

    for(int i=0; i<m_underMap.length(); i++){

        m_underMap[i]->geometryChanged();
    }

}




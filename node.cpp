#include "node.h"
#include <ghostnode.h>
#include <note.h>
#include <QMetaObject>
#include <QGenericArgument>
#include <QNetworkAccessManager>

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
    QString name = m_obj->findChild<QObject*>("textInput")->property("text").toString();
    if(b->nameAlreadyExists(m_name,this)){
        m_obj->findChild<QObject*>("paintNode")->setProperty("color","red");
    }else{
        m_obj->findChild<QObject*>("paintNode")->setProperty("color","grey");
    }
    m_name = name;

}

void Node::inputPassivelyAccepted()
{
    Body * b = Body::getInstance();
    QString name = m_obj->findChild<QObject*>("textInput")->property("text").toString();
    if(b->nameAlreadyExists(m_name,this)){
        m_obj->findChild<QObject*>("paintNode")->setProperty("color","red");
    }else{
        m_obj->findChild<QObject*>("paintNode")->setProperty("color","grey");
    }
    m_name = name;
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
    if(visible){
        obj()->setProperty("highlighted",visible);
    }else{
        if(!m_batchSelected){
            obj()->setProperty("highlighted",visible);
        }
    }

}

void Node::hover(bool b,Body::coordinate c)
{
    Body::coordinate local = c.subtract(m_position);
    if(b){
        int divider1 = m_obj->findChild<QObject*>("nameContainer")->property("height").toInt();
        int divider2 = m_obj->findChild<QObject*>("typeNameContainer")->property("y").toInt();

        if(local.y < divider1){
            m_obj->findChild<QObject*>("textInput")->setProperty("focus",true);
        }
        if(local.y > divider1 && local.y < divider2){
            if(expandState() == 2){
                m_obj->findChild<QObject*>("expandedText")->setProperty("focus",true);
            }
            highlight(false);
        }
        if(local.y > divider2){
            m_obj->findChild<QObject*>("typeName")->setProperty("focus",true);
        }
    }
}

void Node::select(bool b,Body::coordinate c)
{

    Body::coordinate local = c.subtract(m_position);

    int divider1 = m_obj->findChild<QObject*>("nameContainer")->property("height").toInt();
    int divider2 = m_obj->findChild<QObject*>("typeNameContainer")->property("y").toInt();

    if(local.y < divider1){
        highlight(b);
        m_batchSelected = b;
    }
    if(local.y > divider1 && local.y < divider2){
        return;
    }
    if(local.y > divider2){
        highlight(b);
        m_batchSelected = b;
    }
}

void Node::select(bool b)
{
    m_batchSelected = b;
    highlight(b);

}

void Node::preventFocus(bool b)
{
    m_preventFocus = b;
}


void Node::setName(QString name)
{
    m_name = name;
    m_obj->findChild<QObject*>("textInput")->setProperty("text",name);
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

int Node::displayX()
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

int Node::displayY()
{
    int y = getY();
    return y;
}
int Node::displayWidth()
{
    int out = m_width;
    if(m_obj->findChild<QObject*>("typeNameContainer")->property("width").toInt() > m_obj->property("width").toInt()){
        out = m_obj->findChild<QObject*>("typeNameContainer")->property("width").toInt();
    }

    return out;
}

int Node::displayHeight()
{
    int out = m_height;
    if(!typeVisible()){
        out -= m_obj->findChild<QObject*>("typeName")->property("height").toInt();
    }
    return out;
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
    n->setPosition(getPosition());
    Body * body = Body::getInstance();
    body->registerGhost(n);

    return n;
}


void Node::setType(Node *n)
{
    if(n){
        if(n != m_typeNode){
            if(m_typeNode){
                m_typeNode->removeMember(this);
            }
            m_typeNode = n;
            m_type = n->getName();


            obj()->findChild<QObject*>("typeName")->setProperty("text",m_type);
            obj()->findChild<QObject*>("typeName")->setProperty("italic",true);


            n->registerMember(this);
        }
    }else{

        Body * b = Body::getInstance();

        m_typeNode = nullptr;
        m_style = b->defaultStyle();

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
        /*
        for(int i=0; i<m_ghosts.length(); i++){
            if(typeid (*b) == typeid (GhostNode)){
                BaseNode * node = b->getGhostPointer()->getOriginal()->newGhostNode();

                node->setPosition(m_ghosts[i]->getAbsolutePosition());
                //m_ghosts[i]->underMapAppendNode(node);
            }
        }*/

    }
    if(!nodes.isEmpty()){
        reFormatExpandedForm();
        updateAbsolutePosition();
    }


}

void Node::underMapAppendNode(BaseNode *node)
{
    if(m_underMap.contains(node)){
        return;
    }
    BaseNode * append = node;
    if(typeid (*node) == typeid (Note)){
        return;
    }
    if(typeid (*node) == typeid (GhostNode)){
        if(node->getGhostPointer()->getOriginal() == this){
            return;
        }
    }
    if(typeid (*node) == typeid (Node)){

        Node * n = node->getNodePointer();
        GhostNode * g = n->newGhostNode();
        append = g;
    }
    appendToUnderMap(append);
    syncGhosts(append,nullptr);


}

void Node::appendToUnderMap(BaseNode *node)
{
    m_underMap.append(node); 
    
    Body::coordinate c = node->getPosition().subtract(this->getPosition());

    node->obj()->setParentItem(this->obj()->findChild<QQuickItem*>("expandedArea"));

    node->setPosition(c);

    //node->setVisibility(false);
    node->setAbstraction(this);

    reFormatExpandedForm();

}

void Node::syncGhosts(BaseNode *node,BaseNode * caller)
{
    for(int i=0; i<m_ghosts.length(); i++){
        if(typeid (*node) == typeid (GhostNode)){
            if(m_ghosts[i]!=caller){
                BaseNode * b = node->getGhostPointer()->getOriginal()->newGhostNode();

                b->setPosition(m_ghosts[i]->getAbsolutePosition());
                if(!m_ghosts[i]->isExpanded()){

                    b->setVisibility(false);
                }

                m_ghosts[i]->appendToUnderMap(b);
            }

        }
    }
}

bool Node::underMapContains(BaseNode *b)
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

void Node::removeUnderMapFocus()
{
    for(int i=0; i<m_underMap.length(); i++){
        m_underMap[i]->removeUnderMapFocus();
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
                Body::coordinate geometry;
                geometry.x = g->getAbstraction()->width()/2;
                geometry.y = g->getAbstraction()->height()/2;
                Body::coordinate vector = g->getCenterPosition();
                clone->setPositionByCenter(getAbsolutePosition().subtract(geometry).add(vector));

                mySubMap.append(clone);

            }
        }
        setUnderMap(mySubMap);
    }
}



void Node::expand()
{
    if(m_expandState == -1){
        return;
    }
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
    for(int i=0; i<m_underMap.length(); i++){
        m_underMap[i]->expand();
    }

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

void Node::expandTree()
{

}

void Node::expandImage()
{

}

void Node::expandText()
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

bool Node::clickShouldSelect()
{
    if(m_obj->findChild<QObject*>("expandedText")->property("focus").toBool()){
        return false;
    }
    return true;
}

void Node::cycleExpandState(int state)
{
    setExpandState(state);
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
        if(m_expandState == 2){
            m_obj->findChild<QObject*>("expandedTextBox")->setProperty("visible",false);
        }
        reFormatExpandedForm();
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
        m_abstraction->removeSubNode(this);
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
        obj()->findChild<QObject*>("typeName")->setProperty("visible",true);
    }
}

void Node::moving(bool b)
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

void Node::initializeObj()
{
    Body * body = Body::getInstance();
    QQuickItem * object;

    /*
    QNetworkAccessManager * manager = new QNetworkAccessManager();
    QObject::connect(manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(requestFinished(QNetworkReply*)));

    QUrl url("http://127.0.0.1:5000/user/data");
    QNetworkRequest request(url);
    manager->get(request);*/
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
    m_obj->setProperty("opacity",0.7);
    widthChanged();
    heightChanged();
}



BaseNode * Node::isInside(Body::coordinate c)
{

    int x = c.x;
    int y = c.y;
    int width = m_width;
    int height = m_height;

    if(x>m_position.x && x<m_position.x + width && y > m_position.y && y < m_position.y + height){
        m_hoverSelected = true;
        if(preventingFocus()){
            return nullptr;
        }
        if(isExpanded() && expandState() == 0){
            BaseNode * moving = nullptr;
            for(int i=0; i<m_underMap.length(); i++){
                BaseNode * b = m_underMap[i]->isInside(c.subtract(m_position));
                if(b){
                    if(b->isMoving()){
                        moving = b;
                    }
                    else{
                        return b;
                    }
                }
            }
            if(moving){
                return moving;
            }
        }

        /*
        QObject * rect = m_obj->findChild<QObject*>("expandedTextBox");
        if(rect->property("visible").toBool()){
            y-=m_position.y;
            if(y > rect->property("y").toInt() && y < rect->property("height").toInt() + rect->property("y").toInt()){
                response.action = Body::responseAction:: select_expandedText;
                selectTextBox();
                return nullptr;
            }
        }
        */

        hover(true,c);
        return this;
    }else{
        removeUnderMapFocus();
        preventFocus(false);
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



void Node::widthChanged()
{
    m_width = m_obj->property("width").toInt();

}
void Node::heightChanged()
{
    if(m_obj->findChild<QObject*>("typeNameContainer")->property("width").toInt() == 0){
        m_height = m_obj->property("height").toInt();
    }else{
        m_height = m_obj->property("height").toInt();
    }
}

void Node::requestFinished(QNetworkReply *reply)
{
    QByteArray data = reply->readAll();


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




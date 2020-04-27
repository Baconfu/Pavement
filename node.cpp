#include "node.h"
#include <ghostnode.h>

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

    b->setFocusWindow();
}

void Node::typeInputPassivelyAccepted(QString s)
{
    Body * b = Body::getInstance();
    setType(b->getNodeByName(s));


}

void Node::highlight(bool visible)
{


    obj()->setProperty("highlighted",visible);

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

void Node::setPosition(Body::coordinate c)
{
    m_position = c;
    m_obj->setProperty("x",c.x);
    m_obj->setProperty("y",c.y);

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
    refreshWidthHeight();
    getPosition();
    c.x = m_position.x + m_width/2;
    c.y = m_position.y + m_height/2;
    return c;
}

int Node::addParent(Node *n)
{
    if(!m_parents.contains(n)){
        m_parents.append(n);
    }
    return 0;
}


void Node::removeParent(Node * n)
{
    m_parents.removeAt(m_parents.indexOf(n));
}

QVector<Node *> Node::ancestorPath(Node *target)
{
    QVector<Node*> null;
    if(this == target){
        QVector<Node*> path;
        path.insert(0,this);
        return path;
    }
    QVector<Node*> parents = getParents();
    if(parents.length()==0){
        return null;
    }
    for(int i=0; i<parents.length(); i++){
        QVector<Node*> path;
        path = parents[i]->ancestorPath(target);
        if(!path.isEmpty()){
            path.insert(0,this);
            return path;
        }
    }

    return null;
}

int Node::addChild(Node *n)
{
    if(!m_children.contains(n)){
        m_children.append(n);
    }
    return 0;
}

void Node::removeChild(Node *n)
{
    m_children.removeAt(m_children.indexOf(n));

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
        if(m_typeNode){
            n->removeMember(m_typeNode);
        }
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


}

void Node::setAbstraction(BaseNode *n)
{
    m_abstraction = n;

}

void Node::expand()
{
    if(!m_expanded){

        m_expanded = true;
        for(int i=0; i<m_underMap.length(); i++){
            m_underMap[i]->setVisibility(true);
        }
    }
}

void Node::abstract()
{

    if(m_expanded){

        m_expanded = false;
        for(int i=0; i<m_underMap.length(); i++){
            m_underMap[i]->setVisibility(false);

        }

    }
}

void Node::deleteObj()
{
    m_obj->deleteLater();
    m_obj = nullptr;
}

void Node::setVisibility(bool visibility)
{
    m_visible = visibility;
    obj()->setProperty("visible",visibility);

}



void Node::setHidden(bool b)
{
    m_hidden = b;
    QVector<Relation*> relations = getAllRelations();
    for(int i=0; i<relations.length(); i++){
        relations[i]->setVisibility(not(b));
    }
    QVector<structural*> structurals = getAllStructurals();
    for(int i=0; i<structurals.length(); i++){
        structurals[i]->setVisibility(not(b));
    }
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

QVector<Relation*> Node::getAllRelations()
{
    QVector<Relation*> v;
    v+=toNode;
    v+=toRelation;
    v+=fromNode;
    return v;
}

void Node::registerRelation(Relation *r)
{
    if(r->getDestinationType() == Relation::node){
        toNode.append(r);
        toNode_node.append(r->destinationNode());

    }
    if(r->getDestinationType() == Relation::relation){
        toRelation.append(r);
        toRelation_relation.append(r->destinationRelation());

    }

}
void Node::registerIncomingRelation(Relation *r)
{
    if(r->getOriginType() == Relation::node){
        fromNode.append(r);
        fromNode_node.append(r->originNode());
    }
    if(r->getOriginType() == Relation::relation){
        qDebug()<<"error: invalid relation";
    }
}

structural * Node::newStructural()
{

    structural * s =  new structural;
    s->initializeObj();
    s->setChildNode(this);
    s->setDisplayChildNode(this);

    connect(this,SIGNAL(updateStructural()),s,SLOT(update()),Qt::UniqueConnection);
    toParent.append(s);
    return s;
}

void Node::updateRelations()
{
    QVector<Relation*> v = getAllRelations();
    for(int i=0; i<v.length(); i++){
        v[i]->updateSelf();
    }
    updateRelation();
    updateStructural();
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

void Node::initializeObj()
{
    Body * body = Body::getInstance();
    QQuickItem * object;

    QQmlComponent component(body->engine(),QUrl("qrc:/node.qml"));
    object = qobject_cast<QQuickItem*>(component.create());
    QQuickItem * item = body->getRoot();
    object->setParentItem(item->findChild<QQuickItem*>("layer"));
    object->setParent(body->engine());
    connect(object,SIGNAL(widthChanged()),this,SLOT(getWidthFromObj()));
    connect(object,SIGNAL(heightChanged()),this,SLOT(getHeightFromObj()));
    QObject * textObj = object->findChild<QObject*>("textInput");
    connect(textObj,SIGNAL(accepted()),this,SLOT(inputAccepted()));
    connect(object,SIGNAL(passivelyAccepted()),this,SLOT(inputPassivelyAccepted()));
    connect(object,SIGNAL(typeAccepted(QString)),this,SLOT(typeInputAccepted(QString)));
    connect(object,SIGNAL(typePassivelyAccepted(QString)),this,SLOT(typeInputPassivelyAccepted(QString)));
    connect(object,SIGNAL(update()),this,SLOT(updateRelations()));
    m_obj = object;
}



bool Node::isInside(int x, int y)
{
    if(x>m_position.x && x<m_position.x + width() && y > m_position.y && y < m_position.y + height()){
        return true;
    }else{
        return false;
    }
}


void Node::hoverSelect(int y)
{
    int divider = obj()->findChild<QObject*>("typeNameContainer")->property("y").toInt();
    int localY = y - m_position.y;
    if(localY<=divider){
        giveInputFocus();
    }else{
        giveTypeInputFocus();
    }
}




#include "node.h"

Node::Node(QObject * parent):
    QObject (parent)
{

}

void Node::inputAccepted()
{

    preventFocus(true);
    Body * b = Body::getInstance();
    b->setFocusWindow();
}

void Node::typeInputAccepted(QString s)
{
    Body * b = Body::getInstance();
    setType(b->getNodeByName(s));
    qDebug()<<b->getNodeByName(s);
    b->setFocusWindow();
}

void Node::highlight(bool visible)
{
    obj()->setProperty("highlighted",visible);
    if(visible){
        obj()->setProperty("z",-4);
    }else{
        obj()->setProperty("z",0);
    }
}

void Node::preventFocus(bool b)
{
    m_preventFocus = b;
}



void Node::setID(int id)
{
    m_id = id;
    obj()->setProperty("myID",id);
}

void Node::setName(QString name)
{
    m_name = name;
    obj()->setProperty("name",name);
}

int Node::addParent(Node *n)
{
    if(parentExists(n)){
        return 1;
    }
    m_parents.append(n);
    n->addChild(this);
    structural * s = newStructural();
    s->setParentNode(n);
    s->update();
    return 0;
}


void Node::removeParent(Node * n)
{
    n->removeChild(this);
    for(int i=0; i<getParents().length(); i++){
        if(getParents()[i] == n){
            m_parents.removeAt(i);
            break;
        }
    }
}

bool Node::parentExists(Node *n)
{
    for(int i=0; i<getParents().length(); i++){
        if(n->ID() == getParents()[i]->ID()){
            return true;
        }
    }
    return false;
}

int Node::addChild(Node *n)
{
    if(childExists(n)){
        return 1;
    }
    m_children.append(n);
    n->addParent(this);
    structural * s = n->newStructural();
    s->setParentNode(this);
    s->update();
    return 0;
}

void Node::removeChild(Node *n)
{
    for(int i=0; i<m_children.length(); i++){
        if(n == m_children[i]){
            m_children.removeAt(i);
        }
    }
}

bool Node::childExists(Node *n)
{
    for(int i=0; i<getChildren().length(); i++){
        if(n->ID() == getChildren()[i]->ID()){
            return true;
        }
    }
    return false;
}

void Node::setType(Node *n)
{
    if(n){
        m_typeNode = n;
        m_type = n->name();
        m_style = n->getStyle();
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
        if(n->ID() == members()[i]->ID()){
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

void Node::deleteObj()
{
    m_obj->deleteLater();
    m_obj = nullptr;
}

void Node::setVisibility(bool visibility)
{
    m_visible = visibility;
    obj()->setProperty("visible",visibility);
    QVector<Relation*> relations = getAllRelations();
    for(int i=0; i<relations.length(); i++){
        relations[i]->setVisibility(visibility);
    }

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
    if(r->destinationObjectType() == "node"){
        toNode.append(r);
        toNode_node.append(r->destinationNode());

    }
    if(r->destinationObjectType() == "relation"){
        toRelation.append(r);
        toRelation_relation.append(r->destinationRelation());

    }

}
void Node::registerIncomingRelation(Relation *r)
{
    if(r->originObjectType() == "node"){
        fromNode.append(r);
        fromNode_node.append(r->originNode());
    }
    if(r->originObjectType() == "relation"){
        qDebug()<<"error: invalid relation";
    }
}

structural * Node::newStructural()
{
    structural * s =  new structural;
    s->initializeObj();
    s->setChildNode(this);
    toParent.append(s);
    return s;
}

void Node::updateRelations()
{
    QVector<Relation*> v = getAllRelations();
    for(int i=0; i<v.length(); i++){
        v[i]->updateSelf();
    }
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
    connect(object,SIGNAL(typeAccepted(QString)),this,SLOT(typeInputAccepted(QString)));
    m_obj = object;
}

void Node::createObj()
{


    setAbsX(m_absX);
    setAbsY(m_absY);
    setID(m_id);
    setName(m_name);
    setType(m_typeNode);




}

bool Node::isInside(int x, int y)
{
    if(x>absX() && x<absX() + width() && y > absY() && y < absY() + height()){
        return true;
    }else{
        return false;
    }
}

void Node::hoverSelect(int y)
{
    int divider = obj()->findChild<QObject*>("typeNameContainer")->property("y").toInt();
    int localY = y - absY();
    if(localY<=divider){
        giveInputFocus();
    }else{
        giveTypeInputFocus();
    }
}




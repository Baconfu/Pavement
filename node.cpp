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
    if(getType()){
        obj()->findChild<QObject*>("typeName")->setProperty("italic",true);

    }else{
        obj()->findChild<QObject*>("typeName")->setProperty("italic",false);
    }
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

void Node::setParent(Node *n)
{
    m_parent = n;
    if(n){
        n->registerChild(this);
    }
}

void Node::unParent()
{
    getParent()->unRegisterChild(this);
    m_parent = nullptr;
}

void Node::registerChild(Node *n)
{
    m_children.append(n);
}

void Node::unRegisterChild(Node *n)
{
    m_children.removeAt(findChildLocalIDByObject(n));
}

void Node::setType(Node *n)
{
    if(n){
        m_typeNode = n;
        m_type = n->name();
        m_style = n->getStyle();
        obj()->setProperty("type",m_type);
    }else{
        Body * b = Body::getInstance();
        m_typeNode = nullptr;
        m_type = "";
        m_style = b->defaultStyle();
        obj()->setProperty("type",m_type);
    }

}

Node *Node::findChildByObject(Node *n)
{
    Node * child = nullptr;
    for(int i=0; i<getChildren().length(); i++){
        if(n == getChildren()[i]){
            child = getChildren()[i];
        }
    }
    return child;
}

int Node::findChildLocalIDByObject(Node *n)
{
    for(int i=0; i<getChildren().length(); i++){
        if(n == getChildren()[i]){
            return i;
        }
    }
    return 0;
}

void Node::deleteObj()
{
    m_obj->deleteLater();
    m_obj = nullptr;
}

void Node::initializeStructural()
{
    Body * body = Body::getInstance();
    QQuickItem * object;

    QQmlComponent component(body->engine(),QUrl("qrc:/structural.qml"));
    object = qobject_cast<QQuickItem*>(component.create());
    QQuickItem * item = body->getRoot();
    object->setParentItem(item);
    object->setParent(body->engine());
    m_structural = object;
}

void Node::updateStructural()
{


    if(hoveringStructural()){
        Body * b = Body::getInstance();
        setStructuralOrigin(b->mousePosition());
        setStructuralDestination(centerPosition());
        setStructuralCutoff();
    }else{
        setStructuralOrigin(getParent()->centerPosition());
        setStructuralDestination(centerPosition());
        setStructuralCutoff();
    }
}

void Node::setStructuralOrigin(Body::coordinate c)
{
    getStructural()->setX(c.x);
    getStructural()->setY(c.y);
    m_structuralOrigin = c;
}

void Node::setStructuralDestination(Body::coordinate c)
{
    m_structuralDestination = c;
    c = c.subtract(structuralOrigin());
    m_structural->setProperty("width",c.x);
    m_structural->setProperty("height",c.y);


}

void Node::setStructuralCutoff()
{
    Body::coordinate Vector = structuralOrigin().subtract(structuralDestination());
    double radius;
    if(width() > height()){
        radius = width();
    }else{
        radius = height();
    }
    radius -= 13;

    double angle = Vector.getAngle();


    double intersectionX = cos(angle) * radius;
    double intersectionY = sin(angle) * radius;

    intersectionX += structuralDestination().subtract(structuralOrigin()).x;
    intersectionY += structuralDestination().subtract(structuralOrigin()).y;

    m_structural->findChild<QObject*>("line")->setProperty("p1",QPointF(intersectionX,intersectionY));

}

void Node::setHoveringStructural(bool b)
{
    m_hoveringStructural = b;
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
    object->setParentItem(item);
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




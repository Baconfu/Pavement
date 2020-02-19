#include "body.h"
#include <node.h>

using namespace std;

extern QQmlApplicationEngine * enginePtr;
extern QQuickWindow * windowPtr;

Body::Body(QObject * parent):
    QObject(parent)
{

}

Body * Body::getInstance()
{
    if(!instance){
        instance  = new Body;
    }
    return instance;
}

void Body::initialize()
{
    connect(getRoot(),SIGNAL(tabPressed()),this,SLOT(tab()));
    connect(getRoot(),SIGNAL(enterPressed()),this,SLOT(enterPressed()));

    QQmlComponent component(enginePtr,QUrl("qrc:/SearchBar.qml"));
    m_searchBar = qobject_cast<QQuickItem*>(component.create());
    m_searchBar->setParentItem(getRoot());
    m_searchBar->setParent(enginePtr);
    m_searchBar->setProperty("visible",false);

    connect(getRoot()->findChild<QObject*>("mouseArea"),SIGNAL(mouseTransform(int,int)),this,SLOT(mouseTransform(int, int)));
    connect(m_searchBar,SIGNAL(searching(QString)),this,SLOT(searching(QString)));
    connect(m_searchBar,SIGNAL(optionSelected(int)),this,SLOT(acceptedSelection(int)));


    function f;
    f.name = "exit application";
    f.alias = QStringList{"quit","exit","quit application","close","close application"};
    f.commonShorthand = "Null";
    functions.append(f);
    f.name = "new node";
    f.alias = QStringList{"new"};
    f.commonShorthand = "n";
    functions.append(f);
    f.name = "new relation";
    f.alias = QStringList{"relation"};
    f.commonShorthand = "nr";
    functions.append(f);
    f.name = "remove node";
    f.alias = QStringList{"delete node","delete","remove"};
    f.commonShorthand = "rmv";
    functions.append(f);
    f.name = "relationship mode";
    f.alias = QStringList{"rel mode"};
    f.commonShorthand = "Null";
    functions.append(f);
    f.name = "move node";
    f.alias = QStringList{"move","transform","shift node"};
    f.commonShorthand = "mv";
    functions.append(f);
    f.name = "parent";
    f.alias = QStringList{"set parent"};
    f.commonShorthand = "p";
    functions.append(f);
}


QQuickItem * Body::getRoot()
{
    return enginePtr->rootObjects()[0]->findChild<QQuickItem*>("item");
}

QQmlApplicationEngine * Body::engine(){
    return enginePtr;
}
QQuickWindow * Body::window(){
    return windowPtr;
}

void Body::setFocusWindow()
{
    getRoot()->setProperty("focus",true);
}
void Body::saveFile(QString path)
{
    ofstream myfile;
    myfile.open(path.toStdString());
    if(myfile.is_open()){
        myfile<<nodeMap.length();

        for(int i=0; i<nodeMap.length(); i++){
            Node * n = nodeMap[i];
            myfile<<n->ID()<<"\n";
            myfile<<n->name().toStdString()<<"\n";
            myfile<<n->absX()<<"\n";
            myfile<<n->absY()<<"\n";
            myfile<<n->getParent()->ID()<<"\n";
            myfile<<n->getType()->ID()<<"\n";


        }
        myfile<<relationArchive.length();

        for(int i=0; i<relationArchive.length(); i++){
            Relation * r = relationArchive[i];

            if(r->originObjectType()=="error" || r->destinationObjectType()=="error"){
                qDebug()<<"dangling relation";
                continue;
            }

            myfile<<r->ID();
            myfile<<r->originObjectType().toStdString();
            if(r->originObjectType()=="node") {myfile<<r->originNode()->ID();}
            if(r->originObjectType()=="relation") {myfile<<r->originRelation()->ID();}


            myfile<<r->destinationObjectType().toStdString();
            if(r->destinationObjectType()=="node") {myfile<<r->destinationNode()->ID();}
            if(r->destinationObjectType()=="relation") {myfile<<r->destinationRelation()->ID();}

        }
    }
}

void Body::openFile(QString path)
{
    ifstream myfile;
    myfile.open(path.toStdString());
    string str;
    if(myfile.is_open()){
        getline(myfile,str);
        int n = QString::fromStdString(str).toInt();

        for(int i=0; i<n; i++){
            Node * node = new Node;
            node->initializeObj();

            getline(myfile,str);
            node->setID(QString::fromStdString(str).toInt());

            getline(myfile,str);
            node->setName(QString::fromStdString(str));

            getline(myfile,str);
            node->setAbsX(QString::fromStdString(str).toInt());

            getline(myfile,str);
            node->setAbsY(QString::fromStdString(str).toInt());

            getline(myfile,str);


        }
    }
}

Node *Body::getNodeByName(QString name)
{
    Node * n = nullptr;
    for(int i=0; i<nodeMap.length(); i++){
        if(name == nodeMap[i]->name()){
            n=nodeMap[i];
            break;
        }
    }
    return n;
}

void Body::tab()
{
    m_searchBar->setProperty("visible",true);
    coordinate p = mousePosition();
    m_tabPosition = p;
    m_searchBar->setProperty("x",p.x);
    m_searchBar->setProperty("y",p.y);
    m_searchBar->findChild<QObject*>("textInput")->setProperty("focus",true);

}

void Body::enterPressed()
{

    if(context() == Context::creating_relation){
        if(highlightedNode()){

            hoveringRelation()->setDestinationObject(highlightedNode());
            hoveringRelation()->finalizeSelf();
            hoveringRelation()->updateSelf();

            Relation * r = nullptr;
            setHoveringRelation(r);
        }else{
            qDebug()<<"error, no destination selected";

        }
    }
    if(context() == Context::parenting){
        if(highlightedNode()){
            for(int i=0; i<nodeMap.length(); i++){
                if(nodeMap[i]->hoveringStructural()){
                    nodeMap[i]->setParent(highlightedNode());
                    nodeMap[i]->setHoveringStructural(false);
                    nodeMap[i]->updateStructural();

                    nodeMap[i]->setHoveringStructural(false);
                }
            }
        }
    }
}

void Body::mouseTransform(int x,int y)
{
    m_mousePosition.x = x;
    m_mousePosition.y = y;

    //reset highlight node to false each mouse movement
    Node * n = nullptr;
    setHighlightedNode(n);

    if(hoveringRelation()){
        hoveringRelation()->updateSelf();
    }
    bool hoveringStructuralExists = false;
    for(int i=0; i<nodeMap.length(); i++){

        if(nodeMap[i]->hoveringStructural()){
            hoveringStructuralExists = true;
            nodeMap[i]->updateStructural();
        }
    }
    for(int i=0; i<nodeMap.length(); i++){

        if(nodeMap[i]->isInside(x,y)){




            if(hoveringRelation() || hoveringStructuralExists){
                setHighlightedNode(nodeMap[i]);
            }else{
                nodeMap[i]->hoverSelect(y);

                setSelected(nodeMap[i]);
            }
        }else{
            nodeMap[i]->preventFocus(false);
        }


    }


}

void Body::setHoveringRelation(Relation *r)
{
    m_hoveringRelation = r;
    if(r){
        r->setHovering(true);
    }

}

void Body::setHighlightedNode(Node *n)
{
    if(!n && m_highlightedNode){
        m_highlightedNode->highlight(false);
    }
    m_highlightedNode = n;
    if(n){
        n->highlight(true);

    }
}


int Body::searching(QString input)
{
    if(input == "" || input == " " || input.length()==0){
        m_searchBar->setProperty("optionCount",0);
        m_searchBar->setProperty("optionHighlighted",-1);
        m_searchBar->findChild<QObject*>("textInput")->setProperty("text","");
        for(int i=0; i<5; i++){
            QString t = "option" + QString::number(i);
            m_searchBar->findChild<QObject*>(t)->setProperty("text","");
        }
        return 0;
    }
    if(input[input.length()-1]==" "){
        int n = m_searchBar->property("optionHighlighted").toInt();
        m_searchBar->setProperty("optionHighlighted",n+1);
    }
    while(input[input.length()-1] == " "){
        input.remove(input.length()-1,1);

        m_searchBar->findChild<QObject*>("textInput")->setProperty("text",input);
    }

    for(int i=0; i<functions.length(); i++){
        functions[i].match = match(&functions[i],input);

    }

    QVector<function> temp = functions;
    displayFunctions.clear();

    while(displayFunctions.length()<5){
        function largest;
        largest.match = -1;
        int large = 0;
        for(int i=0; i<temp.length(); i++){
            if(temp[i].match > largest.match){
                largest = temp[i];
                large = i;
            }
        }
        displayFunctions.append(largest);
        temp.removeAt(large);

    }

    int n = 0;
    for(int i=0; i<displayFunctions.length(); i++){
        if(displayFunctions[i].match>2){
            QString t = "option" + QString::number(i);
            m_searchBar->findChild<QObject*>(t)->setProperty("text",displayFunctions[i].name);
            n+=1;
        }
    }
    m_searchBar->setProperty("optionCount",n);
    return 0;

}

void Body::acceptedSelection(int n)
{
    m_searchBar->setProperty("visible",false);
    qDebug()<<displayFunctions[n].name;
    QString f = displayFunctions[n].name;

    if(f=="new node"){
        int id = allocateNewID("node");
        int x = m_tabPosition.x;
        int y = m_tabPosition.y;

        newNode(id,"",x,y,nullptr,nullptr);
    }
    if(f=="new relation"){
        int id = allocateNewID("relation");
        if(context() == Context::nothing_selected){
            qDebug()<<"error: no node selected";
        }
        if(context() == Context::node_selected){
            Node * n = nullptr;
            newRelation(id,selectedNode(),n);
            setSelected(n);
            setFocusWindow();
        }
    }
    if(f == "parent"){
        if(context() == Context::nothing_selected){
            qDebug()<<"error: no node selected";
        }
        if(context() == Context::node_selected){
            Node * n = nullptr;
            setContext(Context::parenting);
            selectedNode()->initializeStructural();
            selectedNode()->setHoveringStructural(true);
            selectedNode()->updateStructural();
            setSelected(n);
            setFocusWindow();

        }
    }
}

void Body::newNode(int id, QString name,int x, int y, Node * parent, Node * typeNode)
{
    Node * n = new Node;
    n->initializeObj();
    n->setID(id);
    n->setAbsX(x);
    n->setAbsY(y);
    n->setName(name);
    n->setType(typeNode);
    n->setParent(parent);
    n->giveInputFocus();
    nodeMap.append(n);
    n=nullptr;
}
void Body::newRelation(int id, Node *origin, Node *destination)
{
    Relation * r = new Relation;
    r->initializeObj();
    r->setID(id);
    r->setOriginObject(origin);

    if(destination){
        r->setDestinationObject(destination);
        r->finalizeSelf();
    }else{
        r->clearDestinationObject();
        setHoveringRelation(r);
        setContext(Context::creating_relation);
    }
    r->createObj();
    r->updateSelf();
    relationArchive.append(r);
    qDebug()<<r->obj();
    r=nullptr;
}

int Body::allocateNewID(QString type)
{
    if(type == "node"){
        for(int i=0; i<nodeMap.length(); i++){
            if(nodeMap[i] == nullptr){
                return i;
            }
        }
    }
    if(type == "relation"){
        for(int i=0; i<relationArchive.length(); i++){
            if(relationArchive[i] == nullptr){
                return i;
            }
        }
    }
    return -1;
}

int Body::match(function * f,QString input)
{
    int match = 0;

    if(input == f->commonShorthand){
        return 100;
    }

    QStringList name = f->name.split(" ");
    for(int i=0; i<name.length(); i++){
        match+=matchString(name[i],input);

    }

    int tempMatch = 0;

    for(int i=0; i<f->alias.length(); i++){
        tempMatch = 0;
        QStringList alias = f->alias[i].split(" ");
        for(int j=0; j<alias.length(); j++){

            tempMatch+=matchString(alias[j],input);

        }
        if(tempMatch>match){
            match = tempMatch;
        }
    }

    return match;
}

int Body::matchString(QString s, QString input)
{
    int match = 0;
    QStringList splitInput = input.split("");

    int i = 0;
    while(i<splitInput.length()){
        if(splitInput[i]==""){
            splitInput.removeAt(i);
        }else{
            i++;
        }
    }


    if(splitInput[0] == s[0]){
        match+=2;
    }
    for(int i=0; i<s.length(); i++){
        if(splitInput.isEmpty()){
            break;
        }
        if(s[i] == splitInput[0]){
            match+=1;
            splitInput.pop_front();
        }
    }
    if(splitInput.isEmpty()){
        match+=2;
    }

    return match;

}


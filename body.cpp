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
    timer.setInterval(17);
    connect(&timer,SIGNAL(timeout()),this,SLOT(timeOut()));

    connect(getRoot(),SIGNAL(tabPressed()),this,SLOT(tab()));
    connect(getRoot(),SIGNAL(enterPressed()),this,SLOT(enterPressed()));

    QQmlComponent component(enginePtr,QUrl("qrc:/SearchBar.qml"));
    m_searchBar = qobject_cast<QQuickItem*>(component.create());
    m_searchBar->setParentItem(getRoot()->findChild<QQuickItem*>("layer"));
    m_searchBar->setParent(enginePtr);
    m_searchBar->setProperty("visible",false);

    connect(getRoot()->findChild<QObject*>("mouseArea"),SIGNAL(mouseTransform(int,int,int,int)),this,SLOT(mouseTransform(int,int,int,int)));
    connect(m_searchBar,SIGNAL(searching(QString)),this,SLOT(searching(QString)));
    connect(m_searchBar,SIGNAL(optionSelected(int)),this,SLOT(acceptedSelection(int)),Qt::UniqueConnection);


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
    f.name = "save";
    f.alias = QStringList{"savestate"};
    f.commonShorthand = "sv";
    functions.append(f);
    f.name = "open";
    f.alias = QStringList{"open file","load","load file"};
    f.commonShorthand = "op";
    functions.append(f);
    f.name = "save as";
    f.alias = QStringList{"save file"};
    f.commonShorthand = "sf";
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
        myfile<<nodeMap.length()<<"\n";

        for(int i=0; i<nodeMap.length(); i++){
            Node * n = nodeMap[i];
            myfile<<n->ID()<<"\n";
            myfile<<n->name().toStdString()<<"\n";
            myfile<<n->absX()<<"\n";
            myfile<<n->absY()<<"\n";
            //myfile<<n->getParent()->ID()<<"\n";    There are now multiple parents
            myfile<<n->getParents().length()<<"\n";
            for(int j=0; j<n->getParents().length(); j++){
                myfile<<n->getParents()[j]->ID()<<"\n";
            }
            myfile<<n->getChildren().length()<<"\n";
            for(int j=0; j<n->getChildren().length(); j++){
                myfile<<n->getChildren()[j]->ID()<<"\n";
            }
            if(n->getType()){
                myfile<<n->getType()->ID()<<"\n";
            }
            else{
                myfile<<-1<<"\n";
            }

            myfile<<"members:"<<n->members().length()<<"\n";
            for(int j=0; j<n->members().length(); j++){
                myfile<<n->members()[j]->ID()<<"\n";
            }



        }
        myfile<<relationArchive.length()<<"\n";

        for(int i=0; i<relationArchive.length(); i++){
            Relation * r = relationArchive[i];

            if(r->originObjectType()=="error" || r->destinationObjectType()=="error"){
                qDebug()<<"dangling relation";
                continue;
            }

            myfile<<r->ID()<<"\n";
            myfile<<r->originObjectType().toStdString()<<"\n";
            if(r->originObjectType()=="node") {myfile<<r->originNode()->ID()<<"\n";}
            if(r->originObjectType()=="relation") {myfile<<r->originRelation()->ID()<<"\n";}


            myfile<<r->destinationObjectType().toStdString()<<"\n";
            if(r->destinationObjectType()=="node") {myfile<<r->destinationNode()->ID()<<"\n";}
            if(r->destinationObjectType()=="relation") {myfile<<r->destinationRelation()->ID()<<"\n";}

        }
    }
}

void Body::openFile(QString path)
{
    ifstream myfile;
    myfile.open(path.toStdString());
    qDebug()<<"hi";
    string str;
    if(myfile.is_open()){
        QVector<Node*> nodePool;
        QVector<Relation*> relationPool;

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
            int m = QString::fromStdString(str).toInt();

            for(int j=0; j<m; j++){
                getline(myfile,str);
                Node * n = getNodePointerByID(QString::fromStdString(str).toInt(),nodePool);
                if(n){node->addParent(n);}
            }

            getline(myfile,str);
            m = QString::fromStdString(str).toInt();

            for(int j=0; j<m; j++){
                getline(myfile,str);
                Node * n = getNodePointerByID(QString::fromStdString(str).toInt(),nodePool);
                if(n){node->addChild(n);}
            }

            getline(myfile,str);
            int t = QString::fromStdString(str).toInt();
            if(t!=-1){
                Node * n = getNodePointerByID(t,nodePool);
                node->setType(n);
            }

            getline(myfile,str);
            m = QString::fromStdString(str).split(":")[1].toInt();
            //m = QString::fromStdString(str).toInt();
            for(int j=0; j<m; j++){
                getline(myfile,str);
                Node * n = getNodePointerByID(QString::fromStdString(str).toInt(),nodePool);
                node->registerMember(n);
            }

            nodePool.append(node);
        }

        getline(myfile,str);
        n = QString::fromStdString(str).toInt();

        for(int i=0; i<n; i++){
            Relation * relation = new Relation;
            relation->initializeObj();

            getline(myfile,str);
            relation->setID(QString::fromStdString(str).toInt());

            getline(myfile,str);
            QString originType = QString::fromStdString(str);

            if(originType == "node"){
                getline(myfile,str);
                Node * node = getNodePointerByID(QString::fromStdString(str).toInt(),nodePool);
                relation->setOriginObject(node);
            }
            if(originType == "relation"){
                getline(myfile,str);
                Relation * r = getRelationPointerByID(QString::fromStdString(str).toInt(),relationPool);
                relation->setOriginObject(r);
            }

            getline(myfile,str);
            QString destinationType = QString::fromStdString(str);

            if(destinationType == "node"){
                getline(myfile,str);
                Node * node = getNodePointerByID(QString::fromStdString(str).toInt(),nodePool);
                relation->setDestinationObject(node);
            }
            if(destinationType == "relation"){
                getline(myfile,str);
                Relation * r = getRelationPointerByID(QString::fromStdString(str).toInt(),relationPool);
                relation->setDestinationObject(r);
            }

            relation->finalizeSelf();
            relationPool.append(relation);
        }

        nodeMap+=nodePool;
        relationArchive+=relationPool;
    }
}

QStringList * Body::getSaves(QString path)
{
    QStringList * s = {};
    struct dirent * entry;
    DIR *dir = opendir(path.toUtf8());
    if(dir == nullptr){
        return nullptr;
    }
    while((entry = readdir(dir)) != nullptr){
        s->append(QString::fromStdString(entry->d_name));
    }
    closedir(dir);
    return s;
}


Node *Body::getNodePointerByID(int id)
{
    Node * n = nullptr;
    for(int i=0; i<nodeMap.length(); i++){
        if(id == nodeMap[i]->ID()){
            n = nodeMap[i];
            break;
        }
    }
    return n;
}
Node * Body::getNodePointerByID(int id, QVector<Node *> pool)
{
    Node * n = nullptr;
    for(int i=0; i<pool.length(); i++){
        if(id == pool[i]->ID()){
            n = pool[i];
            break;
        }
    }
    return n;
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

Relation *Body::getRelationPointerByID(int id)
{
    Relation * r = nullptr;
    for(int i=0; i<relationArchive.length(); i++){
        if(id == relationArchive[i]->ID()){
            r = relationArchive[i];
            break;
        }
    }
    return r;
}
Relation * Body::getRelationPointerByID(int id, QVector<Relation *> pool)
{
    Relation *r = nullptr;
    for(int i=0; i<pool.length(); i++){
        if(id == pool[i]->ID()){
            r = pool[i];
            break;
        }
    }
    return r;
}

Body::coordinate Body::getDisplayDimensions()
{
    coordinate c;
    c.x = getRoot()->property("width").toInt();
    c.y = getRoot()->property("height").toInt();
    return c;
}

void Body::startTimer()
{
    if(!timer.isActive()){
        timer.start();
    }

}
void Body::stopTimer()
{
    if(timer.isActive()){
        timer.stop();
    }
}

double Body::velocityMagnitude(int x, int y)
{
    double xm;
    double ym;
    coordinate c2 = getDisplayDimensions();
    coordinate pad = padding(getDisplayDimensions());

    int smallestX = 0;
    int smallestY = 0;
    if(x - pad.x < smallestX){smallestX = x - pad.x;}
    if((c2.x - pad.x) - x < smallestX){smallestX = (c2.x - pad.x) - x;}

    if(y - pad.y < smallestY){smallestY = y - pad.y;}
    if((c2.y - pad.y) - y < smallestY){smallestY = (c2.y - pad.y) - y;}

    xm = smallestX * -1;
    ym = smallestY * -1;
    xm /= double(pad.x);
    ym /= double(pad.y);

    if(xm>ym){
        return xm;
    }else{
        return ym;
    }
}

Body::coordinate Body::padding(int screenWidth, int screenHeight)
{
    coordinate c;
    c.x = screenWidth / 3;
    c.y = screenHeight / 3;
    qDebug()<<c.x<<c.y;
    return c;
}
Body::coordinate Body::padding(coordinate c)
{
    c.x = c.x / 5;
    c.y = c.y / 5;
    return c;
}

bool Body::inBounds(int x, int y)
{

    coordinate pad = padding(getDisplayDimensions());
    coordinate d = getDisplayDimensions();
    if(x < pad.x || x > (d.x - pad.x) || y < pad.y || y > (d.y - pad.y)){

        return true;
    }
    return false;
}

void Body::tab()
{
    m_searchBar->setProperty("visible",true);
    coordinate p = mousePosition();
    m_tabPosition = p;
    m_searchBar->setProperty("x",p.x);
    m_searchBar->setProperty("y",p.y);
    m_searchBar->findChild<QObject*>("textInput")->setProperty("focus",true);
    setContext(Context::tab_searching);

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
    qDebug()<<9;
    if(context() == Context::parenting){
        qDebug()<<10;
        if(highlightedNode()){
            qDebug()<<11;
            for(int i=0; i<nodeMap.length(); i++){

                QVector<structural*> s = nodeMap[i]->getAllStructurals();
                for(int j=0; j<s.length(); j++){
                    if(s[j]->hovering()){
                        qDebug()<<12;

                        nodeMap[i]->addParent(highlightedNode());
                        s[j]->setParentNode(highlightedNode());

                        s[j]->setHovering(false);
                        s[j]->update();

                        structural * s = nullptr;
                        nodeMap[i]->setHoveringStructural(s);
                    }
                }

            }
        }
    }
}

void Body::mouseTransform(int x,int y,int offsetX,int offsetY)
{

    m_mousePosition.x = x - offsetX;
    m_mousePosition.y = y - offsetY;
    if(inBounds(x,y)){
        coordinate c = getDisplayDimensions();
        m_mouseLocalPosition.x = x - c.x/2;
        m_mouseLocalPosition.y = y - c.y/2;

        double angle = m_mouseLocalPosition.getAngle();

        double magnitude = velocityMagnitude(x,y);


        m_velocity.x = int(cos(angle) * magnitude * 10);
        m_velocity.y = int(sin(angle) * magnitude * 10);
        startTimer();
    }else{
        m_velocity.x = 0;

        m_velocity.y = 0;
        stopTimer();
    }



    //reset highlight node to false each mouse movement
    Node * n = nullptr;
    setHighlightedNode(n);

    if(hoveringRelation()){
        hoveringRelation()->updateSelf();
    }
    bool hoveringStructuralExists = false;
    for(int i=0; i<nodeMap.length(); i++){
        QVector<structural*> s = nodeMap[i]->getAllStructurals();
        for(int j=0; j<s.length(); j++){
            if(s[j]->hovering()){
                hoveringStructuralExists = true;
                s[j]->update();
            }
        }
    }
    for(int i=0; i<nodeMap.length(); i++){

        if(nodeMap[i]->isInside(mousePosition().x,mousePosition().y)){

            if(hoveringRelation() || hoveringStructuralExists){
                setHighlightedNode(nodeMap[i]);
            }else{
                nodeMap[i]->hoverSelect(mousePosition().y);

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

int Body::acceptedSelection(int n)
{
    if(context() == Context::node_tab_searching){
        setContext(Context::node_selected);
    }
    setContext(Context::all);

    m_searchBar->setProperty("visible",false);
    if(n == -1){
        return 0;
    }
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
        if(selectedNode()){
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

        if(selectedNode()){
            Node * n = nullptr;
            setContext(Context::parenting);

            selectedNode()->setHoveringStructural(selectedNode()->newStructural());
            selectedNode()->hoveringStructural()->setHovering(true);
            selectedNode()->hoveringStructural()->update();
            setSelected(n);
            setFocusWindow();

        }
    }
    if(f == "save"){
        saveFile("/home/chuan/qt_projects/Pavement_1_1/saves/data.txt");
    }
    if(f == "open"){

        openFile("/home/chuan/qt_projects/Pavement_1_1/saves/data.txt");
    }
    if(f == "save as"){

    }
    return 0;
}

void Body::timeOut()
{
    if(context() != Context::tab_searching){
        pan(m_velocity.x,m_velocity.y);
    }

}

void Body::pan(int x, int y)
{
    QQuickItem * item = getRoot()->findChild<QQuickItem*>("layer");
    item->setProperty("x",item->property("x").toInt() - x);
    item->setProperty("y",item->property("y").toInt() - y);
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
        return nodeMap.length();
    }
    if(type == "relation"){
        for(int i=0; i<relationArchive.length(); i++){
            if(relationArchive[i] == nullptr){
                return i;
            }
        }
        return relationArchive.length();
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


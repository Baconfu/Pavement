#include "body.h"
#include <node.h>
#include <structural.h>
#include <pavementfile.h>
#include <ghostnode.h>

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
    connect(getRoot()->findChild<QObject*>("mouseArea"),SIGNAL(mouseClicked(int,int)),this,SLOT(mouseClicked(int,int)));
    connect(getRoot()->findChild<QObject*>("mouseArea"),SIGNAL(mouseDoubleClicked(int,int)),this,SLOT(mouseDoubleClicked(int,int)));


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
    f.name = "frame";
    f.alias = QStringList{"frame view","reset view","frame camera"};
    f.commonShorthand = "f";
    functions.append(f);
    f.name = "dissolve";
    f.alias = QStringList{"dissolve node"};
    f.commonShorthand = "d";
    functions.append(f);
    f.name = "include";
    f.alias = QStringList{"add include","include node"};
    f.commonShorthand = "inc";
    functions.append(f);
    f.name = "debug";
    f.alias = QStringList{};
    f.commonShorthand = "NULL";
    functions.append(f);
    f.name = "break";
    f.alias = QStringList{};
    f.commonShorthand = "NULL";
    functions.append(f);
    f.name = "expand";
    f.alias = QStringList{"expand node"};
    f.commonShorthand = "NULL";
    f.contexts = {node_selected};
    functions.append(f);

    f.name = "toggle batch select";
    f.alias = QStringList{"toggle batch select"};
    f.commonShorthand = "NULL";
    functions.append(f);

    f.name = "select all children";
    f.alias = QStringList{"select children"};
    f.commonShorthand = "NULL";
    f.contexts = {node_selected};
    functions.append(f);

    f.name = "clear batch selection";
    f.alias = QStringList{"clear selection","clear","empty batch"};
    f.commonShorthand = "NULL";
    f.contexts = {batch_selecting};
    functions.append(f);

    f.name = "deselect batch";
    f.alias = QStringList{"deselect all","deselect"};
    f.commonShorthand = "NULL";
    functions.append(f);

    f.name = "select";
    f.alias = QStringList{"select node","add","append","batch select"};
    f.commonShorthand = "s";
    f.contexts = {node_selected};
    functions.append(f);

    f.name = "deselect";
    f.alias = QStringList{"deselect node"};
    f.commonShorthand = "ds";
    f.contexts = {node_selected};
    functions.append(f);

    f.name = "ghost node";
    f.alias = QStringList{"ghost"};
    f.commonShorthand = "gh";
    f.contexts = {node_selected};
    functions.append(f);

    f.name = "create ghost";
    f.alias = QStringList{"create ghost node","new ghost node"};
    f.commonShorthand = "g";
    functions.append(f);

    f.name = "abstract";
    f.alias = QStringList{};
    f.commonShorthand = "ab";
    functions.append(f);
}


int Body::acceptedSelection(int n)
{

    if(latestContext() == opening_file){

        QString g;

        g = displayFunctions[n].name;
        openFile(defaultPath + "/" + g);
        contextResolved();

        setFocusWindow();
        m_searchBar->setProperty("visible",false);
        return 0;
    }
    if(latestContext() == saving_file){
        QString g;
        if(n == -1){
            g = m_searchBar->findChild<QObject*>("textInput")->property("text").toString();
        }else{
            g = displayFunctions[n].name;
        }

        saveFile(defaultPath + "/" + g);
        contextResolved();
        setFocusWindow();
        m_searchBar->setProperty("visible",false);
        return 0;
    }
    if(latestContext() == node_browsing){
        QString s = displayFunctions[n].name;
        Node * n = getNodeByName(s);
        newGhostNode(n,tabPosition().x,tabPosition().y);
    }


    m_searchBar->setProperty("visible",false);
    if(n == -1){
        setFocusWindow();
        contextResolved();
        return 0;
    }

    QString f = "";
    QVector<int> contexts;

    if(latestContext() != opening_file && latestContext() != saving_file){
        f = displayFunctions[n].name;
         contexts = displayFunctions[n].contexts;

    }

    contextResolved();


    if(f == "frame"){
        frameView();

    }


    if(f=="new node"){
        int id = allocateNewID("node");
        int x = m_tabPosition.x;
        int y = m_tabPosition.y;

        newNode(id,"",x,y,nullptr,nullptr);
    }
    if(f=="new relation"){
        int id = allocateNewID("relation");
        if(latestContext() == Context::node_selected){
            Node * n = nullptr;
            qDebug()<<selectedNode();
            qDebug()<<context();
            newRelation(id,selectedNode(),n);
            setSelected(n);
            setFocusWindow();

        }
        else{
            qDebug()<<"error: no node selected";
        }

    }
    if(f == "parent"){
        if(latestContext() == Context::node_selected){
            Node * n = nullptr;
            setContext(Context::parenting);
            int id = selectedNode()->getID();
            Node * selected = getNodePointerByID(id);
            selected->setHoveringStructural(selected->newStructural());
            selected->hoveringStructural()->setHovering(true);
            connect(this,SIGNAL(mouseMoved()),selected->hoveringStructural(),SLOT(update()));
            selected->hoveringStructural()->update();
            setSelected(n);
            setFocusWindow();
            updateStructuralMap();

        }
        else{
            qDebug()<<"error: no node selected";
        }
    }
    if(f == "include"){
        if(latestContext() == node_selected){
            setContext(including);


        }
    }
    if(f == "dissolve"){
        if(latestContext() == node_selected){
            selectedNode()->dissolve();
        }
    }

    if(f == "break"){
        if(latestContext() == Context::relation_selected){

        }
    }

    if(f == "save"){
        saveFile("/home/chuan/qt_projects/Pavement_1_1/saves/data.txt");
    }
    if(f == "open"){
        //qDebug()<<10;
        autoTab(opening_file);
    }
    if(f == "save as"){
        autoTab(saving_file);
    }
    if(f == "debug"){
        //getRoot()->findChild<QObject*>("line"+QString::number(debugTally))->setProperty("lineWidth",2);
        //getRoot()->findChild<QObject*>("debug")->setProperty("focus",true);


    }
    if(f == "select"){
        if(contexts.contains(latestContext())){
            if(selectedNode()){
                batchSelect(selectedNode());

            }
            contextReset();
            setContext(batch_selecting);

        }
    }
    if(f == "select all children"){
        if(contexts.contains(latestContext())){
            if(selectedNode()->derivedType() == "node"){
                QVector<Node*> selection;
                Node * n = getNodeFromBase(selectedNode());
                n->getDescendants(&selection);
                //batchSelect(selection);
                setContext(batch_selecting);
                contextReset();


                //Needs fix:
                //select all children is targetted at nodes only, not ghost nodes. Get descendants only accepts QVector<Node*> while
                //batchselect only accepts QVector<BaseNode*>

            }


        }
    }
    if(f == "clear batch selection"){
        if(contexts.contains(latestContext())){
            batchDeselect();
            contextReset();
        }
        batchDeselect();

    }
    if(f == "deselect batch"){
        batchDeselect();
    }
    if(f == "toggle batch select"){

    }

    if(f == "deselect"){
        if(contexts.contains(latestContext())){
            batchDeselect(selectedNode());
            contextReset();
        }
    }
    if(f == "ghost node"){
        if(contexts.contains(latestContext())){

            BaseNode * selected = selectedNode();
            if(typeid (*selected) == typeid (Node)){

                Node * node = selectedNode()->getNodePointer();
                newGhostNode(node,mousePosition().x,mousePosition().y);
            }
        }
    }
    if(f == "create ghost"){
        autoTab(node_browsing);

    }

    if(f == "abstract"){


        if(context().contains(batch_selecting)){

            abstract(batchSelected());
            batchDeselect();
            contextReset();
        }else{

            BaseNode * b = selectedNode();
            if(typeid (*b) == typeid (GhostNode)){
                GhostNode * g = b->getGhostPointer();
                if(g->getAbstraction()){
                    if(typeid (*g) == typeid (GhostNode)){

                    }
                    if(typeid (*g) == typeid (Node)){
                        g->getAbstraction()->getNodePointer()->abstract();
                    }

                }
            }
            if(typeid (*b) == typeid (Node)){

                Node * n = b->getNodePointer();
                n->expand();
                n->abstract();
            }
        }

    }
    if(f == "expand"){

        if(contexts.contains(latestContext())){

            BaseNode * b = selectedNode();
            if(typeid (*b) == typeid (Node)){

                b->getNodePointer()->expand();
            }
        }
    }

    return 0;
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

void Body::frameView()
{
    int sumX = 0;
    int sumY = 0;
    int n =  nodeMap.length();
    for(int i=0; i<n; i++){
        sumX += nodeMap[i]->getPosition().x;
        sumY += nodeMap[i]->getPosition().y;

    }

    double x = double(sumX) / double(n);
    double y = double(sumY) / double(n);

    getRoot()->findChild<QObject*>("layer")->setProperty("x",-1 * x + getRoot()->property("width").toReal() / 2);

    getRoot()->findChild<QObject*>("layer")->setProperty("y",-1 * y + getRoot()->property("height").toReal() / 2);

}
void Body::saveFile(QString path)
{
    PavementFile file = PavementFile(path+".json");
    for(int i=0; i<nodeMap.length(); i++){
        file.saveNode(nodeMap[i]);
    }
    for(int i=0; i<relationArchive.length(); i++){
        file.saveRelation(relationArchive[i]);
    }
    for(int i=0; i<ghostNodeMap.length(); i++){
        if(!ghostNodeMap[i]->getAbstraction()){
            file.saveGhost(ghostNodeMap[i]);
        }
    }
    file.writeJson();
    /*
    ofstream myfile;
    myfile.open(path.toStdString());
    if(myfile.is_open()){
        myfile<<nodeMap.length()<<"\n";
        for(int i=0; i<nodeMap.length(); i++){
            file.saveNode(nodeMap[i]);
            Node * n = nodeMap[i];
            myfile<<n->getID()<<"\n";
            myfile<<n->getName().toStdString()<<"\n";
            myfile<<n->getPosition().x<<"\n";
            myfile<<n->getPosition().y<<"\n";
            //myfile<<n->getParent()->ID()<<"\n";    There are now multiple parents
            myfile<<n->getParents().length()<<"\n";
            for(int j=0; j<n->getParents().length(); j++){
                myfile<<n->getParents()[j]->getID()<<"\n";
            }
            myfile<<n->getChildren().length()<<"\n";
            for(int j=0; j<n->getChildren().length(); j++){
                myfile<<n->getChildren()[j]->getID()<<"\n";
            }
            if(n->getType()){
                myfile<<n->getType()->getID()<<"\n";
            }
            else{
                myfile<<-1<<"\n";
            }

            myfile<<"members:"<<n->members().length()<<"\n";
            for(int j=0; j<n->members().length(); j++){
                myfile<<n->members()[j]->getID()<<"\n";
            }



        }

        myfile<<relationArchive.length()<<"\n";

        for(int i=0; i<relationArchive.length(); i++){
            Relation * r = relationArchive[i];

            file.saveRelation(r);


            myfile<<r->ID()<<"\n";
            string s;
            if(r->getOriginType() == 0){
                s = "node";
            }
            if(r->getOriginType() == 1){
                s= "relation";
            }
            if(r->getOriginType() == 2){
                s = "ghost";
            }
            myfile<<s<<"\n";

            if(r->getOriginType() == Relation::node) {myfile<<r->originNode()->getID()<<"\n";}
            if(r->getOriginType() == Relation::relation) {myfile<<r->originRelation()->ID()<<"\n";}

            if(r->getDestinationType() == 0){
                s = "node";
            }
            if(r->getDestinationType() == 1){
                s= "relation";
            }
            if(r->getDestinationType() == 2){
                s = "ghost";
            }
            myfile<<s<<"\n";
            if(r->getDestinationType() == Relation::node) {myfile<<r->destinationNode()->getID()<<"\n";}
            if(r->getDestinationType() == Relation::relation) {myfile<<r->destinationRelation()->ID()<<"\n";}

        }
        file.writeJson();

    }*/
}

void Body::openFile(QString path)
{
    QString alt = path;

    PavementFile file = PavementFile(path);
    file.readJson();

    QVector<Node*> nodePool = file.loadNodes();

    nodeMap.append(nodePool);

    ghostNodeMap.append(file.loadGhosts());

    ghostNodeMap.append(file.getGhostPool());

    relationArchive.append(file.loadRelations());
    updateStructuralMap();
/*
    ifstream myfile;
    myfile.open(path.toStdString());

    string str;
    if(myfile.is_open()){
        QVector<Node*> nodePool;
        QVector<Relation*> relationPool;

        getline(myfile,str);
        int n = QString::fromStdString(str).toInt();

        for(int i=0; i<n; i++){



            getline(myfile,str);
            int id = (QString::fromStdString(str).toInt());
            Node * node = new Node(nullptr,id);
            node->initializeObj();

            getline(myfile,str);
            node->setName(QString::fromStdString(str));

            getline(myfile,str);
            Body::coordinate pos;

            pos.x = (QString::fromStdString(str).toInt());

            getline(myfile,str);
            pos.y = (QString::fromStdString(str).toInt());

            node->setPosition(pos);

            getline(myfile,str);
            int m = QString::fromStdString(str).toInt();

            for(int j=0; j<m; j++){
                getline(myfile,str);
                Node * n = getNodePointerByID(QString::fromStdString(str).toInt(),nodePool);
                if(n){
                    structural *s = node->newStructural();
                    s->setParentNode(n);
                    s->setChildNode(node);
                    n->addChild(node);
                    node->addParent(n);
                    s->update();
                }
            }

            getline(myfile,str);
            m = QString::fromStdString(str).toInt();

            for(int j=0; j<m; j++){
                getline(myfile,str);
                Node * n = getNodePointerByID(QString::fromStdString(str).toInt(),nodePool);
                if(n){
                    structural * s = node->newStructural();
                    s->setParentNode(node);
                    s->setChildNode(n);
                    n->addParent(node);
                    node->addChild(n);
                    s->update();
                }
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
            relation->updateSelf();
            relationPool.append(relation);
        }

        nodeMap+=nodePool;
        relationArchive+=relationPool;
        updateStructuralMap();
    }*/
}

QStringList Body::getSaves(QString path)
{

    QStringList s;
    struct dirent * entry;
    DIR *dir = opendir(path.toUtf8());
    if(dir == nullptr){
        return s;
    }
    while((entry = readdir(dir)) != nullptr){
        if(entry->d_type == 8){
            s.append(QString::fromStdString(entry->d_name));
        }

    }
    closedir(dir);
    return s;
}


Node *Body::getNodePointerByID(int id)
{
    Node * n = nullptr;
    for(int i=0; i<nodeMap.length(); i++){
        if(id == nodeMap[i]->getID()){
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
        if(id == pool[i]->getID()){
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
        if(name == nodeMap[i]->getName()){
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

Node *Body::getNodeFromBase(BaseNode *b)
{
    return getNodePointerByID(b->getID());
}



void Body::abstract(QVector<BaseNode *> nodes)
{
    Node * n = newNode(allocateNewID("node"),"abstraction",mousePosition().x,mousePosition().y,nullptr,nullptr);
    n->setUnderMap(nodes);


    for(int i=0; i<nodes.length(); i++){
        nodes[i]->setAbstraction(n);
    }
    n->expand();
    n->abstract();


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
    //qDebug()<<c.x<<c.y;
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

    //qDebug()<<latestContext();
    setContext(Context::tab_searching);

}

void Body::enterPressed()
{


    if(latestContext() == Context::creating_relation){

        if(highlightedNode() && hoveringRelation()->originNode()!=highlightedNode()){


            hoveringRelation()->setDestinationObject(highlightedNode());
            hoveringRelation()->finalizeSelf();
            hoveringRelation()->updateSelf();

            Relation * r = nullptr;
            setHoveringRelation(r);
            contextResolved();
        }

    }
    if(latestContext() == Context::parenting){
        if(highlightedNode() && highlightedNode()->derivedType() == "node"){
            Node * highlightNode = getNodePointerByID(highlightedNode()->getID());
            for(int i=0; i<nodeMap.length(); i++){

                QVector<structural*> s = nodeMap[i]->getAllStructurals();
                for(int j=0; j<s.length(); j++){
                    if(s[j]->hovering()){


                        nodeMap[i]->addParent(highlightNode);
                        highlightNode->addChild(nodeMap[i]);
                        s[j]->setParentNode(highlightNode);
                        disconnect(this,SIGNAL(mouseMoved()),s[j],SLOT(update()));
                        s[j]->setHovering(false);
                        s[j]->update();

                        structural * s = nullptr;
                        nodeMap[i]->setHoveringStructural(s);
                        contextResolved();
                    }
                }

            }
        }
    }
}

void Body::mouseClicked(int x, int y)
{
    for(int i=0; i<nodeMap.length(); i++){

        if(nodeMap[i]->isInside(x,y)){

            if(hoveringRelation() || latestContext() == parenting || latestContext() == including){
                //setHighlightedNode(nodeMap[i]);

            }else{

                batchSelect(nodeMap[i]);
            }
        }
    }
    for(int i=0; i<ghostNodeMap.length(); i++){
        if(ghostNodeMap[i]->isInside(x,y)){
            if(latestContext() == creating_relation){
                //setHighlightedNode(ghostNodeMap[i]);

            }
            else{
                batchSelect(ghostNodeMap[i]);
            }

        }
    }
}

void Body::mouseDoubleClicked(int x, int y)
{
    for(int i=0; i<nodeMap.length(); i++){
        if(nodeMap[i]->isInside(x,y)){
            nodeMap[i]->abstract();
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

    mouseMoved();


    bool highlighted = false;
    if(latestContext() == tab_searching){
        //setHighlightedNode(selectedNode());
        return;
    }
    for(int i=0; i<nodeMap.length(); i++){

        if(nodeMap[i]->isInside(mousePosition().x,mousePosition().y) && !nodeMap[i]->preventingFocus()){


            if(hoveringRelation() || latestContext() == parenting || latestContext() == including){
                setHighlightedNode(nodeMap[i]);

                highlighted = true;
            }else{
                nodeMap[i]->hoverSelect(mousePosition().y);

                highlighted = true;
                setSelected(nodeMap[i]);
            }
        }else{

            nodeMap[i]->preventFocus(false);
        }

    }
    for(int i=0; i<ghostNodeMap.length(); i++){

        if(ghostNodeMap[i]->isInside(mousePosition().x,mousePosition().y) && !ghostNodeMap[i]->preventingFocus()){
            if(latestContext() == creating_relation){
                setHighlightedNode(ghostNodeMap[i]);
                highlighted = true;
            }
            else{
                setHighlightedNode(ghostNodeMap[i]);
                highlighted = true;
                setSelected(ghostNodeMap[i]);
            }

        }else{
            ghostNodeMap[i]->preventFocus(false);
        }
    }
    for(int i=0; i<relationArchive.length(); i++){
        if(relationArchive[i]->isInside(mousePosition().x,mousePosition().y)){
            relationArchive[i]->setSelected(true);
        }else{
            relationArchive[i]->setSelected(false);
        }
    }

    if(!highlighted){
        setHighlightedNode();
    }
}

void Body::setHoveringRelation(Relation *r)
{

    m_hoveringRelation = r;
    if(r){
        r->setHovering(true);
    }

}

void Body::removeNodes(QVector<BaseNode *> nodes){
    for(int i=0; i<nodes.length(); i++){
        BaseNode * n = nodes[i];
        if(typeid (*n) == typeid (GhostNode)){
            ghostNodeMap.removeOne(n->getGhostPointer());
        }
        if(typeid (*n) == typeid (Node)){
            nodeMap.removeOne(n->getNodePointer());
        }
    }
}

QVector<structural *> Body::getAllStructurals()
{
    QVector<structural*> s;
    for(int i=0; i<nodeMap.length(); i++){
        QVector<structural*> f = nodeMap[i]->getAllStructurals();
        for(int j=0; j<f.length(); j++){
            if(!s.contains(f[j])){
                s.append(f[j]);
            }
        }
    }
    return(s);
}

void Body::updateStructuralMap()
{
    structuralMap = getAllStructurals();
}

void Body::batchSelect(BaseNode *n){
    setContext(batch_selecting);
    if(m_batchSelected.contains(n)){
        m_batchSelected.removeOne(n);
        if(typeid (*n) == typeid (GhostNode)){
            n->getGhostPointer()->setSelected(false);
        }else{
            n->highlight(false);
        }

    }else{
        m_batchSelected.append(n);
        if(typeid (*n) == typeid (GhostNode)){
            n->getGhostPointer()->setSelected(true);
        }else{
            n->highlight(true);
        }
    }

}

void Body::batchSelect(QVector<BaseNode *> n){
    for(int i=0; i<n.length(); i++){
        batchSelect(n[i]);
    }
}

void Body::batchDeselect(BaseNode *n){
    if(m_batchSelected.contains(n)){
        if(typeid (*n) == typeid (GhostNode)){
            n->getGhostPointer()->setSelected(false);
        }else{
            n->highlight(false);
        }
        m_batchSelected.removeOne(n);
    }
}

void Body::setHighlightedNode(BaseNode *n)
{
    if(n){
        m_highlightedNode = n;
        if(typeid (*n) == typeid (GhostNode)){
            n->getGhostPointer()->setHover(true);
        }else{
            n->highlight(true);
        }

    }
}

void Body::setHighlightedNode()
{
    if(m_highlightedNode){
        if(typeid (*m_highlightedNode) == typeid (GhostNode)){
            m_highlightedNode->getGhostPointer()->setHover(false);
        }else{
            m_highlightedNode->highlight(false);

        m_highlightedNode = nullptr;
        }
    }
}

QVector<Body::function> Body::functionFromList(QStringList l)
{
    QVector<function> functions;
    for(int i=0; i<l.length(); i++){
        function f;
        f.name = l[i];
        f.alias = QStringList();
        f.commonShorthand = "NULL";
        functions.append(f);
    }
    return functions;
}


int Body::searching(QString input)
{
    if(input == "" || input == " " || input.length()==0){
        clearSearch();
        return 0;
    }
    if(input[input.length()-1]==" "){ // maybe make getter and setter functions for option highlighted.
        int n = m_searchBar->property("optionHighlighted").toInt();
        m_searchBar->setProperty("optionHighlighted",n+1);
    }

    input = Utility::trimString(input," ");
    m_searchBar->findChild<QObject*>("textInput")->setProperty("text",input);

    QVector<function> pool;

    pool = functions;
    if(latestContext() == opening_file){
        pool.clear();
        QStringList saves = getSaves("/home/chuan/qt_projects/Pavement_1_1/saves");
        pool = functionFromList(saves);
    }
    if(latestContext() == saving_file){
        pool.clear();
        QStringList saves = getSaves("/home/chuan/qt_projects/Pavement_1_1/saves");
        pool = functionFromList(saves);
    }
    if(latestContext() == node_browsing){
        pool.clear();
        QStringList nodes;
        for(int i=0; i<nodeMap.length(); i++){
            nodes.append(nodeMap[i]->getName());
        }
        pool = functionFromList(nodes);
    }
    if(pool.length()==0){

        return 0;
    }

    for(int i=0; i<pool.length(); i++){
        pool[i].match = match(&pool[i],input);

    }



    QVector<function> temp = pool;
    displayFunctions.clear();

    while(displayFunctions.length()<5 && temp.length()>0){
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

void Body::timeOut()
{
    if(latestContext() != Context::tab_searching){
        pan(m_velocity.x,m_velocity.y);
    }

}
void Body::pan(int x, int y)
{
    QQuickItem * item = getRoot()->findChild<QQuickItem*>("layer");
    item->setProperty("x",item->property("x").toInt() - x);
    item->setProperty("y",item->property("y").toInt() - y);
}




Node * Body::newNode(int id, QString name,int x, int y, Node * parent, Node * typeNode)
{

    Node * n = new Node(nullptr,id);
    n->initializeObj();
    coordinate c;
    c.x = x;
    c.y = y;
    n->setPosition(c);
    n->setName(name);
    n->setType(typeNode);
    n->setParent(parent);
    n->giveInputFocus();
    nodeMap.append(n);
    return n;
}
void Body::newRelation(int id, BaseNode *origin, BaseNode *destination)
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
    //qDebug()<<r->obj();
    r=nullptr;
}

GhostNode *Body::newGhostNode(Node *original,int x,int y)
{
    GhostNode * n = new GhostNode(original);
    original->registerGhost(n);
    n->setID(original->allocateGhostID());
    n->initializeObj();
    coordinate c;
    c.x = x;
    c.y = y;
    n->setPosition(c);
    n->adoptOriginal();
    ghostNodeMap.append(n);
    return n;
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

void Body::autoTab(int context)
{
    m_searchBar->setProperty("visible",true);
    coordinate p = mousePosition();
    m_tabPosition = p;
    m_searchBar->setProperty("x",p.x);
    m_searchBar->setProperty("y",p.y);
    m_searchBar->findChild<QObject*>("textInput")->setProperty("focus",true);
    setContext(context);

    if(context == 8){
        m_searchBar->setProperty("selectFirst",false);
    }else{
        m_searchBar->setProperty("selectFirst",true);
    }

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

void Body::clearSearch()
{
    m_searchBar->setProperty("optionCount",0);
    m_searchBar->setProperty("optionHighlighted",-1);
    m_searchBar->findChild<QObject*>("textInput")->setProperty("text","");
    for(int i=0; i<5; i++){
        QString t = "option" + QString::number(i);
        m_searchBar->findChild<QObject*>(t)->setProperty("text","");
    }
}




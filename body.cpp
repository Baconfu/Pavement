#include "body.h"
#include <node.h>
#include <pavementfile.h>
#include <ghostnode.h>
#include <nodearea.h>
#include <note.h>
#include <sync.h>
#include <sys/stat.h>

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

    autosaveTimer.setInterval(1000 * 60);
    connect(&autosaveTimer,SIGNAL(timeout()),this,SLOT(autosave()));
    startAutosaveTimer();

    connect(getRoot(),SIGNAL(tabPressed()),this,SLOT(tab()));
    connect(getRoot(),SIGNAL(enterPressed()),this,SLOT(enterPressed()));
    connect(getRoot(),SIGNAL(closing()),this,SLOT(closeWindow()));
    connect(getRoot(),SIGNAL(escapePressed()),this,SLOT(escapePressed()));

    QDir dir = QDir(QDir::currentPath());
    QStringList contents = dir.entryList();
    if(!contents.contains("saves")){
        dir.mkdir("saves");
    }


    QQmlComponent component(enginePtr,QUrl("qrc:/SearchBar.qml"));
    m_searchBar = qobject_cast<QQuickItem*>(component.create());
    m_searchBar->setParentItem(getRoot()->findChild<QQuickItem*>("layer"));
    m_searchBar->setParent(enginePtr);
    m_searchBar->setProperty("visible",false);

    QObject * m = getRoot()->findChild<QObject*>("mouseArea");
    connect(m,SIGNAL(mouseTransform(int,int,int,int)),this,SLOT(mouseTransform(int,int,int,int)));
    connect(m_searchBar,SIGNAL(searching(QString)),this,SLOT(searching(QString)));
    connect(m_searchBar,SIGNAL(optionSelected(int)),this,SLOT(acceptedSelection(int)),Qt::UniqueConnection);
    connect(m,SIGNAL(mouseClicked(int,int)),this,SLOT(mouseClicked(int,int)));
    connect(m,SIGNAL(mouseDoubleClicked(int,int)),this,SLOT(mouseDoubleClicked(int,int)));
    connect(m,SIGNAL(mousePressed(int,int)),this,SLOT(mousePressed(int,int)));
    connect(m,SIGNAL(mouseReleased()),this,SLOT(mouseReleased()));
    connect(m,SIGNAL(mouseHeld()),this,SLOT(mouseHeld()));
    connect(m,SIGNAL(scroll(int,int,bool)),this,SLOT(scroll(int,int,bool)));

    function f;
    f.name = "exit application";
    f.alias = QStringList{"quit","exit","quit application","close","close application"};
    f.commonShorthand = "Null";
    functions.append(f);

    f.name = "fullscreen";
    f.alias = QStringList{};
    f.commonShorthand = "full";
    functions.append(f);

    f.name = "exit fullscreen";
    f.alias = QStringList{};
    f.commonShorthand = "NULL";
    functions.append(f);

    f.name = "new node";
    f.alias = QStringList{"new"};
    f.commonShorthand = "n";
    functions.append(f);

    f.name = "new relation";
    f.alias = QStringList{"relation"};
    f.commonShorthand = "nr";
    functions.append(f);

    f.name = "new connection";
    f.alias = QStringList{"connection","new line"};
    f.commonShorthand = "nc";
    functions.append(f);



    f.name = "remove node";
    f.alias = QStringList{"delete node","delete","remove"};
    f.commonShorthand = "rmv";
    f.contexts = {node_selected};
    functions.append(f);

    f.name = "relationship mode";
    f.alias = QStringList{"rel mode"};
    f.commonShorthand = "Null";
    functions.append(f);

    f.name = "move node";
    f.alias = QStringList{"move","transform","shift node"};
    f.contexts = {node_selected};
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
    f.contexts = {relation_selected,structural_selected};
    functions.append(f);

    f.name = "expand";
    f.alias = QStringList{"expand node"};
    f.commonShorthand = "ex";
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

    f.name = "create area";
    f.alias = QStringList{"new area","area"};
    f.commonShorthand = "ar";
    functions.append(f);

    f.name = "remove from";
    f.alias = QStringList{"extract","extract from","remove"};
    f.commonShorthand = "NULL";
    functions.append(f);

    f.name = "show text";
    f.alias = QStringList{"expand text","text"};
    f.commonShorthand = "txt";
    functions.append(f);



}


int Body::acceptedSelection(int n)
{

    if(latestContext() == opening_file){

        if(n != -1){
            QString g;


            g = displayFunctions[n].name;
            if(currentFile == ""){
                QString sub = g;
                if(g.contains(".json")){

                    sub.chop(5);
                }
                currentFile = sub;
            }
            openFile(defaultPath + "/" + g);
            contextResolved();

            setFocusWindow();
            m_searchBar->setProperty("visible",false);

            return 0;
        }

    }
    if(latestContext() == saving_file){
        QString g;
        if(n == -1){
            g = m_searchBar->findChild<QObject*>("textInput")->property("text").toString() + ".json";
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
        if(n == -1){

        }else{
            QString s = displayFunctions[n].name;
            Node * n = getNodeByName(s.split("-")[0]);

            GhostNode * g = newGhostNode(n,tabPosition().x,tabPosition().y);
            if(m_selectedNode){
                if(selectedNode()->isInside(mousePosition().x,mousePosition().y)){
                    m_selectedNode->underMapAppendNode(g);
                }

            }
        }

    }


    m_searchBar->setProperty("visible",false);
    m_searchBar->setProperty("enabled",false);
    setFocusWindow();
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

    if(f == "fullscreen"){
        fullscreen(true);
    }

    if(f == "exit fullscreen"){
        fullscreen(false);
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

            newRelation(id,selectedNode(),n);
            setSelected(n);
            setFocusWindow();

        }
        else{
            qDebug()<<"error: no node selected";
        }

    }
    if(f == "new connection"){
        int id = allocateNewID("relation");
        if(latestContext() == node_selected){
            Node * n = nullptr;
            newLine(id,selectedNode(),n);
            setSelected(n);
            setFocusWindow();
        }
    }
    if(f == "parent"){
        if(latestContext() == Context::node_selected){
            BaseNode * n = nullptr;
            setContext(Context::parenting);
            int id = allocateNewID("relation");
            newTriangle(id,selectedNode(),n);
            setSelected(n);
            setFocusWindow();
        }
        else{
            qDebug()<<"error: no node selected";
        }
    }
    if(f == "remove node"){
        if(contexts.contains(latestContext())){
            selectedNode()->destroy();
        }
    }
    if(f == "remove from"){
        if(selectedNode()){
            if(selectedNode()->getAbstraction()){
                selectedNode()->getAbstraction()->exude(selectedNode());
            }
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
        if(contexts.contains(latestContext())){
            if(latestContext() == relation_selected){
                selectedRelation()->destroy();

                contextResolved();
            }

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
        Sync sync;
        sync.sync();

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
                //Node * n = getNodeFromBase(selectedNode());
                //n->getDescendants(&selection);
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
            b->expand();
            b->abstract();

        }

    }
    if(f == "expand"){


        if(contexts.contains(latestContext())){

            BaseNode * b = selectedNode();
            if(b->isExpanded()){
                b->cycleExpandState(0);
            }else{
                b->expand();
            }


        }
    }
    if(f == "show text"){
        if(selectedNode()){
            selectedNode()->cycleExpandState(2);

        }
    }

    if(f == "move node"){

        if(contexts.contains(latestContext())){
            BaseNode * b = selectedNode();
            b->moving(true);
            setContext(moving_node);
        }
    }
    if(f == "create area"){
        if(context().contains(batch_selecting)){
            newNodeArea(batchSelected());
            batchDeselect();
            contextReset();
        }else{

        }
    }

    tabAccepted = true;

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

void Body::fullscreen(bool b)
{
    if(b){
        windowPtr->showFullScreen();
    }else{
        windowPtr->showNormal();
    }
}

void Body::setFocusWindow()
{
    getRoot()->setProperty("focus",true);
}

void Body::frameView()
{


    QVector<BaseNode*> nodes;
    for(int i=0; i<nodeMap.length(); i++){
        nodes.append(nodeMap[i]);

    }

    coordinate c = averagePosition(nodes);


    getRoot()->findChild<QObject*>("layer")->setProperty("x",-1 * c.x + getRoot()->property("width").toReal() / 2);

    getRoot()->findChild<QObject*>("layer")->setProperty("y",-1 * c.y + getRoot()->property("height").toReal() / 2);

}

void Body::zoom(double deltaFactor)
{

    if(m_zoomFactor < 0.7){
        double t = (m_zoomFactor-0.4)*2 - 0.01;
        if(t<0.01){
            t=0.01;
        }
        deltaFactor*= sqrt(t);

    }
    if(m_zoomFactor>0.7){
        double t = (1-m_zoomFactor)*2 - 0.01;
        if(t<0.01){
            t=0.01;
        }
        deltaFactor*= sqrt(t);
    }

    m_zoomFactor += deltaFactor/3000;
    if(m_zoomFactor<0.4){
        m_zoomFactor = 0.4;
    }
    if(m_zoomFactor>1){
        m_zoomFactor = 1;
    }

    QObject * layer = getRoot()->findChild<QObject*>("layer");
    QObject * layerScale = getRoot()->findChild<QObject*>("layerScale");

    QPoint p;

    //p.setX(layer->property("x").toInt() * -1 + int((getRoot()->property("width").toDouble()/2)/m_zoomFactor));
    //p.setY(layer->property("y").toInt() * -1 + int((getRoot()->property("height").toDouble()/2)/m_zoomFactor));
    p.setX(int((getRoot()->property("width").toDouble()/2)) - layer->property("x").toInt());
    p.setY(int((getRoot()->property("height").toDouble()/2)) - layer->property("y").toInt());

    layerScale->setProperty("xScale",m_zoomFactor);
    layerScale->setProperty("yScale",m_zoomFactor);


}
void Body::saveFile(QString path)
{
    QVector<BaseNode *> temp = nodeMap;
    nodeMap.clear();
    for(int i=0; i<temp.length(); i++){
        if(temp[i]){
            temp[i]->setID(allocateNewID("node"));
            nodeMap.append(temp[i]);
        }

    }
    nodeMap = temp;
    PavementFile file = PavementFile(path);
    for(int i=0; i<nodeMap.length(); i++){
        if(nodeMap[i]){
            if(!nodeMap[i]->getAbstraction()){

                file.saveBaseNode(nodeMap[i]);
            }

        }

    }
    for(int i=0; i<relationArchive.length(); i++){
        file.saveRelation(relationArchive[i]);
    }

    file.writeJson();
}

void Body::openFile(QString path)
{
    QString alt = path;

    PavementFile file = PavementFile(path);
    file.readJson();


    QVector<BaseNode*> nodePool = file.loadNodes();

    for(int i=0; i<nodePool.length(); i++){
        nodeMap.append(nodePool[i]);
    }


    QVector<BaseNode*> subPool = file.loadSubNodes();
    for(int i=0; i<subPool.length(); i++){
        nodeMap.append(subPool[i]);
    }

    subPool = file.getNodePool();
    for(int i=0; i<subPool.length(); i++){
        if(!nodeMap.contains(subPool[i])){
            nodeMap.append(subPool[i]);
        }
    }

    relationArchive.append(file.loadRelations());

}

QStringList Body::getSaves(QString path)
{

    QStringList s;
    struct dirent * entry;
    DIR *dir = opendir(path.toUtf8());
    if(dir == nullptr){
        return s;
    }
    if(os() == "windows"){
        struct stat statbuf;

        while((entry = readdir(dir)) != nullptr){

            if(stat(entry->d_name,&statbuf) == 0){
                s.append(QString::fromStdString(entry->d_name));
            }

        }
    }

    if(os() == "ubuntu" ){
        while((entry = readdir(dir)) != nullptr){
            if(entry->d_type == 8){
                s.append(QString::fromStdString(entry->d_name));
            }
        }
    }
    closedir(dir);
    return s;
}

Body::coordinate Body::averagePosition(QVector<BaseNode *> nodes)
{
    double sumX = 0;
    double sumY = 0;
    for(int i=0; i<nodes.length(); i++){
        sumX += nodes[i]->getCenterAbsolutePosition().x;
        sumY += nodes[i]->getCenterAbsolutePosition().y;
    }
    coordinate c;
    c.x = int(sumX/double(nodes.length()));
    c.y = int(sumY/double(nodes.length()));
    return c;
}

Body::coordinate Body::medianPosition(QVector<BaseNode *> nodes)
{
    int leftMost = 10000000;
    int rightMost = -1000000;
    int topMost = 10000000;
    int botMost = -1000000;

    for(int i=0; i<nodes.length(); i++){
        BaseNode * b = nodes[i];


        int x = b->getPosition().x;
        if(x < leftMost){

            leftMost = x;
        }
        x = b->getPosition().x + b->width();
        if(x > rightMost){

            rightMost = x;
        }
        int y = b->getPosition().y;
        if(y < topMost){

            topMost = y;
        }
        y = b->getPosition().y + b->height();
        if(y > botMost){

            botMost = y;
        }

    }
    Body::coordinate median;
    median.x = (leftMost + rightMost) / 2;
    median.y = (topMost + botMost) / 2;
    return median;
}

QVector<Body::coordinate> Body::boundaries(QVector<BaseNode *> nodes)
{
    int leftMost = 10000000;
    int rightMost = -1000000;
    int topMost = 10000000;
    int botMost = -1000000;

    for(int i=0; i<nodes.length(); i++){
        BaseNode * b = nodes[i];
        int x = b->getPosition().x;
        if(x < leftMost){

            leftMost = x;
        }
        x = b->getPosition().x + b->width();
        if(x > rightMost){

            rightMost = x;
        }
        int y = b->getPosition().y;
        if(y < topMost){

            topMost = y;
        }
        y = b->getPosition().y + b->height();
        if(y > botMost){

            botMost = y;
        }
    }
    QVector<coordinate> boundaries;
    coordinate c;
    c.x = leftMost;
    c.y = topMost;
    coordinate c2;
    c2.x = rightMost;
    c2.y = botMost;

    boundaries[0] = c;
    boundaries[1] = c2;
    return boundaries;
}


BaseNode *Body::getNodePointerByID(int id)
{
    for(int i=0; i<nodeMap.length(); i++){
        BaseNode * b = nodeMap[i];
        if(b->getID() == id){
            return b;
        }
    }
}
BaseNode * Body::getNodePointerByID(int id, QVector<BaseNode *> pool)
{
    BaseNode * n = nullptr;
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
        if(nodeMap[i]){
            n = nodeMap[i]->getNodePointer();
            if(n){
                if(n->getName() == name){
                    return n;
                }
            }
        }

    }
    return nullptr;
}

void Body::registerGhost(GhostNode *g){
    if(!nodeMap.contains(g)){
        nodeMap.append(g);
    }
}

void Body::removeNode(Node *n)
{
    int i = nodeMap.indexOf(n);
    delete(nodeMap[i]);
    nodeMap[i] = nullptr;
}

void Body::removeGhost(GhostNode *g)
{
    int i =  nodeMap.indexOf(g);
    delete(nodeMap[i]);
    nodeMap[i] = nullptr;
}

void Body::removeRelation(Relation *r)
{
    int index = relationArchive.indexOf(r);
    delete(relationArchive[index]);
    relationArchive[index] = nullptr;
}

void Body::removeNode(BaseNode *b)
{
    if(typeid (*b) == typeid (Node)){
        removeNode(b->getNodePointer());
    }
    if(typeid (*b) == typeid (GhostNode)){
        removeGhost(b->getGhostPointer());
    }
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



void Body::abstract(QVector<BaseNode *> nodes)
{
    coordinate c = medianPosition(nodes);
    Node * n = newNode(allocateNewID("node"),"abstraction",c.x,c.y,nullptr,nullptr);



    n->setPositionByCenter(c);


    for(int i=0; i<nodes.length(); i++){
        nodes[i]->setAbstraction(n);
    }
    n->setUnderMap(nodes);
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
    m_searchBar->setProperty("enabled",true);
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

    if(tabAccepted){
        tabAccepted = false;
        return;
    }


    if(latestContext() == Context::creating_relation){

        if(highlightedNode() && hoveringRelation()->originNode()!=highlightedNode()){


            hoveringRelation()->setDestinationObject(highlightedNode());
            hoveringRelation()->finalizeSelf();
            hoveringRelation()->updateSelf();

            Relation * r = nullptr;
            setHoveringRelation(r);
            contextReset();
            setHighlightedNode();
        }

    }
    if(latestContext() == Context::parenting){

        BaseNode * b = highlightedNode();
        if(b){

            BaseNode * dest = b;
            BaseNode * origin = hoveringRelation()->originNode();
            if(dest != origin){
                origin->setAbstraction(dest);
                dest->underMapAppendNode(origin);
                hoveringRelation()->setDestinationObject(dest);
                hoveringRelation()->finalizeSelf();
                hoveringRelation()->updateSelf();
                Relation * r = nullptr;
                setHoveringRelation(r);
                contextReset();
                setHighlightedNode();

            }



        }
    }
    if(latestContext() == moving_node){

        for(int i=0; i<nodeMap.length(); i++){
            nodeMap[i]->moving(false);
        }
        contextReset();
    }

    m_searchBar->setProperty("selectFirst",true);
}

void Body::escapePressed()
{
    fullscreen(false);
}

void Body::scroll(int x, int y, bool ctrl)
{
    if(ctrl){
        //zooming in or out
        if(abs(y)>1){
            m_scaling = true;
            m_zoomVelocity = y;


            startTimer();
        }

    }else{
        //panning camera
        x*=-1;
        y*=-1;

        double xv = 0;
        double yv = 0;
        if(x>0){
            xv = log(x);

            m_velocity.x = int(xv*xv);
        }
        if(x<0){
            xv = -1 * log(abs(x));

            m_velocity.x = -1 * int(xv*xv);
        }
        if(y>0){
            yv = log(y);
            m_velocity.y = int(yv*yv);
        }
        if(y<0){
            yv = -1 * log(abs(y));
            m_velocity.y = -1 * int(yv*yv);
        }


        m_scrolling = true;
        startTimer();
    }

}



void Body::mouseClicked(int x, int y)
{

    if(selectedNode()){
        batchSelect(selectedNode());
    }
}

void Body::mouseDoubleClicked(int x, int y)
{

}

void Body::mousePressed(int x, int y)
{

    if(selectedNode()){

        selectedNode()->moving(true);
        setContext(moving_node);

        for(int i=0; i<m_batchSelected.length(); i++){
            m_batchSelected[i]->moving(true);
        }

    }
}


void Body::mouseReleased()
{

    if(latestContext() == moving_node){

        if(m_mouseHeld){

            if(highlightedNode()){

                for(int i=0; i<nodeMap.length(); i++){
                    if(nodeMap[i]){
                        if(nodeMap[i]->isMoving() && nodeMap[i] != highlightedNode()){
                            qDebug()<<nodeMap[i]<<highlightedNode();
                            highlightedNode()->underMapAppendNode(nodeMap[i]);

                        }
                    }

                }
            }
        }
        contextResolved();

    }
    for(int i=0; i<nodeMap.length(); i++){
        if(nodeMap[i]){
            nodeMap[i]->moving(false);
        }

    }
    m_mouseHeld = false;

}

void Body::mouseHeld()
{
    m_mouseHeld = true;
}

void Body::closeWindow()
{
    stopAutosaveTimer();
    stopTimer();
    for(int i=0; i<nodeMap.length(); i++){
        delete(nodeMap[i]);
    }
    for(int i=0; i<relationArchive.length(); i++){
        delete(relationArchive[i]);
    }

    delete(instance);
}


void Body::mouseTransform(int x,int y,int offsetX,int offsetY)
{

    coordinate c = getDisplayDimensions();
    m_mouseLocalPosition.x = x - c.x/2;
    m_mouseLocalPosition.y = y - c.y/2;

    int newX = int(m_mouseLocalPosition.x / m_zoomFactor);
    int newY = int(m_mouseLocalPosition.y / m_zoomFactor);
    newX = newX + c.x/2;
    newY = newY + c.y/2;

    m_mousePosition.x = int(newX - offsetX);
    m_mousePosition.y = int(newY - offsetY);

    /*
    QObject * show = getRoot()->findChild<QObject*>("show");

    show->setProperty("x",m_mousePosition.x);
    show->setProperty("y",m_mousePosition.y);

*/
    m_mouseVector = m_mousePosition.subtract(m_oldMousePosition);
    m_oldMousePosition = m_mousePosition;
    if(inBounds(x,y)){


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




    if(hoveringRelation()){

        hoveringRelation()->updateSelf();
    }

    mouseMoved();


    bool highlighted = false;
    if(latestContext() == tab_searching){
        //setHighlightedNode(selectedNode());
        return;
    }

    bool moving = false;
    for(int i=0; i<nodeMap.length(); i++){
        if(nodeMap[i]){
            if(nodeMap[i]->isMoving()){

                nodeMap[i]->transform(m_mouseVector);
                if(nodeMap[i]->getAbstraction()){
                    nodeMap[i]->getAbstraction()->subNodeMoved();
                }
                moving  = true;
            }
        }

    }


    if(latestContext() != relation_selected){
        for(int i=0; i<nodeMap.length(); i++){
            if(nodeMap[i]){
                if(nodeMap[i]->getAbstraction() == nullptr){
                    BaseNode * b = nodeMap[i]->isInside(mousePosition().x,mousePosition().y);

                    if(b){
                        if(typeid (*b) == typeid (Node)){
                            if(!b->getNodePointer()->preventingFocus()){

                                QVector<int> contexts = {parenting,including,creating_relation,moving_node};

                                if(contexts.contains(latestContext())){
                                    setHighlightedNode(b);


                                    highlighted = true;
                                }else{
                                    b->getNodePointer()->hoverSelect(mousePosition().y);

                                    highlighted = true;
                                    setSelected(b);
                                }

                            }
                        }
                        if(typeid (*b) == typeid (GhostNode)){
                            GhostNode * g = b->getGhostPointer();
                            if(!g->preventingFocus()){
                                if(g->isMoving()){
                                    continue;
                                }


                                QVector<int> contexts = {including,creating_relation,moving_node};
                                if(contexts.contains(latestContext())){
                                    setHighlightedNode(g);
                                    highlighted = true;

                                }else{

                                    highlighted = true;
                                    setSelected(g);
                                    g->hover(true);
                                }
                            }
                            else{
                                g->hover(false);
                                g->preventFocus(false);
                            }



                        }
                        if(typeid (*b) == typeid (NodeArea)){
                            NodeArea * a = b->getAreaPointer();
                            if(a->isMoving()){
                                continue;
                            }
                            QVector<int> contexts = {including,creating_relation,moving_node};
                            if(contexts.contains(latestContext())){
                                setHighlightedNode(a);

                                highlighted = true;

                            }
                            else{
                                highlighted = true;
                                setSelected(a);
                                a->hover(true);
                            }
                        }

                    }
                }
            }


        }

    }





    bool relationSelected = false;
    for(int i=0; i<relationArchive.length(); i++){
        if(relationArchive[i]){
            if(relationArchive[i]->isInside(mousePosition().x,mousePosition().y)){

                relationArchive[i]->setSelected(true);
                setSelected(relationArchive[i]);
                relationSelected = true;
            }else{
                relationArchive[i]->setSelected(false);
            }
        }

    }
    if(!relationSelected){
        Relation * r = nullptr;
        setSelected(r);
        if(latestContext() == relation_selected){
            contextResolved();
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

void Body::removeGhosts(QVector<GhostNode *> ghosts){
    for(int i=0; i<ghosts.length(); i++){
        removeGhost(ghosts[i]);
    }
}

void Body::removeNodes(QVector<BaseNode *> nodes){
    for(int i=0; i<nodes.length(); i++){
        BaseNode * n = nodes[i];
        if(typeid (*n) == typeid (GhostNode)){
            removeGhost(n->getGhostPointer());
        }
        if(typeid (*n) == typeid (Node)){
            removeNode(n->getNodePointer());
        }
    }
}



void Body::batchSelect(BaseNode *n){
    setContext(batch_selecting);

    if(m_batchSelected.contains(n)){
        m_batchSelected.removeOne(n);
        if(typeid (*n) == typeid (GhostNode)){
            n->getGhostPointer()->select(false);
            contextResolved();
        }else{
            n->select(false);
            contextResolved();
        }

    }else{
        m_batchSelected.append(n);
        if(typeid (*n) == typeid (GhostNode)){
            n->getGhostPointer()->select(true);
        }else{
            n->select(true);
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
            n->getGhostPointer()->select(false);
        }
        if(typeid (*n) == typeid (NodeArea)){
            n->select(false);
        }
        if(typeid (*n) == typeid (Node)){
            n->highlight(false);
        }
        m_batchSelected.removeOne(n);
    }
}

void Body::setSelected(BaseNode *n)
{

    m_selectedNode = n;


    if(n){
        m_selectedRelation = nullptr;

        setContext(node_selected);
    }

}

void Body::setHighlightedNode(BaseNode *n)
{
    if(n){

        m_highlightedNode = n;
        if(typeid (*n) == typeid (GhostNode)){
            n->getGhostPointer()->hover(true);
        }else{
            n->hover(true);
        }

    }
}

void Body::setHighlightedNode()
{
    if(m_highlightedNode){
        if(typeid (*m_highlightedNode) == typeid (GhostNode)){
            m_highlightedNode->getGhostPointer()->hover(false);
        }else{
            m_highlightedNode->hover(false);
        }
        m_highlightedNode = nullptr;
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


    for(int i=0; i<functions.length(); i++){
        if(functions[i].contexts.contains(latestContext())){
                pool.append(functions[i]);
        }
    }
    pool = functions;
    if(latestContext() == opening_file){
        pool.clear();
        QStringList saves = getSaves(defaultPath);

        qDebug()<<saves;
        pool = functionFromList(saves);
    }
    if(latestContext() == saving_file){
        pool.clear();
        QStringList saves = getSaves(defaultPath);
        pool = functionFromList(saves);
    }
    if(latestContext() == node_browsing){
        pool.clear();
        QStringList nodes;
        for(int i=0; i<nodeMap.length(); i++){
            BaseNode * b = nodeMap[i];
            if(typeid (*b) == typeid (Node)){
                nodes.append(nodeMap[i]->getName() + "-" + nodeMap[i]->getNodePointer()->getTypeName());
            }
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

    if(m_scrolling){
        if(m_velocity.x>1){
            m_velocity.x-=2;
        }
        if(m_velocity.x<-1){
            m_velocity.x+=2;
        }
        if(m_velocity.x <=1 && m_velocity.x >=-1){
            m_velocity.x = 0;
        }

        if(m_velocity.y>1){
            m_velocity.y -= 2;
        }
        if(m_velocity.y<-1){
            m_velocity.y += 2;
        }
        if(m_velocity.y <= 1 && m_velocity.y >= -1){
            m_velocity.y = 0;
        }
        if(m_velocity.y == 0  && m_velocity.x == 0){
            m_scrolling = false;
            stopTimer();
        }
    }
    if(m_scaling){
        zoom(double(m_zoomVelocity));
        if(m_zoomVelocity >10){
            m_zoomVelocity -= 20;
        }
        if(m_zoomVelocity <-10){
            m_zoomVelocity +=20;
        }
        if(m_zoomVelocity <= 10 && m_zoomVelocity >= -10){
            m_zoomVelocity = 0;
            m_scaling = false;
            stopTimer();
        }

    }

}
void Body::pan(int x, int y)
{
    QQuickItem * item = getRoot()->findChild<QQuickItem*>("layer");
    item->setProperty("x",item->property("x").toInt() - x);
    item->setProperty("y",item->property("y").toInt() - y);

    QObject* layerScale = getRoot()->findChild<QObject*>("layerScale");
    layerScale->setProperty("x",getRoot()->property("width").toReal()/2 - item->property("x").toInt());
    layerScale->setProperty("y",getRoot()->property("height").toReal()/2 - item->property("y").toInt());
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
    Relation * r = new Relation(nullptr,"arrow");
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

void Body::newLine(int id, BaseNode *origin, BaseNode *destination)
{
    Relation * r  = new Relation(nullptr, "line");
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

void Body::newTriangle(int id, BaseNode *origin, BaseNode *destination)
{
    Relation * r = new Relation(nullptr,"triangle");
    r->initializeObj();
    r->setID(id);
    r->setOriginObject(origin);
    if(destination){
        r->setDestinationObject(destination);
        r->finalizeSelf();
    }else{
        r->clearDestinationObject();
        setHoveringRelation(r);
        setContext(creating_relation);
    }
    r->createObj();
    r->updateSelf();
    relationArchive.append(r);
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
    nodeMap.append(n);

    return n;
}

NodeArea *Body::newNodeArea(QVector<BaseNode *> nodes)
{
    NodeArea * n = new NodeArea;
    n->initializeObj();
    n->setUnderMap(nodes);
    n->reFormatExpandedForm();
    nodeMap.append(n);
    return n;
}

void Body::autosave()
{
    qDebug()<<"lol";
    if(currentFile == ""){
        return;
    }
    sessionLength+=1;
    QString divider = "/";
    QDir dir = defaultPath;
    QStringList folder = dir.entryList();
    if(!folder.contains(currentFile + "-autosave")){
        dir.mkdir(currentFile + "-autosave");
    }
    saveFile(defaultPath + divider + currentFile + "-autosave" + divider + "1-minute-ago.json");
    if(sessionLength % 5 == 0){
        saveFile(defaultPath + divider + currentFile + "-autosave" + divider + "5-minutes-ago.json");
    }
    if(sessionLength % 10 == 0){
        saveFile(defaultPath + divider + currentFile + "-autosave" + divider + "10-minutes-ago.json");
    }
    if(sessionLength % 20 == 0){
        saveFile(defaultPath + divider + "saves" + divider + currentFile + "-autosave" + divider + "autosave-"
                 + QString::number(sessionLength%20));
    }

}

int Body::allocateNewID(QString type)
{
    if(type == "node"){
        for(int i=0; i<nodeMap.length(); i++){
            if(nodeMap[i] == nullptr){
                qDebug()<<"id allocated: "<<i;
                return i;
            }
        }
        qDebug()<<"id: "<<nodeMap.length();
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

void Body::startAutosaveTimer()
{
    autosaveTimer.start();
}

void Body::stopAutosaveTimer()
{
    autosaveTimer.stop();
}

void Body::autoTab(int context)
{
    m_searchBar->setProperty("enabled",true);
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




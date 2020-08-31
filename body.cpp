#include "body.h"
#include <node.h>
#include <pavementfile.h>
#include <ghostnode.h>
#include <nodearea.h>
#include <tip.h>
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
    for(int i=0; i<6; i++){
        freshBoot.append(true);
    }
    timer.setInterval(17);
    connect(&timer,SIGNAL(timeout()),this,SLOT(timeOut()));

    autosaveTimer.setInterval(1000 * 60);
    connect(&autosaveTimer,SIGNAL(timeout()),this,SLOT(autosave()));
    startAutosaveTimer();

    connect(getRoot(),SIGNAL(tabPressed()),this,SLOT(tab()));
    connect(getRoot(),SIGNAL(enterPressed()),this,SLOT(enterPressed()));
    connect(getRoot(),SIGNAL(closing()),this,SLOT(closeWindow()));
    connect(getRoot(),SIGNAL(escapePressed()),this,SLOT(escapePressed()));
    connect(getRoot(),SIGNAL(scroll(int,int,bool)),this,SLOT(scroll(int,int,bool)));
    connect(getRoot(),SIGNAL(windowChanged()),this,SLOT(windowChanged()));

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
    m_searchBar->findChild<QObject*>("rectangle")->setProperty("width",m_searchBar_defaultWidth);

    QObject * m = getRoot()->findChild<QObject*>("mouseArea");
    connect(m,SIGNAL(mouseTransform(int,int,int,int)),this,SLOT(mouseTransform(int,int,int,int)));
    connect(m_searchBar,SIGNAL(searching(QString)),this,SLOT(searching(QString)));
    connect(m_searchBar,SIGNAL(optionSelected(int)),this,SLOT(acceptedSelection(int)),Qt::UniqueConnection);
    connect(m,SIGNAL(mouseClicked(int,int)),this,SLOT(mouseClicked(int,int)));
    connect(m,SIGNAL(mouseDoubleClicked(int,int)),this,SLOT(mouseDoubleClicked(int,int)));
    connect(m,SIGNAL(mousePressed(int,int)),this,SLOT(mousePressed(int,int)));
    connect(m,SIGNAL(mouseReleased()),this,SLOT(mouseReleased()));
    connect(m,SIGNAL(mouseHeld()),this,SLOT(mouseHeld()));
    connect(m,SIGNAL(mouseInWindowChanged(bool)),this,SLOT(mouseInWindowChanged(bool)));

    showTip("Press <TAB> to open search bar");


    function f;
    f.name = "exit application";
    f.alias = QStringList{"quit","exit","quit application","close","close application"};
    f.commonShorthand = "Null";
    f.match = 0;
    functions.append(f);

    f.name = "fullscreen";
    f.alias = QStringList{};
    f.commonShorthand = "full";
    f.match = 0;
    functions.append(f);

    f.name = "exit fullscreen";
    f.alias = QStringList{};
    f.commonShorthand = "NULL";
    f.match = 0;
    functions.append(f);

    f.name = "new node";
    f.alias = QStringList{"new"};
    f.commonShorthand = "n";
    f.match = 0;
    functions.append(f);

    f.name = "new relation";
    f.alias = QStringList{"relation"};
    f.commonShorthand = "nr";
    f.match = 1;
    functions.append(f);

    f.name = "new connection";
    f.alias = QStringList{"connection","new line"};
    f.commonShorthand = "nc";
    f.match = 0;
    functions.append(f);



    f.name = "remove node";
    f.alias = QStringList{"delete node","delete","remove"};
    f.commonShorthand = "rmv";
    f.contexts = {node_selected};
    f.match = 0;
    functions.append(f);

    /*
    f.name = "relationship mode";
    f.alias = QStringList{"rel mode"};
    f.commonShorthand = "Null";
    f.match=0;
    functions.append(f);*/

    f.name = "move node";
    f.alias = QStringList{"move","transform","shift node"};
    f.contexts = {node_selected};
    f.commonShorthand = "mv";
    f.match = 0;
    functions.append(f);

    f.name = "parent";
    f.alias = QStringList{"set parent"};
    f.commonShorthand = "p";
    f.match = 0;
    functions.append(f);

    f.name = "save";
    f.alias = QStringList{"savestate"};
    f.commonShorthand = "sv";
    f.match = 0;
    functions.append(f);

    f.name = "open";
    f.alias = QStringList{"open file","load","load file"};
    f.commonShorthand = "op";
    f.match = 0;
    functions.append(f);

    f.name = "save as";
    f.alias = QStringList{"save file"};
    f.commonShorthand = "sf";
    f.match = 0;
    functions.append(f);

    f.name = "frame";
    f.alias = QStringList{"frame view","reset view","frame camera"};
    f.commonShorthand = "f";
    f.match = 0;
    functions.append(f);

    f.name = "dissolve";
    f.alias = QStringList{"dissolve node"};
    f.commonShorthand = "d";
    f.match = 0;
    functions.append(f);

    f.name = "include";
    f.alias = QStringList{"add include","include node"};
    f.commonShorthand = "inc";
    f.match = 0;
    functions.append(f);

    f.name = "debug";
    f.alias = QStringList{};
    f.commonShorthand = "NULL";
    f.match = 0;
    functions.append(f);

    f.name = "break";
    f.alias = QStringList{};
    f.commonShorthand = "NULL";
    f.contexts = {relation_selected,structural_selected};
    f.match = 0;
    functions.append(f);

    f.name = "expand";
    f.alias = QStringList{"expand node"};
    f.commonShorthand = "ex";
    f.contexts = {node_selected};
    f.match = 0;
    functions.append(f);

    f.name = "toggle batch select";
    f.alias = QStringList{"toggle batch select"};
    f.commonShorthand = "NULL";
    f.match = 0;
    functions.append(f);

    f.name = "select all children";
    f.alias = QStringList{"select children"};
    f.commonShorthand = "NULL";
    f.contexts = {node_selected};
    f.match = 0;
    functions.append(f);

    f.name = "clear batch selection";
    f.alias = QStringList{"clear selection","clear","empty batch"};
    f.commonShorthand = "NULL";
    f.contexts = {batch_selecting};
    f.match = 0;
    functions.append(f);

    f.name = "deselect batch";
    f.alias = QStringList{"deselect all","deselect"};
    f.commonShorthand = "NULL";
    f.match = 0;
    functions.append(f);

    f.name = "select";
    f.alias = QStringList{"select node","add","append","batch select"};
    f.commonShorthand = "s";
    f.contexts = {node_selected};
    f.match = 0;
    functions.append(f);

    f.name = "deselect";
    f.alias = QStringList{"deselect node"};
    f.commonShorthand = "ds";
    f.contexts = {node_selected};
    f.match = 0;
    functions.append(f);

    f.name = "ghost node";
    f.alias = QStringList{"ghost"};
    f.commonShorthand = "gh";
    f.contexts = {node_selected};
    f.match = 0;
    functions.append(f);

    f.name = "create ghost";
    f.alias = QStringList{"create ghost node","new ghost node"};
    f.commonShorthand = "g";
    f.match = 0;
    functions.append(f);

    f.name = "note";
    f.alias = QStringList{"create note","new note"};
    f.commonShorthand = "nt";
    f.match = 0;
    functions.append(f);

    f.name = "abstract";
    f.alias = QStringList{};
    f.commonShorthand = "ab";
    f.match = 0;
    functions.append(f);

    f.name = "create area";
    f.alias = QStringList{"new area","area"};
    f.commonShorthand = "ar";
    f.match = 0;
    functions.append(f);

    f.name = "remove from";
    f.alias = QStringList{"extract","extract from","remove"};
    f.commonShorthand = "NULL";
    f.match = 0;
    functions.append(f);

    f.name = "show text";
    f.alias = QStringList{"expand text","text"};
    f.commonShorthand = "txt";
    f.match = 0;
    functions.append(f);

    f.name = "migrate real nodes";
    f.alias = QStringList{};
    f.commonShorthand = "NULL";
    f.match = -2;
    functions.append(f);

    f.name = "get node";
    f.alias = QStringList{};
    f.commonShorthand = "NULL";
    f.match = 0;
    functions.append(f);

    f.name = "connection mode";
    f.alias = QStringList{};
    f.commonShorthand = "NULL";
    f.match = 0;
    functions.append(f);

    f.name = "copy submap";
    f.alias = QStringList{};
    f.commonShorthand = "NULL";
    f.match = 0;
    functions.append(f);

    f.name = "toggle mouse";
    f.alias = QStringList{};
    f.commonShorthand = "NULL";
    f.match = 0;
    functions.append(f);

    f.name = "toggle trackpad";
    f.alias = QStringList{};
    f.commonShorthand = "NULL";
    f.match = 0;
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

        QString sub = g;
        if(g.contains(".json")){
            sub.chop(5);
        }
        currentFile = sub;

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
            QStringList split = s.split("-");
            if(split.length() > 2){
                qDebug()<<"error: your node name has symbol '-' in it";
            }
            Node * n = nullptr;
            if(split.length() == 2){
                n = getNodeByInfo(s.split("-")[0],split[1]);
            }
            if(split.length() == 1){
                n = getNodeByName(split[0]);
            }

            if(n){
                GhostNode * g = newGhostNode(n,tabPosition().x,tabPosition().y);
                if(selectedNode()){
                    selectedNode()->underMapAppendNode(g);
                }
            }

        }

    }
    if(latestContext() == getting_node){
        if(n == -1){

        }else{
            QString s = displayFunctions[n].name;
            QStringList split = s.split("-");
            if(split.length() > 2){
                qDebug()<<"error: your node name has symbol '-' in it";
            }
            Node * n = nullptr;
            if(split.length() == 2){
                n = getNodeByInfo(s.split("-")[0],split[1]);
            }
            if(split.length() == 1){
                n = getNodeByName(split[0]);
            }
            if(n){
                n->setPosition(mousePosition());
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
    if(f == "toggle mouse"){
        getRoot()->findChild<QObject*>("trackpad")->setProperty("enabled",false);
        getRoot()->findChild<QObject*>("mouseWheel")->setProperty("enabled",true);
    }
    if(f == "toggle trackpad"){
        getRoot()->findChild<QObject*>("trackpad")->setProperty("enabled",true);
        getRoot()->findChild<QObject*>("mouseWheel")->setProperty("enabled",false);
    }


    if(f == "new node"){
        int id = allocateNewID("node");
        int x = m_tabPosition.x;
        int y = m_tabPosition.y;
        BaseNode * n = newNode(id,"",x,y,nullptr,nullptr);
        if(selectedNode()){
            selectedNode()->underMapAppendNode(n);
        }
        if(freshBoot[first_node]){
            showTip("Move node by clicking and dragging. Drag node onto other node to create ghost under target.");
            freshBoot[first_node] = false;
        }

    }
    if(f == "new relation"){
        int id = allocateNewID("relation");

        if(latestContext() == Context::node_selected && selectedNode()){
            Node * n = nullptr;

            newRelation(id,selectedNode(),n);
            setSelected(n);
            setFocusWindow();
            if(freshBoot[first_relation]){
                showTip("create relation by hovering over another node and pressing <ENTER>");
                freshBoot[first_relation] = false;
            }
        }
        else{
            qDebug()<<"error: no node selected";
            if(latestContext() == batch_selected){
                contextResolved();
            }
        }

    }
    if(f == "new connection"){
        int id = allocateNewID("relation");
        if(selectedNode()){
            Node * n = nullptr;
            newLine(id,selectedNode(),n);
            setSelected(n);
            setFocusWindow();
        }
        if(freshBoot[first_relation]){
            showTip("create relation by hovering over another node and pressing <ENTER>");
            freshBoot[first_relation] = false;
        }
    }
    if(f == "connection mode"){
        contextReset();
        setContext(connection_mode);

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
            if(selectedNode()){
                selectedNode()->destroy();
            }
            BaseNode * b = nullptr;
            setSelected(b);

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
        saveFile(QDir::currentPath() + "/saves/"+ currentFile + ".json");
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
        tip * t = new tip;
        t->initializeObj("test tip coming through");

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
    if(f == "get node"){
        autoTab(getting_node);
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
            if(selected){
                if(typeid (*selected) == typeid (Node)){

                    Node * node = selectedNode()->getNodePointer();
                    newGhostNode(node,mousePosition().x,mousePosition().y);
                }
            }

        }
    }
    if(f == "create ghost"){
        autoTab(node_browsing);

    }
    if(f == "note"){
        newNote(allocateNewID("node"),tabPosition().x,tabPosition().y);
    }

    if(f == "abstract"){


        if(context().contains(batch_selecting)){

            abstract(batchSelected());
            batchDeselect();
            contextReset();
            if(freshBoot[first_abstraction]){
                showTip("You have abstracted a node. Hover over the node and use 'expand' to expand");
                freshBoot[first_abstraction] = false;
            }
        }else{

            BaseNode * b = selectedNode();
            if(b){
                b->expand();
                b->abstract();
            }


        }

    }
    if(f == "expand"){
        if(contexts.contains(latestContext())){

            BaseNode * b = selectedNode();
            b->cycleExpandState(0);


        }
    }
    if(f == "copy submap"){
        if(selectedNode()){
            setContext(copying_submap);
            target_node = selectedNode();
        }
    }
    if(f == "show text"){
        if(selectedNode()){
            qDebug()<<10;
            selectedNode()->cycleExpandState(2);

        }
    }

    if(f == "move node"){

        if(contexts.contains(latestContext())){
            BaseNode * b = selectedNode();
            if(b){
                b->moving(true);
                setContext(moving_node);
            }

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
    if(f == "migrate real nodes"){
        Body::coordinate pos;
        pos.x=0;
        pos.y=0;
        int highest=0;
        for(int i=0; i<nodeMap.length(); i++){
            BaseNode * b = nodeMap[i];
            if(typeid (*b) == typeid (Node)){
                b->setPosition(m_mousePosition.add(pos));
                if(b->height()>highest){
                    highest=b->height();
                }
                pos.x+=b->width()+10;
                if(pos.x>1300){
                    pos.x=0;
                    pos.y+=highest+10;
                    highest = 0;
                }
            }
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
        if(relationArchive[i]){
            file.saveRelation(relationArchive[i]);
        }

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
    nodeMap.append(file.loadNotes());

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
            string name = path.toStdString()+"/"+entry->d_name;

            if(stat(name.c_str(),&statbuf) == 0){
                if(statbuf.st_mode & S_IFREG){

                    s.append(QString::fromStdString(entry->d_name));
                }
            }

        }
    }

    if(os() == "ubuntu" ){
        while((entry = readdir(dir)) != nullptr){
            //if(entry->d_type == 8){
                //s.append(QString::fromStdString(entry->d_name));
            //}
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
    return nullptr;
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

Node *Body::getNodeByInfo(QString name, QString type)
{
    BaseNode * n = nullptr;
    for(int i=0; i<nodeMap.length(); i++){
        if(nodeMap[i]){
            n = nodeMap[i];
            if(typeid (*n) == typeid (Node)){
                if(n->getName() == name && n->getTypeName() == type){
                    return n->getNodePointer();
                }
            }

        }

    }
    return nullptr;
}

bool Body::nameAlreadyExists(QString name,BaseNode * asker)
{
    for(int i=0; i<nodeMap.length(); i++){
        BaseNode * b = nodeMap[i];
        if(b){
            if(b != asker && b->getName() == name && typeid (*b) == typeid (Node)){
                return true;
            }
        }

    }
    return false;
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
    int i = nodeMap.indexOf(b);
    delete(nodeMap[i]);
    nodeMap[i] = nullptr;
}

void Body::showTip(QString s)
{
    for(int i=0; i<allTips.length(); i++){
        allTips[i]->destroy();
    }
    tip * t = new tip;
    allTips.append(t);
    t->initializeObj(s);
}

void Body::removeTip(tip *t)
{
    if(allTips.contains(t)){
        int i = allTips.indexOf(t);
        delete(allTips[i]);
        allTips.remove(i);
    }
}

void Body::autoRemoveTip(QString s)
{
    for(int i=0; i<allTips.length(); i++){
        if(s == allTips[i]->getText()){
            allTips[i]->destroy();
        }
    }
}

BaseNode *Body::getCommonAbstraction(QVector<BaseNode *> nodes)
{
    BaseNode * abstraction = nodes[0]->getAbstraction();
    while(abstraction){
        bool strike = false;
        for(int i=0; i<nodes.length(); i++){
            if(!nodes[i]->abstractionExists(abstraction)){
                strike = true;
                break;
            }
        }
        if(strike){
            abstraction = abstraction->getAbstraction();
        }else{
            return abstraction;
        }
    }
    return nullptr;
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
    BaseNode * buffer = getCommonAbstraction(nodes);
    Node * n = newNode(allocateNewID("node"),"abstraction",c.x,c.y,nullptr,nullptr);

    for(int i=0; i<nodes.length(); i++){
        nodes[i]->getAbstraction()->exude(nodes[i]);
    }
    n->setPositionByCenter(c);


    for(int i=0; i<nodes.length(); i++){
        nodes[i]->setAbstraction(n);
    }
    n->setUnderMap(nodes);
    n->expand();
    n->abstract();

    if(buffer){
        BaseNode * highestAbstraction = buffer->getHighestAbstraction();
        coordinate pos = highestAbstraction->getPosition();
        pos.x += highestAbstraction->width() + 5;
        n->setPosition(pos);
        BaseNode * b = n->newGhostNode();
        b->setPosition(c.add(buffer->getAbsolutePosition()));
        b->setAbstraction(buffer);
        buffer->underMapAppendNode(b);

    }



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
    autoRemoveTip("Press <TAB> to open search bar");
    if(freshBoot[first_tab]){
        showTip("Refer to 'Guide to using Pavement.txt' for list of usefull actions.");
        freshBoot[first_tab] = false;
    }
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
    if(latestContext() == copying_submap){

        if(highlightedNode()){

            target_node->clearUnderMap();
            target_node->cloneSubMap(highlightedNode());
            target_node = nullptr;
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

void Body::windowChanged()
{
    for(int i=0; i<allTips.length(); i++){
        allTips[i]->setY(getRoot()->property("height").toInt()-50);
    }
}



void Body::mouseClicked(int x, int y)
{
    bool ctrl = true;
    if(highlightedNode() && latestContext() == creating_relation && context().contains(connection_mode)){

        if(hoveringRelation()->originNode()!=highlightedNode()){

            if(!hoveringRelation()->originNode()->abstractionExists(highlightedNode())){
                hoveringRelation()->setDestinationObject(highlightedNode());
                hoveringRelation()->finalizeSelf();
                hoveringRelation()->updateSelf();

                Relation * r = nullptr;
                setHoveringRelation(r);
                contextReset();
                setHighlightedNode();
                setContext(connection_mode);
                ctrl = false;
            }
        }



    }
    if(selectedNode()){
        if(context().contains(connection_mode)){
            if(latestContext() != creating_relation){
                int id = allocateNewID("relation");

                if(latestContext() == Context::node_selected){
                    Node * n = nullptr;

                    newLine(id,selectedNode(),n);
                    setSelected(n);
                    setFocusWindow();
                }
            }
        }else{
            if(selectedNode()->clickShouldSelect()){
                batchSelect(selectedNode());
            }
        }
        ctrl = false;
    }
    if(ctrl){

        if(context().contains(connection_mode)){
            qDebug()<<"stopped";
            hoveringRelation()->destroy();
            Relation * r = nullptr;
            setHoveringRelation(r);
            contextReset();
        }
    }
}

void Body::mouseDoubleClicked(int x, int y)
{

}

void Body::mousePressed(int x, int y)
{
    switchvar = true;
    if(selectedNode()){
        if(selectedNode()->clickShouldSelect()){
            selectedNode()->moving(true);
            setContext(moving_node);

            for(int i=0; i<m_batchSelected.length(); i++){
                m_batchSelected[i]->moving(true);
            }
            getRoot()->setProperty("acceptMouseEvent",true);
        }else{
            getRoot()->setProperty("acceptMouseEvent",false);

        }


    }else{
        stopTimer();
        if(draggingCamera()){
            dragCamera((false));
        }else{
            dragCamera(true);
        }
    }
}


void Body::mouseReleased()
{

    if(latestContext() == moving_node){

        if(m_mouseHeld){
            if(highlightedNode()){
                if(!m_batchSelected.contains(highlightedNode())){
                    for(int i=0; i<nodeMap.length(); i++){
                        if(nodeMap[i]){

                            if(nodeMap[i]->isMoving() && nodeMap[i] != highlightedNode() && !highlightedNode()->underMapContains(nodeMap[i])){
                                if(nodeMap[i]->getAbstraction()){
                                    nodeMap[i]->getAbstraction()->exude(nodeMap[i]);
                                }
                                highlightedNode()->underMapAppendNode(nodeMap[i]);
                                BaseNode * b = nodeMap[i];
                                if(typeid (*b) == typeid (Node)){
                                    b->returnToPositionBeforeDragged();
                                }
                                if(freshBoot[first_abstraction]){
                                    showTip("You have abstracted a node. Hover over the node and use 'expand' to expand");
                                    freshBoot[first_abstraction] = false;
                                }
                            }
                        }
                    }
                }
            }
        }
        contextResolved();
    }

    dragCamera(false);

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

void Body::mouseInWindowChanged(bool b)
{
    m_mouseInWindow = b;
    if(!b){

        m_velocity.x = 0;

        m_velocity.y = 0;
        stopTimer();

    }

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
    m_mouseScreenSpacePosition.x = x - c.x/2;
    m_mouseScreenSpacePosition.y = y - c.y/2;

    int newX = int(m_mouseScreenSpacePosition.x / m_zoomFactor);
    int newY = int(m_mouseScreenSpacePosition.y / m_zoomFactor);
    newX = newX + c.x/2;
    newY = newY + c.y/2;
    coordinate newMouseScreenSpaceScaledPosition;
    newMouseScreenSpaceScaledPosition.x = newX;
    newMouseScreenSpaceScaledPosition.y = newY;

    m_mousePosition.x = int(newX - offsetX);
    m_mousePosition.y = int(newY - offsetY);

    /*
    QObject * show = getRoot()->findChild<QObject*>("show");

    show->setProperty("x",m_mousePosition.x);
    show->setProperty("y",m_mousePosition.y);

*/
    m_mouseVector = m_mousePosition.subtract(m_oldMousePosition);
    m_oldMousePosition = m_mousePosition;
    coordinate mouseLocalVector = newMouseScreenSpaceScaledPosition.subtract(m_oldMouseScreenSpaceScaledPosition);
    m_oldMouseScreenSpaceScaledPosition = newMouseScreenSpaceScaledPosition;
    if(draggingCamera()){
        coordinate invert = mouseLocalVector.invert();
        pan(invert.x,invert.y);
    }else{
        if(inBounds(x,y)){

            double angle = m_mouseScreenSpacePosition.getAngle();
            double magnitude = velocityMagnitude(x,y);
            m_velocity.x = int(cos(angle) * magnitude * 10);
            m_velocity.y = int(sin(angle) * magnitude * 10);

            startTimer();
        }else{
            m_velocity.x = 0;

            m_velocity.y = 0;
            if(!m_scaling){
                stopTimer();
            }

        }
    }


    if(hoveringRelation()){

        hoveringRelation()->updateSelf();
    }

    mouseMoved();


    bool highlighted = false;
    if(latestContext() == tab_searching){

        return;
    }

    for(int i=0; i<nodeMap.length(); i++){
        if(nodeMap[i]){
            if(nodeMap[i]->isMoving()){

                nodeMap[i]->transform(m_mouseVector);
                if(nodeMap[i]->getAbstraction()){
                    nodeMap[i]->getAbstraction()->subNodeMoved();
                }
            }
        }

    }

    bool control = false;

    if(latestContext() != relation_selected){
        for(int i=0; i<nodeMap.length(); i++){
            if(nodeMap[i]){
                if(nodeMap[i]->getAbstraction() == nullptr){


                    BaseNode * b = nodeMap[i]->isInside(mousePosition());
                    if(b){

                        control = true;
                        QVector<int> contexts = {parenting,including,creating_relation,moving_node,copying_submap};

                        if(contexts.contains(latestContext())){
                            if(!b->isMoving()){
                                setHighlightedNode(b);
                                highlighted = true;
                            }
                        }else{
                            b->hover(true,mousePosition());

                            setSelected(b);
                            highlighted = true;
                        }

                    }

                }
            }


        }

    }

    if(!control){
        BaseNode * null = nullptr;
        setSelected(null);
    }



    bool relationSelected = false;
    for(int i=0; i<relationArchive.length(); i++){
        if(relationArchive[i]){
            if(relationArchive[i]!=hoveringRelation()){
                if(relationArchive[i]->isInside(mousePosition().x,mousePosition().y)){
                    relationArchive[i]->setSelected(true);
                    setSelected(relationArchive[i]);
                    relationSelected = true;
                }else{
                    relationArchive[i]->setSelected(false);
                }
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
    if(freshBoot[first_batch]){
        showTip("You have activated multi select by clicking on a node. See 'Guide to using Pavement.txt' for things you can do");
        freshBoot[first_batch] = false;
    }
    setContext(batch_selecting);

    if(m_batchSelected.contains(n)){
        m_batchSelected.removeOne(n);
        n->select(false);
        contextResolved();

    }else{
        m_batchSelected.append(n);
        n->select(true);
    }

}

void Body::batchSelect(QVector<BaseNode *> n){
    for(int i=0; i<n.length(); i++){
        batchSelect(n[i]);
    }
}

void Body::batchDeselect(BaseNode *n){
    if(m_batchSelected.contains(n)){
        n->select(false);

        m_batchSelected.removeOne(n);
    }
}

void Body::setSelected(BaseNode *n)
{


    m_selectedNode = n;
    /*
    if(switchvar){
        qDebug()<<"set selected: "<<n;
        switchvar = false;
    }*/

    if(n){

        m_selectedRelation = nullptr;

        setContext(node_selected);
    }

}

void Body::setHighlightedNode(BaseNode *n)
{
    if(n){
        n->highlight(true);
        m_highlightedNode = n;
    }
}

void Body::setHighlightedNode()
{
    if(m_highlightedNode){
        m_highlightedNode->highlight(false);
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
        cycleSearchBarSelection();
    }

    input = Utility::trimString(input," ");
    m_searchBar->findChild<QObject*>("textInput")->setProperty("text",input);
    input = input.toLower();
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
        pool = functionFromList(saves);
    }
    if(latestContext() == saving_file){
        pool.clear();
        QStringList saves = getSaves(defaultPath);
        pool = functionFromList(saves);
    }
    if(latestContext() == node_browsing || latestContext() == getting_node){
        pool.clear();
        QStringList nodes;
        for(int i=0; i<nodeMap.length(); i++){
            if(nodeMap[i]){
                BaseNode * b = nodeMap[i];
                if(typeid (*b) == typeid (Node)){
                    nodes.append(nodeMap[i]->getName() + "-" + nodeMap[i]->getNodePointer()->getTypeName());
                }
            }

        }

        pool = functionFromList(nodes);
    }
    if(pool.length()==0){
        return 0;
    }
    for(int i=0; i<pool.length(); i++){
        pool[i].match += match(&pool[i],input);
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
    int longestContentWidth = 0;
    for(int i=0; i<displayFunctions.length(); i++){
        if(displayFunctions[i].match>2){
            QString t = "option" + QString::number(i);
            QObject * opt = m_searchBar->findChild<QObject*>(t);
            opt->setProperty("text",displayFunctions[i].name);
            if(opt->property("contentWidth").toInt() > longestContentWidth){
                longestContentWidth = opt->property("contentWidth").toInt();
            }
            n+=1;
        }
    }
    if(longestContentWidth+13> 186){
        //if(longestContentWidth +13> m_searchBar->findChild<QObject*>("rectangle")->property("width").toInt()){

        m_searchBar->findChild<QObject*>("rectangle")->setProperty("width",longestContentWidth+13);

    }else{
        m_searchBar->findChild<QObject*>("rectangle")->setProperty("width",186);
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

    }

    if(m_scaling){
        zoom(double(m_zoomVelocity));
        if(m_zoomVelocity >10){
            m_zoomVelocity -= 20;
        }
        if(m_zoomVelocity <-10){
            m_zoomVelocity +=20;
        }


    }
    if(m_velocity.y == 0  && m_velocity.x == 0 && m_zoomVelocity <= 10 && m_zoomVelocity >= -10){
        m_scrolling = false;
        m_zoomVelocity = 0;
        m_scaling = false;
        stopTimer();
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
Relation * Body::newRelation(int id, BaseNode *origin, BaseNode *destination)
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
    r->updateSelf();
    relationArchive.append(r);
    //qDebug()<<r->obj();
    return r;
}

Relation * Body::newLine(int id, BaseNode *origin, BaseNode *destination)
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

    r->updateSelf();
    relationArchive.append(r);
    //qDebug()<<r->obj();
    return r;
}

Relation * Body::newTriangle(int id, BaseNode *origin, BaseNode *destination)
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
    r->updateSelf();
    relationArchive.append(r);
    return r;
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
    coordinate c = medianPosition(nodes);

    BaseNode * buffer = getCommonAbstraction(nodes);

    NodeArea * n = new NodeArea;
    n->initializeObj();
    for(int i=0; i<nodes.length(); i++){
        if(nodes[i]->getAbstraction()){
            nodes[i]->getAbstraction()->exude(nodes[i]);
        }
    }
    n->setUnderMap(nodes);
    n->reFormatExpandedForm();
    if(buffer){
        coordinate dimensions;
        dimensions.x = n->width()/2;
        dimensions.y = n->height()/2;
        n->setPosition(c.subtract(dimensions).add(buffer->getAbsolutePosition()));

        buffer->underMapAppendNode(n);

        qDebug()<<"oh joy";
    }
    nodeMap.append(n);
    return n;
}

NodeArea *Body::newNodeArea(BaseNode * node)
{
    NodeArea * n = new NodeArea;
    n->initializeObj();
    n->cloneSubMap(node);
    n->reFormatExpandedForm();
    nodeMap.append(n);
    return n;
}

Note *Body::newNote(int id,int x, int y)
{
    Note * n = new Note;
    n->initializeObj();
    n->setID(id);
    coordinate c;
    c.x = x;
    c.y = y;
    n->setPosition(c);

    nodeMap.append(n);
    return n;
}

void Body::autosave()
{
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
    if(sessionLength % 5 == 0 && sessionLength != 0){
        saveFile(defaultPath + divider + currentFile + "-autosave" + divider + "5-minutes-ago.json");
    }
    if(sessionLength % 10 == 0 && sessionLength != 0){
        saveFile(defaultPath + divider + currentFile + "-autosave" + divider + "10-minutes-ago.json");
    }
    if(sessionLength % 20 == 0 && sessionLength != 0){
        saveFile(defaultPath + divider + currentFile + "-autosave" + divider + "autosave-"
                 + QString::number(sessionLength%20));
    }

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

    if(context == saving_file){
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
    QString nospace = "";
    for(int i=0; i<name.length(); i++){
        nospace+=name[i];
    }
    match+=matchString(nospace,input);

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
    input = input.toLower();
    s = s.toLower();
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
    m_searchBar->findChild<QObject*>("rectangle")->setProperty("width",m_searchBar_defaultWidth);
    m_searchBar->setProperty("optionCount",0);
    m_searchBar->setProperty("optionHighlighted",-1);
    m_searchBar->findChild<QObject*>("textInput")->setProperty("text","");
    for(int i=0; i<5; i++){
        QString t = "option" + QString::number(i);
        m_searchBar->findChild<QObject*>(t)->setProperty("text","");
    }
}

void Body::cycleSearchBarSelection()
{
    int n = m_searchBar->property("optionHighlighted").toInt();
    m_searchBar->setProperty("optionHighlighted",n+1);
}




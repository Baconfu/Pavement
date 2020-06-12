#include "pavementfile.h"

PavementFile::PavementFile(QString fileName)
{
    m_fileName = fileName;

}

void PavementFile::saveBaseNode(BaseNode *n)
{
    if(typeid (*n) == typeid (Node))   {
        saveNode(n->getNodePointer());
    }
    if(typeid (*n) == typeid (GhostNode)){
        saveGhost(n->getGhostPointer());
    }
    if(typeid (*n) == typeid (NodeArea)){
        saveArea(n->getAreaPointer());
    }
    if(typeid (*n) == typeid (Note)){
        saveNote(n);
    }

}

void PavementFile::saveNode(Node *n)
{
    QJsonArray nodes = m_obj["nodes"].toArray();

    QJsonObject node;

    node["id"] = n->getID();
    node["name"] = n->getName();
    node["x"] = n->getX();
    node["y"] = n->getY();

    if(n->getType()){
        node["type"] = n->typeName();
        node["typeNode"] = n->getType()->getID();
    }

    node["text"] = n->getText();

    if(n->isExpanded()){
        node["expanded"] = true;
    }else{
        node["expanded"] = false;
    }

    QJsonObject subMap;
    QJsonArray subNodes = subMap["subMap"].toArray();
    QVector<BaseNode*> subMapNodes = n->getUnderMap();


    for(int i=0; i<subMapNodes.length(); i++){
        QJsonObject subNode;
        subNode = writeSubNode(subMapNodes[i]);


        subNodes.append(subNode);
    }


    node["submap"] = subNodes;


    nodes.append(node);

    m_obj["nodes"] = nodes;
}


void PavementFile::saveGhost(GhostNode *n)
{
    QJsonArray ghosts = m_obj["ghosts"].toArray();

    ghosts.append(writeSubNode(n));

    m_obj["ghosts"] = ghosts;
}

void PavementFile::saveArea(NodeArea *n)
{
    QJsonArray areas = m_obj["areas"].toArray();

    areas.append(writeSubNode(n));

    m_obj["areas"] = areas;
}


void PavementFile::saveRelation(Relation *r)
{
    QJsonArray relations = m_obj["relations"].toArray();

    QJsonObject relation;

    relation["id"] = r->ID();

    relation["name"] = r->getName();

    relation["type"] = r->getType();

    BaseNode * originNode = r->originNode();
    BaseNode * destinationNode = r->destinationNode();
    if(r->getOriginType() == Relation::node){
        relation["originNode"] = originNode->getID();
    }else{
        relation["originNode"] = -1;
    }

    if(r->getOriginType() == Relation::relation){
        relation["originRelation"] = r->originRelation()->ID();
    }else{
        relation["originRelation"] = -1;
    }


    if(r->getDestinationType() == Relation::node){
        relation["destinationNode"] = destinationNode->getID();
    }else{
        relation["destinationNode"] = -1;
    }

    if(r->getDestinationType() == Relation::relation){
        relation["destinationRelation"] = r->destinationRelation()->ID();
    }else{
        relation["destinationRelation"] = -1;
    }

    relations.append(relation);

    m_obj["relations"] = relations;

}

void PavementFile::saveNote(BaseNode *b)
{
    QJsonArray notes = m_obj["notes"].toArray();
    QJsonObject note;
    note["id"] = b->getID();
    note["text"] = b->getText();
    Body::coordinate c = b->getPosition();
    note["x"] = c.x;
    note["y"] = c.y;
    note["width"] = b->width();
    note["height"] = b->height();
    notes.append(note);
    m_obj["notes"] = notes;
}

QVector<BaseNode*> PavementFile::loadNodes()
{
    QJsonArray nodes = m_obj["nodes"].toArray();
    nodePool.clear();


    for(int i=0; i<nodes.count(); i++){
        nodePool.append(loadNode(nodes[i].toObject()));
    }

    QVector<BaseNode*> tempPool = nodePool;


    for(int i=0; i<tempPool.length(); i++){
        Node * n = nodePool[i]->getNodePointer();
        QJsonObject node = nodes[i].toObject();


        for(int j=0; j<nodePool.length(); j++){
            if(node["typeNode"].toString() != "Null" && node["typeNode"].toString() != ""){

                Node * typeNode = findNodeByID(nodePool,node["typeNode"].toInt())->getNodePointer();
                if(typeNode && typeNode->getName() == node["type"].toString()){
                    n->setType(typeNode);
                    typeNode->registerMember(n);
                }
            }

        }
    }
    for(int i=0; i<tempPool.length(); i++){
        QJsonObject node = nodes[i].toObject();

        QJsonArray subNodeMap = node["submap"].toArray();


        QVector<BaseNode*> sub;
        for(int j=0; j<subNodeMap.count(); j++){

            QJsonObject ghost = subNodeMap[j].toObject();
            BaseNode * b = loadSubNode(ghost,nodePool[i]->getPosition());
            b->setAbstraction(nodePool[i]);
            sub.append(b);
        }


        nodePool[i]->getNodePointer()->setUnderMap(sub);
        if(node["expanded"].toBool() == true){
            nodePool[i]->expand();
        }

        Body::coordinate c;
        c.x = nodes[i].toObject()["x"].toInt();
        c.y = nodes[i].toObject()["y"].toInt();
        nodePool[i]->setPosition(c);

    }
    return nodePool;
}

Node *PavementFile::loadNode(QJsonObject node)
{
    int id = node["id"].toInt();
    Node * n = new Node(nullptr,id);
    n->initializeObj();
    n->setName(node["name"].toString());

    Body::coordinate c;
    c.x = node["x"].toInt();
    c.y = node["y"].toInt();


    n->setText(node["text"].toString());

    n->setPosition(c);
    return n;
}

QVector<BaseNode *> PavementFile::loadSubNodes()
{
    QJsonArray nodes = m_obj["ghosts"].toArray();
    QJsonArray areas = m_obj["areas"].toArray();

    QVector<BaseNode*> pool;
    for(int i=0; i<nodes.count(); i++){
        Body::coordinate c;
        c.x = 0;
        c.y = 0;

        pool.append(loadSubNode(nodes[i].toObject(),c));
    }
    for(int i=0; i<areas.count(); i++){
        Body::coordinate c;
        c.x = 0;
        c.y = 0;

        pool.append(loadSubNode(areas[i].toObject(),c));
    }
    return pool;
}

BaseNode *PavementFile::loadSubNode(QJsonObject node,Body::coordinate positionOffset)
{
    BaseNode * n = nullptr;
    QString type = node["type"].toString();

    if(type == "ghost"){
        int original = node["original"].toInt();
        Node * originalNode = findNodeByID(nodePool,original)->getNodePointer();
        n = new GhostNode(originalNode);
        originalNode->registerGhost(n->getGhostPointer());

    }

    if(type == "area"){
        n = new NodeArea;
    }



    n->initializeObj();


    n->setID(node["id"].toInt());
    Body::coordinate c;
    c.x = node["x"].toInt();
    c.y = node["y"].toInt();

    n->setPosition(c.add(positionOffset));



    QJsonArray subMap = node["subMap"].toArray();

    QVector<BaseNode*> subNodes;
    for(int i=0; i<subMap.count(); i++){

        QJsonObject subNode = subMap[i].toObject();

        BaseNode * b = loadSubNode(subNode,n->getAbsolutePosition());
        b->setAbstraction(n);
        subNodes.append(b);
    }

    n->setUnderMap(subNodes);
    if(type == "ghost"){
        n->getGhostPointer()->adoptOriginal();
        if(node["expanded"].toBool()){
            n->expand();
        }
    }
    n->setPosition(c.add(positionOffset));



    //n->reFormatExpandedForm();
    nodePool.append(n);
    return n;
}

QVector<BaseNode*> PavementFile::loadNotes()
{
    QJsonArray notes = m_obj["notes"].toArray();

    QVector<BaseNode*> notePool;
    for(int i=0; i<notes.count(); i++){
        notePool.append(loadNote(notes[i].toObject()));
    }
    return notePool;
}
BaseNode *PavementFile::loadNote(QJsonObject note)
{
    Note * n = new Note;
    n->initializeObj();
    n->setID(note["id"].toInt());
    n->setText(note["text"].toString());
    Body::coordinate c;
    c.x = note["x"].toInt();
    c.y = note["y"].toInt();

    QObject * t = n->obj()->findChild<QObject*>("textArea");
    t->setProperty("implicitWidth",note["width"].toInt());
    t->setProperty("implicitHeight",note["height"].toInt());

    n->setPosition(c);
    return n;
}

QVector<Relation*> PavementFile::loadRelations()
{
    QJsonArray relations = m_obj["relations"].toArray();

    QVector<Relation*> relationPool;

    for(int i=0; i<relations.count(); i++){
        QJsonObject relation = relations[i].toObject();


        Relation * r = new Relation(nullptr,relation["type"].toString());
        r->initializeObj();

        r->setID(relation["id"].toInt());

        r->setName(relation["name"].toString());



        relationPool.append(r);
    }
    for(int i=0; i<relationPool.length(); i++){
        Relation * r = relationPool[i];
        QJsonObject relation = relations[i].toObject();
        for(int j=0; j<relationPool.length(); j++){
            if(relationPool[j]->ID() == relation["destinationRelation"].toInt()){
                r->setDestinationObject(relationPool[j]);
            }
            if(relationPool[j]->ID() == relation["originRelation"].toInt()){
                r->setOriginObject(relationPool[j]);
            }
        }

        r->setOriginObject(findNodeByID(nodePool,relation["originNode"].toInt()));
        r->setDestinationObject(findNodeByID(nodePool,relation["destinationNode"].toInt()));



        r->finalizeSelf();
        r->updateSelf();
    }



    return relationPool;
}

void PavementFile::writeJson()
{
    write(m_fileName,m_obj);
}

void PavementFile::readJson()
{
    m_obj = load(m_fileName);
}

QJsonObject PavementFile::writeSubNode(BaseNode *n)
{

    QJsonObject subNode;

    subNode["id"] = n->getID();

    if(typeid (*n) == typeid (GhostNode)){
        subNode["type"] = "ghost";
        subNode["original"] = n->getGhostPointer()->getOriginal()->getID();
    }
    if(typeid (*n) == typeid (NodeArea)){
        subNode["type"] = "area";
    }

    Body::coordinate c = n->getPosition();
    subNode["x"] = c.x;
    subNode["y"] = c.y;

    subNode["expanded"] = n->isExpanded();
    QJsonArray subMap;
    QVector<BaseNode*> underMap = n->getUnderMap();
    for(int i=0; i<underMap.length(); i++){

        QJsonObject sub;
        sub = writeSubNode(underMap[i]);

        subMap.append(sub);
    }
    subNode["subMap"] = subMap;


    return subNode;

}



QJsonObject PavementFile::findElementByID(QJsonArray elements, int id)
{

    for(int i=0; i<elements.count(); i++){
        QJsonObject element = elements[i].toObject();
        if(element["id"] == id){
            return element;
        }
    }
}

BaseNode *PavementFile::findNodeByID(QVector<BaseNode *> nodes, int id)
{
    for(int i=0; i<nodes.length(); i++){
        if(nodes[i]->getID() == id){
            return nodes[i];
        }
    }
    return nullptr;
}






#include "pavementfile.h"

PavementFile::PavementFile(QString fileName)
{
    m_fileName = fileName;

}

void PavementFile::saveNode(BaseNode *n)
{
    if(typeid (*n) == typeid (Node*))   {
        saveNode(n->getNodePointer());
    }
    if(typeid (*n) == typeid (GhostNode*)){
        saveGhost(n->getGhostPointer());
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



    QJsonArray parents;
    QVector<Node*> parentNodes = n->getParents();
    for(int i=0; i<parentNodes.length(); i++){
        QJsonObject parent;
        parent["id"] = parentNodes[i]->getID();
        parent["name"] = parentNodes[i]->getName();
        parents.append(parent);
    }
    node["parents"] = parents;

    QJsonObject subMap;
    QJsonArray subNodes = subMap["subNodes"].toArray();
    QJsonArray subGhosts = subMap["subGhosts"].toArray();
    QVector<BaseNode*> subMapNodes = n->getUnderMap();


    for(int i=0; i<subMapNodes.length(); i++){
        QJsonObject subNode;
        BaseNode * temp = subMapNodes[i];
        if(typeid (*temp) == typeid (GhostNode)){
            subNode = writeGhostNode(subMapNodes[i]->getGhostPointer());


        }


        subGhosts.append(subNode);
    }

    subMap["subGhosts"] = subGhosts;
    node["submap"] = subMap;


    nodes.append(node);

    m_obj["nodes"] = nodes;
}


void PavementFile::saveGhost(GhostNode *n)
{
    QJsonArray ghosts = m_obj["ghosts"].toArray();

    ghosts.append(writeGhostNode(n));

    m_obj["ghosts"] = ghosts;
}


void PavementFile::saveRelation(Relation *r)
{
    QJsonArray relations = m_obj["relations"].toArray();

    QJsonObject relation;

    relation["id"] = r->ID();

    relation["name"] = r->getName();

    BaseNode * originNode = r->originNode();
    BaseNode * destinationNode = r->destinationNode();
    if(r->getOriginType() == Relation::node){
        relation["originNode"] = originNode->getID();
    }else{
        relation["originNode"] = -1;
    }
    if(typeid (*originNode) == typeid (GhostNode)){
        relation["originalOrigin"] = originNode->getGhostPointer()->getOriginal()->getID();
    }else{
        relation["originalOrigin"] = -1;
    }
    if(typeid (*originNode) == typeid (NodeArea)){
        relation["originArea"] = originNode->getID();
    }else{
        relation["originArea"] = -1;
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
    if(typeid (*destinationNode) == typeid (GhostNode)){
        relation["originalDestination"] = destinationNode->getGhostPointer()->getOriginal()->getID();
    }else{
        relation["originalDestination"] = -1;
    }
    if(typeid (*destinationNode) == typeid (NodeArea)){
        relation["destinationArea"] = destinationNode->getID();
    }else{
        relation["destinationArea"] = -1;
    }
    if(r->getDestinationType() == Relation::relation){
        relation["destinationRelation"] = r->destinationRelation()->ID();
    }else{
        relation["destinationRelation"] = -1;
    }

    relations.append(relation);

    m_obj["relations"] = relations;

}

QVector<Node*> PavementFile::loadNodes()
{
    QJsonArray nodes = m_obj["nodes"].toArray();
    nodePool.clear();

    for(int i=0; i<nodes.count(); i++){
        nodePool.append(loadNode(nodes[i].toObject()));
    }



    for(int i=0; i<nodePool.length(); i++){
        Node * n = nodePool[i];
        QJsonObject node = nodes[i].toObject();
        QJsonArray parents = node["parents"].toArray();
        for(int j=0; j<parents.count(); j++){
            QJsonObject parent = parents[j].toObject();
            Node * p = findNodeByID(nodePool,parent["id"].toInt());
            if(p){
                n->addParent(p);
                p->addChild(n);
                structural * s = n->newStructural();
                s->setChildNode(n);
                s->setParentNode(p);
                s->update();
            }
        }

        for(int j=0; j<nodePool.length(); j++){
            Node * typeNode = findNodeByID(nodePool,node["typeNode"].toInt());
            if(typeNode && typeNode->getName() == node["type"].toString()){
                n->setType(typeNode);
                typeNode->registerMember(n);
            }
        }
    }
    QVector<Node*> nArray = nodePool;
    for(int i=0; i<nodePool.length(); i++){
        QJsonObject node = nodes[i].toObject();
        QJsonObject subMap = node["submap"].toObject();
        QJsonArray subNodeMap = subMap["subGhosts"].toArray();

        QVector<BaseNode*> sub;
        for(int j=0; j<subNodeMap.count(); j++){
            QJsonObject ghost = subNodeMap[j].toObject();
            GhostNode * g = loadGhost(ghost,nodePool[i]->getPosition());
            g->setAbstraction(nodePool[i]);
            sub.append(g);
        }

        nodePool[i]->setUnderMap(sub);

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

    n->setPosition(c);
    return n;
}

QVector<GhostNode *> PavementFile::loadGhosts()
{
    QJsonArray ghosts = m_obj["ghosts"].toArray();
    QVector<GhostNode*> ghostPool;
    for(int i=0; i<ghosts.count(); i++){
        Body::coordinate c;
        c.x = 0;
        c.y = 0;
        ghostPool.append(loadGhost(ghosts[i].toObject(),c));
    }
    return ghostPool;
}

GhostNode *PavementFile::loadGhost(QJsonObject ghost,Body::coordinate positionOffset)
{
    int original = ghost["original"].toInt();
    Node * originalNode = findNodeByID(nodePool,original);
    GhostNode * n = new GhostNode(originalNode);
    originalNode->registerGhost(n);

    n->initializeObj();


    n->setID(ghost["id"].toInt());
    Body::coordinate c;
    c.x = ghost["x"].toInt();
    c.y = ghost["y"].toInt();
    n->setPosition(c.add(positionOffset));

    QJsonArray subMap = ghost["subMap"].toArray();
    QVector<BaseNode*> underMap;
    for(int i=0; i<subMap.count(); i++){
        QJsonObject subNode = subMap[i].toObject();
        GhostNode * g = loadGhost(subNode,n->getPosition());
        g->setAbstraction(n);
        underMap.append(g);
    }

    n->setUnderMap(underMap);

    n->adoptOriginal();
    ghostPool.append(n);
    return n;
}

QVector<Relation*> PavementFile::loadRelations()
{
    QJsonArray relations = m_obj["relations"].toArray();

    QVector<Relation*> relationPool;

    for(int i=0; i<relations.count(); i++){
        QJsonObject relation = relations[i].toObject();
        Relation * r = new Relation;
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
        bool originGhost = false;
        bool destinationGhost = false;
        if(relation["originalOrigin"].toInt() != -1){
            int originID = relation["originalOrigin"].toInt();

            r->setOriginObject(nodePool[originID]->getGhostByID(relation["originNode"].toInt()));
            originGhost = true;

        }
        if(relation["originalDestination"].toInt() != -1){

            int destinationID = relation["originalDestination"].toInt();
            r->setDestinationObject(nodePool[destinationID]->getGhostByID(relation["destinationNode"].toInt()));
            destinationGhost = true;
        }
        for(int j=0; j<nodePool.length(); j++){

            if(!originGhost){
                if(nodePool[j]->getID() == relation["originNode"].toInt()){
                    r->setOriginObject(nodePool[j]);
                }
            }
            if(!destinationGhost){
                if(nodePool[j]->getID() == relation["destinationNode"].toInt()){
                    r->setDestinationObject(nodePool[j]);
                }
            }

        }
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

QJsonObject PavementFile::writeGhostNode(GhostNode *n)
{

    QJsonObject ghost;


    ghost["id"] = n->getID();
    ghost["original"] = n->getOriginal()->getID();
    Body::coordinate c = n->getPosition();
    ghost["x"] = c.x;
    ghost["y"] = c.y;
    QJsonArray subMap;
    QVector<BaseNode*> underMap = n->getUnderMap();
    for(int i=0; i<underMap.length(); i++){
        BaseNode * b = underMap[i];
        QJsonObject subNode;
        if(typeid (*b) == typeid (GhostNode)){

            subNode = writeGhostNode(b->getGhostPointer());
        }

        subMap.append(subNode);
    }
    ghost["subMap"] = subMap;


    return ghost;

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

Node *PavementFile::findNodeByID(QVector<Node *> nodes, int id)
{
    for(int i=0; i<nodes.length(); i++){
        if(nodes[i]->getID() == id){
            return nodes[i];
        }
    }
    return nullptr;
}






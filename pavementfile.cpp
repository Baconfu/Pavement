#include "pavementfile.h"

PavementFile::PavementFile(QString fileName)
{
    m_fileName = fileName;

}

void PavementFile::saveNode(Node *n)
{
    QJsonArray nodes = m_obj["nodes"].toArray();

    QJsonObject node;

    node["id"] = n->ID();
    node["name"] = n->name();
    node["x"] = n->absX();
    node["y"] = n->absY();
    node["type"] = n->typeName();
    node["typeNode"] = n->getType()->ID();

    QJsonArray parents;
    QVector<Node*> parentNodes = n->getParents();
    for(int i=0; i<parentNodes.length(); i++){
        QJsonObject parent;
        parent["id"] = parentNodes[i]->ID();
        parent["name"] = parentNodes[i]->name();
        parents.append(parent);
    }
    node["parents"] = parents;




    nodes.append(node);

    m_obj["nodes"] = nodes;
}

void PavementFile::writeJson()
{
    write(m_fileName,m_obj);
}






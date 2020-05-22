#ifndef PAVEMENTFILE_H
#define PAVEMENTFILE_H

#include <file.h>
#include <node.h>
#include <relation.h>
#include <structural.h>
#include <ghostnode.h>
#include <nodearea.h>


class PavementFile : public File
{
public:
    PavementFile(QString fileName);
    void saveBaseNode(BaseNode * n);

    void saveNode(Node * n);
    void saveGhost(GhostNode * n);
    void saveArea(NodeArea * n);
    void saveRelation(Relation * r);

    QJsonObject writeSubNode(BaseNode * n);

    QVector<BaseNode*> getNodePool(){return nodePool;}

    QVector<BaseNode*> loadNodes();
    Node * loadNode(QJsonObject node);
    QVector<BaseNode*> loadSubNodes();
    BaseNode * loadSubNode(QJsonObject subNode,Body::coordinate positionOffset);

    QVector<Relation *> loadRelations();
    void writeJson();
    void readJson();




private:
    QJsonObject m_obj;
    QJsonDocument m_doc;


    QVector<BaseNode*> nodePool;


    QJsonObject findElementByID(QJsonArray elements,int id);
    BaseNode * findNodeByID(QVector<BaseNode*> nodes,int id);
};

#endif // PAVEMENTFILE_H

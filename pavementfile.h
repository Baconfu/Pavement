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
    void saveNode(BaseNode * n);
    void saveNode(Node * n);
    void saveGhost(GhostNode * n);
    void saveRelation(Relation * r);
    QVector<Node*> loadNodes();
    Node * loadNode(QJsonObject node);
    QVector<GhostNode*> loadGhosts();
    GhostNode * loadGhost(QJsonObject ghost,Body::coordinate positionOffset);

    QVector<Relation *> loadRelations();
    void writeJson();
    void readJson();

    QJsonObject writeGhostNode(GhostNode * n);

    QVector<GhostNode*> getGhostPool(){return ghostPool;}

private:
    QJsonObject m_obj;
    QJsonDocument m_doc;

    QVector<Node*> nodePool;
    QVector<GhostNode*> ghostPool;
    QVector<NodeArea*> areaPool;

    QJsonObject findElementByID(QJsonArray elements,int id);
    Node * findNodeByID(QVector<Node*> nodes,int id);
};

#endif // PAVEMENTFILE_H

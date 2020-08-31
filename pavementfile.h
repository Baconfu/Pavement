#ifndef PAVEMENTFILE_H
#define PAVEMENTFILE_H

#include <file.h>
#include <node.h>
#include <relation.h>
#include <ghostnode.h>
#include <nodearea.h>
#include <note.h>


class PavementFile : public File
{
public:
    PavementFile(QString fileName);
    void saveBaseNode(BaseNode * n);

    void saveNode(Node * n);
    void saveGhost(GhostNode * n);
    void saveArea(NodeArea * n);
    void saveRelation(Relation * r);
    void saveNote(BaseNode * b);

    QJsonObject writeSubNode(BaseNode * n);

    QVector<BaseNode*> getNodePool(){return nodePool;}

    QVector<BaseNode*> loadNodes();
    Node * loadNode(QJsonObject node);
    QVector<BaseNode*> loadSubNodes();
    BaseNode * loadSubNode(QJsonObject subNode,Body::coordinate positionOffset);

    QVector<BaseNode*> loadNotes();
    BaseNode * loadNote(QJsonObject note);
    QVector<Relation *> loadRelations();
    void writeJson();
    void readJson();


    QJsonObject obj(){return m_obj;}
    void formatPosition(BaseNode * b);
    void expandFormat(int index);

private:
    QJsonObject m_obj;
    QJsonDocument m_doc;

    QVector<Body::coordinate> format;

    QVector<BaseNode*> nodePool;


    QJsonObject findElementByID(QJsonArray elements,int id);
    BaseNode * findNodeByID(QVector<BaseNode*> nodes,int id);
};

#endif // PAVEMENTFILE_H

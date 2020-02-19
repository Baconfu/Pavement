#ifndef RELATION_H
#define RELATION_H

#include <QObject>
#include <body.h>

class Node;

class Relation: public QObject
{
    Q_OBJECT
public:
    Relation(QObject * parent = nullptr);

    int ID(){return m_id;}
    void setID(int n){m_id = n;}

    void isTouching(int x,int y);

private:
    enum cellStyle{
        minimal = 0,
        closed = 1,
        noBorder = 2
    };
    enum relationType{
        type_outGoing = 1,
        type_incoming = 0,
        type_toRelation = 2,
        type_toNode = 3
    };

    struct style{
        QColor textColor;
        QColor fillColor;
        QColor borderColor;

        int fontSize;
        int cellStyle;

    };

    int m_id;

    bool m_hovering = false;

    QVector<Relation*> toRelation;
    QVector<Relation*> toRelation_relation;
    QVector<Relation*> fromRelation;
    QVector<Relation*> fromRelation_relation;
    QVector<Relation*> fromNode;
    QVector<Node*> fromNode_node;

    Node * m_origin_node = nullptr;
    Relation * m_origin_relation = nullptr;
    Node * m_destination_node = nullptr;
    Relation * m_destination_relation = nullptr;

    Body::coordinate m_origin;
    Body::coordinate m_destination;
    int m_width;
    int m_height;

    QQuickItem * m_obj = nullptr;

public:
    void setOriginObject(Node * n);
    void setOriginObject(Relation * r);
    void setDestinationObject(Node * n);
    void setDestinationObject(Relation * r);
    void clearDestinationObject();
    void setHovering(bool b);
    QString originObjectType(){
        if(m_origin_node && !m_origin_relation){return "node";}
        if(m_origin_relation && !m_origin_node){return "relation";}
        if(m_origin_node && m_origin_relation){
            qDebug()<<"Error: relation origin is both a node and a relation";
            return "error";
        }
        if(!m_origin_node && !m_origin_relation) {
            qDebug()<<"Error: relation has no origin object";
            return "error";
        }
        return "";
    }
    QString destinationObjectType(){

        if(m_destination_node && !m_destination_relation){return "node";}
        if(m_destination_relation && !m_destination_node){return "relation";}
        if(m_destination_node && m_destination_relation){
            qDebug()<<"Error: relation origin is both a node and a relation";
            return "error";
        }
        if(!m_destination_node && !m_destination_relation) {
            if(!hovering()){
                qDebug()<<"Error: relation has no destination object";
            }

        }
        return "";
    }

    void updateSelf();

    Node * originNode(){return m_origin_node;}
    Relation * originRelation(){return m_origin_relation;}
    Node * destinationNode(){return m_destination_node;}
    Relation * destinationRelation(){return m_destination_relation;}
    bool hovering(){return m_hovering;}


    void setOrigin(Body::coordinate c);
    void setOrigin(int x,int y);
    void setDestination(Body::coordinate c);
    void setDestination(int x,int y);

    int setRelationCutoff();

    Body::coordinate origin(){return m_origin;}
    Body::coordinate destination(){return m_destination;}
    int width(){return m_width;}
    int height(){return m_height;}

    Body::coordinate worldMidPoint();
    Body::coordinate localMidPoint();

    void updateRelations();
    void registerRelation(Relation * r);
    void registerIncomingRelation(Relation * r);

    void finalizeSelf();
    void initializeObj();
    void createObj();
    void deleteObj();
    QQuickItem * obj(){return m_obj;}

};

#endif // RELATION_H

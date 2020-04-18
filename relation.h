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

    bool isInside(int x,int y);

    enum type{
        node = 0,
        relation = 1,
        ghost = 2
    };
private:


    int tally = 0;


    int m_id;

    bool m_visible = true;
    bool m_hovering = false;
    bool m_highlighted = false;


    /*
    QVector<Relation*> toRelation;
    QVector<Relation*> toRelation_relation;
    QVector<Relation*> fromRelation;
    QVector<Relation*> fromRelation_relation;
    QVector<Relation*> fromNode;
    QVector<Node*> fromNode_node;*/

    int originType;
    int destinationType;

    Node * m_origin_node = nullptr;
    Relation * m_origin_relation = nullptr;
    GhostNode * m_origin_ghost = nullptr;

    Node * m_destination_node = nullptr;
    Relation * m_destination_relation = nullptr;
    GhostNode * m_destination_ghost = nullptr;


    Node * m_displayOrigin = nullptr;
    Node * m_displayDestination = nullptr;

    Body::coordinate m_origin;
    Body::coordinate m_destination;
    int m_width;
    int m_height;

    QQuickItem * m_obj = nullptr;

public:
    void setOriginObject(Node * n);
    void setOriginObject(Relation * r);
    void setOriginObject(GhostNode * g);
    void setDestinationObject(Node * n);
    void setDestinationObject(Relation * r);
    void setDestinationObject(GhostNode * g);
    void clearDestinationObject();
    void setHovering(bool b);

    void determineOriginType();
    void determineDestinationType();
    void determineTypes();

    int getDestinationType(){return destinationType;}
    int getOriginType(){return originType;}

    Node * originNode(){return m_origin_node;}
    Relation * originRelation(){return m_origin_relation;}
    Node * destinationNode(){return m_destination_node;}
    Relation * destinationRelation(){return m_destination_relation;}


    bool isVisible(){return m_visible;}
    void setVisibility(bool visibility);
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

    void setHighlighted(bool b);

    void finalizeSelf();
    void initializeObj();
    void createObj();
    void deleteObj();
    QQuickItem * obj(){return m_obj;}
public slots:
    void updateSelf();
};

#endif // RELATION_H

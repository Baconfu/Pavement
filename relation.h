#ifndef RELATION_H
#define RELATION_H

#include <QObject>
#include <body.h>

class Node;
class BaseNode;

class Relation: public QObject
{
    Q_OBJECT
public:
    Relation(QObject * parent = nullptr);

    int ID(){return m_id;}
    void setID(int n){m_id = n;}

    QString getName();
    void setName(QString s);

    bool isInside(int x,int y);

    enum type{
        node = 0,
        relation = 1,
    };
private:


    int tally = 0;


    int m_id;

    bool m_visible = true;
    bool m_hovering = false;
    bool m_highlighted = false;
    bool m_selected = false;


    /*
    QVector<Relation*> toRelation;
    QVector<Relation*> toRelation_relation;
    QVector<Relation*> fromRelation;
    QVector<Relation*> fromRelation_relation;
    QVector<Relation*> fromNode;
    QVector<Node*> fromNode_node;*/

    int originType;
    int destinationType;

    BaseNode * m_origin_node = nullptr;
    Relation * m_origin_relation = nullptr;

    BaseNode * m_destination_node = nullptr;
    Relation * m_destination_relation = nullptr;

    Body::coordinate m_origin;
    Body::coordinate m_destination;
    int m_width;
    int m_height;

    QQuickItem * m_obj = nullptr;

public:
    void setOriginObject(BaseNode * n);
    void setOriginObject(Relation * r);
    void setDestinationObject(BaseNode * n);
    void setDestinationObject(Relation * r);



    void clearDestinationObject();
    void setHovering(bool b);

    void determineOriginType();
    void determineDestinationType();
    void determineTypes();

    int getDestinationType(){return destinationType;}
    int getOriginType(){return originType;}

    BaseNode * originNode(){return m_origin_node;}
    Relation * originRelation(){return m_origin_relation;}
    BaseNode * destinationNode(){return m_destination_node;}
    Relation * destinationRelation(){return m_destination_relation;}


    bool isVisible(){return m_visible;}
    void setVisibility(bool visibility);
    bool hovering(){return m_hovering;}


    void setOrigin(Body::coordinate c);
    void setOrigin(int x,int y);
    void setDestination(Body::coordinate c);
    void setDestination(int x,int y);

    void syncCutoff();
    Body::coordinate calculateRelationCutoff(BaseNode * origin, BaseNode * destination);
    void setDestinationCutoff(Body::coordinate c);
    void setOriginCutoff(Body::coordinate c);

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
    void setSelected(bool b);

    void finalizeSelf();
    void initializeObj();
    void createObj();

    QQuickItem * obj(){return m_obj;}
public slots:
    void updateSelf();
    void destroy();
};

#endif // RELATION_H

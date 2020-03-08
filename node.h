#ifndef NODE_H
#define NODE_H

#include <QObject>
#include <relation.h>
#include <body.h>
#include <structural.h>


class Node: public QObject
{
    Q_OBJECT
    Q_PROPERTY(int m_absX READ absX WRITE setAbsX NOTIFY absXChanged)
    Q_PROPERTY(int m_absY READ absY WRITE setAbsY NOTIFY absYChanged)
public:
    Node(QObject * parent = nullptr);

    int ID(){return m_id;}
    void setID(int id);

    QString name(){
        m_name = obj()->findChild<QObject*>("textInput")->property("text").toString();
        return m_name;
    }
    void setName(QString name);

    int absX() {return m_absX;}
    void setAbsX(int x) {
        m_absX = x;
        obj()->setProperty("absX",x);
        updateRelations();
    }

    int absY() {return m_absY;}
    void setAbsY(int y){
        m_absY = y;
        obj()->setProperty("absY",y);
        updateRelations();
    }

    Body::coordinate centerPosition(){
        setCenterPosition();
        return m_centerPosition;
    }
    void setCenterPosition(Body::coordinate c){
        m_centerPosition = c;
    }
    void setCenterPosition(){
        m_centerPosition.x = absX() + width() / 2;
        m_centerPosition.y = absY() + height() / 2;

    }

    QVector<Node*> getParents(){return m_parents;}
    int addParent(Node * n);
    void registerParent(Node * n);
    void removeParent(Node * n);
    bool parentExists(Node * n);


    QVector<Node*> ancestorPath(Node * target);
    QVector<Node*> ancestorPathSet(Node * target);

    QVector<Node*> getIncludes(){return includes;}
    void setIncludes(QVector<Node*> path){includes = path;}
    void include(Node * n);



    QVector<Node*> getChildren(){return m_children;}
    int addChild(Node * n);
    void registerChild(Node * n);
    void removeChild(Node * n);
    bool childExists(Node * n);


    QString typeName(){return m_type;}

    Node * getType(){return m_typeNode;}
    void setType(Node * n);

    void refreshWidthHeight(){

        m_width = obj()->property("width").toInt();
        m_height = obj()->property("height").toInt();
    }
    int width(){refreshWidthHeight(); return m_width;}
    int height(){refreshWidthHeight(); return m_height;}



    QVector<Node*> members(){return m_members;}
    int registerMember(Node * n);
    void removeMember(Node * n);
    bool memberExists(Node * n);



    QVector<Relation*> getAllRelations();
    void registerRelation(Relation * r);
    void registerIncomingRelation(Relation * r);

    void deleteRelationByTarget(Node * n);
    void deleteAllRelations();

    QVector<structural*> getAllStructurals(){return toParent;}
    structural * newStructural();
    structural * hoveringStructural(){return m_hoveringStructural;}
    void setHoveringStructural(structural * s){m_hoveringStructural = s;}

    void giveInputFocus();
    void giveTypeInputFocus();

    void initializeObj(); //just creates the obj and sets it to m_obj
    void createObj(); //sets all attributes of m_obj based on Node
    void deleteObj();
    QQuickItem * obj(){return m_obj;}

    bool isVisible(){return m_visible;}
    void setVisibility(bool visibility);

    bool hidden(){return m_hidden;}
    void setHidden(bool b);

    bool isDissolved(){return m_dissolve;}
    void dissolve();
    void distill();


    void highlight(bool visible);

    void preventFocus(bool b);
    bool preventingFocus(){return m_preventFocus;}


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




    QVector<Relation*> toNode;
    QVector<Node*> toNode_node;
    QVector<Relation*> fromNode;
    QVector<Node*> fromNode_node;
    QVector<Relation*> toRelation;
    QVector<Relation*> toRelation_relation;



    void updateRelations();
    void setStyle();
    Body::style getStyle(){return m_style;}
    int m_id;
    QString m_name;

    int m_absX;
    int m_absY;
    int m_width;
    int m_height;
    Body::coordinate m_centerPosition;

    QVector<Node*> m_parents;
    QVector<Node*> m_children;
    QVector<structural*> toParent;

    QVector<Node*> includes;

    structural * m_hoveringStructural = nullptr;


    QString m_type;
    Node * m_typeNode = nullptr;
    QVector<Node*> m_members;

    Body::style m_style;

    bool m_visible = true;

    bool m_hidden = false;
    bool m_dissolve = false;

    QQuickItem * m_obj = nullptr;

    bool m_preventFocus = false;


public:
    bool isInside(int x, int y);
    void hoverSelect(int y);

signals:
    void absXChanged();
    void absYChanged();

    void updateStructural();
public slots:
    void getWidthFromObj(){
        m_width = obj()->property("width").toInt();
    }
    void getHeightFromObj(){
        m_height = obj()->property("height").toInt();
    }

    void inputAccepted();
    void typeInputAccepted(QString s);
};

#endif // NODE_H

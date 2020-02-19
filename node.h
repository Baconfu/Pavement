#ifndef NODE_H
#define NODE_H

#include <QObject>
#include <relation.h>
#include <body.h>


class Node: public QObject
{
    Q_OBJECT
    Q_PROPERTY(int m_absX READ absX WRITE setAbsX NOTIFY absXChanged)
    Q_PROPERTY(int m_absY READ absY WRITE setAbsY NOTIFY absYChanged)
public:
    Node(QObject * parent = nullptr);


    void setID(int id);
    void setName(QString name);
    void setAbsX(int x) {
        m_absX = x;
        obj()->setProperty("absX",x);
        updateRelations();
    }
    void setAbsY(int y){
        m_absY = y;
        obj()->setProperty("absY",y);
        updateRelations();
    }

    void setCenterPosition(Body::coordinate c){
        m_centerPosition = c;
    }
    void setCenterPosition(){
        m_centerPosition.x = absX() + width() / 2;
        m_centerPosition.y = absY() + height() / 2;

    }


    void setParent(Node * n);
    void unParent();

    void registerChild(Node * n);
    void unRegisterChild(Node * n);

    void setType(Node * n);
    void findType(QString s);


    int ID(){return m_id;}
    QString name(){
        m_name = obj()->findChild<QObject*>("textInput")->property("text").toString();
        return m_name;
    }
    int absX() {return m_absX;}
    int absY() {return m_absY;}
    void refreshWidthHeight(){
        m_width = obj()->property("width").toInt();
        m_height = obj()->property("height").toInt();
    }
    int width(){refreshWidthHeight(); return m_width;}
    int height(){refreshWidthHeight(); return m_height;}

    Node * getParent(){return m_parent;}

    QVector<Node*> getChildren(){return m_children;}
    Node * findChildByID(int id);
    Node * findChildByObject(Node * n);
    int findChildLocalIDByObject(Node * n);

    Node * getType(){return m_typeNode;}
    QString type(){return m_type;}

    Body::coordinate centerPosition(){
        setCenterPosition();
        return m_centerPosition;
    }

    QVector<Relation*> getAllRelations();
    void registerRelation(Relation * r);
    void registerIncomingRelation(Relation * r);

    void deleteRelationByLocalID(int id);
    void deleteRelationByTarget(Node * n);
    void deleteAllRelations();

    void giveInputFocus();
    void giveTypeInputFocus();

    void initializeObj(); //just creates the obj and sets it to m_obj
    void createObj(); //sets all attributes of m_obj based on Node
    void deleteObj();
    QQuickItem * obj(){return m_obj;}

    void initializeStructural();
    void deleteStructural();
    QQuickItem * getStructural(){return m_structural;}
    void updateStructural();
    void setStructuralOrigin(Body::coordinate c);
    void setStructuralDestination(Body::coordinate c);
    Body::coordinate structuralOrigin(){return m_structuralOrigin;}
    Body::coordinate structuralDestination(){return m_structuralDestination;}
    void setStructuralCutoff();

    void setHoveringStructural(bool b);
    bool hoveringStructural(){return m_hoveringStructural;}


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

    Node * m_parent = nullptr;
    QVector<Node*> m_children;

    QString m_type;
    Node * m_typeNode = nullptr;

    Body::style m_style;

    QQuickItem * m_obj = nullptr;

    QQuickItem * m_structural = nullptr;
    bool m_hoveringStructural = false;
    Body::coordinate m_structuralOrigin;
    Body::coordinate m_structuralDestination;

    bool m_preventFocus = false;


public:
    bool isInside(int x, int y);
    void hoverSelect(int y);

signals:
    void absXChanged();
    void absYChanged();
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

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
    void removeParent(Node * n);

    QVector<Node*> ancestorPath(Node * target);

    QVector<Node*> getChildren(){return m_children;}
    int addChild(Node * n);
    void removeChild(Node * n);

    void getDescendants(QVector<Node*> * out){
        if(!out->contains(this)){
            out->append(this);
            for(int i=0; i<m_children.length(); i++){
                m_children[i]->getDescendants(out);
            }
            return;
        }
    }

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

    void setUnderMap(QVector<Node*> nodes);
    QVector<Node*> getUnderMap(){return m_underMap;}

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


    QVector<Node*> m_underMap;



    //Foreign diplomacy
    QVector<Relation*> toNode;
    QVector<Node*> toNode_node;
    QVector<Relation*> fromNode;
    QVector<Node*> fromNode_node;
    QVector<Relation*> toRelation;
    QVector<Relation*> toRelation_relation;

    QString m_type;
    Node * m_typeNode = nullptr;
    QVector<Node*> m_members;

    //vassalage diplomacy
    QVector<Node*> m_parents;
    QVector<Node*> m_children;
    QVector<structural*> toParent;


    //Geopolitical information.
    int m_absX;
    int m_absY;
    int m_width;
    int m_height;
    Body::coordinate m_centerPosition;


    //Domestic policy
    void setStyle();
    Body::style getStyle(){return m_style;}
    int m_id;
    QString m_name;


    structural * m_hoveringStructural = nullptr;


    Body::style m_style;

    //STATE VARIABLES

    bool m_visible = true;
    bool m_hidden = false;
    bool m_dissolve = false;
    bool m_expanded = false;

    bool m_preventFocus = false;

    QQuickItem * m_obj = nullptr;




public:
    bool isInside(int x, int y);
    void hoverSelect(int y);

signals:
    void absXChanged();
    void absYChanged();

    void updateStructural();
    void updateRelation();
public slots:
    void getWidthFromObj(){
        m_width = obj()->property("width").toInt();
    }
    void getHeightFromObj(){
        m_height = obj()->property("height").toInt();
    }

    void inputAccepted();
    void typeInputAccepted(QString s);
    void updateRelations();
};

#endif // NODE_H

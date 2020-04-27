#ifndef NODE_H
#define NODE_H

#include <QObject>
#include <relation.h>
#include <body.h>
#include <structural.h>
#include <basenode.h>


class Node: public BaseNode
{
    Q_OBJECT

public:
    Node(QObject * parent = nullptr, int id = -1);

    Node * getNodePointer(){return this;}


    QString derivedType(){return "node";}

    QString getName();
    void setName(QString name);

    void setPosition(Body::coordinate c);
    Body::coordinate getPosition();
    Body::coordinate getCenterPosition();
    int getX(){return getPosition().x;}
    int getY(){return getPosition().y;}

    int getID(){return m_id;}

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


    QVector<GhostNode*> m_ghosts;
    void registerGhost(GhostNode * n){m_ghosts.append(n);}
    int allocateGhostID(){return m_ghosts.length();}
    GhostNode * getGhostByID(int id);
    QVector<GhostNode*> getGhosts(){return m_ghosts;}
    GhostNode * newGhostNode();

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

    void setUnderMap(QVector<BaseNode*> nodes);
    QVector<BaseNode*> getUnderMap(){return m_underMap;}

    void setAbstraction(BaseNode * n);

    void expand();
    void abstract();

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
    void syncCenterPosition(){
        Body::coordinate c;
        c.x = m_position.x + m_width/2;

        c.y = m_position.y + m_height/2;
        m_centerPosition = c;
    }



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


    QVector<BaseNode*> m_underMap;

    BaseNode * m_abstraction = nullptr;


    //Foreign diplomacy
    QVector<Relation*> toNode;
    QVector<BaseNode*> toNode_node;
    QVector<Relation*> fromNode;
    QVector<BaseNode*> fromNode_node;
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
    int m_width;
    int m_height;
    void setCenterPosition(Body::coordinate c){
        m_centerPosition = c;
    }



    //Domestic policy
    void setStyle();

    structural * m_hoveringStructural = nullptr;


    Body::style m_style;

    //STATE VARIABLES

    bool m_visible = true;
    bool m_hidden = false;
    bool m_dissolve = false;
    bool m_expanded = false;

    bool m_preventFocus = false;




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
    void inputPassivelyAccepted();
    void typeInputAccepted(QString s);
    void typeInputPassivelyAccepted(QString s);
    void updateRelations();
};

#endif // NODE_H

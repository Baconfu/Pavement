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

    void setID(int id){m_id = id;}


    //TRANSFORM: SETTING FUNCTIONS
    void transform(Body::coordinate c);
    void transformIgnoreSubMap(Body::coordinate c);
    void setPosition(Body::coordinate c);
    void setPositionIgnoreSubMap(Body::coordinate c);
    void setPositionByCenter(Body::coordinate c);
    void setPositionByCenterIgnoreSubMap(Body::coordinate c);


    //TRANSFORM: GETTING FUNCTIONS
    Body::coordinate getPosition();
    Body::coordinate getCenterPosition();
    Body::coordinate getCenterAbsolutePosition(){
        Body::coordinate c;
        c.x = m_absolutePosition.x + m_width/2;
        c.y = m_absolutePosition.y + m_height/2;
        return c;
    }
    Body::coordinate getAbsolutePosition(){updateAbsolutePosition(); return m_absolutePosition;}
    int getX(){return getPosition().x;}
    int getY(){return getPosition().y;}


    //NODE DIMENSIONS: GETTING FUNCTIONS
    int width(){return m_width;}
    int height(){return m_height;}


    //NODE INFORMATION: GETTING FUNCTIONS
    int getID(){return m_id;}


    //NODE PARENTS AND CHILDREN
    QVector<Node*> getParents(){return m_parents;}
    int addParent(BaseNode * n);
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


    //NODE GHOSTS
    void registerGhost(GhostNode * n){m_ghosts.append(n);}
    void unregisterGhost(GhostNode *n){m_ghosts.removeOne(n);}
    int allocateGhostID();
    GhostNode * getGhostByID(int id);
    QVector<GhostNode*> getGhosts(){return m_ghosts;}
    GhostNode * newGhostNode();


    //NODE TYPES AND MEMBERS
    QString typeName(){return m_type;}

    Node * getType(){return m_typeNode;}
    QString getTypeName(){return m_type;}
    void setType(Node * n);

    QVector<Node*> members(){return m_members;}
    int registerMember(Node * n);
    void removeMember(Node * n);
    bool memberExists(Node * n);

    //NODE SUBMAP
    void setUnderMap(QVector<BaseNode*> nodes);
    void underMapAppendNode(QVector<BaseNode*> nodes);
    void underMapAppendNode(BaseNode * node);
    QVector<BaseNode*> getUnderMap(){return m_underMap;}
    void removeSubNode(BaseNode * b);
    void transformSubMap(Body::coordinate vector);

    void subNodeMoved();
    void reFormatExpandedForm();

    void setAbstraction(BaseNode * n);

    void expand();
    bool isExpanded(){return m_expanded;}
    void abstract();
    void exude(BaseNode * b);



    //NODE RELATIONS
    QVector<Relation*> getAllRelations();
    void registerRelation(Relation * r);
    void registerIncomingRelation(Relation * r);
    void deleteRelationByTarget(Node * n);
    void deleteAllRelations();

    //NODE STRUCTURALS
    QVector<structural*> getAllStructurals(){return toParent;}
    structural * newStructural();
    structural * hoveringStructural(){return m_hoveringStructural;}
    void setHoveringStructural(structural * s){m_hoveringStructural = s;}

    //NODE CONTROLS
    void giveInputFocus();
    void giveTypeInputFocus();

    //NODE STATE VARIABLES
    void moving(bool b);
    bool isMoving(){return m_moving;}

    bool isVisible(){return m_visible;}
    void setVisibility(bool visibility);

    bool hidden(){return m_hidden;}
    void setHidden(bool b);

    bool isDissolved(){return m_dissolve;}
    void dissolve();
    void distill();

    void highlight(bool visible);
    void hover(bool b);
    void select(bool b);
    //Implement highlight state controllers
    void preventFocus(bool b);
    bool preventingFocus(){return m_preventFocus;}



    void initializeObj(); //just creates the obj and sets it to m_obj

    void deleteObj();
    void destroy();
    QQuickItem * obj(){return m_obj;}



private:
    int tally = 0;
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


    QVector<GhostNode*> m_ghosts;

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

    bool m_batchSelected = false;
    bool m_hover = false;
    bool m_visible = true;
    bool m_hidden = false;
    bool m_dissolve = false;
    bool m_expanded = false;
    bool m_moving = false;

    bool m_preventFocus = false;




public:
    BaseNode * isInside(int x, int y);

    void updateAbsolutePosition();

    void hoverSelect(int y);

signals:


    void updateStructural();
    void updateRelation();
    void terminate();
public slots:
    void widthChanged();
    void heightChanged();


    void mouseClicked();
    void mousePressed();
    void mouseReleased();

    void geometryChanged();

    void inputAccepted();
    void inputPassivelyAccepted();
    void typeInputAccepted(QString s);
    void typeInputPassivelyAccepted(QString s);
    void updateRelations();
};

#endif // NODE_H

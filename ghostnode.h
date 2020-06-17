#ifndef GHOSTNODE_H
#define GHOSTNODE_H

#include <node.h>
#include <QObject>
#include <QQuickItem>
#include <body.h>



class GhostNode: public BaseNode
{
    Q_OBJECT
public:
    GhostNode(Node * original = nullptr,QObject * parent = nullptr);


    void setID(int id){m_id = id;}
    GhostNode * getGhostPointer(){return this;}

    void updateAbsolutePosition();

    void transform(Body::coordinate c);
    void transformSubMap(Body::coordinate c);
    void transformIgnoreSubMap(Body::coordinate c);
    void setPosition(Body::coordinate c);
    void setPositionByCenter(Body::coordinate c);
    void setPositionByCenterIgnoreSubMap(Body::coordinate c);
    Body::coordinate getPosition();
    Body::coordinate getAbsolutePosition(){updateAbsolutePosition();return m_absolutePosition;}
    Body::coordinate getCenterAbsolutePosition(){
        Body::coordinate c;
        updateAbsolutePosition();
        c.x = m_absolutePosition.x + m_width/2;
        c.y = m_absolutePosition.y + m_height/2;
        return c;
    }
    QString getName(){m_name = m_original->getName(); return m_name;}
    void setName(QString name);


    bool typeVisible(){return m_obj->findChild<QObject*>("typeName")->property("visible").toBool();}

    void setText(QString s);
    QString getText();

    Body::coordinate getCenterPosition();
    int getX(){return m_position.x;}
    int getY(){return m_position.y;}



    int width(){return m_width;}
    int height(){return m_height;}
    int displayWidth(){return m_width;}
    int displayHeight();

    int getID(){return m_id;}

    QVector<BaseNode*> getUnderMap(){return m_underMap;}
    void setUnderMap(QVector<BaseNode*> subMap);
    void underMapAppendNode(BaseNode * b);
    void appendToUnderMap(BaseNode * b);
    void syncOriginal(BaseNode * b);
    void removeSubNode(BaseNode * b){m_underMap.removeOne(b);}
    bool underMapContains(BaseNode * b);

    void subNodeMoved();
    void reFormatExpandedForm();

    void cloneSubMap(BaseNode * b);

    void expandMap();
    void expandTree();
    void expandImage();
    void expandText();

    bool clickAction();


    void cycleExpandState(int state);

    void abstract();
    void expand();
    int isExpanded(){return m_expanded;}
    void extract();
    void exude(BaseNode * b);

    void setAbstraction(BaseNode * n);
    BaseNode * getAbstraction(){return m_abstraction;}

    void shiftSubMap(Body::coordinate vector);

    bool isMoving(){return m_moving;}
    void moving(bool b){m_moving = b;}

    void selectTextBox();

    void destroy();
    void initializeObj();
    void adoptOriginal();


    BaseNode * isInside(int x,int y);


    void select(bool b);
    bool selected(){return m_batchSelected;}

    void hover(bool b);
    bool hoverSelected(){return m_hoverSelected;}

    void preventFocus(bool b){
        m_preventFocus = b;
    }
    bool preventingFocus(){return m_preventFocus;}
    void highlight(bool b);

    Node * getOriginal(){return m_original;}

    void setVisibility(bool b);
    bool isVisible(){return m_visible;}



private:
    void selectTextBox(bool b);
    bool textBoxSelected(){return m_obj->findChild<QObject*>("expandedText")->property("focus").toBool();}
    int tally = 0;
    Node * m_original;

    bool m_preventFocus = false;

    int m_width = 10;
    int m_height = 10;


    int m_expanded = -1;
    bool m_batchSelected = false;
    bool m_hoverSelected = false;
    bool m_moving = false;

    Body::coordinate m_localVector;

    bool m_visible = true;

    bool markedForDestruction = false;

    QVector<BaseNode*> m_underMap;
    BaseNode * m_abstraction = nullptr;
signals:

    void updateRelation();
    void terminate();
public slots:
    void mouseClicked();
    void mousePressed();
    void mouseReleased();


    void geometryChanged();
    void widthChanged();
    void heightChanged();
};

#endif // GHOSTNODE_H

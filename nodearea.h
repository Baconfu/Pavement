#ifndef NODEAREA_H
#define NODEAREA_H

#include <QObject>
#include <body.h>
#include <node.h>
#include <basenode.h>

class NodeArea : public BaseNode
{
    Q_OBJECT
public:
    NodeArea(QObject * parent = nullptr);

    NodeArea * getAreaPointer(){return this;}


    int getID(){return m_id;}
    void setID(int id){m_id = id;}

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

    void updateAbsolutePosition();


    //NODE DIMENSIONS: GETTING FUNCTIONS
    int width(){return m_width;}
    int height(){return m_height;}

    BaseNode * isInside(int x,int y);

    //NODE INFORMATION: GETTING FUNCTIONS


    void transform(Body::coordinate c);
    void transformSubMap(Body::coordinate vector);
    void transformIgnoreSubMap(Body::coordinate c);

    void setPosition(Body::coordinate c);
    void setPositionByCenter(Body::coordinate c);

    QVector<BaseNode*> getUnderMap(){return m_underMap;}
    void setUnderMap(QVector<BaseNode*> nodes);
    void underMapAppendNode(BaseNode * b);
    void removeSubNode(BaseNode * b){
        m_underMap.removeOne(b);
    }

    void subNodeMoved();
    void reFormatExpandedForm();

    bool isExpanded(){return m_expanded;}

    void setAbstraction(BaseNode * b){m_abstraction = b;}
    BaseNode * getAbstraction(){return m_abstraction;}

    void hover(bool b);
    void select(bool b);
    void highlight(bool b);

    void moving(bool b){m_moving = b;}
    bool isMoving(){return m_moving;}

    bool isVisible(){return m_visible;}
    void setVisibility(bool b){m_visible = b; m_obj->setProperty("visible",b);}
    void initializeObj();
    QQuickItem * obj(){return m_obj;}

private:

    int m_width = 0;
    int m_height = 0;

    bool m_expanded = true;
    bool m_visible = true;
    bool m_moving = false;

    BaseNode * m_abstraction = nullptr;

    bool m_hoverSelected = false;
    bool m_batchSelected = false;
    QVector<BaseNode*> m_underMap;

signals:
    void updateRelation();

public slots:
    void widthChanged();
    void heightChanged();
};

#endif // NODEAREA_H

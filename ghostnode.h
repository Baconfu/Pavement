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

    void setPosition(Body::coordinate c);
    Body::coordinate getPosition();
    QString getName(){return m_name;}
    void setName(QString name);


    Body::coordinate getCenterPosition();
    int getX(){return m_position.x;}
    int getY(){return m_position.y;}

    void calculateLocalVector();
    Body::coordinate getLocalVector();
    void setPositionByLocalVector();
    void imposeLocalVector(Body::coordinate c);

    int width(){return m_width;}
    int height(){return m_height;}

    int getID(){return m_id;}

    void abstract();
    void expand();

    void setAbstraction(BaseNode * n);
    BaseNode * getAbstraction(){return m_abstraction;}

    void initializeObj();
    void adoptOriginal();

    bool isInside(int x,int y);


    void setSelected(bool b);
    bool selected(){return m_batchSelected;}

    void setHover(bool b);
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
    Node * m_original;

    bool m_preventFocus = false;

    int m_width = 10;
    int m_height = 10;

    bool m_batchSelected = false;
    bool m_hoverSelected = false;

    Body::coordinate m_localVector;

    bool m_visible = true;

    QVector<BaseNode*> m_underMap;
    BaseNode * m_abstraction = nullptr;
signals:
    void updateRelation();
public slots:
    void getWidth(){m_width = m_obj->property("width").toInt();}
    void getHeight(){m_height = m_obj->property("height").toInt();}
};

#endif // GHOSTNODE_H

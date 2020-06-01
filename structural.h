#ifndef STRUCTURAL_H
#define STRUCTURAL_H

#include <QObject>
#include <QQuickItem>
#include <body.h>

class Node;

class structural: public QObject
{
    Q_OBJECT
public:
    structural(QObject * parent = nullptr);

    BaseNode * childNode(){return m_childNode;}
    BaseNode * parentNode(){return m_parentNode;}
    void setChildNode(BaseNode * n);
    void setParentNode(BaseNode * n);

    bool isInside(int x,int y);


    void setOrigin(Body::coordinate c);
    void setDestination(Body::coordinate c);
    Body::coordinate origin(){return m_origin;}
    Body::coordinate destination(){return m_destination;}
    void setStructuralCutoff();

    void setHovering(bool b);
    bool hovering(){return m_hovering;}

    void setHighlighted(bool b);
    bool highlighted(){return m_highlighted;}

    void setSelected(bool b);
    bool isSelected(){return m_selected;}

    void setVisibility(bool b);
    bool visible(){return m_visible;}

    QQuickItem * obj(){return m_obj;}
    void initializeObj();


private:

    bool m_visible = true;
    bool m_hovering = false;

    bool m_highlighted = false;
    bool m_selected = false;

    Body::coordinate m_origin;
    Body::coordinate m_destination;

    BaseNode * m_childNode;
    BaseNode * m_parentNode;


    QQuickItem * m_obj = nullptr;

    int tally = 0;
public slots:
    void update();
    void destroy();
};

#endif // STRUCTURAL_H

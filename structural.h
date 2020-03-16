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

    Node * childNode(){return m_childNode;}
    Node * parentNode(){return m_parentNode;}
    void setChildNode(Node * n);
    void setParentNode(Node * n);

    Node * displayChildNode(){return m_displayChildNode;}
    Node * displayParentNode(){return m_displayParentNode;}
    void setDisplayChildNode(Node * n);
    void setDisplayParentNode(Node * n);

    Node * findDisplayParentNode();
    Node * findDisplayChildNode();

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

    void setVisibility(bool b);
    bool visible(){return m_visible;}

    QQuickItem * obj(){return m_obj;}
    void initializeObj();
    void deleteObj();

private:
    QVector<Node*> parentAncestorPath();
    QVector<Node*> childAncestorPath();
    bool m_visible = true;
    bool m_hovering = false;

    bool m_highlighted = false;

    Body::coordinate m_origin;
    Body::coordinate m_destination;

    Node * m_childNode;
    Node * m_parentNode;

    Node * m_displayChildNode;
    Node * m_displayParentNode;

    QQuickItem * m_obj = nullptr;

    int tally = 0;
public slots:
    void update();
};

#endif // STRUCTURAL_H

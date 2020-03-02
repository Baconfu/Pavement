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



    void update();
    void setOrigin(Body::coordinate c);
    void setDestination(Body::coordinate c);
    Body::coordinate origin(){return m_origin;}
    Body::coordinate destination(){return m_destination;}
    void setStructuralCutoff();

    void setHovering(bool b);
    bool hovering(){return m_hovering;}


    QQuickItem * obj(){return m_obj;}
    void initializeObj();
    void deleteObj();

private:

    bool m_hovering = false;

    Body::coordinate m_origin;
    Body::coordinate m_destination;

    Node * m_childNode;
    Node * m_parentNode;

    QQuickItem * m_obj = nullptr;

};

#endif // STRUCTURAL_H

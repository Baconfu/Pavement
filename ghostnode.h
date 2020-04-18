#ifndef GHOSTNODE_H
#define GHOSTNODE_H

#include <node.h>
#include <QObject>
#include <QQuickItem>
#include <body.h>



class GhostNode: public QObject
{
    Q_OBJECT
public:
    GhostNode(Node * original = nullptr,QObject * parent = nullptr);

    int getX(){return m_x;}
    int getY(){return m_y;}
    void setX(int x);
    void setY(int y);

    Body::coordinate getCenterPosition();

    void initializeObj();
    void adoptOriginal();

    bool isInside(int x,int y);

    void highlight(bool b);

private:
    Node * m_original;



    int m_x = 0;
    int m_y = 0;
    int m_width = 10;
    int m_height = 10;




    QQuickItem * m_obj = nullptr;
public slots:
    void getWidth(){m_width = m_obj->property("width").toInt();}
    void getHeight(){m_height = m_obj->property("height").toInt();}
};

#endif // GHOSTNODE_H

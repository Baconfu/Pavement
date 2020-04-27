#ifndef BASENODE_H
#define BASENODE_H

#include <QObject>
#include <QQuickItem>
#include <body.h>
#include <QString>
#include <QDebug>

class Relation;
class BaseNode: public QObject
{
    Q_OBJECT
public:
    BaseNode(QObject * parent = nullptr,int id = -1);


    virtual Body::coordinate getPosition();
    virtual Body::coordinate getCenterPosition();
    virtual Body::coordinate getLocalCenterPosition();
    virtual int getX();
    virtual int getY();

    virtual int width();
    virtual int height();

    virtual void registerRelation(Relation * r);
    virtual void registerIncomingRelation(Relation * r);

    virtual void dissolve();

    virtual void setID(int id);
    virtual int getID();

    virtual void highlight(bool b);


    virtual QString derivedType(){return "base";}

    virtual Node * getNodePointer(){return nullptr;}
    virtual GhostNode * getGhostPointer(){return nullptr;}

    virtual void abstract(){}
    virtual void expand(){}

    virtual void setAbstraction(BaseNode * b){qDebug()<<"BaseNode virtual function called. "<<b;}
    virtual BaseNode * getAbstraction();

    virtual void setVisibility(bool visibility);
    virtual bool isVisible(){return true;}

    virtual bool isInside(int x,int y){qDebug()<<"BaseNode virtual function called. "<< x<<y; return false;}

    virtual void calculateLocalVector(){}
    virtual Body::coordinate getLocalVector();
    virtual void setPositionByLocalVector(){}
    virtual void imposeLocalVector(Body::coordinate c){qDebug()<<"BaseNode virtual function called. "<<c.x<<c.y;}

protected:
    int m_id;

    QString m_name;



    Body::coordinate m_position;
    Body::coordinate m_centerPosition;

    QQuickItem * m_obj;
};

#endif // BASENODE_H

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


    virtual void setPosition(Body::coordinate){}
    virtual void geometryChanged(){}

    virtual Body::coordinate getPosition();
    virtual Body::coordinate getAbsolutePosition(){return m_absolutePosition;}
    virtual Body::coordinate getCenterPosition();
    virtual Body::coordinate getCenterAbsolutePosition();
    virtual Body::coordinate getLocalCenterPosition();
    virtual int getX();
    virtual int getY();

    virtual QString getName(){QString s; return s;}

    virtual void transform(Body::coordinate){}
    virtual void transformIgnoreSubMap(Body::coordinate){}

    virtual int width();
    virtual int height();

    virtual void registerRelation(Relation * r);
    virtual void registerIncomingRelation(Relation * r);

    virtual void dissolve();

    virtual void setID(int id);
    virtual int getID();


    virtual void hover(bool b){qDebug()<<b;}
    virtual void select(bool){}
    virtual void highlight(bool b);


    virtual QString derivedType(){return "base";}

    virtual Node * getNodePointer(){return nullptr;}
    virtual GhostNode * getGhostPointer(){return nullptr;}
    virtual NodeArea * getAreaPointer(){return nullptr;}

    virtual void abstract(){}
    virtual void expand(){}

    virtual QVector<BaseNode*> getUnderMap(){QVector<BaseNode*> null; return null;}
    virtual void underMapAppendNode(BaseNode*){}
    virtual void setUnderMap(QVector<BaseNode*>){}
    virtual void removeSubNode(BaseNode * b){qDebug()<<b;}

    virtual void subNodeMoved(){}
    virtual void reFormatExpandedForm(){}
    virtual bool isExpanded(){return false;}

    virtual void setAbstraction(BaseNode * b){qDebug()<<"BaseNode virtual function called. "<<b;}
    virtual BaseNode * getAbstraction();
    virtual void extract(){} // extract node from Abstraction.
    virtual void exude(BaseNode * b){qDebug()<<"Basenode virtual function called. "<<b;} //exude node from underMap

    virtual void setVisibility(bool visibility);
    virtual bool isVisible(){return true;}

    virtual BaseNode * isInside(int x,int y){qDebug()<<"BaseNode virtual function called. "<< x<<y; return nullptr;}

    virtual void updateAbsolutePosition(){}

    virtual void moving(bool){}
    virtual bool isMoving(){return false;}


    virtual void initializeObj(){}
    virtual void destroy(){}
    virtual QQuickItem * obj(){return m_obj;}



protected:
    int m_id;

    QString m_name;



    Body::coordinate m_position;
    Body::coordinate m_absolutePosition;
    Body::coordinate m_centerPosition;

    QQuickItem * m_obj;
};

#endif // BASENODE_H

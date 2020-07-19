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

    //SETTING NODE GEOMETRY
    virtual void setPosition(Body::coordinate){}
    virtual void geometryChanged(){}
    virtual Body::coordinate getPosition();
    virtual Body::coordinate getAbsolutePosition(){return m_absolutePosition;}
    virtual void updateAbsolutePosition(){}
    virtual Body::coordinate getCenterPosition();
    virtual Body::coordinate getCenterAbsolutePosition();
    virtual Body::coordinate getLocalCenterPosition();
    virtual int getX();
    virtual int getY();
    virtual int displayX(){return getX();}
    virtual int displayY(){return getY();}
    virtual void setPositionBeforeDragged(Body::coordinate c){m_positionBeforeDragged = c;}
    virtual Body::coordinate getPositionBeforeDragged(){return m_positionBeforeDragged;}
    virtual void returnToPositionBeforeDragged(){setPosition(m_positionBeforeDragged);}

    virtual void transform(Body::coordinate){}
    virtual void transformIgnoreSubMap(Body::coordinate){}

    virtual int width();
    virtual int height();
    virtual int displayHeight(){return 0;}
    virtual int displayWidth(){return 0;}

    //NODE INFORMATION
    virtual void setID(int id);
    virtual int getID();

    virtual void setText(QString){}
    virtual QString getText(){return QString("Basenode function called");}

    virtual QString getName(){QString s; return s;}
    virtual QString getTypeName(){return "NULL";}

    virtual Body::response isInside(int,int){qDebug()<<"BaseNode virtual function called. "; return Body::response();}

    virtual bool clickAction(){return true;}

    //NODE RELATIONS
    virtual void registerRelation(Relation * r);
    virtual void registerIncomingRelation(Relation * r);

    //NODE FUNCTIONS
    virtual void dissolve();

    virtual void hover(bool b){qDebug()<<b;}
    virtual void select(bool){}
    virtual void highlight(bool b);

    virtual Node * getNodePointer(){return nullptr;}
    virtual GhostNode * getGhostPointer(){return nullptr;}
    virtual NodeArea * getAreaPointer(){return nullptr;}

    //EXPANDED FORM
    virtual void abstract(){}
    virtual void expand(){}
    virtual void cloneSubMap(BaseNode*){}
    virtual void cycleExpandState(int){}

    virtual void selectExpandedTextBox(bool){}

    virtual QVector<BaseNode*> getUnderMap(){QVector<BaseNode*> null; return null;}
    virtual void underMapAppendNode(BaseNode*){}
    virtual void setUnderMap(QVector<BaseNode*>){}
    virtual void removeSubNode(BaseNode *){}
    virtual bool underMapContains(BaseNode*){return false;}

    virtual void subNodeMoved(){}
    virtual void reFormatExpandedForm(){}
    virtual bool isExpanded(){return false;}
    int expandState(){return m_expandState;}

    virtual void setAbstraction(BaseNode * b){qDebug()<<"BaseNode virtual function called. "<<b;}
    virtual BaseNode * getAbstraction();
    virtual bool abstractionExists(BaseNode *){return false;}
    virtual BaseNode * getHighestAbstraction(){return this;}

    virtual void extract(){} // extract node from Abstraction.
    virtual void exude(BaseNode * b){qDebug()<<"Basenode virtual function called. "<<b;} //exude node from underMap


    //NODE STATUS
    virtual void setVisibility(bool visibility);
    virtual bool isVisible(){return true;}

    virtual void preventFocus(bool b){m_preventFocus = b;}
    virtual bool preventingFocus(){return m_preventFocus;}

    virtual void moving(bool){}
    virtual bool isMoving(){return false;}

    //NODE OBJECT

    virtual void initializeObj(){}
    virtual void destroy(){}
    virtual QQuickItem * obj(){return m_obj;}


protected:
    int m_id;
    int m_expandState = -1;
    bool m_preventFocus = false;

    QString m_name;



    Body::coordinate m_position;
    Body::coordinate m_absolutePosition;
    Body::coordinate m_centerPosition;
    Body::coordinate m_positionBeforeDragged;


    QQuickItem * m_obj;
};

#endif // BASENODE_H

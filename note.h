#ifndef NOTE_H
#define NOTE_H

#include <QObject>
#include <body.h>
#include <basenode.h>

class Note: public BaseNode
{
    Q_OBJECT
public:
    Note(QObject * parent = nullptr);
    void setID(int id){m_id = id;}
    int getID(){return m_id;}

    int width(){return m_width;}
    int height(){return m_height;}

    void setPosition(Body::coordinate c);
    void transform(Body::coordinate c);
    Body::coordinate getPosition(){return m_position;}

    void hover(bool b);
    void highlight(bool b);
    bool textBoxSelected();

    bool clickAction();

    QString getText();
    void setText(QString s);

    BaseNode * isInside(int x,int y);

    void moving(bool b){m_moving = b;}
    bool isMoving(){return m_moving;}

    //void hover(bool b);

    QQuickItem * obj(){return m_obj;}
    void initializeObj();
private:
    bool m_preventFocus = false;

    int m_width;
    int m_height;

    bool m_moving =  false;

public slots:
    void widthChanged();
    void heightChanged();

    void intputAccepted();
};

#endif // NOTE_H

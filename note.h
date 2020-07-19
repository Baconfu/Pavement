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
    int displayWidth(){return m_width;}
    int displayHeight(){return m_height;}

    void setPosition(Body::coordinate c);
    void transform(Body::coordinate c);
    Body::coordinate getPosition(){return m_position;}
    int getX(){return getPosition().x;}
    int getY(){return getPosition().y;}

    void hover(bool b);
    void highlight(bool b);
    void selectExpandedTextBox(bool);
    bool expandedTextBoxSelected(){return m_expandedTextBox_selected;}

    bool clickAction();

    QString getText();
    void setText(QString s);

    Body::response isInside(int x,int y);

    void moving(bool b);
    bool isMoving(){return m_moving;}

    //void hover(bool b);

    void destroy();

    QQuickItem * obj(){return m_obj;}
    void initializeObj();
private:
    bool m_preventFocus = false;

    bool m_expandedTextBox_selected = false;

    int m_width;
    int m_height;

    bool m_moving =  false;

public slots:
    void widthChanged();
    void heightChanged();

    void intputAccepted();
};

#endif // NOTE_H

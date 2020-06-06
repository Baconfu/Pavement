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

    void setPosition(Body::coordinate c);


    QString text();
    void setText(QString s);

    BaseNode * isInside(int x,int y);

    //void hover(bool b);

    void initializeObj();
private:
    bool m_preventFocus = false;


public slots:
    void widthChanged();
    void heightChanged();

    void intputAccepted();
};

#endif // NOTE_H

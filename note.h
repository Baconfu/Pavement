#ifndef NOTE_H
#define NOTE_H

#include <QObject>
#include <body.h>

class Note: public QObject
{
    Q_OBJECT
public:
    Note(QObject * parent = nullptr);

    QString text();
    void setText(QString s);

    bool isInside(int x,int y);



    void initializeObj();
private:
    bool m_preventFocus = false;

    QQuickItem * m_obj = nullptr;

public slots:
    void widthChanged();
    void heightChanged();

    void intputAccepted();
};

#endif // NOTE_H

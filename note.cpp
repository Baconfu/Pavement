#include "note.h"

Note::Note(QObject * parent)
{

}

void Note::setPosition(Body::coordinate c)
{
    m_obj->setProperty("x",c.x);
    m_obj->setProperty("y",c.y);
}

QString Note::text()
{
    return m_obj->findChild<QObject*>("textArea")->property("text").toString();
}

void Note::setText(QString s)
{
    m_obj->findChild<QObject*>("textArea")->setProperty("text",s);
}

BaseNode * Note::isInside(int x,int y)
{

}



void Note::initializeObj()
{
    Body * body = Body::getInstance();

    QQuickItem * object;
    QQmlComponent component(body->engine(),QUrl("qrc:/note.qml"));
    object = qobject_cast<QQuickItem*>(component.create());
    QQuickItem * item = body->getRoot();
    object->setParentItem(item->findChild<QQuickItem*>("layer"));
    object->setParent(body->engine());

    m_obj = object;
}

void Note::widthChanged()
{

}

void Note::heightChanged()
{

}

void Note::intputAccepted()
{

}


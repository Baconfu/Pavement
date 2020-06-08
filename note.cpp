#include "note.h"

Note::Note(QObject * parent)
{

}

void Note::setPosition(Body::coordinate c)
{
    m_obj->setProperty("x",c.x);
    m_obj->setProperty("y",c.y);
    m_position = c;
}

void Note::hover(bool b)
{
    if(b != m_obj->findChild<QObject*>("textArea")->property("focus").toBool()){
        m_obj->findChild<QObject*>("textArea")->setProperty("focus",b);
        if(!b){
            Body * b = Body::getInstance();
            b->setFocusWindow();
        }
    }
}

QString Note::getText()
{
    return m_obj->findChild<QObject*>("textArea")->property("text").toString();
}

void Note::setText(QString s)
{
    m_obj->findChild<QObject*>("textArea")->setProperty("text",s);
}

BaseNode * Note::isInside(int x,int y)
{
    Body::coordinate position = getPosition();

    int width = m_width;
    int height = m_height;
    if(x>position.x && x<position.x + width && y > position.y && y < position.y + height){

        return this;
    }else{

        hover(false);
        return nullptr;
    }
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

    connect(object,SIGNAL(update()),SLOT(widthChanged()));
    connect(object,SIGNAL(update()),SLOT(heightChanged()));

    m_obj = object;
    widthChanged();
    heightChanged();

}

void Note::widthChanged()
{
    m_width = m_obj->property("width").toInt();
}

void Note::heightChanged()
{
    m_height = m_obj->property("height").toInt();
}

void Note::intputAccepted()
{

}


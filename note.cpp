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

void Note::transform(Body::coordinate c)
{
    setPosition(m_position.add(c));
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

void Note::highlight(bool b)
{
    m_obj->setProperty("highlighted",b);
}

bool Note::textBoxSelected()
{
    return m_obj->findChild<QObject*>("textArea")->property("focus").toBool();
}

bool Note::clickAction()
{
    if(textBoxSelected()){
        return false;
    }
    return true;
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
    int s = 15;
    bool horizontalbound = ((x>position.x-s && x<position.x) || (x>position.x+width && x<position.x + width + s)) &&
            (y>position.y-s && y<position.y+height+s);
    bool verticalbound = ((y>position.y - s && y<position.y) || (y>position.y+height && y<position.y + height + s)) &&
            (x > position.x-s && x<position.x + width + s);
    bool highlighted = false;
    if(horizontalbound || verticalbound){
        highlight(true);
        highlighted = true;
        hover(false);
        return this;
    }
    if(x>position.x && x<position.x + width && y > position.y && y < position.y + height){
        highlighted = true;
        highlight(false);
        hover(true);
        return this;
    }
    if(!highlighted){
        highlight(false);
        hover(false);
        return nullptr;
    }
}

void Note::destroy()
{
    disconnect();
    Body * body = Body::getInstance();
    m_obj->deleteLater();
    delete(m_obj);
    m_obj = nullptr;
    body->removeNode(this);
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


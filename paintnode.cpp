#include "paintnode.h"

PaintNode::PaintNode(QQuickItem * parent):
    QQuickPaintedItem (parent)
{

}

void PaintNode::paint(QPainter * painter)
{
    if(m_mode == "standard"){
        QSizeF mySize = size();
        QLineF l;

        QPen pen;
        pen.setColor(color());
        pen.setWidth(lineWidth());



        painter->setPen(pen);
        painter->setBrush(Qt::NoBrush);


        l.setP1(QPointF(0,mySize.height()));
        l.setP2(QPointF(mySize.width(),mySize.height()));
        painter->drawLine(l);
        l.setP1(QPointF(mySize.width(),0));
        l.setP2(QPointF(mySize.width(),mySize.height()));
        painter->drawLine(l);
    }

}


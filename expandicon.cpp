#include "expandicon.h"

ExpandIcon::ExpandIcon(QQuickItem * parent):
    QQuickPaintedItem (parent)
{

}

void ExpandIcon::paint(QPainter * painter)
{
    QSizeF mySize = QSize(width(),height());
    QRectF rect = QRectF(1,1,mySize.width()-2,mySize.height()-2);

    QPen pen;
    pen.setColor(Qt::white);
    painter->setPen(pen);
    painter->setBrush(QBrush(Qt::white));
    painter->drawPie(rect,0,5760);
    pen.setColor(Qt::black);
    pen.setWidth(1);
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);

    painter->drawEllipse(rect);
    painter->drawLine(int(mySize.width()/2),3,int(mySize.width()/2),int(mySize.height()-3));
    painter->drawLine(3,int(mySize.height()/2),int(mySize.width() - 3),int(mySize.height()/2));

}

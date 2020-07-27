#include "paintnodearea.h"

PaintNodeArea::PaintNodeArea(QQuickItem * parent):
    QQuickPaintedItem (parent)
{

}

void PaintNodeArea::paint(QPainter * painter)
{
    QSizeF s =QSizeF(width(),height());

    QVector<QLineF> border;
    int final = 0;
    for(int i=0; i<s.width(); i+=15){
        QLineF segment;
        segment.setP1(QPointF(i,0));
        segment.setP2(QPointF(i+10,0));
        border.append(segment);

        final = i;
    }
    QLineF segment;
    segment.setP1(QPointF(final+15,0));
    segment.setP2(QPointF(s.width(),0));
    border.append(segment);

    final = 0;
    for(int i=0; i<s.width(); i+=15){
        QLineF segment;
        segment.setP1(QPointF(i,s.height()-1));
        segment.setP2(QPointF(i+10,s.height()-1));
        border.append(segment);
        final = i;
    }

    segment.setP1(QPointF(final+15,s.height()-1));
    segment.setP2(QPointF(s.width(),s.height()-1));
    border.append(segment);

    final = 0;
    for(int i=0; i<s.height(); i+=15){
        QLineF segment;
        segment.setP1(QPointF(0,i));
        segment.setP2(QPointF(0,i+10));
        border.append(segment);
        final = i;
    }

    segment.setP1(QPointF(0,final+15));
    segment.setP2(QPointF(0,s.height()));
    border.append(segment);

    final = 0;
    for(int i=0; i<s.height(); i+=15){
        QLineF segment;
        segment.setP1(QPointF(s.width()-1,i));
        segment.setP2(QPointF(s.width()-1,i+10));
        border.append(segment);
        final = i;
    }

    segment.setP1(QPointF(s.width()-1,final+15));
    segment.setP2(QPointF(s.width()-1,s.height()));
    border.append(segment);

    painter->drawLines(border);
}

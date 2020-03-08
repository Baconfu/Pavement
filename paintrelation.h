#ifndef PAINTRELATION_H
#define PAINTRELATION_H

#include <QObject>
#include <QQuickPaintedItem>
#include <QImage>
#include <QPainter>
#include <QColor>
#include <QSizeF>
#include <QSGGeometryNode>
#include <QSGFlatColorMaterial>
#include <math.h>
#include <QPointF>

class PaintRelation: public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(QPointF p1 READ p1 WRITE setP1 NOTIFY p1Changed)


public:
    PaintRelation(QQuickItem * parent = nullptr);
    ~PaintRelation();

    QSGNode *updatePaintNode(QSGNode *, UpdatePaintNodeData* );

    QColor color(){return m_color;}
    void setColor(QColor color){m_color = color;}

    QPointF p1(){return m_p1;}
    void setP1(QPointF point){m_p1 = point;}

private:
    QColor m_color;
    QPointF m_p1;

signals:

    void colorChanged();
    void p1Changed();
};

#endif // PAINTRELATION_H
#ifndef PAINTTRIANGLE_H
#define PAINTTRIANGLE_H


#include <QObject>
#include <QQuickItem>
#include <QColor>
#include <QSizeF>
#include <QSGGeometryNode>
#include <QSGFlatColorMaterial>
#include <QSGMaterial>
#include <math.h>
#include <QPointF>
#include <QElapsedTimer>

class PaintTriangle: public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(QPointF p1 READ p1 WRITE setP1 NOTIFY p1Changed)
    Q_PROPERTY(QPointF p0 READ p0 WRITE setP0 NOTIFY p0Changed)
    Q_PROPERTY(bool expandable READ expandable WRITE setExpandable NOTIFY expandableChanged)
    Q_PROPERTY(double angle READ angle WRITE setAngle NOTIFY angleChanged)
    Q_PROPERTY(int lineWidth READ lineWidth WRITE setLineWidth NOTIFY lineWidthChanged)
public:
    PaintTriangle(QQuickItem * parent = nullptr);
    ~PaintTriangle();

    QSGNode *updatePaintNode(QSGNode *, UpdatePaintNodeData* );

    QColor color(){return m_color;}
    void setColor(QColor color){m_color = color;}

    QPointF p1(){return m_p1;}
    void setP1(QPointF point){m_p1 = point;}

    QPointF p0(){return m_p0;}
    void setP0(QPointF point){m_p0 = point;}

    bool expandable(){return m_expandable;}
    void setExpandable(bool b){m_expandable = b;}

    double angle(){qDebug()<<m_angle;return m_angle;}
    void setAngle(double a){qDebug()<<"setting angle"<<a;m_angle = a;}

    int lineWidth(){return m_lineWidth;}
    void setLineWidth(int n);

private:
    int tally = 0;
    QColor m_color;
    QPointF m_p1;
    QPointF m_p0;
    bool m_expandable = false;
    double m_angle;
    int m_lineWidth;

signals:

    void lineWidthChanged();
    void angleChanged();
    void colorChanged();
    void p1Changed();
    void p0Changed();
    void expandableChanged();
};


#endif // PAINTTRIANGLE_H

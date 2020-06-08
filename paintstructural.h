#ifndef PAINTSTRUCTURAL_H
#define PAINTSTRUCTURAL_H

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

class PaintStructural: public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(QPointF p1 READ p1 WRITE setP1 NOTIFY p1Changed)
    Q_PROPERTY(bool expandable READ expandable WRITE setExpandable NOTIFY expandableChanged)
    Q_PROPERTY(double angle READ angle WRITE setAngle NOTIFY angleChanged)
    Q_PROPERTY(int lineWidth READ lineWidth WRITE setLineWidth NOTIFY lineWidthChanged)
public:
    PaintStructural(QQuickItem * parent = nullptr);
    ~PaintStructural();

    QSGNode *updatePaintNode(QSGNode *, UpdatePaintNodeData* );

    QColor color(){return m_color;}
    void setColor(QColor color){m_color = color;}

    QPointF p1(){return m_p1;}
    void setP1(QPointF point){m_p1 = point;}

    bool expandable(){return m_expandable;}
    void setExpandable(bool b){m_expandable = b;}

    double angle(){return m_angle;}
    void setAngle(double a){m_angle = a;}

    int lineWidth(){return m_lineWidth;}
    void setLineWidth(int n);

private:
    int tally = 0;
    QColor m_color;
    QPointF m_p1;
    bool m_expandable = false;
    double m_angle;
    int m_lineWidth;

signals:

    void lineWidthChanged();
    void angleChanged();
    void colorChanged();
    void p1Changed();
    void expandableChanged();
};

#endif // PAINTSTRUCTURAL_H

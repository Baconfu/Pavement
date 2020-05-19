#ifndef PAINTNODEAREA_H
#define PAINTNODEAREA_H

#include <QQuickItem>
#include <QQuickPaintedItem>
#include <QQuickItem>
#include <QPainter>
#include <QColor>
#include <QString>
#include <QSizeF>


class PaintNodeArea: public QQuickPaintedItem
{
    Q_OBJECT
public:
    PaintNodeArea(QQuickItem * parent = nullptr);

    void paint(QPainter * painter);
};

#endif // PAINTNODEAREA_H

#ifndef EXPANDICON_H
#define EXPANDICON_H

#include <QObject>
#include <QPainter>
#include <QQuickItem>
#include <QQuickPaintedItem>
#include <QRectF>

class ExpandIcon: public QQuickPaintedItem
{
    Q_OBJECT
public:
    ExpandIcon(QQuickItem * parent  = nullptr);

    void paint(QPainter * painter);

};

#endif // EXPANDICON_H

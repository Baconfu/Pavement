#ifndef PAINTNODE_H
#define PAINTNODE_H

#include <QObject>
#include <QQuickPaintedItem>
#include <QQuickItem>
#include <QPainter>
#include <QColor>
#include <QString>
#include <QSizeF>

class PaintNode: public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(int lineWidth READ lineWidth WRITE setLineWidth NOTIFY lineWidthChanged)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(QString mode READ mode WRITE setMode NOTIFY modeChanged)
public:
    PaintNode(QQuickItem * parent = nullptr);

    void paint(QPainter * painter);

    int lineWidth(){return m_lineWidth;}
    void setLineWidth(int pixels){m_lineWidth = pixels;}

    QColor color(){return m_color;}
    void setColor(QColor c){m_color = c; update();}

    QString mode(){return m_mode;}
    void setMode(QString m){m_mode = m;}


private:
    int m_lineWidth;
    QColor m_color;
    QString m_mode;

signals:
    void lineWidthChanged();
    void colorChanged();
    void modeChanged();
};

#endif // PAINTNODE_H

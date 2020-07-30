#ifndef TIP_H
#define TIP_H

#include <QObject>
#include <QQuickItem>

class tip: public QObject
{
    Q_OBJECT
public:
    tip();

    void setText(QString text);
    QString getText(){return m_text;}

    void initializeObj(QString text);
    QQuickItem * obj(){return m_obj;}


private:
    QString m_text;

    QQuickItem * m_obj = nullptr;
public slots:
    void destroy();
};

#endif // TIP_H

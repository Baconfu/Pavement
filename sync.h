#ifndef SYNC_H
#define SYNC_H

#include <QVector>
#include <QString>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QObject>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QByteArray>
#include <QJsonArray>
#include <file.h>


class Sync: public QObject
{
public:
    Sync();

    void sync();


    QVector<QString> data;
    QString test = "/home/chuan/qt_projects/Pavement1.1/saves/trol.json";

public slots:
    void syncRequestFinished(QNetworkReply*);
};

#endif // SYNC_H

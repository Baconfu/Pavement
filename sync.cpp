#include "sync.h"
#include <pavementfile.h>

Sync::Sync()
{




}

void Sync::init()
{
    manager = new QNetworkAccessManager();
    QObject::connect(manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(requestFinished(QNetworkReply*)));
}

void Sync::sync()
{

    //connect(manager, SIGNAL(finished(QNetworkReply*)),
    //        this, SLOT(syncRequestFinished(QNetworkReply*)));


    //QByteArray data = QtJson::Json::serialize(collectSyncData());

    PavementFile * f = new PavementFile(test);
    f->readJson();
    QJsonDocument doc;
    doc.setObject(f->obj());


    QByteArray data = doc.toJson();

    // FIXME for debug
    qDebug() << "Sync" << QString::fromUtf8(data.data(), data.size());

}

void Sync::get()
{



    //connect(manager, SIGNAL(finished(QNetworkReply*)),
    //        this, SLOT(syncRequestFinished(QNetworkReply*)));


    //QByteArray data = QtJson::Json::serialize(collectSyncData());



    // FIXME for debug


    QUrl url("http://127.0.0.1:5000/user/data");
    QNetworkRequest request(url);
    manager->get(request);


    /*qDebug() << "Sync" << QString::fromUtf8(data.data(), data.size());

    PavementFile * f = new PavementFile(test);
    f->readJson();
    QJsonDocument doc;
    doc.setObject(f->obj());


    QByteArray data = doc.toJson();*/
}

void Sync::requestFinished(QNetworkReply * reply)
{
    QByteArray data = reply->readAll();
    qDebug()<<data;
    QJsonDocument doc = QJsonDocument::fromBinaryData(data);
    QJsonObject obj = doc.object();
    qDebug()<<obj["nodes"];
}

void Sync::syncRequestFinished(QNetworkReply *)
{

}

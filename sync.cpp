#include "sync.h"
#include <pavementfile.h>

Sync::Sync()
{

}

void Sync::sync()
{
    QUrl url("http://192.168.20.18:5000/data");
    QNetworkRequest request(url);

    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkAccessManager *manager = new QNetworkAccessManager();

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

    manager->post(request, data);
}

void Sync::syncRequestFinished(QNetworkReply *)
{

}
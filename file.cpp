#include "file.h"

File::File()
{

}

QJsonObject File::load(QString path)
{
    QFile file(path);
    file.open(QIODevice::ReadOnly);

    QByteArray data = file.readAll();
    QJsonDocument loadData(QJsonDocument::fromJson(data));

    return loadData.object();
}

void File::write(QString path,QJsonObject obj)
{

    QFile file(path);
    file.open(QIODevice::WriteOnly);
    QJsonDocument saveDoc(obj);
    file.write(saveDoc.toJson());
}

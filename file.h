#ifndef FILE_H
#define FILE_H

#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QByteArray>
#include <QJsonArray>



class File
{
public:
    File();

    QJsonObject load(QString path);

    void write(QString path,QJsonObject obj);


protected:
    QString m_fileName;

};

#endif // FILE_H

#ifndef PAVEMENTFILE_H
#define PAVEMENTFILE_H

#include <file.h>
#include <node.h>


class PavementFile : public File
{
public:
    PavementFile(QString fileName);

    void saveNode(Node * n);
    void writeJson();
    void readJson();

private:
    QJsonObject m_obj;
    QJsonDocument m_doc;
};

#endif // PAVEMENTFILE_H

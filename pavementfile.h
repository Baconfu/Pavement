#ifndef PAVEMENTFILE_H
#define PAVEMENTFILE_H

#include <file.h>
#include <node.h>

class PavementFile : public File
{
public:
    PavementFile(QString fileName);

    void saveNode(Node * n);


};

#endif // PAVEMENTFILE_H

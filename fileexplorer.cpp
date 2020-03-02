#include "fileexplorer.h"
#include <body.h>

FileExplorer::FileExplorer(QObject * parent):
    QObject(parent)
{

}

void FileExplorer::initialize()
{
    getFiles("/home/chuan/qt_projects/Pavement_1_1/saves");

}

void FileExplorer::update()
{
    for(int i=0; i)
}

void FileExplorer::setPath(QString p)
{
    m_path = p.toStdString();
    emit(pathChanged());
}

void FileExplorer::getFiles(QString p)
{
    m_files.clear();
    struct dirent * entry;
    DIR *dir = opendir(p.toUtf8());

    if(dir == nullptr){
        return;
    }
    while((entry = readdir(dir)) != nullptr){
        m_files.append(entry->d_name);
    }
    closedir(dir);
}


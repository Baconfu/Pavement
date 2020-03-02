#ifndef FILEEXPLORER_H
#define FILEEXPLORER_H

#include <QObject>
#include <iostream>
#include <string>
#include <dirent.h>
#include <sys/types.h>
#include <QQuickItem>

using namespace std;

class Body;

class FileExplorer: public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString path READ path WRITE setPath NOTIFY pathChanged)
public:
    FileExplorer(QObject * parent = nullptr);
    void initialize();
    void update();

    string path(){return m_path;}
    void setPath(QString p);

    QStringList file(){return m_files;}
    void getFiles(QString p);

private:
    string m_path;

    QStringList m_files;

    QQuickItem * m_obj = nullptr;

signals:
    void pathChanged();
};

#endif // FILEEXPLORER_H

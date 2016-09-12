#include "filesystembasepathfinder.h"
#include <QStandardPaths>
#include <QFileInfo>
#include <QDebug>

FileSystemBasePathFinder::FileSystemBasePathFinder(QObject *parent) :
    QObject(parent), env(QProcessEnvironment::systemEnvironment())
{
}

void FileSystemBasePathFinder::setEV(QString value)
{
    ev = value;
}

void FileSystemBasePathFinder::setPath(QString value)
{
    path = value;
}

void FileSystemBasePathFinder::setShortcutMode(bool value)
{
    isShortcut = value;
}

QString FileSystemBasePathFinder::GetBasePath()
{
    QFileInfo file;
    QString evValue(env.value(ev));
    qDebug() << ev << " : " << evValue << "\n";
    if(evValue.isEmpty())
    {
        if(ev == "startmenu")
        {
            QStringList paths(QStandardPaths::standardLocations(QStandardPaths::StandardLocation::ApplicationsLocation));
            foreach (const QString &base, paths)
            {
                file.setFile(base + path);
                qDebug() << file.path() << "\n";
                if(file.exists())
                {
                    qDebug() << "Found.\n";
                    return file.path();
                }
            }
        }
    }
    else
    {
        file.setFile(evValue + path);
        if(file.exists())
        {
            return file.path();
        }
    }
    return QString();
}

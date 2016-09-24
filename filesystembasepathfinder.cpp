#include "filesystembasepathfinder.h"
#include <QStandardPaths>
#include <QFileInfo>
#include <QDebug>
#include <QSettings>

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
        bool isStartMenu(ev == "startmenu");
        if(isStartMenu || ev == "userdocuments")
        {
            QStandardPaths::StandardLocation location(
                        isStartMenu
                        ? QStandardPaths::ApplicationsLocation
                        : QStandardPaths::DocumentsLocation);
            QStringList paths(QStandardPaths::standardLocations(location));
            foreach (const QString &base, paths)
            {
                // remove extra "Programs\" from the front because it is already included in the standard location string
                file.setFile(base + "\\" + (isStartMenu ? path.remove(0, 9) : path));
                qDebug() << file.path() << "\n";
                if(file.exists())
                {
                    qDebug() << "Found.\n";
                    return isShortcut ? file.symLinkTarget() : file.path();
                }
            }
        }
        else if(ev == "steamcommon")
        {
            QSettings settings("HKEY_LOCAL_MACHINE\\SOFTWARE\\Valve\\Steam", QSettings::NativeFormat);
            QString steamPath(settings.value("InstallPath").toString());
            if(!steamPath.isEmpty())
            {
                file.setFile(steamPath + "\\steamapps\\common\\" + path);
                if(file.exists())
                {
                    return isShortcut ? file.symLinkTarget() : file.path();
                }
            }
        }
    }
    else
    {
        file.setFile(evValue + path);
        if(file.exists())
        {
            return isShortcut ? file.symLinkTarget() : file.path();
        }
    }
    return QString();
}

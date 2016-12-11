#include "filesystembasepathfinder.h"
#include <QStandardPaths>
#include <QDir>
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
        if(isStartMenu || ev == "userdocuments" || ev == "DOCUMENTS")
        {
            QStandardPaths::StandardLocation location(
                        isStartMenu
                        ? QStandardPaths::ApplicationsLocation
                        : QStandardPaths::DocumentsLocation);
            QStringList paths(QStandardPaths::standardLocations(location));
            for (const QString &base : paths)
            {
                // remove extra "Programs\" from the front because it is already included in the standard location string
                file.setFile(base + "/" + (isStartMenu ? path.remove(0, 9) : path));
                qDebug() << file.path() << "\n";
                if(file.exists())
                {
                    qDebug() << "Found " << file.filePath() << ".\n";
                    return isShortcut ? file.symLinkTarget() : file.filePath();
                }
            }
        }
        else if(ev.startsWith("steam", Qt::CaseInsensitive))
        {
#ifdef Q_OS_WIN
            QString registryPath(QSysInfo::kernelType() == "winnt"
                                 && QSysInfo::currentCpuArchitecture() == "x86_64"
                                 ? "HKEY_LOCAL_MACHINE\\SOFTWARE\\WOW6432Node\\Valve\\Steam"
                                 : "HKEY_LOCAL_MACHINE\\SOFTWARE\\Valve\\Steam");
            QSettings settings(registryPath, QSettings::NativeFormat);
            QString steamPath(settings.value("InstallPath").toString());
#else
            QString steamPath(QDir::homePath() + "/.steam/steam/");
#endif
            if(!steamPath.isEmpty())
            {
                if (ev == "steamcommon") {
                    steamPath.append("/steamapps/common");
                }
                else if (ev == "STEAM_CLOUD") {
                    QDir dir(steamPath + "/userdata/");
                    const QFileInfoList& infos(dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot));
                    if (infos.count() > 0) {
                        steamPath = infos.first().filePath();
                    }
                }
                else if (ev == "STEAM_CACHE") {
                    steamPath.append("/appcache");
                }
                file.setFile(steamPath + "/" + path);
                qDebug() << file.path() << "\n";
                if(file.exists())
                {
                    qDebug() << "Found " << file.filePath() << ".\n";
                    return isShortcut ? file.symLinkTarget() : file.filePath();
                }
            }
#ifdef Q_OS_WIN
        } else if (ev == "SHARED_DOCUMENTS") {
            QStringList paths(QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation));
            for (QString base : paths)
            {
                base.replace(env.value("USERNAME"), "Public");
                file.setFile(base + "/" + path);
                qDebug() << file.path() << "\n";
                if(file.exists())
                {
                    qDebug() << "Found " << file.filePath() << ".\n";
                    return isShortcut ? file.symLinkTarget() : file.filePath();
                }
            }
#endif
        }
    }
    else
    {
        file.setFile(evValue + path);
        if(file.exists())
        {
            return isShortcut ? file.symLinkTarget() : file.filePath();
        }
    }
    return QString();
}

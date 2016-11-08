#ifndef FILESYSTEMBASEPATHFINDER_H
#define FILESYSTEMBASEPATHFINDER_H

#include <QObject>
#include <QProcessEnvironment>

class FileSystemBasePathFinder : public QObject
{
    Q_OBJECT
public:
    explicit FileSystemBasePathFinder(QObject *parent = 0);
    void setEV(QString value);
    void setPath(QString value);
    void setShortcutMode(bool value);
    QString GetBasePath();

signals:

public slots:

private:
    QString ev;
    QString path;
    bool isShortcut;
    QProcessEnvironment env;
};

#endif // FILESYSTEMBASEPATHFINDER_H

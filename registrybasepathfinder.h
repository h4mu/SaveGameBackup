#ifndef REGISTRYBASEPATHFINDER_H
#define REGISTRYBASEPATHFINDER_H

#include <QObject>

class RegistryBasePathFinder : public QObject
{
    Q_OBJECT
public:
    explicit RegistryBasePathFinder(QObject *parent = 0);
    void setRoot(QString value);
    void setPath(QString value);
    void setKey(QString value);
    QString GetBasePath();

signals:

public slots:

private:
    QString root;
    QString path;
    QString key;
};

#endif // REGISTRYBASEPATHFINDER_H

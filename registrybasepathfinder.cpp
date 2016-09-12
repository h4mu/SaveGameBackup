#include "registrybasepathfinder.h"
#include <QSettings>

RegistryBasePathFinder::RegistryBasePathFinder(QObject *parent) : QObject(parent)
{

}

void RegistryBasePathFinder::setRoot(QString value)
{
    root = value;
}

void RegistryBasePathFinder::setPath(QString value)
{
    path = value;
}

void RegistryBasePathFinder::setKey(QString value)
{
    key = value;
}

QString RegistryBasePathFinder::GetBasePath()
{
    QSettings settings("HKEY_" + root.toUpper() + "\\" + path, QSettings::NativeFormat);
    return settings.value(key).toString();
}

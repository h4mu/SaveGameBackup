#include "settingsprovider.h"
#include <QStandardPaths>

SettingsProvider::SettingsProvider()
{
}

QString SettingsProvider::backupDir()
{
    return settings.value("BackupDir", QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/SaveGameBackups").toString();
}

void SettingsProvider::setBackupDir(const QString& dir)
{
    settings.setValue("BackupDir", dir);
}

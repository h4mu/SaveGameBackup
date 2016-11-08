#ifndef SETTINGSPROVIDER_H
#define SETTINGSPROVIDER_H

#include <QSettings>

class SettingsProvider
{
public:
    SettingsProvider();
    QString backupDir();
    void setBackupDir(const QString& dir);
private:
    QSettings settings;
};

#endif // SETTINGSPROVIDER_H

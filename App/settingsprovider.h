#ifndef SETTINGSPROVIDER_H
#define SETTINGSPROVIDER_H

#include <QSettings>

class SettingsProvider
{
public:
    SettingsProvider();
    QString backupDir() const;
    void setBackupDir(const QString& dir);
    bool isGameSaveManagerFormatSelected() const;
    void setGameSaveManagerFormatSelected(bool selected);
    QString gameDatabaseUri() const;
    void setGameDatabaseUri(const QString& uri);
private:
    QSettings settings;
};

#endif // SETTINGSPROVIDER_H

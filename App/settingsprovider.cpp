#include "settingsprovider.h"
#include <QStandardPaths>

SettingsProvider::SettingsProvider()
{
}

QString SettingsProvider::backupDir() const
{
    return settings.value("backupDir", QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/SaveGameBackups").toString();
}

void SettingsProvider::setBackupDir(const QString& dir)
{
    settings.setValue("backupDir", dir);
}

bool SettingsProvider::isGameSaveManagerFormatSelected() const
{
    return settings.value("isGameSaveManagerFormatSelected", QVariant(false)).toBool();
}

void SettingsProvider::setGameSaveManagerFormatSelected(bool selected)
{
    settings.setValue("isGameSaveManagerFormatSelected", selected);
}

QString SettingsProvider::gameDatabaseUri() const
{
    return settings.value("gameDatabaseUri", isGameSaveManagerFormatSelected()
                          ? "http://rawcdn.githack.com/h4mu/Data/blob/master/GSM/db_update_92013060802"
                          : "http://rawcdn.githack.com/h4mu/Data/master/%1.xml").toString();
}

void SettingsProvider::setGameDatabaseUri(const QString& uri)
{
    settings.setValue("gameDatabaseUri", uri);
}

#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include "settingsprovider.h"
#include <QDialog>

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = 0);
    ~SettingsDialog();

private slots:
    void on_pushButton_clicked();

    void on_buttonBox_accepted();

private:
    Ui::SettingsDialog *ui;
    SettingsProvider settings;
    QString backupDir;
};

#endif // SETTINGSDIALOG_H

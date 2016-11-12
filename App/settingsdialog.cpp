#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include "settingsprovider.h"
#include <QFileDialog>

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    backupDir = settings.backupDir();
    ui->isGSMFormat->setChecked(settings.isGameSaveManagerFormatSelected());
    ui->dbUrl->setText(settings.gameDatabaseUri());
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::on_pushButton_clicked()
{
    backupDir = QFileDialog::getExistingDirectory(this,
                                                    tr("Open Backup Directory"),
                                                    backupDir,
                                                    QFileDialog::ShowDirsOnly);
}

void SettingsDialog::on_buttonBox_accepted()
{
    settings.setBackupDir(backupDir);
    settings.setGameSaveManagerFormatSelected(ui->isGSMFormat->isChecked());
    settings.setGameDatabaseUri(ui->dbUrl->text());
}

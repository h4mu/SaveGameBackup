#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include "settingsprovider.h"
#include <QFileDialog>

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::on_pushButton_clicked()
{
    SettingsProvider settings;
    QString dir = QFileDialog::getExistingDirectory(this,
                                                    tr("Open Backup Directory"),
                                                    settings.backupDir(),
                                                    QFileDialog::ShowDirsOnly);
    settings.setBackupDir(dir);
}

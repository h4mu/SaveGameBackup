#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include <QFileDialog>
#include <QSettings>

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
    QSettings settings;
    QString dir = QFileDialog::getExistingDirectory(this,
                                                    tr("Open Backup Directory"),
                                                    settings.value("BackupDir").toString(),
                                                    QFileDialog::ShowDirsOnly);
    settings.setValue("BackupDir", dir);
}

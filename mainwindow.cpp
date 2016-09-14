#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settingsdialog.h"
#include "directoryscanner.h"
#include <QtXmlPatterns>
#include <QDebug>
#include <QScopedPointer>
#include <QStandardItemModel>
#include <QSettings>
#include <QMessageBox>
#include <QtWidgets>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    model(new QStandardItemModel(0, 5))
{
    ui->setupUi(this);
    ui->tableView->setModel(model);

    readSettings();
    QStringList headers;
    headers << "name" << "title" << "basePath" << "includes" << "excludes";
    model->setHorizontalHeaderLabels(headers);
}

void MainWindow::readSettings()
{
    QSettings settings;
    int rows = settings.beginReadArray("model");
    for (int row = 0; row < rows; ++row) {
        settings.setArrayIndex(row);
        int columns = settings.beginReadArray("row");
        QList<QStandardItem*> values;
        for (int col = 0; col < columns; ++col) {
            settings.setArrayIndex(col);
            values << new QStandardItem(settings.value("value").toString());
        }
        model->appendRow(values);
        settings.endArray();
    }
    settings.endArray();
}

void MainWindow::writeSettings()
{
    QSettings settings;
    settings.clear();
    settings.beginWriteArray("model");
    for (int row = 0; row < model->rowCount(); ++row) {
        settings.setArrayIndex(row);
        settings.beginWriteArray("row");
        for (int col = 0; col < model->columnCount(); ++col) {
            settings.setArrayIndex(col);
            settings.setValue("value", model->item(row, col)->text());
        }
        settings.endArray();
    }
    settings.endArray();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    writeSettings();
    event->accept();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete model;
}

void MainWindow::on_actionSettings_triggered()
{
    const QScopedPointer<SettingsDialog> dlg(new SettingsDialog(this));
    dlg->exec();
}

void MainWindow::on_action_Scan_Computer_triggered()
{
    QXmlQuery query;
    query.setQuery(QUrl("qrc:///GameSaveInfo202.xq"));
    if (!query.isValid()) {
        QMessageBox::critical(this, tr("Query Error"), tr("Unable to query games database."));
        return;
    }

    model->clear();
    DirectoryScanner directoryScanner(model, query.namePool());
    if (!query.evaluateTo(&directoryScanner)) {
        QMessageBox::critical(this, tr("Query Error"), tr("Unable to parse games database."));
        return;
    }
}

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settingsdialog.h"
#include "directoryscanner.h"
#include <QtXmlPatterns>
#include <QDebug>
#include <QScopedPointer>
#include <QStandardItemModel>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    model(new QStandardItemModel)
{
    ui->setupUi(this);
    ui->tableView->setModel(model);
    QStringList headers;
    headers << "name" << "title" << "basePath" << "includes" << "excludes";
    model->setHorizontalHeaderLabels(headers);
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
        qDebug() << "The query is invalid";
        return;
    }

    model->clear();
    DirectoryScanner directoryScanner(model, query.namePool());
    if (!query.evaluateTo(&directoryScanner)) {
        qDebug() << "Can't evaluate the query";
        return;
    }
}

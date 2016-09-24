#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settingsdialog.h"
#include "directoryscanner.h"
#include <QtXmlPatterns>
#include <QDebug>
#include <QScopedPointer>
#include <QStandardItemModel>
#include <QSettings>
//#include <QMessageBox>
#include <QtWidgets>
#include <QtConcurrent>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    model(new QStandardItemModel(0, 5)),
    watcher(new QFutureWatcher<QList<QStringList> >(this))
{
    ui->setupUi(this);
    ui->tableView->setModel(model);
    connect(watcher, SIGNAL(finished()), this, SLOT(updateModel()));

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
    delete watcher;
}

void MainWindow::on_actionSettings_triggered()
{
    const QScopedPointer<SettingsDialog> dlg(new SettingsDialog(this));
    dlg->exec();
}

QList<QStringList> readGamesDb(const QString &batch)
{
    QXmlQuery query;
#ifdef Q_OS_WIN
    query.bindVariable("os", QVariant("Windows"));
#elif defined(Q_OS_ANDROID)
    query.bindVariable("os", QVariant("Android"));
#elif defined(Q_OS_OSX)
    query.bindVariable("os", QVariant("OSX"));
#else
    query.bindVariable("os", QVariant("Linux"));
#endif
    query.bindVariable("batch", QVariant(batch));
    query.setQuery(QUrl("qrc:///GameSaveInfo202.xq"));
    if (!query.isValid()) {
        qDebug() << "Invalid query";
        QMessageBox::critical(0, QObject::tr("Query Error"), QObject::tr("Unable to query games database."));
        return QList<QStringList>();
    }
    DirectoryScanner directoryScanner(query.namePool());
    if (!query.evaluateTo(&directoryScanner)) {
        qDebug() << "Query error";
        QMessageBox::critical(0, QObject::tr("Query Error"), QObject::tr("Unable to parse games database."));
        return QList<QStringList>();
    }
    return directoryScanner.foundGames();
}

void MainWindow::updateModel()
{
    foreach (QList<QStringList> rows, watcher->future()) {
        foreach(QStringList row, rows){
          QList<QStandardItem *> itemRow;
          foreach(QString val, row){
            itemRow << new QStandardItem(val);
          }
          model->appendRow(itemRow);
        }
    }
}

void MainWindow::on_action_Scan_Computer_triggered()
{
    model->clear();
    QStringList batches;
    batches << "numeric";
    for(char batch = 'a'; batch <= 'z'; ++batch){
      batches << QString(QChar(batch));
    }
    QProgressDialog dialog;
    dialog.setLabelText("Scanning computer for games...");
    connect(watcher, SIGNAL(finished()), &dialog, SLOT(reset()));
    connect(&dialog, SIGNAL(canceled()), watcher, SLOT(cancel()));
    connect(watcher, SIGNAL(progressRangeChanged(int,int)), &dialog, SLOT(setRange(int,int)));
    connect(watcher, SIGNAL(progressValueChanged(int)), &dialog, SLOT(setValue(int)));

    watcher->setFuture(QtConcurrent::mapped(batches, readGamesDb));
    dialog.exec();
}
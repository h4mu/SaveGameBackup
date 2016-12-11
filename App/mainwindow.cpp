#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settingsdialog.h"
#include "directoryscanner.h"
#include "settingsprovider.h"
#include "backuprestore.h"
#include <QtXmlPatterns>
#include <QDebug>
#include <QScopedPointer>
#include <QStandardItemModel>
#include <QtWidgets>
#include <QtConcurrent>
#include <functional>

const static SaveGameItemDataRole roles[] = {NameRole, TitleRole, PathRole};
const static int numRoles = sizeof(roles) / sizeof(*roles);

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    model(new QStandardItemModel(0, 5)),
    watcher(new QFutureWatcher<QVariantList>(this))
{
    ui->setupUi(this);
    ui->listView->setModel(model);
    connect(watcher, SIGNAL(finished()), this, SLOT(updateModel()));

    readSettings();
}

void MainWindow::readSettings()
{
    QSettings settings;
    int rows = settings.beginReadArray("model");
    for (int row = 0; row < rows; ++row) {
        settings.setArrayIndex(row);
        int columns = settings.beginReadArray("row");
        Q_ASSERT(columns == numRoles);
        QStandardItem *item = new QStandardItem;
        for (int col = 0; col < columns; ++col) {
            settings.setArrayIndex(col);
            item->setData(settings.value("value"), roles[col]);
        }
        model->appendRow(item);
        settings.endArray();
    }
    settings.endArray();
}

void MainWindow::writeSettings()
{
    QSettings settings;
    settings.remove("model");
    settings.beginWriteArray("model");
    for (int row = 0; row < model->rowCount(); ++row) {
        settings.setArrayIndex(row);
        settings.beginWriteArray("row");
        for (int col = 0; col < numRoles; ++col) {
            settings.setArrayIndex(col);
            settings.setValue("value", model->item(row, 0)->data(roles[col]));
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

QVariantList readGamesDb(bool isSaveGameManagerFormatUsed, const QString &url)
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
    query.bindVariable("url", QVariant(url));
    query.setQuery(QUrl(isSaveGameManagerFormatUsed ? "qrc:///GameSaveManager.xq" : "qrc:///GameSaveInfo202.xq"));
    if (!query.isValid()) {
        qDebug() << "Invalid query";
        QMessageBox::critical(0, QObject::tr("Query Error"), QObject::tr("Unable to query games database."));
        return QVariantList();
    }
    DirectoryScanner directoryScanner(query.namePool());
    if (!query.evaluateTo(&directoryScanner)) {
        qDebug() << "Query error";
        QMessageBox::critical(0, QObject::tr("Query Error"), QObject::tr("Unable to parse games database."));
        return QVariantList();
    }
    return directoryScanner.foundGames();
}

void MainWindow::updateModel()
{
    for (const QVariantList& batch : watcher->future()) {
        for (const QVariant& entry : batch) {
            const QVariantList& entryItems(entry.toList());
            qDebug() << entry;
            QStandardItem *item = new QStandardItem;
            for (int i = 0; i < numRoles; ++i) {
                item->setData(entryItems[i], roles[i]);
            }
            model->appendRow(item);
        }
    }
}

void MainWindow::on_action_Scan_Computer_triggered()
{
    model->clear();

    QProgressDialog dialog;
    dialog.setLabelText(tr("Scanning computer for games..."));
    connect(watcher, SIGNAL(finished()), &dialog, SLOT(reset()));
    connect(&dialog, SIGNAL(canceled()), watcher, SLOT(cancel()));
    connect(watcher, SIGNAL(progressRangeChanged(int,int)), &dialog, SLOT(setRange(int,int)));
    connect(watcher, SIGNAL(progressValueChanged(int)), &dialog, SLOT(setValue(int)));

    const SettingsProvider settings;
    const bool isSaveGameManagerFormatUsed(settings.isGameSaveManagerFormatSelected());
    const QString& url(settings.gameDatabaseUri());
    QStringList urls;
    if (url.contains("%1")) {
        urls << url.arg("numeric");
        for(char batch = 'a'; batch <= 'z'; ++batch){
          urls << url.arg(batch);
        }
    } else {
        urls << url;
    }
    watcher->setFuture(QtConcurrent::mapped(urls, std::bind(readGamesDb, isSaveGameManagerFormatUsed, std::placeholders::_1)));
    dialog.exec();
}

void MainWindow::on_action_Backup_triggered()
{
    QFutureWatcher<void> restoreWatcher;
    QProgressDialog dialog;
    dialog.setLabelText(tr("Creating game save backups..."));
    connect(&restoreWatcher, SIGNAL(finished()), &dialog, SLOT(reset()));
    connect(&dialog, SIGNAL(canceled()), &restoreWatcher, SLOT(cancel()));
    connect(&restoreWatcher, SIGNAL(progressRangeChanged(int,int)), &dialog, SLOT(setRange(int,int)));
    connect(&restoreWatcher, SIGNAL(progressValueChanged(int)), &dialog, SLOT(setValue(int)));

    restoreWatcher.setFuture(QtConcurrent::mapped(ui->listView->selectionModel()->selectedIndexes(), RunBackupFor));
    dialog.exec();
}

void MainWindow::on_action_Restore_triggered()
{
    QFutureWatcher<void> restoreWatcher;
    QProgressDialog dialog;
    dialog.setLabelText(tr("Restoring game saves..."));
    connect(&restoreWatcher, SIGNAL(finished()), &dialog, SLOT(reset()));
    connect(&dialog, SIGNAL(canceled()), &restoreWatcher, SLOT(cancel()));
    connect(&restoreWatcher, SIGNAL(progressRangeChanged(int,int)), &dialog, SLOT(setRange(int,int)));
    connect(&restoreWatcher, SIGNAL(progressValueChanged(int)), &dialog, SLOT(setValue(int)));

    restoreWatcher.setFuture(QtConcurrent::mapped(ui->listView->selectionModel()->selectedIndexes(), RunRestoreFor));
    dialog.exec();
}


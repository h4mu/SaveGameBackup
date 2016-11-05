#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settingsdialog.h"
#include "directoryscanner.h"
#include "settingsprovider.h"
#include <QtXmlPatterns>
#include <QDebug>
#include <QScopedPointer>
#include <QStandardItemModel>
#include <QtWidgets>
#include <QtConcurrent>

enum SaveGameItemDataRole
{
    NameRole = Qt::UserRole,
    TitleRole = Qt::DisplayRole,
    PathRole = Qt::UserRole + 1,
    IncludesRole = Qt::UserRole + 2,
    ExcludesRole = Qt::UserRole + 3
};
const static SaveGameItemDataRole roles[] = {NameRole, TitleRole, PathRole, IncludesRole, ExcludesRole};
const static int numRoles = sizeof(roles) / sizeof(*roles);

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    model(new QStandardItemModel(0, 5)),
    watcher(new QFutureWatcher<QList<QStringList> >(this))
{
    ui->setupUi(this);
    ui->listView->setModel(model);
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
        foreach (QStringList row, rows) {
            Q_ASSERT(row.length() == numRoles);
            qDebug() << row;
            QStandardItem *item = new QStandardItem;
            for (int role = 0; role < numRoles; ++role) {
              item->setData(row.at(role), roles[role]);
            }
            model->appendRow(item);
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
    dialog.setLabelText(tr("Scanning computer for games..."));
    connect(watcher, SIGNAL(finished()), &dialog, SLOT(reset()));
    connect(&dialog, SIGNAL(canceled()), watcher, SLOT(cancel()));
    connect(watcher, SIGNAL(progressRangeChanged(int,int)), &dialog, SLOT(setRange(int,int)));
    connect(watcher, SIGNAL(progressValueChanged(int)), &dialog, SLOT(setValue(int)));

    watcher->setFuture(QtConcurrent::mapped(batches, readGamesDb));
    dialog.exec();
}

void MainWindow::on_action_Backup_triggered()
{
    foreach (const QModelIndex& idx, ui->listView->selectionModel()->selectedIndexes()) {
        RunBackupFor(idx);
    }
}

QFileInfoList FindFiles(const QString& root, QString includes, QString excludes)
{
    QFileInfoList files;
    QRegularExpression includeRegEx(includes.replace(":", "|")
                                    .replace("\\", "/")
                                    .replace(".", "\\.")
                                    .replace("*", ".*")
                                    .replace("?", ".?"));
    includeRegEx.optimize();
    QRegularExpression excludeRegEx(excludes.replace(":", "|")
                                    .replace("\\", "/")
                                    .replace(".", "\\.")
                                    .replace("*", ".*")
                                    .replace("?", ".?"));
    bool isExcludePatternInvalid(excludeRegEx.pattern().isEmpty());
    if (!isExcludePatternInvalid) {
        excludeRegEx.optimize();
    }
    QQueue<QFileInfo> entries;
    entries.enqueue(QFileInfo(root));
    while (!entries.isEmpty()) {
        const QFileInfo& info(entries.dequeue());
        if (info.exists()) {
            if (info.isDir()) {
                QDir dir(info.filePath());
                entries << dir.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries);
            } else if (info.isFile() && includeRegEx.match(info.filePath()).hasMatch()
                       && (isExcludePatternInvalid || !excludeRegEx.match(info.filePath()).hasMatch())) {
                files << info;
            }
        }
    }
    return files;
}

extern Q_CORE_EXPORT int qt_ntfs_permission_lookup;

void SaveFiles(const QString& root, const QString& name, const QFileInfoList& files)
{
    QDir rootDir(root);
    QDir saveDir(SettingsProvider().backupDir());
    if (!saveDir.exists()) {
        saveDir.mkpath(".");
    }
    QFile file(saveDir.absoluteFilePath(name
                                        + "_"
                                        + QDateTime::currentDateTime().toString(Qt::ISODate).replace(":", "")
                                        + ".sgb"));
    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "Error saving " << file.fileName();
        QMessageBox::critical(0, QObject::tr("Save Error"), QObject::tr("Cannot save file %1").arg(file.fileName()));
        return;
    }
    QDataStream out(&file);
    out.startTransaction();
    qt_ntfs_permission_lookup++; // turn checking on
    foreach (const QFileInfo& info, files) {
        QFile in(info.filePath());
        if (!in.open(QIODevice::ReadOnly)) {
            qDebug() << "Error opening " << info.filePath();
            QMessageBox::critical(0, QObject::tr("Save Error"), QObject::tr("Cannot open file %1").arg(info.filePath()));
            out.abortTransaction();
            return;
        }
        out << rootDir.relativeFilePath(info.filePath())
            << static_cast<quint64>(info.permissions())
            << qCompress(in.readAll());
    }
    qt_ntfs_permission_lookup--; // turn it off again
    out.commitTransaction();
}

void MainWindow::RunBackupFor(const QModelIndex& idx)
{
    if (idx.isValid() && idx.data(PathRole).isValid() && idx.data(IncludesRole).isValid() && idx.data(ExcludesRole).isValid()) {
        qDebug() << "Backup for " << idx.data(PathRole) << " " << idx.data(IncludesRole) << " " << idx.data(ExcludesRole);
        const QFileInfoList& files(FindFiles(idx.data(PathRole).toString(), idx.data(IncludesRole).toString(), idx.data(ExcludesRole).toString()));
        SaveFiles(idx.data(PathRole).toString(), idx.data(NameRole).toString(), files);
    }
}

void MainWindow::on_action_Restore_triggered()
{
    foreach (const QModelIndex& idx, ui->listView->selectionModel()->selectedIndexes()) {
        RunRestoreFor(idx);
    }
}

void RestoreFiles(const QString& root, const QString& name)
{
    QDir rootDir(root);
    QDir saveDir(SettingsProvider().backupDir());
    if (!saveDir.exists()) {
        qDebug() << "Backup folder " << saveDir.path() << " not found.";
        return;
    }
    const QFileInfoList& files(saveDir.entryInfoList(QStringList(name + "_*.sgb"), QDir::Files, QDir::Name | QDir::Reversed));
    if (files.empty()) {
        qDebug() << "No backup file found for " << name;
        return;
    }
    QFile file(files.first().filePath());
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Error opening " << file.fileName();
        QMessageBox::critical(0, QObject::tr("Restore Error"), QObject::tr("Cannot open file %1").arg(file.fileName()));
        return;
    }
    QDataStream in(&file);
    qt_ntfs_permission_lookup++; // turn checking on
    while (!in.atEnd()) {
        QString fileName;
        in >> fileName;
        QFile outFile(rootDir.filePath(fileName));
        if (!outFile.open(QIODevice::WriteOnly)) {
            qDebug() << "Error opening " << outFile.fileName();
            QMessageBox::critical(0, QObject::tr("Restore Error"), QObject::tr("Cannot open file %1").arg(outFile.fileName()));
            return;
        }
        {
            quint64 permissions;
            in >> permissions;
            outFile.setPermissions(static_cast<QFile::Permission>(permissions));
        }
        {
            QByteArray data;
            in >> data;
            outFile.write(qUncompress(data));
        }
    }
    qt_ntfs_permission_lookup--; // turn it off again
}

void MainWindow::RunRestoreFor(const QModelIndex& idx)
{
    if (idx.isValid() && idx.data(PathRole).isValid() && idx.data(IncludesRole).isValid() && idx.data(ExcludesRole).isValid()) {
        qDebug() << "Backup for " << idx.data(PathRole);
        RestoreFiles(idx.data(PathRole).toString(), idx.data(NameRole).toString());
    }
}


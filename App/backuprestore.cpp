#include "backuprestore.h"
#include "settingsprovider.h"
#include "mainwindow.h"
#include <QtWidgets>
#include <QtConcurrent>
#include <QStandardItemModel>
#include <QDebug>

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

#ifdef Q_OS_WIN
extern Q_CORE_EXPORT int qt_ntfs_permission_lookup;
#endif

void SaveFiles(const QString& root, const QString& name, const QFileInfoList& files)
{
    QDir rootDir(root);
#ifdef Q_OS_WIN
    qt_ntfs_permission_lookup++; // turn checking on
#endif
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
        QMessageBox::critical(0, QObject::tr("Save Error"), QObject::tr("Cannot save file %1, reason: %2").arg(file.fileName(), file.errorString()));
        return;
    }
    QDataStream out(&file);
    out.startTransaction();
    foreach (const QFileInfo& info, files) {
        QFile in(info.filePath());
        if (!in.open(QIODevice::ReadOnly)) {
            out.abortTransaction();
            in.close();
            file.close();
            qDebug() << "Error opening " << in.fileName() << ", reason: " << in.errorString();
            QMessageBox::critical(0, QObject::tr("Save Error"), QObject::tr("Cannot open file %1, reason: %2").arg(in.fileName(), in.errorString()));
            return;
        }
        out << rootDir.relativeFilePath(info.filePath())
            << static_cast<quint64>(info.permissions())
            << qCompress(in.readAll());
        in.close();
    }
    out.commitTransaction();
    file.close();
#ifdef Q_OS_WIN
    qt_ntfs_permission_lookup--; // turn it off again
#endif
}

bool RunBackupFor(const QModelIndex& idx)
{
    if (idx.isValid() && idx.data(PathRole).isValid() && idx.data(IncludesRole).isValid() && idx.data(ExcludesRole).isValid() && idx.data(NameRole).isValid()) {
        qDebug() << "Backup for " << idx.data(PathRole) << " " << idx.data(IncludesRole) << " " << idx.data(ExcludesRole);
        const QFileInfoList& files(FindFiles(idx.data(PathRole).toString(), idx.data(IncludesRole).toString(), idx.data(ExcludesRole).toString()));
        SaveFiles(idx.data(PathRole).toString(), idx.data(NameRole).toString(), files);
    }
    return true;
}

void RestoreFiles(const QString& root, const QString& name, const QString& title)
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
        QMessageBox::information(0, QObject::tr("Failed to restore"), QObject::tr("No backup file found for %1").arg(title));
        return;
    }
#ifdef Q_OS_WIN
    qt_ntfs_permission_lookup++; // turn checking on
#endif
    QFile file(files.first().filePath());
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Error opening " << file.fileName() << ", reason: " << file.errorString();
        QMessageBox::critical(0, QObject::tr("Restore Error"), QObject::tr("Cannot open file %1, reason: %2").arg(file.fileName(), file.errorString()));
        return;
    }
    QDataStream in(&file);
    while (!in.atEnd()) {
        QString fileName;
        in >> fileName;
        QFile outFile(rootDir.filePath(fileName));
        {
            const QDir& dir(QFileInfo(outFile.fileName()).dir());
            if (!dir.exists()) {
                dir.mkpath(".");
            }
        }
        if (!outFile.open(QIODevice::WriteOnly)) {
            qDebug() << "Error opening " << outFile.fileName() << ", reason: " << outFile.errorString();
            QMessageBox::warning(0, QObject::tr("Restore Error"), QObject::tr("Cannot open file %1, reason: %2").arg(outFile.fileName(), outFile.errorString()));
            continue;
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
        outFile.close();
    }
    file.close();
#ifdef Q_OS_WIN
    qt_ntfs_permission_lookup--; // turn it off again
#endif
}

bool RunRestoreFor(const QModelIndex &idx)
{
    if (idx.isValid() && idx.data(PathRole).isValid() && idx.data(NameRole).isValid() && idx.data(TitleRole).isValid()) {
        qDebug() << "Backup for " << idx.data(PathRole);
        RestoreFiles(idx.data(PathRole).toString(), idx.data(NameRole).toString(), idx.data(TitleRole).toString());
    }
    return true;
}

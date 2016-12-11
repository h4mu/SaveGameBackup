#include "backuprestore.h"
#include "settingsprovider.h"
#include "mainwindow.h"
#include <QFile>
#include <QStandardItem>
#include <QtTest>

SettingsProvider::SettingsProvider()
{
}

QString SettingsProvider::backupDir() const
{
    return ".";
}

void SettingsProvider::setBackupDir(const QString& /*dir*/)
{
}

class BackupRestoreTests : public QObject
{
    Q_OBJECT

public:
    BackupRestoreTests();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void backupAndRestore();
    void backupAndRestoreWithMultipleRoots();
private:
    QStringList paths;
    const QString testFileName;
    const QString testFileContents;
    const QString testName;
};

BackupRestoreTests::BackupRestoreTests() : testFileName("testfile%1.txt"),
    testFileContents("asdfASDF%1"), testName("testName")
{
    paths << "./a/" << "./b/" << "./c/";
}

void messageHandler(QtMsgType /*type*/, const QMessageLogContext &/*context*/, const QString &/*msg*/)
{}

void BackupRestoreTests::initTestCase()
{
    qInstallMessageHandler(messageHandler);
    for (int i = 0; i < paths.length(); ++i) {
        QDir root(paths[i]);
        if (!root.exists()) {
            root.mkpath(".");
        }
        QFile file(root.absoluteFilePath(testFileName.arg(i)));
        if (!file.open(QIODevice::WriteOnly)) {
            QFAIL(file.errorString().toStdString().c_str());
        }
        {
            QTextStream out(&file);
            out << testFileContents.arg(i);
        }
        file.close();
        QVERIFY2(file.exists(), QString("Input file %1 does not exist").arg(file.fileName()).toStdString().c_str());
    }
}

void BackupRestoreTests::cleanupTestCase()
{
    for (const QString& path : paths) {
        Q_ASSERT(QDir(path).removeRecursively());
    }
    for (const QFileInfo& file : QDir(".").entryInfoList(QStringList(testName + "*"), QDir::Files)) {
        Q_ASSERT(QFile(file.fileName()).remove());
    }
}

QString GetFileContent(const QString& path, const QString& fileName)
{
    const QFileInfoList& infoList(QDir(path).entryInfoList(QStringList(fileName + "*"), QDir::Files));
    Q_ASSERT(infoList.count() >= 1);
    QFile file(infoList.first().filePath());
    Q_ASSERT(file.exists());
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << file.errorString().toStdString().c_str();
    }
    QString result;
    {
        QTextStream in(&file);
        in >> result;
    }
    file.close();
    return result;
}

void BackupRestoreTests::backupAndRestore()
{
    const QScopedPointer<QStandardItemModel> model(new QStandardItemModel(0,0));
    QStandardItem *item = new QStandardItem;
    item->setData(QVariant("Test Title"), TitleRole);
    item->setData(testName, NameRole);
    QVariantList dirs;
    dirs.push_back(QVariantList() << paths.first() << testFileName.arg(0) << "");
    item->setData(dirs, PathRole);
    model->setItem(0,0, item);
    const QModelIndex& index(model->index(0,0));
    QVERIFY(index.isValid());
    QVERIFY(index.data(TitleRole).isValid());
    QVERIFY(index.data(NameRole).isValid());
    QVERIFY(index.data(PathRole).isValid());
    RunBackupFor(index);
    QCOMPARE(QDir(".").entryInfoList(QStringList(testName + "*"), QDir::Files).count(), 1);
    RunRestoreFor(index);
    QCOMPARE(GetFileContent(paths.first(), testFileName.arg(0)), testFileContents.arg(0));
}

void BackupRestoreTests::backupAndRestoreWithMultipleRoots()
{
    const QScopedPointer<QStandardItemModel> model(new QStandardItemModel(0,0));
    QStandardItem *item = new QStandardItem;
    item->setData(QVariant("Test Title"), TitleRole);
    item->setData(testName, NameRole);
    QVariantList dirs;
    for (int i = 0; i < paths.length(); ++i) {
        dirs.push_back(QVariantList() << paths[i] << testFileName.arg(i) << "");
    }
    item->setData(dirs, PathRole);
    model->setItem(0,0, item);
    const QModelIndex& index(model->index(0,0));
    QVERIFY(index.isValid());
    QVERIFY(index.data(TitleRole).isValid());
    QVERIFY(index.data(NameRole).isValid());
    QVERIFY(index.data(PathRole).isValid());
    RunBackupFor(index);
    QCOMPARE(QDir(".").entryInfoList(QStringList(testName + "*"), QDir::Files).count(), 1);
    RunRestoreFor(index);
    for (int i = 0; i < paths.length(); ++i) {
        QCOMPARE(GetFileContent(paths[i], testFileName.arg(i)), testFileContents.arg(i));
    }
}

QTEST_APPLESS_MAIN(BackupRestoreTests)

#include "tst_backuprestoretests.moc"

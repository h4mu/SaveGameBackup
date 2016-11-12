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
};

#define TEST_FILE_PATH ("testfile.txt")
#define TEST_FILE_CONTENTS ("asdfASDF")
#define TEST_NAME ("testName")

BackupRestoreTests::BackupRestoreTests()
{
}

void messageHandler(QtMsgType /*type*/, const QMessageLogContext &/*context*/, const QString &/*msg*/)
{}

void BackupRestoreTests::initTestCase()
{
    qInstallMessageHandler(messageHandler);
    QFile file(TEST_FILE_PATH);
    if (!file.open(QIODevice::WriteOnly)) {
        QFAIL(file.errorString().toStdString().c_str());
    }
    {
        QTextStream out(&file);
        out << TEST_FILE_CONTENTS;
    }
    file.close();
    QVERIFY2(file.exists(), "Input file does not exist");
}

void BackupRestoreTests::cleanupTestCase()
{
    QFile(TEST_FILE_PATH).remove();
    foreach (QFileInfo info, QDir(".").entryInfoList(QStringList(QString(TEST_NAME) + "*"), QDir::Files)) {
        QFile(info.filePath()).remove();
    }
}

QString GetFileContent(const QString& fileName)
{
    const QFileInfoList& infoList(QDir(".").entryInfoList(QStringList(fileName + "*"), QDir::Files));
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
    item->setData(QVariant(TEST_NAME), NameRole);
    item->setData(QVariant("."), PathRole);
    item->setData(QVariant(TEST_FILE_PATH), IncludesRole);
    item->setData(QVariant(""), ExcludesRole);
    model->setItem(0,0, item);
    const QModelIndex& index(model->index(0,0));
    QVERIFY(index.isValid());
    QVERIFY(index.data(TitleRole).isValid());
    QVERIFY(index.data(NameRole).isValid());
    QVERIFY(index.data(PathRole).isValid());
    QVERIFY(index.data(IncludesRole).isValid());
    QVERIFY(index.data(ExcludesRole).isValid());
    RunBackupFor(index);
    QCOMPARE(QDir(".").entryInfoList(QStringList(QString(TEST_NAME) + "*"), QDir::Files).count(), 1);
    RunRestoreFor(index);
    QCOMPARE(GetFileContent(TEST_FILE_PATH), QString(TEST_FILE_CONTENTS));
}

QTEST_APPLESS_MAIN(BackupRestoreTests)

#include "tst_backuprestoretests.moc"

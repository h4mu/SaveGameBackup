#ifndef DIRECTORYSCANNER_H
#define DIRECTORYSCANNER_H
#include <QAbstractXmlReceiver>
#include "filesystembasepathfinder.h"
#include "registrybasepathfinder.h"
#include <QStandardItemModel>
#include <QXmlNamePool>

class DirectoryScanner : public QAbstractXmlReceiver
{
public:
    DirectoryScanner(QStandardItemModel * model, QXmlNamePool namePool);
    virtual void startElement(const QXmlName &name);
    virtual void endElement();
    virtual void attribute(const QXmlName &name, const QStringRef &value);
    virtual void comment(const QString &value);
    virtual void characters(const QStringRef &value);
    virtual void startDocument();
    virtual void endDocument();
    virtual void processingInstruction(const QXmlName &target,
                                       const QString &value);
    virtual void atomicValue(const QVariant &value);
    virtual void namespaceBinding(const QXmlName &name);
    virtual void startOfSequence();
    virtual void endOfSequence();

private:
    QStandardItemModel * model;
    QXmlNamePool namePool;
    int depth;
    QString name;
    QString title;
    QString type;
    QStringList include;
    QStringList exclude;
    FileSystemBasePathFinder fsBaseFinder;
    RegistryBasePathFinder regBaseFinder;

};

#endif // DIRECTORYSCANNER_H

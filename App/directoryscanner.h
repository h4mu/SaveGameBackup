#ifndef DIRECTORYSCANNER_H
#define DIRECTORYSCANNER_H
#include <QAbstractXmlReceiver>
#include <QXmlNamePool>
#include "filesystembasepathfinder.h"
#include "registrybasepathfinder.h"

class DirectoryScanner : public QAbstractXmlReceiver
{
public:
    DirectoryScanner(QXmlNamePool namePool);
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
    QVariantList foundGames() const;

private:
    int depth;
    QXmlNamePool namePool;
    QString name;
    QString title;
    QString type;
    QStringList include;
    QStringList exclude;
    FileSystemBasePathFinder fsBaseFinder;
    RegistryBasePathFinder regBaseFinder;
    QVariantList dirs;
    QVariantList result;
};

#endif // DIRECTORYSCANNER_H

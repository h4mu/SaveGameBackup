#include "directoryscanner.h"
#include <QList>

DirectoryScanner::DirectoryScanner(QXmlNamePool namePool)
    : depth(0), namePool(namePool)
{
}

QVariantList DirectoryScanner::foundGames() const
{
    return result;
}

void DirectoryScanner::startElement(const QXmlName &/*name*/)
{
    ++depth;
}

void DirectoryScanner::endElement()
{
    switch (--depth) {
    case 1:
    {
        QString basePath(
                    type == "registry"
                    ? regBaseFinder.GetBasePath()
                    : fsBaseFinder.GetBasePath());
        if(!basePath.isEmpty()) {
            dirs.push_back(QVariantList() << basePath << include.join('|') << exclude.join('|'));
        }
        include.clear();
        exclude.clear();
        break;
    }
    case 0:
        if (dirs.length() > 0) {
            QVariantList entry;
            entry  << name << title;
            entry.push_back(dirs);
            result.push_back(entry);
            dirs.clear();
        }
        break;
    default:
        break;
    }
}

void DirectoryScanner::attribute(const QXmlName &name, const QStringRef &value)
{
    QString localName(name.localName(namePool));
    if(localName == "name") {
        this->name = value.toString();
    } else if(localName == "title") {
        title = value.toString();
    } else if(localName == "type") {
        type = value.toString();
        fsBaseFinder.setShortcutMode(type == "shortcut");
    } else if(localName == "ev") {
        fsBaseFinder.setEV(value.toString());
    } else if(localName == "path") {
        fsBaseFinder.setPath(value.toString());
        regBaseFinder.setPath(value.toString());
    } else if(localName == "key") {
        regBaseFinder.setKey(value.toString());
    } else if(localName == "root") {
        regBaseFinder.setRoot(value.toString());
    } else if(localName == "includepath") {
        include << value.toString();
    } else if(localName == "excludepath") {
        exclude << value.toString();
    }
}

void DirectoryScanner::comment(const QString &/*value*/){}
void DirectoryScanner::characters(const QStringRef &/*value*/){}
void DirectoryScanner::startDocument(){}
void DirectoryScanner::endDocument(){}
void DirectoryScanner::processingInstruction(const QXmlName &/*target*/, const QString &/*value*/){}
void DirectoryScanner::atomicValue(const QVariant &/*value*/){}
void DirectoryScanner::namespaceBinding(const QXmlName &/*name*/){}
void DirectoryScanner::startOfSequence(){}
void DirectoryScanner::endOfSequence(){}

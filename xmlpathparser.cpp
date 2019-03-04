#include "xmlpathparser.h"
#include <QFile>
#include <QtDebug>

XmlPathParser::XmlPathParser()
{

}

XmlPathParser::~XmlPathParser()
{

}

bool XmlPathParser::openXmlDoc(const QString xmlFile)
{
    QFile file(xmlFile);
    QString errorMsg;
    int errorLine, errorColumn;

    if(!file.open(QIODevice::ReadOnly))
    {
        mErrorString = QObject::tr("Open XML file %1 failed: %2").arg(xmlFile).arg(file.errorString());
        return false;
    }
    if(!dom.setContent(&file, &errorMsg, &errorLine, &errorColumn))
    {
        mErrorString = QObject::tr("Parse XML document failed: %1, Line %2, Column %3").arg(errorMsg).arg(errorLine).arg(errorColumn);
        file.close();
        return false;
    }
    file.close();

    return true;
}

bool XmlPathParser::parseXmlDoc()
{
    if(dom.isNull())
        return false;
    QDomElement d = dom.documentElement();
    d = d.firstChildElement(modelTag());

    if(d.isNull())
    {
        mErrorString = QObject::tr("No tag %1 in XML document").arg(modelTag());
        return false;
    }
    else
    {
        QString path;
        mModelName = d.attribute(nameAttr());
        d = d.firstChildElement();
        parseXmlAttrAndPath(d, path);
        return true;
    }
}

void XmlPathParser::closeXmlDoc()
{
    dom.clear();
    mXmlAttrList.clear();
    mXmlPathList.clear();
    mErrorString.clear();
    mModelName.clear();
}

const QList<QStringList> &XmlPathParser::xmlAttrList() const
{
    return mXmlAttrList;
}

const QStringList &XmlPathParser::xmlPathList() const
{
    return mXmlPathList;
}

void XmlPathParser::parseXmlAttrAndPath(QDomElement &d, QString path)
{
    QStringList list;
    QDomNode n = d.firstChild();
    path.append(d.attribute(nameAttr()));
    if(!n.isNull())
        path.append(".");
    list.append(path);
    foreach(QString attr, otherAttr()) {
        list.append(d.attribute(attr));
    }
    mXmlAttrList.append(list);
    mXmlPathList.append(path);
    while (!n.isNull()) {
        QDomElement e = n.toElement();
        if(!e.isNull()) {
            parseXmlAttrAndPath(e, path);
        }
        n = n.nextSibling();
    }
}

QString XmlPathParser::errorString() const
{
    return mErrorString;
}

QString XmlPathParser::modelName() const
{
    return mModelName;
}

QString XmlPathParser::modelTag()
{
    return "model";
}

QString XmlPathParser::objectTag()
{
    return "object";
}

QString XmlPathParser::parameterTag()
{
    return "parameter";
}

QString XmlPathParser::descriptionTag()
{
    return "description";
}

QString XmlPathParser::nameAttr()
{
    return "name";
}

QStringList XmlPathParser::otherAttr()
{
    return QStringList();
}

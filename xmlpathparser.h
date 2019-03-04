#ifndef XMLPATHPARSER_H
#define XMLPATHPARSER_H

#include <QDomDocument>
#include <QStringList>


class XmlPathParser
{
public:
    XmlPathParser();
    virtual ~XmlPathParser();

    bool openXmlDoc(const QString xmlFile);
    bool parseXmlDoc();
    void closeXmlDoc();

    QList<QStringList> &xmlAttrList();
    const QStringList &xmlPathList() const;
    QString errorString() const;
    QString modelName() const;

protected:
    virtual QString modelTag();
    virtual QString objectTag();
    virtual QString parameterTag();
    virtual QString descriptionTag();
    virtual QString nameAttr();
    virtual QStringList otherAttr();

private:
    void parseXmlAttrAndPath(QDomElement &d, QString path);

    QDomDocument dom;
    QList<QStringList> mXmlAttrList;
    QStringList mXmlPathList;
    QString mErrorString;
    QString mModelName;
};

#endif // XMLPATHPARSER_H

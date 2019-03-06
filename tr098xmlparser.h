#ifndef TR098XMLPARSER_H
#define TR098XMLPARSER_H

#include "xmlpathparser.h"


class Tr098XmlParser : public XmlPathParser
{
public:
    Tr098XmlParser();
    void preprocessData();

protected:
    QStringList otherAttr();

private:
    static bool isTrueValue(const QString &str);
};

#endif // TR098XMLPARSER_H

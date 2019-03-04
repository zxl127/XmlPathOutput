#ifndef TR098XMLPARSER_H
#define TR098XMLPARSER_H

#include "xmlpathparser.h"


class Tr098XmlParser : public XmlPathParser
{
public:
    Tr098XmlParser();

protected:
    QStringList otherAttr();
};

#endif // TR098XMLPARSER_H

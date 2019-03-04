#include "tr098xmlparser.h"

Tr098XmlParser::Tr098XmlParser()
{

}

QStringList Tr098XmlParser::otherAttr()
{
    return QStringList() << "rw" << "forcedInform" << "description";
}

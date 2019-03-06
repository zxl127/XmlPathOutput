#include "tr098xmlparser.h"


Tr098XmlParser::Tr098XmlParser()
{

}

void Tr098XmlParser::preprocessData()
{
    QList<QStringList> &list = xmlAttrList();

    for (int i = 0; i < list.count(); i++) {
        if(isTrueValue(list[i][1]))
            list[i][1] = "true";
        else
            list[i][1] = "";
        if(isTrueValue(list[i][2]))
            list[i][2] = "true";
        else
            list[i][2] = "";
    }
}

QStringList Tr098XmlParser::otherAttr()
{
    return QStringList() << "rw" << "forcedInform" << "description";
}

bool Tr098XmlParser::isTrueValue(const QString &str)
{
    if(str == QString("1") || str.compare(QString("true"), Qt::CaseInsensitive) == 0)
        return true;
    else
        return false;
}

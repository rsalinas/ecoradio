#include "rssparser.h"

#include <QtXml/QtXml>
#include <QStack>
#include <QDebug>

RssParser::RssParser(const QString &filename)
{
    QXmlSimpleReader parser;
    parser.setContentHandler(this);
    QFile f(filename);
    QXmlInputSource source(&f);
    if (parser.parse(&source))     {
    }     else     {
        qDebug() << " " << filename;
        throw RssException("RSS Parsing Failed " + filename);
    }
}


QStringList RssParser::getStreams() {
    return m_streams;
}


bool RssParser::startElement(const QString & namespaceURI, const QString & localName,
                             const QString & qName, const QXmlAttributes & atts )
{
    stack.push(qName);
//    qDebug() << localName << "STACK: "<< stack;

    QString url;
    int size = 0;

    for(int index = 0 ; index < atts.length();index++)
    {
        qDebug().noquote() <<printIndent() << atts.type(index)<< "=" << atts.value(index) << atts.qName(index);
        if (stack.top() == "enclosure" && atts.type(index) == "CDATA") {
            if (atts.qName(index) == "url") {
                url = atts.value(index);
            } else if (atts.qName(index) == "length") {
                size = atts.value(index).toInt();
            }
        }
    }
    if (url.size()) {
        m_streams.push_back(url);
    }
    return true;
}
bool RssParser::endElement(const QString & namespaceURI, const QString & localName, const QString & qName) {
    stack.pop();
    return true;
}

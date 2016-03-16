#include "rssparser.h"

#include <QtXml/QtXml>
#include <QStack>
#include <QDebug>

RssParser::RssParser(QIODevice *dev, QObject * parent) : QObject(parent), m_file(dev)
{
        qDebug() << "paser from dev";
    QXmlSimpleReader parser;
    parser.setContentHandler(this);
    QXmlInputSource source(m_file);
    if (parser.parse(&source))     {
    }     else     {
        throw RssException("RSS Parsing Failed from dev");
    }
}


RssParser::RssParser(const QString &filename, QObject * parent) : RssParser((fprintf(stderr, "a ver %s\n", filename.toStdString().c_str()), new QFile(filename, parent)))
{  
    qDebug() << "paser from file" << filename;
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
        //        qDebug().noquote() <<printIndent() << atts.type(index)<< "=" << atts.value(index) << atts.qName(index);
        if (stack.top() == "enclosure" && atts.type(index) == "CDATA") {
            if (atts.qName(index) == "url") {
                url = atts.value(index);
            } else if (atts.qName(index) == "length") {
                size = atts.value(index).toInt();
            }
        }
    }
    if (url.size()) {
        (void) size;
        m_streams.push_back(url);
    }
    return true;
}

bool RssParser::endElement(const QString & namespaceURI, const QString & localName, const QString & qName)
{
    stack.pop();
    return true;
}

RssParser::~RssParser()
{
    qDebug() << __FUNCTION__;
}

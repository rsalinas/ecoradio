#include "rssparser.h"

#include <QtXml/QtXml>
#include <QStack>
#include <QDebug>

class Item
{
public:
};


class MyXmlContentHandler :public QXmlDefaultHandler
{
    int indent = 0;
    QStack<QString> stack;

public:
    QString printIndent() {
        return QString::fromStdString(std::string(indent*4, ' '));
    }
    MyXmlContentHandler():QXmlDefaultHandler()
    {

    };
    ~MyXmlContentHandler()
    {

    };

    bool startElement(const QString & namespaceURI, const QString & localName,
                      const QString & qName, const QXmlAttributes & atts ) override
    {
        stack.push(qName);
        //        qDebug() << localName << "STACK: "<< stack;

        QString url;
        int size = 0;

        for(int index = 0 ; index < atts.length();index++)
        {
            //            qDebug() <<printIndent() << atts.type(index)<< "=" << atts.value(index) << atts.qName(index);
            if (stack.top() == "enclosure" && atts.type(index) == "CDATA") {
                if (atts.qName(index) == "url") {
                    url = atts.value(index);
                } else if (atts.qName(index) == "length") {
                    size = atts.value(index).toInt();
                }

            }

        }
        if (url.size()) {
            qDebug() << "MP3: " << url << size;
//            m_stream
        }

        //        qDebug() <<printIndent()<< "------------------------" ;
        indent ++;
        return true;
    };
    bool endElement(const QString & namespaceURI, const QString & localName, const QString & qName) override {
        indent--;;
        stack.pop();
        //        qDebug() << stack;
        return true;
    }
};

RssParser::RssParser(const QString &filename)
{

    QXmlSimpleReader* parser 		= new QXmlSimpleReader();
    MyXmlContentHandler* handler 	= new MyXmlContentHandler();
    
    parser->setContentHandler(handler);
    
    
    QFile f(filename);
    if (parser->parse(new QXmlInputSource(&f)))
    {
        qDebug() <<"Parsed Successfully!";
    }
    else
    {
        qDebug() <<"Parsing Failed...";
    }
}




QStringList RssParser::getStreams() {
    return m_streams;
}

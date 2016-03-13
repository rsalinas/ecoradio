#pragma once

#include <QStringList>
#include <QtXml/QtXml>

class RssParser : private QXmlDefaultHandler
{
public:
    RssParser(const QString &filename);
    QStringList getStreams();

private:
    QStringList m_streams;
    friend class MyXmlContentHandler;
};

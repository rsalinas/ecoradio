#pragma once

#include <QStringList>

class RssParser
{
public:
    RssParser(const QString &filename);
    QStringList getStreams();

private:
    QStringList m_streams;
    friend class MyXmlContentHandler;
};

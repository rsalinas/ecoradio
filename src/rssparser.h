#pragma once

#include <QStringList>

class RssParser
{
public:
    RssParser(const QString &filename);
    QStringList getStreams();
//    ~RssParser();

private:
    QStringList m_streams;
};

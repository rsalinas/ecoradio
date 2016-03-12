#include "util.h"

#include <QDebug>

TimeMeter::TimeMeter() : m_start(QDateTime::currentMSecsSinceEpoch()) {
}

decltype(QDateTime::currentMSecsSinceEpoch()) TimeMeter::ellapsed() {
    return QDateTime::currentMSecsSinceEpoch() - m_start;
}



AutoTimeMeter::AutoTimeMeter(const QString &name,
                             int threshold) :
    m_name(name), m_threshold(threshold)
{
}

AutoTimeMeter::~AutoTimeMeter()
{
    auto ellapsedTime = ellapsed();
    if (ellapsedTime >= m_threshold)
        qDebug() << "Finished " << m_name << ellapsedTime;
}

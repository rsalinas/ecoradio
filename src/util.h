#pragma once

#include <QDateTime>

class TimeMeter {
public:
    TimeMeter();
    decltype(QDateTime::currentMSecsSinceEpoch()) ellapsed();

private:
    decltype(QDateTime::currentMSecsSinceEpoch()) m_start;
};


class AutoTimeMeter : public TimeMeter
{
public:
    AutoTimeMeter(const QString &name, int threshold = 100);
    ~AutoTimeMeter();

private:
    decltype(QDateTime::currentMSecsSinceEpoch()) m_start;
    QString m_name;
    int m_threshold;
};

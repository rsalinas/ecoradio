#pragma once

#include <QDateTime>
#include <QObject>
#include <QEventLoop>

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

class SynSleep: public QObject {
    Q_OBJECT
public:
    SynSleep(){
        needRunning=1;
    }

public slots:

    void sleep(const QObject *sender, const char *signal){
        QObject::connect(sender, signal, this   , SLOT(finish()));
        if (needRunning==1)
            loop.exec();
    }
    void sleep(){
        if (needRunning==1)
            loop.exec();
    }

    void reset(){
        needRunning=1;
    }

    virtual void finish();


private:
    QEventLoop loop;
    int needRunning;

};

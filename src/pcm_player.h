#pragma once

#include <QMutex>
#include <QWaitCondition>
#include <QMutexLocker>

#include <ao/ao.h>
#include <cstdio>
#include <exception>
#include <cstring>
#include <memory>
#include <QThread>
#include <QObject>
#include "sink.h"

class SoundSource;
class PlayingThread;

class PcmPlayer : public QThread
{
    Q_OBJECT
public:
    class PcmPlayerException : public std::exception
    {

    };

    PcmPlayer(const SndSink::Format &format = SndSink::Format() /*FIXME*/);
    virtual ~PcmPlayer();

    int addSource(std::shared_ptr<SoundSource> s);
    int addSink(std::shared_ptr<SndSink> s);

    void run() override;

    void waitEnd();

    const int buf_size;

private:
    const SndSink::Format m_format;
    std::list<std::shared_ptr<SndSink>> m_sinks;
    std::list<std::shared_ptr<SoundSource>> m_sources;

    QMutex mutex;
    QWaitCondition condition;
    bool abort = false;

signals:
    void silenceFinished();
    void silenceStarted();

public slots:

};




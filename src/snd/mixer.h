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
#include "sinks/sink.h"

class SoundSource;
class PlayingThread;

Q_DECLARE_METATYPE(std::shared_ptr<SoundSource>)


class Mixer : public QThread
{
    Q_OBJECT
public:
    class PcmPlayerException : public std::exception
    {

    };

    Mixer(const SndFormat &format = SndFormat() /*FIXME*/);
    virtual ~Mixer();

    int addSource(std::shared_ptr<SoundSource> s);
    int addSink(std::shared_ptr<SndSink> s);

    void run() override;

    void waitEnd();

    const int buf_size;
    size_t activeSourceCount();

private:
    const SndFormat m_format;
    std::list<std::shared_ptr<SndSink>> m_sinks;
    std::list<std::shared_ptr<SoundSource>> m_sources;

    QMutex mutex;
    QWaitCondition condition;
    bool abort = false;

signals:   
    void silenceFinished();
    void silenceStarted();
    void sourceFinished(std::shared_ptr<SoundSource> s);
    void songFinishing(std::shared_ptr<SoundSource> s);
    void vumeter(int channel, int value);

public slots:

};




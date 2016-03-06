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

class SoundSource;
class PlayingThread;
class ao_device;

class PcmPlayer : public QThread
{
    Q_OBJECT
public:
    PcmPlayer(int bufferMillis = 200);
    virtual ~PcmPlayer();

    int addStream(std::shared_ptr<SoundSource> s);

    void run() override;

    ao_sample_format format;
    size_t buf_size;
    void waitEnd();

private:
    char * buffer = nullptr;
    ao_device *device = nullptr;    
    QMutex mutex;
    QWaitCondition condition;
    bool abort = false;
    std::list<std::shared_ptr<SoundSource>> m_sources;

signals:
public slots:

};


class PcmPlayerException : public std::exception
{

};

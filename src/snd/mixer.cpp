#include "mixer.h"


#include <QDebug>
#include <QEventLoop>
#include "util/util.h"
#include "sources/soundsource.h"


Mixer::Mixer(const SndFormat &format)
    : m_format(format)
    , buf_size(format.bufferSize)
    , buffer(new char[buf_size])
    , zeros(new char[buf_size])
{

    qRegisterMetaType<std::shared_ptr<SoundSource> >();
}

void Mixer::mixOnto(char * newBuffer, char * destBuffer) {
    auto dest = reinterpret_cast<signed short int *>(destBuffer);
    auto signal = reinterpret_cast<signed short int *>(newBuffer);
    for (size_t i = 0; i < buf_size/ (m_format.sampleSizeBits/8 ); ++i) {
        int sample = dest[i] + signal[i];
        if (sample > SHRT_MAX)
            sample = SHRT_MAX;
        if (sample < SHRT_MIN)
            sample = SHRT_MIN;
        dest[i] = (short int) sample;
    }
}

void Mixer::run() {
    if (m_sinks.empty()) {
        qWarning() << "Refusing to start mixer without sinks";
        return;
    }

    char newBuffer[buf_size];
    memset(zeros, 0, buf_size);
    memset(buffer, 0, buf_size);
    memset(newBuffer, 0, buf_size);
    qDebug() << __FUNCTION__;
    bool silence = true;

    while (!abort) {
        TimeMeter tm;
        decltype(m_sources) sourcesNow;
        {
            QMutexLocker lock(&mutex);
            sourcesNow = m_sources;
        }
        int actualSourceCount = 0;
        for (auto source : sourcesNow) {            
            int n = source->readFading(actualSourceCount==0 ? buffer: newBuffer, buf_size);
            switch (n) {
            case 0:
//                qDebug() << "raro: zero" << source->name();
                //Silence
                continue;
            case -1:
                qDebug() << source->name() << n;
                removeSource(source);
                continue;
            default:
                if (buf_size != n)
                    memset((actualSourceCount==0 ? buffer: newBuffer)+n, 0, buf_size-n);

                if (actualSourceCount >= 1) {
                    mixOnto(newBuffer, buffer);

                }
            }

            actualSourceCount++;

        }
        auto ellapsed = tm.ellapsed();

        if (m_sinks.empty()) {
            usleep(buf_size*1000/44100/2/2);
        } else
            for (auto sink : m_sinks) {
                sink->writePcm(actualSourceCount ? buffer : zeros, buf_size);
            }

        if (actualSourceCount) {
            if (silence)  {
                silence = false;
                emit silenceFinished();
            }
        } else {
            if (!silence) {
                silence = true;
                emit silenceStarted();
            }
        }

        calculateVuMeter();
        if (ellapsed > 100)
            qDebug() << "In main: "  << ellapsed;
    }
    qDebug() << __FUNCTION__ << " finished properly";
}

void Mixer::calculateVuMeter() {
    quint64 sqSum = 0;
    auto b = reinterpret_cast<signed short int *>(buffer);
    for (int pos=0; pos < buf_size/ (m_format.sampleSizeBits/8 ); ++pos) {
        sqSum += b[pos] * b[pos];
    }
    float asp = std::sqrt(float(sqSum) / buf_size/2);
    emit vumeter(0, asp * 255.0);
}

size_t Mixer::activeSourceCount() {
    return m_sources.size();
}

void Mixer:: waitEnd() {
    QMutexLocker lock(&mutex);
    while (m_sources.size()) {
        qDebug() << "Waiting for finish. Sources: " << m_sources.size();
        condition.wait(&mutex);
    }
}

int Mixer::addSource(std::shared_ptr<SoundSource> source) {
    QMutexLocker lock(&mutex);
    m_sources.push_back(source);
    if (m_sinks.empty()) {
        qWarning() << "Mixer has no sinks";
    }
    if(!isRunning()) {
        qWarning() << "Mixer is not started";
    }
    return 0;
}

int Mixer::addSink(std::shared_ptr<SndSink> sink) {
    if (!sink) {
        qFatal("Cannot add empty sink");
    }
    QMutexLocker lock(&mutex);
    m_sinks.push_back(sink);
    return 0;
}

Mixer::~Mixer() {
    mutex.lock();
    abort = true;
    condition.wakeAll();
    mutex.unlock();
    wait();
    delete [] buffer;
    delete [] zeros;
}


void Mixer::removeSource(std::shared_ptr<SoundSource> source)
{
    QMutexLocker lock(&mutex);
    m_sources.remove(source);
    condition.wakeAll();
    emit sourceFinished(source);
}

#include "mixer.h"


#include <QDebug>
#include <QEventLoop>
#include "util/util.h"
#include "sources/soundsource.h"


Mixer::Mixer(const SndFormat &format) :
    m_format(format), buf_size(format.bufferSize)
{
    qDebug() << "qRegisterMetaType<std::shared_ptr<SoundSource> >()";
    qRegisterMetaType<std::shared_ptr<SoundSource> >();
}

void Mixer::run() {
    char zeros[buf_size];
    char buffer[buf_size];
    char newBuffer[buf_size];
    memset(zeros, 0, sizeof(buffer));
    memset(buffer, 0, sizeof(buffer));
    memset(newBuffer, 0, sizeof(buffer));
    qDebug() << __FUNCTION__;
    bool silence = true;

    while (!abort) {
        TimeMeter tm;
        decltype(m_sources) sourcesNow;
        {
            QMutexLocker lock(&mutex);
            sourcesNow = m_sources;
        }
        int i=0;
        for (auto source : sourcesNow) {
            int n = source->readFading(i==0 ? buffer: newBuffer, sizeof(buffer));
            switch (n) {
            case 0:
                qDebug() << "raro: zero" << source->name();
                continue;
            case -1:
            {
                QMutexLocker lock(&mutex);
                qDebug() << source->name() << n;
                m_sources.remove(source);
                condition.wakeAll();
                emit sourceFinished(source);
                continue;
            }
            default:
                if (sizeof(buffer) != n)
                    memset((i==0 ? buffer: newBuffer)+n, 0, sizeof(buffer)-n);

                if (i >= 1) {
                    auto b = reinterpret_cast<signed short int *>(buffer);
                    auto b1 = reinterpret_cast<signed short int *>(newBuffer);
                    for (int i=0; i < sizeof(buffer)/ (m_format.sampleSizeBits/8 ); i++) {
                        int sample = b[i] + b1[i];
                        if (sample > SHRT_MAX)
                            sample=SHRT_MAX;
                        if (sample < SHRT_MIN)
                            sample = SHRT_MIN;
                        b[i] = (short int) sample;
                    }
                }
            }

            i++;

        }
        auto ellapsed = tm.ellapsed();


        if (i) {
            for (auto sink : m_sinks) {
                sink->writePcm(buffer, sizeof(buffer));
            }
            if (silence)  {
                silence = false;
                emit silenceFinished();
            }

        } else {
            for (auto sink : m_sinks) {
                sink->writePcm(zeros, sizeof(zeros));

            }
            if (!silence) {
                silence = true;
                emit silenceStarted();
            }
        }

        quint64 sqSum = 0;
        auto b = reinterpret_cast<signed short int *>(buffer);
        for (int pos=0; pos < sizeof(buffer)/ (m_format.sampleSizeBits/8 ); ++pos) {
            sqSum += b[pos] * b[pos];
        }
        float asp = std::sqrt(float(sqSum) / sizeof(buffer)/2);
        emit vumeter(0, asp * 255.0);


        if (ellapsed > 100)
            qDebug() << "In main: "  << ellapsed;
    }
    qDebug() << __FUNCTION__ << " finished properly";
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
}

int Mixer::addSink(std::shared_ptr<SndSink> sink) {
    if (!sink) {
        qFatal("Cannot add empty sink");
    }
    QMutexLocker lock(&mutex);
    m_sinks.push_back(sink);
}

Mixer::~Mixer() {
    mutex.lock();
    abort = true;
    condition.wakeAll();
    mutex.unlock();
    wait();
}

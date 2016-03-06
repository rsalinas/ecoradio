#include "pcm_player.h"
#include <cstring>
#include <QDebug>
#include <climits>
#include <QThread>
#include <list>
#include "util.h"
#include "soundsource.h"

void PcmPlayer::run() {
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
                continue;
            }
            default:
                if (sizeof(buffer) != n)
                    memset((i==0 ? buffer: newBuffer)+n, 0, sizeof(buffer)-n);

                if (i >= 1) {
                    auto b = reinterpret_cast<signed short int *>(buffer);
                    auto b1 = reinterpret_cast<signed short int *>(newBuffer);
                    for (int i=0; i < sizeof(buffer)/ (format.bits/8 ); i++) {
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

            if (1 != ao_play(device, buffer, sizeof(buffer))) {
                qDebug() << "ao_play error";
            }
            if (silence)  {
                silence = false;
                emit silenceFinished();
            }

        } else {
            if (1 != ao_play(device, zeros, sizeof(zeros))) {
                qDebug() << "ao_play error";
            }
            if (!silence) {
                silence = true;
                emit silenceStarted();
            }

        }

        if (ellapsed > 100)
            qDebug() << "In main: "  << ellapsed;
    }
    qDebug() << __FUNCTION__ << "finished";
}

void PcmPlayer::waitEnd() {
    QMutexLocker lock(&mutex);
    while (m_sources.size()) {
        qDebug() << "Waiting for finish. Sources: " << m_sources.size();
        condition.wait(&mutex);
    }
}

int PcmPlayer::addStream(std::shared_ptr<SoundSource> s) {
    QMutexLocker lock(&mutex);
    m_sources.push_back(s);
}

PcmPlayer::~PcmPlayer() {
    mutex.lock();
    abort = true;
    condition.wakeAll();
    mutex.unlock();
    wait();



    if (buffer)
        free(buffer);
    /* -- Close and shutdown -- */
    ao_close(device);

    ao_shutdown();
}


PcmPlayer::PcmPlayer(int bufferMillis) {
    ao_initialize();
    /* -- Setup for default driver -- */

    int default_driver = ao_default_driver_id();
    memset(&format, 0, sizeof(format));
    format.bits = 16;
    format.channels = 2;
    format.rate = 44100;
    format.byte_format = AO_FMT_LITTLE;

    buf_size = format.bits/8 * format.channels * format.rate * bufferMillis/ 1000;
    qDebug() << buf_size << "buffer;";
    buffer = static_cast<char*>(calloc(buf_size, sizeof(char)));

    /* -- Open driver -- */
    device = ao_open_live(default_driver, &format, NULL /* no options */);
    if (device == NULL) {
        fprintf(stderr, "Error opening device.\n");
        throw PcmPlayerException();
    }
    start();
}

#include "soundsource.h"

#include <QDebug>
#include "util/util.h"

SoundSource::SoundSource(const QString &name) : m_name(name)
{
    start();
}


SoundSource::~SoundSource()
{
    m_mutex.lock();
    m_abort = true;
    m_cv.wakeAll();
    m_mutex.unlock();
    wait();

}


int SoundSource::readFading(char * buf, const size_t length)  {
    AutoTimeMeter atm("readFading");
    int n = readPcm(buf, length);
    if (n <= 0 || m_fading == 0)
        return n;

    double volume;
    auto b = reinterpret_cast<signed short int *>(buf);
    for (int i = 0; i < length/ 2; i++) {
        size_t here = m_bytes + i*2;
        if (here >= fadingEndBytes) {
            if (m_fading == 1 ){
                m_fading = 0;
                volume = 1;
            } else if (m_fading == -1 ){
                m_fading = 0;
                volume = 0;
                switch (m_fadeAction) {
                case Stop:
                    close();
                    break;
                case Pause:
                    pause();
                    break;
                case Silence:
                    break;
                }

            }
        } else {
            if (m_fading == 1) {
                volume =  float(here) / fadingEndBytes;

            } else {
                volume = 1 - float(here)  / fadingEndBytes;
            }
        }

        assert(volume >= 0);
        assert(volume <= 1);

        b[i] = volume*b[i];
    }
    //    qDebug() << m_bytes << "volume " << volume << fadingEndBytes;
    m_bytes += n;
    return n;
}


SinWave::SinWave(float freq, int rate) : SoundSource("sin"+QString::number(freq)), m_freq(freq), m_rate(rate)
{

}

int SinWave::readPcm(char * buffer, const size_t length) {
    if (m_status == Finished) {
        return -1;
    }
    for (int i=0; i < length/4; i++) {
        int sample = (int)(0.75 * 32768.0 *
                           sin(2 * M_PI * m_freq * ((float) (i+m_base)/m_rate)));

        /* Put the same stuff in left and right channel */
        buffer[4*i] = buffer[4*i+2] = sample & 0xff;
        buffer[4*i+1] = buffer[4*i+3] = (sample >> 8) & 0xff;
    }
    m_base+=length/4;
    return length/4*4;
}

void SoundSource::close() {
    Status previousStatus;
    {
        QMutexLocker lock(&m_mutex);
        previousStatus = m_status;
        m_status = Finished;
        m_cv.wakeAll();
    }
    if (previousStatus != Finished)
        emit finished();
}

void SoundSource::pause() {
        QMutexLocker lock(&m_mutex);
        m_status = Paused;
        m_cv.wakeAll();
}

void SoundSource::play() {
        QMutexLocker lock(&m_mutex);
        m_status = Playing;
        m_cv.wakeAll();
}


int SoundSource::skip(int millis) {
    char buffer[65536];
    size_t bytesToSkip = 44100 * 2 * 2 * millis / 1000;
    while (bytesToSkip) {
        int ret = readPcm(buffer, std::min(sizeof(buffer), bytesToSkip));
        if (ret <= 0)
            return ret;
    }
    return 0;
}
int SoundSource::setFadeIn(int millis) {
    if (millis <= 0) {
        m_fading =0;
        return 0;
    }
    fadingEndBytes = m_bytes + (44100 * 2 * 2 * millis / 1000);
    m_bytes = 0;
    m_fading = 1;
    qDebug() << "Fading up " << m_fading << fadingEndBytes;
    return 1;
}

int SoundSource::fadeOut(int millis, FadeAction fadeAction) {
    fadingEndBytes = m_bytes + (44100 * 2 * 2 * millis / 1000);
    m_bytes = 0;
    m_fading = -1;
    m_fadeAction = fadeAction;
    qDebug() << "fading " << m_fading << fadingEndBytes;
}


void SoundSource::run() {
    qDebug() << __FUNCTION__ << "source";
    //        while (!m_abort) {

    //        }
}


void SoundSource::waitEnd() {
    QMutexLocker lock(&m_mutex);
    while (m_status == Playing) {
        qDebug() << "not closed";
        m_cv.wait(&m_mutex);
    }
}



ProcessDecoder::ProcessDecoder(const QString &name, const QString& binary, QStringList args) : SoundSource(name) {
    m_process.setProcessChannelMode(QProcess::ForwardedErrorChannel);
    m_process.setReadChannel(QProcess::StandardOutput);

    m_process.start(binary, args);
    if (!m_process.waitForStarted(1000)) {
        qDebug() << "error starting mpg123";

    }
    assert(m_process.waitForReadyRead(3000));
}


ProcessDecoder::~ProcessDecoder() {
    qDebug() << "killing process" << name();
    m_process.terminate();
    qDebug() << name() << "waitForFinished";
    m_process.waitForFinished(1000);
    qDebug() << name() << "waitForFinished done";
}


int ProcessDecoder::readPcm(char * buf, const size_t length) {
    ssize_t remaining = length;
    if (m_status == Finished)
        return -1;

    if (m_status == Paused)
        return 0;

    do  {
        if (! m_process.waitForReadyRead(3000)) {
            qDebug() << "mp3: no data in time";
        }
        auto ret = m_process.read(buf, remaining);
        if (ret < 0) {
            qDebug() << "end of file";
            memset(buf, 0, remaining);
            if (length-remaining)
                return length-remaining;
            else
                return -1;
        }
        if (ret > 0) {
            remaining -= ret;
            buf+= ret;
        }

    } while (remaining > 0);

    return length;
}

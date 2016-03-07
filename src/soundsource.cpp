#include "soundsource.h"


#include "util.h"

SoundSource::SoundSource(const QString &name) : m_name(name)
{
}


SoundSource::~SoundSource()
{
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
                close();
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
    if (m_closed) {
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





ThreadingDecoder::ThreadingDecoder(std::unique_ptr<SoundSource> base, size_t buffers) :
    SoundSource("threaded "+ base->name()), m_base(std::move(base)), buffers(buffers) {

}


ThreadingDecoder::~ThreadingDecoder() {
    mutex.lock();
    abort = true;
    condition.wakeAll();
    mutex.unlock();
    wait();
}



int ThreadingDecoder::readPcm(char * buffer, const size_t length) {
    if (m_closed)
        return -1;
    return m_base->readPcm(buffer, length);
}


void SoundSource::close() {
    bool wasClosed;
    {
        QMutexLocker lock(&m_mutex);
        wasClosed = m_closed;
        m_closed = true;
        m_cv.wakeAll();
    }
    if (!wasClosed)
        emit finished();
}

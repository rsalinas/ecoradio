#include "decoder.h"

Decoder::Decoder(const QString &name) : m_name(name)
{
    qDebug() << __FUNCTION__;
}


Decoder::~Decoder()
{
    qDebug() << __FUNCTION__;
}


int Decoder::readFading(char * buf, const size_t length)  {
    int n = readPcm(buf, length);
    if (n <= 0 || m_fading == 0)
        return n;

    double volume;
    auto b = reinterpret_cast<signed short int *>(buf);
    for (int i = 0; i < length/ 2; i++) {
        size_t here = m_bytes + i*2;
        if (here >= fadingEndBytes) {
            if (m_fading == 1 ){
                qDebug() << "Finished fading";
                m_fading = 0;
                volume = 1;
            } else if (m_fading == -1 ){
                qDebug() << "finishing fading down";
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
    qDebug() << m_bytes << "volume " << volume << fadingEndBytes;
    m_bytes += n;
    return n;
}


SinWave::SinWave(float freq, int rate) : Decoder("sin"+QString::number(freq)), m_freq(freq), m_rate(rate)
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





ThreadingDecoder::ThreadingDecoder(std::unique_ptr<Decoder> base, size_t buffers) :
       Decoder("threaded "+ base->name()), m_base(std::move(base)), buffers(buffers) {

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

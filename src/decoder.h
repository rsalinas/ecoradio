#pragma once

#include <QString>
#include <QProcess>
#include <QDebug>
#include <cassert>
#include  <algorithm>


//
class Decoder
{
public:
    Decoder();
    virtual int readPcm(char * buf, const size_t length) = 0;
    virtual int skip(int millis) {
        char buffer[65536];
        size_t bytesToSkip = 44100 * 2 * 2 * millis / 1000;
        while (bytesToSkip) {
            int ret = readPcm(buffer, std::min(sizeof(buffer), bytesToSkip));
            if (ret <= 0)
                return ret;
        }
        return 0;
    }
    int setFadeIn(int millis) {

    }

    int stopFadeOut(int millis) {

    }

};

class ProcessDecoder : public Decoder {
public:
    ProcessDecoder(const QString& binary, QStringList args) {
        m_process.setProcessChannelMode(QProcess::ForwardedErrorChannel);
        m_process.setReadChannel(QProcess::StandardOutput);

        m_process.start(binary, args);
        if (!m_process.waitForStarted(1000)) {
            qDebug() << "error starting mpg123";

        }
        assert(m_process.waitForReadyRead(3000));
    }
    virtual int readPcm(char * buf, const size_t length) {
        qDebug() << length;
        ssize_t remaining = length;
        do  {
            if (! m_process.waitForReadyRead(3000)) {
                qDebug() << "mp3: no data in time";
            }
            auto ret = m_process.read(buf, remaining);
            qDebug() << __FUNCTION__ << ret << remaining;
            if (ret < 0) {
                qDebug() << "end of file";
                memset(buf, 0, remaining);
                return length-remaining;
            }
            if (ret > 0) {
                remaining -= ret;
                buf+= ret;
            }

        } while (remaining > 0);

        return length;
    }
private:
    QProcess m_process;
};

class Mp3Decoder : public ProcessDecoder  {
public:
    Mp3Decoder(const QString &filename) : ProcessDecoder("mpg123", QStringList() << "-s" << "-r" << "44100" << "--stereo" << filename ) {

    }
};


class Arecord  : public ProcessDecoder {
public:
    Arecord() : ProcessDecoder("arecord", QStringList() << "-f" << "cd" << "-t" << "raw") {
    }

};

class SinWave : public Decoder
{
public:
    SinWave(float freq = 440 , int rate=44100) : m_freq(freq), m_rate(rate)
    {

    }

    virtual int readPcm(char * buffer, const size_t length) {
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
private:
    int m_rate;
    int m_base = 0;
    float m_freq = 440.0;
};


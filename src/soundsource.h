#pragma once

#include <cassert>
#include  <algorithm>


#include <QString>
#include <QProcess>
#include <QDebug>
#include <QMutex>
#include <QWaitCondition>
#include <QMutexLocker>
#include <QThread>
#include <memory>


class SoundSource : public QThread
{
    Q_OBJECT
public:
    SoundSource(const QString &name);
    virtual ~SoundSource();
    int readFading(char * buf, const size_t length);
    virtual int readPcm(char * buf, const size_t length) = 0;
    virtual int goTo(int millis) {
        return -1;
    }
    virtual int lengthMillis() {
        return -1;
    }

    virtual int currentMillis() {
        return -1;
    }
    virtual void run() {
        qDebug() << __FUNCTION__ << "source";
//        while (!m_abort) {

//        }
    }

    virtual int skip(int millis);
    int setFadeIn(int millis);
    int stopFadeOut(int millis);

    QString name() {
        return m_name;
    }

    void close();

    unsigned char m_mastervolume = 255;
    size_t m_bytes = 0;
    size_t fadingEndBytes;
    int m_fading = 0;

    void waitEnd() {
        QMutexLocker lock(&m_mutex);
        while (!m_closed) {
            qDebug() << "not closed";
            m_cv.wait(&m_mutex);
        }
    }

protected:
    bool m_closed = false;

private:
    QString m_name;
    QMutex m_mutex;
    QWaitCondition m_cv;

    //    std::vector<char*> buffers;
    bool m_abort = false;


signals:
    void finished();
};


class ProcessDecoder : public SoundSource {
public:
    ProcessDecoder(const QString &name, const QString& binary, QStringList args) : SoundSource(name) {
        m_process.setProcessChannelMode(QProcess::ForwardedErrorChannel);
        m_process.setReadChannel(QProcess::StandardOutput);

        m_process.start(binary, args);
        if (!m_process.waitForStarted(1000)) {
            qDebug() << "error starting mpg123";

        }
        assert(m_process.waitForReadyRead(3000));
    }
    virtual ~ProcessDecoder() {
        qDebug() << "killing process" << name();
        m_process.terminate();
        qDebug() << name() << "waitForFinished";
        m_process.waitForFinished(1000);
        qDebug() << name() << "waitForFinished done";
    }

    virtual int readPcm(char * buf, const size_t length) {
        ssize_t remaining = length;
        if (m_closed)
            return -1;
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
private:
    QProcess m_process;
};

class Mp3Decoder : public ProcessDecoder  {
public:
    Mp3Decoder(const QString &filename) : ProcessDecoder(filename, "mpg123", QStringList() << "-s" << "-r" << "44100" << "--stereo" << filename ) {

    }
};


class Arecord  : public ProcessDecoder {
public:
    Arecord() : ProcessDecoder("arecord", "arecord", QStringList() << "-f" << "cd" << "-t" << "raw") {
    }

};

class SinWave : public SoundSource
{
public:
    SinWave(float freq = 440 , int rate=44100);
    virtual int readPcm(char * buffer, const size_t length);

private:
    int m_rate;
    int m_base = 0;
    float m_freq = 440.0;
};


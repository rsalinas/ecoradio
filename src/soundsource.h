#pragma once

#include <cassert>
#include  <algorithm>


#include <QString>
#include <QProcess>
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
    virtual void run();


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

    void waitEnd();

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
    ProcessDecoder(const QString &name, const QString& binary, QStringList args);
    virtual ~ProcessDecoder();
    virtual int readPcm(char * buf, const size_t length);

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


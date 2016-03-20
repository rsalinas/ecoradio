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

class SndSink;


class SoundSource : public QThread
{
    Q_OBJECT
public:
    enum FadingDirection {
        NoFading, FadingUp, FadingDown
    };

    enum FadeAction {
        WillStop, WillPause, WillSilence
    };
    enum Status {
        Initial, Playing, Paused, Silence, Finished
    };

    SoundSource(const QString &name);
    virtual ~SoundSource() = 0;
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
    int fadeIn(int millis);
    int fadeOut(int millis, FadeAction);
    int stopFadeOut(int millis) {
        return fadeOut(millis, FadeAction::WillStop);
    }

    QString name() {
        return m_name;
    }

    void play();
    void pause();
    void close();
    void setSilence() {
        m_status = Status::Silence;
    }

    unsigned char m_mastervolume = 255;
    size_t m_bytes = 0;
    size_t fadingEndBytes;


    FadingDirection m_fading = NoFading;

//    int m_fading = 0;
    FadeAction m_fadeAction = FadeAction::WillStop;

    void waitEnd();

    void addSink(std::shared_ptr<SndSink> sink);
    void removeSink(std::shared_ptr<SndSink> sink);
    std::list<std::shared_ptr<SndSink>> getSinks();

    class Exception : public std::exception
    {

    };

protected:
    Status m_status = Status::Playing;

private:
    QString m_name;
    QMutex m_mutex;
    QWaitCondition m_cv;

    std::list<std::shared_ptr<SndSink>> m_sinks;

    //    std::vector<char*> buffers;
    bool m_abort = false;


signals:
    void finished();
};

Q_DECLARE_METATYPE(std::shared_ptr<SoundSource>)



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


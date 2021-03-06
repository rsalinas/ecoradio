#include "ecoradio.h"

#include <QDebug>
#include <memory>

#include "snd/sinks/ogg_encoder.h"
#include "snd/sinks/aosink.h"
#include "snd/sinks/sink.h"
#include "snd/sources/mpg123wrap.h"
#include "core/liverecord.h"
#include "core/database.h"

Ecoradio::Ecoradio(QObject *parent)
    : QObject(parent)
    , m_settings("ecoradio.ini", QSettings::Format::NativeFormat)
    , m_db("radio.sqlite")
    , m_sched(m_db)
    , m_wss(*this, quint16(m_settings.value("wss.port", 1234).toInt()), this)
    , m_current(m_sched.getCurrent())
    , m_currentPlayer(getProgramPlayer(*m_current))
    , m_nextPrograms(m_sched.getNext())

{
    m_posTimer.start(1000);
    try {
        m_ao = std::make_shared<AoSink>(m_format);
        m_mixer.addSink(m_ao);
    } catch (std::exception &e) {
        qWarning() << "Cannot open Alsa output";
    }

    try {
        m_ogg = std::make_shared<OggEncoder>(std::unique_ptr<OggFwd>(new OggFwd(OggFwd::Config(m_settings))));
        m_mixer.addSink(m_ogg);
    } catch (std::exception &e) {
        qWarning() << "Cannot open ogg output";
    }

    m_linein = std::make_shared<Mpg123>(QStringLiteral("samples/long.mp3"));
    m_linein->setSilence();
    m_mixer.addSource(m_linein);


    m_mixer.start();
    QObject::connect(&m_sched, SIGNAL(programChanged(std::shared_ptr<ProgramTime>)), this, SLOT(newProgram(std::shared_ptr<ProgramTime>)));
    QObject::connect(&m_mixer, SIGNAL(sourceFinishing(std::shared_ptr<SoundSource>)), this, SLOT(mixerSongFinishing(std::shared_ptr<SoundSource>)));
    QObject::connect(&m_mixer, SIGNAL(sourceFinished(std::shared_ptr<SoundSource>)), this, SLOT(mixerSongFinished(std::shared_ptr<SoundSource>)));
    QObject::connect(&m_mixer, SIGNAL(vumeter(int,int)), &m_wss, SLOT(vumeter(int,int)));
    QObject::connect(&m_posTimer, SIGNAL(timeout()), this, SLOT(everySecond()));
    QObject::connect(&m_wss, SIGNAL(cmd_ptt(bool)), this, SLOT(cmd_ptt(bool)));
    QObject::connect(&m_wss, SIGNAL(startProgram(uint64_t,QString,int)), this, SLOT(startProgram(uint64_t,QString,int)));
    QObject::connect(&m_wss, SIGNAL(endProgram()), this, SLOT(endProgram()));
    qDebug() << __FUNCTION__ << "Running ";
    if (m_current) {
        qDebug() << "current: "<< *m_current;
        m_currentStream  = m_currentPlayer->getNextSong();
        if (m_currentStream) {
            emit m_wss.currentSong(m_currentStream->name());
        } else {
            emit m_wss.currentSong("");
        }
        m_nextStream = m_currentPlayer->getNextSong();
        if (m_nextStream) {
            emit m_wss.nextSong(m_nextStream->name());
        } else {
            emit m_wss.currentSong("");
        }

        if (m_currentStream) {
            m_mixer.addSource(m_currentStream);
        } else {
            qWarning() << "could not get song from current stream" << *m_current;
        }
    }
    auto progLogger = qDebug().noquote() << "Programs:";
    for (auto p : m_db.getPrograms())
        progLogger.quote().nospace() << "  " << p->name;
    //    m_mixer.addSource(std::make_shared<StreamSrc>("http://stream.freefm.de:8100/"));
}

void Ecoradio::run()
{
    // Do processing here
    qDebug() << "ecoradio.run()";
    //    emit finished();
}


void Ecoradio::newProgram(std::shared_ptr<ProgramTime> p)
{
    m_current = p;
    qDebug() << "Ecoradio::programChanged: " << *p;
    m_nextPrograms = m_sched.getNext();
    qDebug() << "next programs: " << m_nextPrograms.size();
    m_wss.programChange(m_current, m_nextPrograms);
}


void Ecoradio::mixerSongFinishing(std::shared_ptr<SoundSource> s) {
    qDebug() << __FUNCTION__ << s->name();
}



void Ecoradio::mixerSongFinished(std::shared_ptr<SoundSource> finishedSource) {
    assert(finishedSource.get());
    finishedSource->close();
    if (finishedSource == m_currentStream)
        playNextSong();
}

void Ecoradio::playNextSong() {
    qDebug() << __FUNCTION__;
    if (m_currentStream)
        m_currentStream->close();

    m_currentStream = m_nextStream;
    if (m_currentStream) {
        emit m_wss.currentSong(m_currentStream->name());
    } else {
        emit m_wss.currentSong("?");
    }
    //    m_currentStream->stopFadeOut(10);
    m_mixer.addSource(m_currentStream);

    if (m_current) {
        qDebug() << "current: "<< *m_current;
        m_nextStream = m_currentPlayer->getNextSong();
        if (!m_nextStream) {
            qWarning() << "could not get song from current stream" << *m_current;
            emit m_wss.nextSong("??");
        } else {

            emit m_wss.nextSong(m_nextStream->name());
        }
    }
}



void Ecoradio::cmd_ptt(bool on) {
    qDebug() << "PTT " << on;
    if (on) {
        m_currentStream->fadeOut(1000, SoundSource::FadeAction::WillSilence);
        m_linein->fadeIn(1000);
        qDebug() << m_linein->currentMillis();
    } else {
        m_linein->fadeOut(1000, SoundSource::FadeAction::WillSilence);
        m_currentStream->fadeIn(1000);
        qDebug() << m_linein->currentMillis();
    }
}


void Ecoradio::clientConnected(QWebSocket *client) {
    qDebug() << __FUNCTION__;
    qDebug() << "current:" << *m_current ;
    qDebug() << "next: " << m_nextPrograms.size();
    emit m_wss.programChange(m_current, m_nextPrograms);
    if (m_currentStream) {
        emit m_wss.currentSong(m_currentStream->name());
        everySecond();
    } else {
        emit m_wss.currentSong("?");
    }

    if (m_nextStream) {
        emit m_wss.nextSong(m_nextStream->name());
    } else {
        emit m_wss.nextSong("?");
    }

}

void Ecoradio::clientDisconnected(QWebSocket *client) {
    qDebug() << __FUNCTION__;
}


void Ecoradio::skipSong() {
    qDebug() << __FUNCTION__;
    m_currentStream->fadeOut(1000, SoundSource::FadeAction::WillStop);
    playNextSong();
}


void Ecoradio::everySecond() {
    //    qDebug() << __FUNCTION__;
    if (m_currentStream) {
        emit m_wss.currentPos(m_currentStream->currentMillis()/1000.0,
                              m_currentStream->lengthMillis()/1000.0);
    }
}

void Ecoradio::startProgram(uint64_t programId, QString title, int delay)
{
    qDebug() << "ecoradio:" << __FUNCTION__ << programId << title << delay;
    auto p = m_db.getProgramTimeById(programId);
    qDebug() << "program is: " << *p;
    auto fn = p->name;
    QDir programDir((m_settings.value("live.dir", ".").toString()));
    QString file(programDir.absoluteFilePath(fn));

    LiveProgram lp;
    lp.p_id = p->id;
    lp.name = p->name;

    m_currentLiveProgram = std::make_shared<LiveProgramRecording>(lp, file);

    m_linein->addSink(m_currentLiveProgram->getWriter());
    m_linein->fadeIn(1000);
    m_currentStream->fadeOut(1000, SoundSource::FadeAction::WillPause);
    qDebug() << "Cambiado a línea";
}


void Ecoradio::endProgram()
{
    qDebug() << __FUNCTION__;
    if (!m_currentLiveProgram) {
        qWarning() << "Error: was not recording";
        return;
    }
    m_currentStream->fadeIn(1000);
    m_linein->removeSink(m_currentLiveProgram->getWriter());
    qDebug() << m_currentLiveProgram->compress();
    m_linein->fadeOut(1000, SoundSource::FadeAction::WillSilence);
    m_currentLiveProgram.reset();

    qDebug() << "back to replay";
}

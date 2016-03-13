#include "ecoradio.h"

#include <QDebug>
#include <memory>

#include "snd/sinks/ogg_encoder.h"
#include "snd/sinks/aosink.h"
#include "snd/sinks/sink.h"
#include "snd/sources/mpg123wrap.h"
#include "util/streamsrc.h"


std::shared_ptr<ProgramPlayer> getProgramPlayer(const Program &p) {
    if (dynamic_cast<const FolderProgram*>(&p)) {
        return std::make_shared<FolderProgramPlayer>(*dynamic_cast<const FolderProgram*>(&p));
    } else if (dynamic_cast<const PodcastProgram*>(&p)) {
        return std::make_shared<PodcastProgramPlayer>(*dynamic_cast<const PodcastProgram*>(&p));
    } else if (dynamic_cast<const LiveProgram*>(&p)) {
        return std::make_shared<LiveProgramPlayer>(*dynamic_cast<const LiveProgram*>(&p));
    } else if (dynamic_cast<const StreamProgram*>(&p)) {
        return std::make_shared<StreamProgramPlayer>(*dynamic_cast<const StreamProgram*>(&p));
    } else
        return nullptr;
}

Ecoradio::Ecoradio(QObject *parent) :
    QObject(parent),
    m_settings("ecoradio.ini", QSettings::Format::NativeFormat),
    m_ao(std::make_shared<AoSink>(SndFormat())),
    m_ogg(std::make_shared<OggEncoder>(std::unique_ptr<OggFwd>(new OggFwd(OggFwd::Config(m_settings))))),
    m_sched("radio.sqlite"),
    m_current(m_sched.getCurrent()),
    m_currentPlayer(getProgramPlayer(*m_current)),
    m_nextPrograms(m_sched.getNext()),
    m_wss(*this, quint16(m_settings.value("wss.port", 1234).toInt()), this)
{
    m_linein = std::make_shared<Mpg123>(QStringLiteral("/home/rsalinas/Sync/mp3/01 - Llover sobre mojado.mp3"));
    m_mixer.addSink(m_ao);
    m_mixer.addSink(m_ogg);
    m_mixer.start();
    QObject::connect(&m_sched, SIGNAL(programChanged(std::shared_ptr<Program>)), this, SLOT(newProgram(std::shared_ptr<Program>)));
    QObject::connect(&m_mixer, SIGNAL(songFinishing(std::shared_ptr<SoundSource>)), this, SLOT(songFinishing(std::shared_ptr<SoundSource>)));
    QObject::connect(&m_mixer, SIGNAL(songFinished(std::shared_ptr<SoundSource>)), this, SLOT(songFinished(std::shared_ptr<SoundSource>)));
    QObject::connect(&m_mixer, SIGNAL(vumeter(int,int)), &m_wss, SLOT(vumeter(int,int)));
    QObject::connect(&m_wss, SIGNAL(cmd_ptt(bool)), this, SLOT(cmd_ptt(bool)));
    qDebug() << __FUNCTION__ << "Running ";
    if (m_current) {
        qDebug() << "current: "<< *m_current;
        m_currentStream  = m_currentPlayer->getNextSong();
        m_nextStream = m_currentPlayer->getNextSong();

        if (m_currentStream) {
            m_mixer.addSource(m_currentStream);
        } else {
            qWarning() << "could not get song from current stream" << *m_current;
        }
    }
    qDebug() << "progs" << m_sched.getPrograms();
    //    m_mixer.addSource(std::make_shared<StreamSrc>("http://stream.freefm.de:8100/"));
}

void Ecoradio::run()
{
    // Do processing here
    qDebug() << "ecoradio.run()";
    //    emit finished();
}


void Ecoradio::newProgram(std::shared_ptr<Program> p)
{
    qDebug() << "Ecoradio::programChanged: " << *p;
    QStringList nextProgramNames = m_sched.getPrograms();
    m_wss.programChange(p->name, nextProgramNames);
}


void Ecoradio::songFinishing(std::shared_ptr<SoundSource> s) {
    qDebug() << __FUNCTION__;
}

void Ecoradio::songFinished(std::shared_ptr<SoundSource> finishedSource) {
    qDebug() << __FUNCTION__;
    m_currentStream = m_nextStream;
    //    m_currentStream->stopFadeOut(10);
    m_mixer.addSource(m_currentStream);
    if (m_current) {
        qDebug() << "current: "<< *m_current;
        m_nextStream = m_currentPlayer->getNextSong();
        if (!m_nextStream) {
            qWarning() << "could not get song from current stream" << *m_current;
        }
    }
}



void Ecoradio::cmd_ptt(bool on) {
    qDebug() << "PTT " << on;
    if (on) {
        m_currentStream->fadeOut(2000, SoundSource::FadeAction::Silence);
        m_linein->setFadeIn(2000);
    } else {
        m_linein->fadeOut(2000, SoundSource::FadeAction::Silence);
        m_currentStream->setFadeIn(2000);
    }
}


void Ecoradio::clientConnected() {
    qDebug() << __FUNCTION__;
    emit m_wss.programChange(m_current->name, m_sched.getPrograms());
}

void Ecoradio::clientDisconnected() {
    qDebug() << __FUNCTION__;
}


void Ecoradio::skipSong() {
    songFinished(std::shared_ptr<SoundSource> ());
}

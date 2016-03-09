#include "ecoradio.h"
#include <QDebug>
#include "ogg_encoder.h"
#include "aosink.h"
#include "ogg_encoder.h"
#include "sink.h"
#include "mpg123wrap.h"
#include "streamsrc.h"
#include <memory>

Ecoradio::Ecoradio(QObject *parent) :
    QObject(parent),
    m_settings("ecoradio.ini", QSettings::Format::NativeFormat),
    m_ao(std::make_shared<AoSink>(SndFormat())),
    m_ogg(std::make_shared<OggEncoder>(std::unique_ptr<OggFwd>(new OggFwd(OggFwd::Config(m_settings))))),
    m_sched("radio.sqlite"),
    m_current(m_sched.getCurrent()),
    m_nextPrograms(m_sched.getNext()),
    wss(1234, this)
{
    m_mixer.addSink(m_ao);
    m_mixer.addSink(m_ogg);
    m_mixer.start();
    QObject::connect(&m_sched, SIGNAL(programChanged(std::shared_ptr<Program>)), this, SLOT(newProgram(std::shared_ptr<Program>)));
    QObject::connect(&m_mixer, SIGNAL(songFinishing(std::shared_ptr<SoundSource>)), this, SLOT(songFinishing(std::shared_ptr<SoundSource>)));
    QObject::connect(&m_mixer, SIGNAL(songFinished(std::shared_ptr<SoundSource>)), this, SLOT(songFinished(std::shared_ptr<SoundSource>)));
    qDebug() << __FUNCTION__ << "Running ";
    if (m_current) {
        qDebug() << "current: "<< *m_current;
        m_currentStream  = m_current->getNextSong();
//        m_currentStream->stopFadeOut(500);
        m_nextStream = m_current->getNextSong();

        if (m_currentStream) {
            m_mixer.addSource(m_currentStream);
        } else {
            qWarning() << "could not get song from current stream" << *m_current;
        }
    }


    //    m_mixer.addSource(std::make_shared<Mpg123>("beep.mp3"));
    //    m_mixer.addSource(std::make_shared<StreamSrc>("http://stream.freefm.de:8100/"));
    //    m_current->
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
        m_nextStream = m_current->getNextSong();
        if (!m_nextStream) {
            qWarning() << "could not get song from current stream" << *m_current;
        }
    }
}

#include "ecoradio.h"
#include <QDebug>
#include "ogg_encoder.h"
#include "aosink.h"
#include "ogg_encoder.h"
#include "sink.h"
#include "mpg123wrap.h"
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
    QObject::connect(&m_mixer, SIGNAL(songFinished(std::shared_ptr<SoundSource>)), this, SLOT(songFinished(std::shared_ptr<SoundSource>)));
    qDebug() << __FUNCTION__ << "Running ";
    if (m_current) {
        qDebug() << "current: "<< *m_current;

    }

    m_mixer.addSource(std::make_shared<Mpg123>("beep.mp3"));
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
    auto s= std::make_shared<Mpg123>("beep.mp3");
    s->setFadeIn(5000);
    m_mixer.addSource(s);
}

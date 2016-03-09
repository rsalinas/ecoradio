#pragma once

#include <QObject>

#include "scheduler.h"
#include "mixer.h"
#include "websockserver.h"
#include "oggfwd.h"
#include <QSettings>
class SoundSrc;

class SndSink;

class Program;


class Ecoradio : public QObject
{
    Q_OBJECT
public:
    Ecoradio(QObject *parent = 0);

private slots:
    void run();

    void newProgram(std::shared_ptr<Program>);
    void songFinishing(std::shared_ptr<SoundSource> s);
    void songFinished(std::shared_ptr<SoundSource> s);

public slots:

signals:
    void finished();
private:
    QSettings m_settings;
    Mixer m_mixer;
    std::shared_ptr<SndSink> m_ao;
    std::shared_ptr<SndSink> m_ogg;
    Scheduler m_sched;
    WebsockServer wss;
    std::shared_ptr<Program> m_current, m_currentFallback;
    std::vector<std::shared_ptr<Program>> m_nextPrograms;
    std::shared_ptr<SoundSource> m_currentStream, m_nextStream;

};


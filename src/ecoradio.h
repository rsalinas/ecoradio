#pragma once

#include <QObject>

#include "scheduler.h"
#include "mixer.h"
#include "websockserver.h"
#include "oggfwd.h"
#include <QSettings>
#include <memory>
#include "sources/soundsource.h"

class SndSink;
class Program;


class Ecoradio : public QObject
{
    Q_OBJECT
public:
    Ecoradio(QObject *parent = 0);
    Scheduler &getScheduler() {
        return m_sched;
    }

private slots:
    void run();

    void newProgram(std::shared_ptr<Program>);
    void songFinishing(std::shared_ptr<SoundSource> s);
    void songFinished(std::shared_ptr<SoundSource> s);
    void cmd_ptt(bool);

public slots:

signals:
    void finished();

private:
    QSettings m_settings;
    Mixer m_mixer;
    std::shared_ptr<SndSink> m_ao;
    std::shared_ptr<SndSink> m_ogg;
    Scheduler m_sched;
    WebsockServer m_wss;
    std::shared_ptr<Program> m_current, m_currentFallback;
    std::vector<std::shared_ptr<Program>> m_nextPrograms;
    std::shared_ptr<SoundSource> m_currentStream, m_nextStream;
    std::shared_ptr<SoundSource> m_linein;
};


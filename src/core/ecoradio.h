#pragma once

#include <memory>
#include <QObject>
#include <QSettings>

#include "scheduler.h"
#include "snd/mixer.h"
#include "snd/oggfwd.h"
#include "snd/sources/soundsource.h"
#include "websockserver.h"
#include "programplayer.h"

class SndSink;
class Program;


class Ecoradio : public QObject
{
    Q_OBJECT
public:
    Ecoradio(QObject *parent = 0);
    Scheduler &getScheduler()
    {
        return m_sched;
    }

    void skipSong();

private slots:
    void run();
    void newProgram(std::shared_ptr<Program>);
    void songFinishing(std::shared_ptr<SoundSource> s);
    void songFinished(std::shared_ptr<SoundSource> s);
    void cmd_ptt(bool);
    void everySecond();

public slots:
    void clientConnected();
    void clientDisconnected();

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
    std::shared_ptr<ProgramPlayer> m_currentPlayer;
    std::vector<std::shared_ptr<Program>> m_nextPrograms;
    std::shared_ptr<SoundSource> m_currentStream, m_nextStream;
    std::shared_ptr<SoundSource> m_linein;
    QTimer m_posTimer;
};


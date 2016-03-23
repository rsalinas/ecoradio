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
#include "core/database.h"

class LiveProgramRecording;
class SndSink;
class ProgramTime;
class RadioDb;

class Ecoradio : public QObject
{
    Q_OBJECT
public:
    enum CoreState {
        Silence, InProgram, ProgramWaitingToLive, Live
    };
    CoreState  state = Silence;

    Ecoradio(QObject *parent = 0);
    Scheduler &getScheduler()
    {
        return m_sched;
    }

    RadioDb &getDatabase() {
        return m_db;
    }

    void skipSong();


private slots:
    void run();
    void newProgram(std::shared_ptr<ProgramTime>);
    void mixerSongFinishing(std::shared_ptr<SoundSource> s);
    void mixerSongFinished(std::shared_ptr<SoundSource> s);
    void cmd_ptt(bool);
    void everySecond();
    void endProgram();
    void startProgram(uint64_t programId, QString title, int delay);

public slots:
    void clientConnected(QWebSocket *client);
    void clientDisconnected(QWebSocket *client);


signals:
    void finished();
    void songFinishing(std::shared_ptr<SoundSource> s);
    void songFinished(std::shared_ptr<SoundSource> s);

private:
    void playNextSong();
    QSettings m_settings;
    SndFormat m_format; //FIXME (m_settings)
    Mixer m_mixer;
    std::shared_ptr<SndSink> m_ao;
    std::shared_ptr<SndSink> m_ogg;
    RadioDb m_db;
    Scheduler m_sched;
    WebsockServer m_wss;
    QTimer m_posTimer;

    std::shared_ptr<ProgramTime> m_current, m_currentFallback;
    std::shared_ptr<ProgramPlayer> m_currentPlayer;
    std::vector<std::shared_ptr<ProgramTime>> m_nextPrograms;
    std::shared_ptr<SoundSource> m_currentStream, m_nextStream;
    std::shared_ptr<SoundSource> m_linein;
    std::shared_ptr<LiveProgramRecording> m_currentLiveProgram;
};

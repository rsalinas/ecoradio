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
    bool startProgram(const LiveProgram &p, const QString &title,
                      const QDateTime &when);


private slots:
    void run();
    void newProgram(std::shared_ptr<ProgramTime>);
    void mixerSongFinishing(std::shared_ptr<SoundSource> s);
    void mixerSongFinished(std::shared_ptr<SoundSource> s);
    void cmd_ptt(bool);
    void everySecond();

public slots:
    void clientConnected(QWebSocket *client);
    void clientDisconnected(QWebSocket *client);
    void startProgram(uint64_t programId, QString title, int delay);

signals:
    void finished();
    void songFinishing(std::shared_ptr<SoundSource> s);
    void songFinished(std::shared_ptr<SoundSource> s);

private:
    QSettings m_settings;
    SndFormat m_format; //FIXME (m_settings)
    Mixer m_mixer;
    std::shared_ptr<SndSink> m_ao;
    std::shared_ptr<SndSink> m_ogg;
    Scheduler m_sched;
    RadioDb m_db;
    WebsockServer m_wss;
    QTimer m_posTimer;

    std::shared_ptr<ProgramTime> m_current, m_currentFallback;
    std::shared_ptr<ProgramPlayer> m_currentPlayer;
    std::vector<std::shared_ptr<ProgramTime>> m_nextPrograms;
    std::shared_ptr<SoundSource> m_currentStream, m_nextStream;
    std::shared_ptr<SoundSource> m_linein;
    std::shared_ptr<LiveProgramRecording> m_currentLiveProgram;
};

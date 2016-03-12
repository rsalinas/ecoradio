#pragma once

#include "snd/sources/soundsource.h"
#include "file_scan.h"
#include <QDateTime>
typedef uint64_t rowid_t;
class SoundSource;

class Program {
public:
    rowid_t id;
    int dow;
    QDateTime ts;
    QString name;
    Program(rowid_t id, int dow, const QDateTime &ts, const QString &name) :
        id(id), dow(dow), ts(ts), name(name) {
    }

    virtual std::shared_ptr<SoundSource> getNextSong() = 0;
};

class LiveProgram : public Program {
public:
    int lengthSeconds;
    bool live;
    LiveProgram(rowid_t id, int dow, const QDateTime &ts, const QString &name);
    std::shared_ptr<SoundSource> getNextSong() override;


};


class StreamProgram : public Program {
public:
    StreamProgram(rowid_t id, int dow, const QDateTime &ts, const QString &name) :
        Program(id, dow, ts, name) {

    }
    int play() {

    }
    std::shared_ptr<SoundSource> getNextSong() override;
};


class PodcastProgram : public Program {
public:
    PodcastProgram(rowid_t id, int dow, const QDateTime &ts, const QString &name) :
        Program(id, dow, ts, name) {

    }
    std::shared_ptr<SoundSource> getNextSong() override;
};


class FolderProgram : public Program {
public:
    FolderProgram(rowid_t id, int dow, const QDateTime &ts, const QString &name);
    std::shared_ptr<SoundSource> getNextSong() override;

private:
    Traverse traverse;
};

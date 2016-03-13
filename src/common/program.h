#pragma once

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
    Program(const Program&) = default;
    virtual ~Program() {}
};


class LiveProgram : public Program {
public:
    int lengthSeconds;
    bool live;
    LiveProgram(rowid_t id, int dow, const QDateTime &ts, const QString &name);
};


class StreamProgram : public Program {
public:
    StreamProgram(rowid_t id, int dow, const QDateTime &ts, const QString &name) :
        Program(id, dow, ts, name) {

    }
    int play() {

    }
};

class PodcastProgram : public Program {
public:
    PodcastProgram(rowid_t id, int dow, const QDateTime &ts, const QString &name) :
        Program(id, dow, ts, name) {

    }
};



class FolderProgram : public Program {
public:
    FolderProgram(rowid_t id, int dow, const QDateTime &ts, const QString &name);


};

#pragma once

typedef uint64_t rowid_t;



class Program {
public:
    rowid_t id;
    int dow;
    QDateTime ts;
    QString name;
    Program(rowid_t id, int dow, const QDateTime &ts, const QString &name) :
        id(id), dow(dow), ts(ts), name(name) {

    }
};

class LiveProgram : public Program {
public:
    int lengthSeconds;
    bool live;
    LiveProgram(rowid_t id, int dow, const QDateTime &ts, const QString &name) :
        Program(id, dow, ts, name) {

    }
};


class StreamProgram : public Program {
public:
    int lengthSeconds;
    bool live;
    StreamProgram(rowid_t id, int dow, const QDateTime &ts, const QString &name) :
        Program(id, dow, ts, name) {

    }
    int play() {

    }
};


class PodcastProgram : public Program {
public:
    int lengthSeconds;
    bool live;
    PodcastProgram(rowid_t id, int dow, const QDateTime &ts, const QString &name) :
        Program(id, dow, ts, name) {

    }
};

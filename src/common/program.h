#pragma once

#include <QObject>
#include <QDateTime>
#include <QJsonObject>

#include <memory>

typedef uint64_t rowid_t;

class ProgramTime
{
public:
    rowid_t id;
    int dow;
    QDateTime ts;
    QString name;
    ProgramTime();
    ProgramTime(rowid_t id, int dow, const QDateTime &ts, const QString &name);
    ProgramTime(const ProgramTime&) = default;

    virtual ~ProgramTime() = default;
    bool operator==(const ProgramTime &other) const;
};

Q_DECLARE_METATYPE(ProgramTime)
Q_DECLARE_METATYPE(std::shared_ptr<ProgramTime>)
Q_DECLARE_METATYPE(QList<std::shared_ptr<ProgramTime>>)

class LiveProgram : public ProgramTime
{
public:
    int lengthSeconds;
    bool live;
    LiveProgram(rowid_t id, int dow, const QDateTime &ts, const QString &name);
};


class StreamProgram : public ProgramTime
{
public:
    StreamProgram(rowid_t id, int dow, const QDateTime &ts, const QString &name) :
        ProgramTime(id, dow, ts, name) {

    }
};

class PodcastProgram : public ProgramTime
{
public:
    PodcastProgram(rowid_t id, int dow, const QDateTime &ts, const QString &name) :
        ProgramTime(id, dow, ts, name) {

    }
};



class FolderProgram : public ProgramTime
{
public:
    FolderProgram(rowid_t id, int dow, const QDateTime &ts, const QString &name);
};


//FIXME: organize this:
std::shared_ptr<ProgramTime> readProgram(const QJsonObject &json);
bool setProgram(QJsonObject &o, QString key, const ProgramTime& program);
QJsonObject toJson(ProgramTime p) ;
std::shared_ptr<ProgramTime> programFromJson(const QJsonObject obj);
QJsonObject toJson(std::shared_ptr<ProgramTime> current, std::vector<std::shared_ptr<ProgramTime>> next) ;
QDebug operator<<(QDebug dbg, const ProgramTime &program);
QList<std::shared_ptr<ProgramTime>> programListFromJson(const QJsonArray &array);

#pragma once

#include <QObject>
#include <QDateTime>
#include <QJsonObject>

#include <memory>

typedef uint64_t rowid_t;

class Program
{
public:
    rowid_t id;
    int dow;
    QDateTime ts;
    QString name;
    Program();
    Program(rowid_t id, int dow, const QDateTime &ts, const QString &name);
    Program(const Program&) = default;

    virtual ~Program() = default;
    bool operator==(const Program &other) const;
};

Q_DECLARE_METATYPE(Program)
Q_DECLARE_METATYPE(std::shared_ptr<Program>)
Q_DECLARE_METATYPE(QList<std::shared_ptr<Program>>)



class LiveProgram : public Program
{
public:
    int lengthSeconds;
    bool live;
    LiveProgram(rowid_t id, int dow, const QDateTime &ts, const QString &name);
};


class StreamProgram : public Program
{
public:
    StreamProgram(rowid_t id, int dow, const QDateTime &ts, const QString &name) :
        Program(id, dow, ts, name) {

    }
};

class PodcastProgram : public Program
{
public:
    PodcastProgram(rowid_t id, int dow, const QDateTime &ts, const QString &name) :
        Program(id, dow, ts, name) {

    }
};



class FolderProgram : public Program
{
public:
    FolderProgram(rowid_t id, int dow, const QDateTime &ts, const QString &name);
};


//FIXME: organize this:
std::shared_ptr<Program> readProgram(const QJsonObject &json);
bool setProgram(QJsonObject &o, QString key, const Program& program);
QJsonObject toJson(Program p) ;
std::shared_ptr<Program> programFromJson(const QJsonObject obj);
QJsonObject toJson(std::shared_ptr<Program> current, std::vector<std::shared_ptr<Program>> next) ;
QDebug operator<<(QDebug dbg, const Program &program);
QList<std::shared_ptr<Program>> programListFromJson(const QJsonArray &array);

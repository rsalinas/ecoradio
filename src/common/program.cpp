#include "program.h"

#include <memory>
#include <QVariant>
#include <QJsonArray>
#include <QDebug>


ProgramTime::ProgramTime(rowid_t id, int dow, const QDateTime &ts, const QString &name) :
    pt_id(id), dow(dow), ts(ts), name(name) {
}

ProgramTime::ProgramTime() {
    qDebug() << "STRANGE:" << __FUNCTION__;
}


LiveProgram::LiveProgram(rowid_t id, int dow, const QDateTime &ts, const QString &name) :
    ProgramTime(id, dow, ts, name)
{    
}


FolderProgram::FolderProgram(rowid_t id, int dow, const QDateTime &ts, const QString &name) :
    ProgramTime(id, dow, ts, name)

{
}






bool ProgramTime::operator==(const ProgramTime &other) const {
    return pt_id == other.pt_id
            && dow == other.dow
            && ts == other.ts
            && name == other.name;
}


QJsonObject toJson(ProgramTime p) {
    QJsonObject o;
    o.insert("id", QJsonValue(qint64(p.pt_id)));
    o.insert("name", QJsonValue(p.name));
    o.insert("ts", QJsonValue(p.ts.toMSecsSinceEpoch()));
    o.insert("dow", QJsonValue(p.dow));
    return o;
}

std::shared_ptr<ProgramTime> programFromJson(const QJsonObject obj) {
    auto id = obj["id"].toInt();
    auto dow = obj["dow"].toInt();
    auto ts = obj["ts"].toVariant().toLongLong();
    auto name = obj["name"].toString();
    auto ret = std::make_shared<ProgramTime>(id, dow, QDateTime::fromMSecsSinceEpoch(ts), name);
    return ret;
}


QList<std::shared_ptr<ProgramTime>> programListFromJson(const QJsonArray &array) {
    QList<std::shared_ptr<ProgramTime>>  ret;
    for (int i=0; i < array.size(); i++) {
        ret.push_back(programFromJson(array.at(i).toObject()));
    }

    return ret;
}

QJsonObject toJson(std::shared_ptr<ProgramTime> current, std::vector<std::shared_ptr<ProgramTime>> next) {
    qDebug() << __FUNCTION__ << next.size();
    QJsonObject o;
    o.insert("current", toJson(*current));
    QJsonArray a;
    for (auto p : next) {
        a.append(toJson(*p));
    }
    o.insert("next", a);
    return o;
}


QDebug operator<<(QDebug dbg, const ProgramTime &program)
{
    dbg.noquote() << QDate::longDayName(program.dow) << program.ts.toString() << program.name;
    auto now = QDateTime::currentDateTime();
    qDebug() << program.ts;
    //    if (program.ts < now) {

    dbg <<  now.msecsTo(program.ts) /1000/60 << "minutes";
    //    } else
    //        dbg << -program.ts.msecsTo(now) /1000/60  << "minuteS";
    return dbg;
}

std::shared_ptr<ProgramTime> instantiateProgramTime(QString type,
        rowid_t rowid, int lineDow, QDateTime dt, QString name)
{

    if (type == LiveProgram::programType()) {
        return std::make_shared<LiveProgram>(rowid, lineDow, dt, name);
    } else if (type == "stream") {
        return std::make_shared<StreamProgram>(rowid, lineDow, dt, name);
    } else if (type == "podcast") {
        return  std::make_shared<PodcastProgram>(rowid, lineDow, dt, name);
    } else if (type == "folder") {
        return std::make_shared<FolderProgram>(rowid, lineDow, dt, name);
    } else {
        qWarning() << "Bad program found, ignoring. " << type;
        return nullptr;
    }
}

Program::Program(rowid_t id, const QString &name)
    : id(id)
    , name(name) {

}

#include "program.h"

#include <memory>
#include <QVariant>
#include <QJsonArray>
#include <QDebug>


ProgramTime::ProgramTime(rowid_t id, int dow, const QDateTime &ts, const QString &name) :
    id(id), dow(dow), ts(ts), name(name) {
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


std::shared_ptr<ProgramTime> readProgram(const QJsonObject &json) {
    auto type = json.value("type").toString();

    if (type == "folder") {
        int id =  json.value("id").toInt();
        int dow = json.value("dow").toInt();
        QDateTime ts = QDateTime::fromMSecsSinceEpoch(json.value("ts").toString().toLongLong());
        auto name = json.value("name").toString();
    }
}


bool setProgram(QJsonObject &o, QString key, const ProgramTime& program) {


}

bool ProgramTime::operator==(const ProgramTime &other) const {
    return id == other.id
            && dow == other.dow
            && ts == other.ts
            && name == other.name;
}


QJsonObject toJson(ProgramTime p) {
    QJsonObject o;
    o.insert("id", QJsonValue(qint64(p.id)));
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


#include "program.h"

#include <memory>
#include <QVariant>
#include <QJsonArray>
#include <QDebug>


Program::Program(rowid_t id, int dow, const QDateTime &ts, const QString &name) :
    id(id), dow(dow), ts(ts), name(name) {
}

Program::Program() {
    qDebug() << "STRANGE:" << __FUNCTION__;
}


LiveProgram::LiveProgram(rowid_t id, int dow, const QDateTime &ts, const QString &name) :
    Program(id, dow, ts, name)
{    
}


FolderProgram::FolderProgram(rowid_t id, int dow, const QDateTime &ts, const QString &name) :
    Program(id, dow, ts, name)

{
}


std::shared_ptr<Program> readProgram(const QJsonObject &json) {
    auto type = json.value("type").toString();

    if (type == "folder") {
        int id =  json.value("id").toInt();
        int dow = json.value("dow").toInt();
        QDateTime ts = QDateTime::fromMSecsSinceEpoch(json.value("ts").toString().toLongLong());
        auto name = json.value("name").toString();
    }
}


bool setProgram(QJsonObject &o, QString key, const Program& program) {


}

bool Program::operator==(const Program &other) const {
    return id == other.id
            && dow == other.dow
            && ts == other.ts
            && name == other.name;
}


QJsonObject toJson(Program p) {
    QJsonObject o;
    o.insert("id", QJsonValue(qint64(p.id)));
    o.insert("name", QJsonValue(p.name));
    o.insert("ts", QJsonValue(p.ts.toMSecsSinceEpoch()));
    o.insert("dow", QJsonValue(p.dow));
    return o;
}

std::shared_ptr<Program> programFromJson(const QJsonObject obj) {
    auto id = obj["id"].toInt();
    auto dow = obj["dow"].toInt();
    auto ts = obj["ts"].toVariant().toLongLong();
    auto name = obj["name"].toString();
    auto ret = std::make_shared<Program>(id, dow, QDateTime::fromMSecsSinceEpoch(ts), name);
    return ret;
}


QList<std::shared_ptr<Program>> programListFromJson(const QJsonArray &array) {
    QList<std::shared_ptr<Program>>  ret;
    for (int i=0; i < array.size(); i++) {
           ret.push_back(programFromJson(array.at(i).toObject()));
    }

    return ret;
}

QJsonObject toJson(std::shared_ptr<Program> current, std::vector<std::shared_ptr<Program>> next) {
    QJsonObject o;
    o.insert("current", toJson(*current));
    QJsonArray a;
    for (auto p : next) {
        a.append(toJson(*p));
    }
    o.insert("next", a);
    return o;
}


QDebug operator<<(QDebug dbg, const Program &program)
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


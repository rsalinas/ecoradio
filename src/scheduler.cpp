#include "scheduler.h"
#include <QtSql/QtSql>
#include <QDebug>
#include <QDateTime>
#include <memory>
#include "util.h"

Scheduler::Scheduler(const QString &filename) : m_db(QSqlDatabase::addDatabase("QSQLITE"))
{
    m_db.setDatabaseName(filename);
    if (!m_db.open()) {
        qFatal("Cannot open db");
    }
}

std::shared_ptr<Program> Scheduler::getCurrent(const QDateTime &ts) {
    auto l = getPlan(true, ts);
    if (l.size()) {
        return l.front();
    }
    else
        return nullptr;
}

static QString makeQuery() {
}


std::vector<std::shared_ptr<Program>>
Scheduler::getPlan(bool current,
                   const QDateTime &ts) {
    QSqlQuery query(m_db);
    QDateTime now = QDateTime::currentDateTime();
    auto dow = now.date().dayOfWeek();
    auto minOfTheDay = dow*60*24+now.time().hour()*60+now.time().minute();
    auto sql = QString("select program.rowid as program_rowid, * from program_time"
                       " inner join program on program.rowid = program_id"
                       " where dow*60*24+hour*60+minute %1 %2"
                       " order by dow*60*24+hour*60+minute %3").
            arg(current ? "<" : ">",
                QString::number(minOfTheDay),
                current ? "desc limit 1" : "asc"  );
    qDebug() << sql;
    if (!query.exec(sql)) {
        qWarning() << QStringLiteral("error querying database: ")+  m_db.lastError().text();
        throw SqlException();
    }
    std::vector<std::shared_ptr<Program>> l;
    while (query.next()) {
        auto lineDow = query.value("dow").toInt();
        auto nowDow = QDate().currentDate().dayOfWeek();
        QTime time(query.value("hour").toInt(), query.value("minute").toInt());
        QDateTime dt(QDate::currentDate().addDays(
                         current ?
                             -((nowDow-lineDow+7)%7) :
                             ((lineDow-nowDow+7)%7)), time);
        auto type = query.value("ptype").toString();
        auto rowid = query.value("program_rowid").toULongLong();
        std::shared_ptr<Program> p;
        type = "folder"; //FIXME
        if (type == "live") {
            p = std::make_shared<LiveProgram>(rowid, lineDow, dt, query.value("name").toString());
        } else if (type == "stream") {
            p = std::make_shared<StreamProgram>(rowid, lineDow, dt, query.value("name").toString());
        } else if (type == "podcast") {
            p = std::make_shared<PodcastProgram>(rowid, lineDow, dt, query.value("name").toString());
        } else if (type == "folder") {
            p = std::make_shared<FolderProgram>(rowid, lineDow, dt, query.value("name").toString());
        } else {
            qWarning() << "Bad program found, ignoring. " << type;
        }
        l.push_back(p);
//        qDebug() << *p;
    }

    if (!current && l.size()) {
        auto nextProgram = l.front();
        auto ms = now.msecsTo(nextProgram->ts);
        qDebug() << "next is" << *nextProgram << " in " << (ms/1000);
        QTimer::singleShot(ms, this, SLOT(programTimerExpired()));
        //        qDebug() << "time started";
        //        QTimer::singleShot(2000, this, SLOT(programTimerExpired()));
    }
    return l;

}

std::vector<std::shared_ptr<Program>> Scheduler::getNext(const QDateTime &ts) {
    return getPlan(false, ts);
}


QDebug operator<<(QDebug dbg, const Program &program) {
    dbg.noquote() << QDate::longDayName(program.dow) << program.ts.toString() << program.name;
    auto now = QDateTime::currentDateTime();
    qDebug() << program.ts;
    //    if (program.ts < now) {

    dbg <<  now.msecsTo(program.ts) /1000/60 << "minutes";
    //    } else
    //        dbg << -program.ts.msecsTo(now) /1000/60  << "minuteS";
    return dbg;
}


void Scheduler::programTimerExpired() {
    qDebug() << "Program changed: ";
    emit programChanged(getCurrent());
    getNext();
}

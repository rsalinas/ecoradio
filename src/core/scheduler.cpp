#include "scheduler.h"

#include <QtSql/QtSql>
#include <QDebug>
#include <QDateTime>
#include <memory>
#include "database.h"
#include "util/util.h"

Scheduler::Scheduler(RadioDb &db) : m_db(db)
{
}

std::shared_ptr<ProgramTime> Scheduler::getCurrent(const QDateTime &ts) {
    auto l = getPlan(true, ts);
    if (l.size()) {
        return l.front();
    }
    else
        return nullptr;
}

static int minOfTheDay(const QDateTime &now) {
    return now.date().dayOfWeek()*60*24
            +now.time().hour()*60
            +now.time().minute();
}

std::vector<std::shared_ptr<ProgramTime>>
Scheduler::getPlan(bool current,
                   const QDateTime &now) {
    QSqlQuery query(m_db.getDb());
    QString sql;
    if (current) {
        QString what = "select program.rowid as program_rowid, dow*60*24+hour*60+minute as ts2, * from program_time"
                       " inner join program on program.rowid = program_id"
                       " union all"
                       " select program.rowid as program_rowid, (dow-7)*60*24+hour*60+minute as ts2, * from program_time"
                       " inner join program on program.rowid = program_id";
        sql = what + " WHERE ts2 <" + QString::number(minOfTheDay(now)) + " order by ts2 desc limit 1";
    } else {
        QString what = "select program.rowid as program_rowid, dow*60*24+hour*60+minute as ts2, * from program_time"
                       " inner join program on program.rowid = program_id"
                       " union all"
                       " select program.rowid as program_rowid, (dow+7)*60*24+hour*60+minute as ts2, * from program_time"
                       " inner join program on program.rowid = program_id";
        sql = what + " WHERE ts2 >" + QString::number(minOfTheDay(now)) + " order by ts2 asc";
    }
    qDebug() << sql;
    if (!query.exec(sql)) {
        qWarning() << QStringLiteral("error querying database: ")+  m_db.getDb().lastError().text();
        throw SqlException();
    }
    std::vector<std::shared_ptr<ProgramTime>> l;
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
        std::shared_ptr<ProgramTime> p;
        type = "folder"; //FIXME

        l.push_back(instantiateProgramTime(
                        type, rowid, lineDow, dt, query.value("name").toString()));
        if (false)
            qDebug() << *p;
    }

    if (!current && l.size()) {
        auto nextProgram = l.front();
        auto ms = now.msecsTo(nextProgram->ts);
        qDebug() << "next is" << *nextProgram << " in " << (ms/1000);
        QTimer::singleShot(ms, this, SLOT(programTimerExpired()));
    }

    return l;
}

std::vector<std::shared_ptr<ProgramTime>> Scheduler::getNext(const QDateTime &ts)
{
    auto ret = getPlan(false, ts);
    qDebug() << __FUNCTION__ << ret.size();
    return ret;
}

void Scheduler::programTimerExpired()
{
    qDebug() << "Program changed: ";
    emit programChanged(getCurrent());
    getNext();
}

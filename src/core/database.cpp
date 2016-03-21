#include "database.h"

#include <QtSql/QSqlQuery>
#include <QDebug>


/*
 * PRAGMA foreign_keys=ON;
drop table if exists program ;
create table program(ptype string, name string, path string, length int);
drop table if exists program_time;
create table program_time(program_id int not null, dow int, hour int, minute int, live int);
drop table if exists program_instance;
create table program_instance(program_id int not null, ts datetime, int program_number);

*/
RadioDb::RadioDb(const QString &filename) : m_db(QSqlDatabase::addDatabase("QSQLITE"))
{
    m_db.setDatabaseName(filename);
    if (!m_db.open()) {
        qFatal("Cannot open db");
    }
}


RadioDb::~RadioDb() {

}

quint64 RadioDb::createProgram(const Program &p)
{
    QSqlQuery query(m_db);
    if (!query.prepare("INSERT INTO program (ptype, name) VALUES (:ptype, :name)")) {
        qFatal("cannot prepare sql");
    }
    query.bindValue(":ptype", p.type);
    query.bindValue(":name", p.name);
    if (!query.exec()) {
        qDebug() << "Error in " << __FUNCTION__;
        return 0;
    }
    if (!query.exec("last_insert_rowid") ||
            !query.next())
        return false;

    auto id = query.value(0).toLongLong();
    qDebug() << "inserted program with id" << id;
    return id;
}


bool RadioDb::deleteProgram(uint64_t id)
{
    QSqlQuery query(m_db);
    if (!query.prepare("DELETE FROM program WHERE rowid = :id")) {
        qFatal("cannot prepare sql");
    }
    query.bindValue(":id", QVariant::fromValue(id));
    if (!query.exec()) {
        qDebug() << "Error in " << __FUNCTION__;
        return false;
    }
    return true;
}

quint64 RadioDb::createProgramTime(uint64_t programId,
                                   int dow,
                                   QTime time,
                                   bool live)
{
    //create table program_time(program_id int not null, dow int, hour int, minute int, live int);
    QSqlQuery query(m_db);
    if (!query.prepare("INSERT INTO program_time (program_id, dow, hour, minute, live) "
                       "VALUES (:program_id, :dow, :hour, :minute, :live)")) {
        qFatal("cannot prepare sql");
    }
    query.bindValue(":program_id", QVariant::fromValue(programId));
    query.bindValue(":dow", dow);
    query.bindValue(":hour", time.hour());
    query.bindValue(":minute", time.minute());
    query.bindValue(":live", live);
    if (!query.exec()) {
        qDebug() << "Error in " << __FUNCTION__;
        return 0;
    }
    if (!query.exec("last_insert_rowid") ||
            !query.next())
        return false;

    auto id = query.value(0).toLongLong();
    qDebug() << "inserted program with id" << id;
    return id;

}

bool RadioDb::deleteProgramTime(uint64_t id)
{
    QSqlQuery query(m_db);
    if (!query.prepare("DELETE FROM program_time WHERE rowid = :id")) {
        qFatal("cannot prepare sql");
    }
    query.bindValue(":id", QVariant::fromValue(id));
    if (!query.exec()) {
        qDebug() << "Error in " << __FUNCTION__;
        return false;
    }
    return true;
}


QList<std::shared_ptr<Program>> RadioDb::getPrograms()
{
    QList<std::shared_ptr<Program>>  ret;
    QSqlQuery query(m_db);
    //FIXME only active programs
    if (query.exec("SELECT rowid id, name FROM program ORDER BY name")) {
        while (query.next())  {
            ret.push_back(std::make_shared<Program>(query.value("id").toLongLong(),
                                                    query.value("name").toString()));
        }
    }
    qDebug() << __FUNCTION__ << ret.size();
    return ret;
}


std::shared_ptr<Program> RadioDb::getProgramTimeById(uint64_t programId)
{
    QSqlQuery query(m_db);
    //FIXME only active programs
    if (!query.prepare("SELECT rowid id, name FROM program WHERE id = :id ORDER BY name")) {
        qWarning() << "error in " << __FUNCTION__;
        return nullptr;
    }
    query.bindValue(":id", QVariant::fromValue(programId));
    if (!query.exec()) {
        qDebug() << "Error in " << __FUNCTION__;
        return nullptr;
    }
    if (!query.next())  {
        qDebug() << "Not found id" << __FUNCTION__;
        return nullptr;
    }
    uint64_t id = query.value("id").toLongLong();
    QString name = query.value("name").toString();
    return std::make_shared<Program>(id, name);
}

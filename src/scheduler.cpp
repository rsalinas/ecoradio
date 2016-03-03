#include "scheduler.h"
#include <QtSql/QtSql>
#include <QDebug>

Scheduler::Scheduler() : m_db(QSqlDatabase::addDatabase("QSQLITE"))
{
    m_db.setDatabaseName("db.sqlite");
    if (!m_db.open()) {
        qFatal("Cannot open db");
    }
    QSqlQuery query(m_db);
    if (query.exec("select 5+5")) {
        while (query.next())
        qDebug() << query.value(0).toString();
    } else {
        qDebug() << "error" <<  query.lastError();
    }

}


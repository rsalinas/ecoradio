#pragma once

#include <vector>
#include <QtSql/QSqlDatabase>
#include <QDateTime>
#include <QObject>
#include <QTimer>
#include <memory>
#include <exception>

#include "common/program.h"

class RadioDb;

class SqlException : public std::exception {
};

class Scheduler : public QObject
{
    Q_OBJECT
public:
    Scheduler(RadioDb &db);

    std::shared_ptr<ProgramTime> getCurrent(const QDateTime &ts = QDateTime::currentDateTime());
    std::vector<std::shared_ptr<ProgramTime>> getNext(const QDateTime &ts = QDateTime::currentDateTime());

private:
    std::vector<std::shared_ptr<ProgramTime>>  getPlan(bool current, const QDateTime &ts);
    RadioDb &m_db;
    std::vector<ProgramTime> puntual;
    QTimer m_timer;

signals:
    void programChanged(std::shared_ptr<ProgramTime>);

public slots:
    void programTimerExpired();
};




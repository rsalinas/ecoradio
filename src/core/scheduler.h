#pragma once

#include <vector>
#include <QtSql/QSqlDatabase>
#include <QDateTime>
#include <QObject>
#include <QTimer>
#include <memory>
#include <exception>

#include "program.h"

class SqlException : public std::exception {
};

class Scheduler : public QObject
{
    Q_OBJECT
public:
    Scheduler(const QString &filename);

    std::shared_ptr<Program> getCurrent(const QDateTime &ts = QDateTime::currentDateTime());
    std::vector<std::shared_ptr<Program>> getNext(const QDateTime &ts = QDateTime::currentDateTime());
    QStringList getPrograms();

private:
    std::vector<std::shared_ptr<Program>>  getPlan(bool current, const QDateTime &ts);
    QSqlDatabase m_db;
    std::vector<Program> puntual;
    QTimer m_timer;

signals:
    void programChanged(std::shared_ptr<Program>);

public slots:
    void programTimerExpired();
};

QDebug operator<<(QDebug dbg, const Program &program);


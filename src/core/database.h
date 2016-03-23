#pragma once

#include <QtSql/QSqlDatabase>
#include <QTime>
#include "common/program.h"

class RadioDb {
public:
    RadioDb(const QString &filename);
    ~RadioDb();
    quint64 createProgram(const Program &p);
    bool deleteProgram(uint64_t id);
    quint64 createProgramTime(uint64_t programId,
                              int dow,
                              QTime time,
                              bool live);
    bool deleteProgramTime(uint64_t id);
    QList<std::shared_ptr<Program>> getPrograms();
    std::shared_ptr<Program> getProgramTimeById(uint64_t programId);
    QSqlDatabase  &getDb() {
        return m_db;
    }

protected:
    QSqlDatabase m_db;
};

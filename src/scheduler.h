#pragma once

#include <vector>
#include <QtSql/QSqlDatabase>

class Scheduler
{
public:
    Scheduler();

private:
    struct Program {

    };

    QSqlDatabase m_db;
    std::vector<Program> periodic;
    std::vector<Program> puntual;
};

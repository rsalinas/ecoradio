#include "program.h"

#include <memory>

LiveProgram::LiveProgram(rowid_t id, int dow, const QDateTime &ts, const QString &name) :
    Program(id, dow, ts, name)
{    
}


FolderProgram::FolderProgram(rowid_t id, int dow, const QDateTime &ts, const QString &name) :
    Program(id, dow, ts, name)

{
}

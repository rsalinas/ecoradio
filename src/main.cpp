#include <QCoreApplication>
#include <QtSql/QtSql>
#include <QObject>
#include "scheduler.h"
#include "mixer.h"

class Ecoradio : public QObject
{
public:
    Ecoradio();
    ~Ecoradio();
    Mixer m_mixer;
    Scheduler m_sched;
};


int main(int argc, char *argv[])
{
    qDebug() << "ecoradio";
    QCoreApplication a(argc, argv);

    return a.exec();
}


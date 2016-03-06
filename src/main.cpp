#include <QCoreApplication>
#include <QtSql/QtSql>
#include <QObject>
#include "scheduler.h"
#include "pcm_player.h"

class Ecoradio : public QObject
{
public:
    Ecoradio();
    ~Ecoradio();
    PcmPlayer m_mixer;
    Scheduler m_sched;
};


int main(int argc, char *argv[])
{
    qDebug() << "ecoradio";
    QCoreApplication a(argc, argv);

    return a.exec();
}


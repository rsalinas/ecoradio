#include "ecoradio.h"
#include <QDebug>


Ecoradio::Ecoradio(QObject *parent) :
    QObject(parent), wss(1234, this) {
}

void Ecoradio::run()
{
    // Do processing here
    qDebug() << "ecoradio";
//    emit finished();
}

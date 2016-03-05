#include "mixer.h"
#include <QDebug>

Mixer::~Mixer()
{

}

Mixer::Mixer(QObject *parent) : QObject(parent)
{

}


void Mixer::run() {
    qDebug() << __FUNCTION__;
    while (true) {
        char buf[4096];
        memset(buf, 0, sizeof(buf));
        char aux[4096];
        for (auto & in : m_active) {
            memset(aux, 0, sizeof(aux));
            int n = in.readBytes(aux, sizeof(aux));
        }
        //mix aux into buf
//        pcmPlayer.write(buf, sizeof(buf));
        oggEncoder.write(buf, sizeof(buf));
    }
}

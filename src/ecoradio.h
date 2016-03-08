#pragma once

#include <QObject>

#include "scheduler.h"
#include "mixer.h"
#include "websockserver.h"


class Ecoradio : public QObject
{
    Q_OBJECT
public:
    Ecoradio(QObject *parent = 0);

public slots:
    void run();

signals:
    void finished();
private:
    Mixer m_mixer;
    Scheduler m_sched;
    WebsockServer wss;
};


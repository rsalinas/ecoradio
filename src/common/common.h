#pragma once

#include <QObject>

class WebSocketApi : public QObject
{
    Q_OBJECT

signals:

public slots:

};


static const int c_ecoRadioWsPort = 1234;

class StubApi : public QObject {
    Q_OBJECT
public:
    StubApi(QObject *parent) : QObject(parent) {

    }

    virtual void vumeter(int channel, int value) = 0;
    virtual void currentPos(float pos) = 0;
    virtual void currentSong(QString currentSong) = 0;
    virtual void nextSong(QString currentSong) = 0;
};

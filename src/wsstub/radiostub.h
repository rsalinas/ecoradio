#pragma once

#include <QObject>
#include <QUrl>
#include <QtWebSockets/QtWebSockets>

#include "common/common.h"

class RadioStub : public QObject
{
    Q_OBJECT
public:
    explicit RadioStub(const QUrl &url,
                       QObject *parent = 0);
    bool startProgram(int id);
    bool endProgram();
    bool pause();
    bool resume();
    bool getPrograms();
    bool ptt(bool);
    bool skipSong();

signals:
    void connectionClosed();
    void vuMeterUpdate(int channel, int value);
    void programListReady(QStringList programs);
    void newProgram(QString current, QStringList nextPrograms);
    void currentSong(QString currentSong);
    void nextSong(QString currentSong);
    void currentPos(float pos, float length);

public slots:
    void onConnected();
    void disconnected();
    void onTextMessageReceived(QString message);

private:
    QWebSocket m_websocket;
};

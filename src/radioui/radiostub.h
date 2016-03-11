#pragma once

#include <QObject>
#include <QUrl>
#include <QtWebSockets/QtWebSockets>

#include "../interfaces/common.h"

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

public slots:
    void onConnected();
    void disconnected();
    void onTextMessageReceived(QString message);

private:
    QWebSocket m_websocket;
};

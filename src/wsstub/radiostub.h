#pragma once

#include <QObject>
#include <QUrl>
#include <QtWebSockets/QtWebSockets>

#include "common/common.h"
#include "common/program.h"

class RadioStub : public QObject
{
    Q_OBJECT
public:
    explicit RadioStub(const QUrl &url,
                       QObject *parent = 0);
    bool startProgram(ProgramTime program, QString title, int delay);
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
    void newProgram(std::shared_ptr<ProgramTime> current,
                    QList<std::shared_ptr<ProgramTime>> nextPrograms);
    void currentSong(QString currentSong);
    void nextSong(QString currentSong);
    void currentPos(float pos, float length);

public slots:
    void onConnected();
    void disconnected();
    void onTextMessageReceived(QString message);
    void onBinaryMessageReceived(QByteArray message);

private:
    QWebSocket m_websocket;
};

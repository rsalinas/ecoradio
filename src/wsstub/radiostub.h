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
    ~RadioStub();

    bool endProgram();
    bool pause();
    bool resume();
    bool getPrograms();
    bool ptt(bool) __attribute_warn_unused_result__;
    bool skipSong();

signals:
    void connected();
    void disconnected();
    void connectionClosed();
    void vuMeterUpdate(int channel, int value);
    void programListReady(QStringList programs);
    void newProgram(std::shared_ptr<ProgramTime> current,
                    QList<std::shared_ptr<ProgramTime>> nextPrograms);
    void currentSong(QString currentSong);
    void nextSong(QString currentSong);
    void currentPos(float pos, float length);

public slots:
    bool startProgram(ProgramTime program, QString title, int delay);
    void onConnected();
    void onDisconnected();
    void onTextMessageReceived(QString message);
    void onBinaryMessageReceived(QByteArray message);

private:
    QWebSocket m_websocket;
};

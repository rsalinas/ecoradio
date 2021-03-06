#pragma once

#include <QObject>
#include <memory>
#include "common/common.h"

class ProgramTime;

class Ecoradio;

QT_FORWARD_DECLARE_CLASS(QWebSocketServer)
QT_FORWARD_DECLARE_CLASS(QWebSocket)

class WebsockServer : public QObject
{
       Q_OBJECT;
public:
    WebsockServer(Ecoradio &m_ecoradio, quint16 port, QObject * parent = nullptr);
    ~WebsockServer();
    class WebsockServerException : public std::exception {
    public:
    };


public slots:
    void programChange(std::shared_ptr<ProgramTime> current,
                       std::vector<std::shared_ptr<ProgramTime>> next);

signals:
    void closed();
    void cmd_ptt(bool on);
    void endProgram();
    void startProgram(uint64_t p, QString title, int delay);

private slots:
    void onNewConnection();
    void processTextMessage(QString message);
    void processBinaryMessage(QByteArray message);
    void socketDisconnected();

public slots:
    void vumeter(int channel, int value);
    void currentPos(float pos, float length);
    void currentSong(QString currentSong);
    void nextSong(QString currentSong);

private:
    void broadCastTextMessage(const QString &s);
    void broadCastTextMessage(const QByteArray &ba);
    Ecoradio &m_ecoradio;
    std::unique_ptr<QWebSocketServer> m_pWebSocketServer;
    QList<QWebSocket *> m_clients;
};

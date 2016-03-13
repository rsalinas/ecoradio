#pragma once

#include <QObject>
#include <memory>
#include "common/common.h"

class Program;

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
    void programChange(std::shared_ptr<Program> current,
                       std::vector<std::shared_ptr<Program>> next);

signals:
    void closed();
    void cmd_ptt(bool on);

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
    void broadCastTextMessage(const QString &msg);
    Ecoradio &m_ecoradio;
    std::unique_ptr<QWebSocketServer> m_pWebSocketServer;
    QList<QWebSocket *> m_clients;
};

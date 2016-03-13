#pragma once

#include <QObject>
#include <memory>

class Ecoradio;

QT_FORWARD_DECLARE_CLASS(QWebSocketServer)
QT_FORWARD_DECLARE_CLASS(QWebSocket)

class WebsockServer : public QObject
{
       Q_OBJECT;
public:
    WebsockServer(Ecoradio &m_ecoradio, quint16 port, QObject * parent = nullptr);
    ~WebsockServer();

public slots:
    void programChange(QString program, QStringList nextPrograms);

signals:
    void closed();
    void cmd_ptt(bool on);

private slots:
    void onNewConnection();
    void processTextMessage(QString message);
    void processBinaryMessage(QByteArray message);
    void socketDisconnected();
    void vumeter(int channel, int value);

public slots:
    void currentSong(QString currentSong);
    void nextSong(QString currentSong);

private:
    Ecoradio &m_ecoradio;
    std::unique_ptr<QWebSocketServer> m_pWebSocketServer;
    QList<QWebSocket *> m_clients;
};

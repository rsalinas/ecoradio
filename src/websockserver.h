#ifndef WEBSOCKSERVER_H
#define WEBSOCKSERVER_H

#include <QObject>
#include <memory>

QT_FORWARD_DECLARE_CLASS(QWebSocketServer)
QT_FORWARD_DECLARE_CLASS(QWebSocket)

class WebsockServer : public QObject
{
       Q_OBJECT;
public:
    WebsockServer(quint16 port, QObject * parent = nullptr);
    ~WebsockServer();

Q_SIGNALS:
    void closed();

private Q_SLOTS:
    void onNewConnection();
    void processTextMessage(QString message);
    void processBinaryMessage(QByteArray message);
    void socketDisconnected();


private:
    std::unique_ptr<QWebSocketServer> m_pWebSocketServer;
    QList<QWebSocket *> m_clients;
};

#endif // WEBSOCKSERVER_H

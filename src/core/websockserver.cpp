#include "websockserver.h"

#include "QtWebSockets/qwebsocketserver.h"
#include "QtWebSockets/qwebsocket.h"
#include <QtCore/QDebug>

#include "ecoradio.h"

QT_USE_NAMESPACE

WebsockServer::WebsockServer(Ecoradio &ecoradio, quint16 port, QObject *parent)
    : m_ecoradio(ecoradio)
    , QObject(parent)
    , m_pWebSocketServer(new QWebSocketServer(QStringLiteral("Ecoradio Server"),
                                              QWebSocketServer::NonSecureMode, this))
    , m_clients()
{
    if (m_pWebSocketServer->listen(QHostAddress::Any, port)) {
        qDebug() << "WebsockServer listening on port" << port;
        connect(m_pWebSocketServer.get(), &QWebSocketServer::newConnection,
                this, &WebsockServer::onNewConnection);
        connect(m_pWebSocketServer.get(), &QWebSocketServer::closed, this, &WebsockServer::closed);
    }
}

WebsockServer::~WebsockServer()
{
    m_pWebSocketServer->close();
    qDeleteAll(m_clients.begin(), m_clients.end());
}

void WebsockServer::onNewConnection()
{
    QWebSocket *pSocket = m_pWebSocketServer->nextPendingConnection();

    connect(pSocket, &QWebSocket::textMessageReceived, this, &WebsockServer::processTextMessage);
    connect(pSocket, &QWebSocket::binaryMessageReceived, this, &WebsockServer::processBinaryMessage);
    connect(pSocket, &QWebSocket::disconnected, this, &WebsockServer::socketDisconnected);
    pSocket->sendTextMessage("VU 0 50");
    m_clients << pSocket;
    m_ecoradio.clientConnected();
}

void WebsockServer::processTextMessage(QString message)
{
    QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
    qDebug() << "Message received:" << message;
    QStringList split = message.split(" ");
    qDebug() << split;
    if (split[0] == "PTT") {
        int value = split[1].toInt();
        emit cmd_ptt(value != 0);
    } else if (split[0] == "GET_PROGRAMS") {
        auto list = m_ecoradio.getScheduler().getPrograms();
        QString msg("PRGLIST");
        for (const auto &s : list) {
            msg.append("\n").append(s);
        }
        pClient->sendTextMessage(msg);
    } else if (split[0] == "skipSong") {
        m_ecoradio.skipSong();
    }
}

void WebsockServer::processBinaryMessage(QByteArray message)
{
    QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
    qDebug() << "Binary Message received:" << message;
    if (pClient) {
        pClient->sendBinaryMessage(message);
    }
}

void WebsockServer::socketDisconnected()
{
    QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
    qDebug() << "socketDisconnected:" << pClient;
    if (pClient) {
        m_clients.removeAll(pClient);
        pClient->deleteLater();
    }
    emit m_ecoradio.clientDisconnected();
}


void WebsockServer::vumeter(int channel, int value) {
    for (auto c : m_clients) {
        c->sendTextMessage(QString("VU ") + QString::number(channel)+ " "+ QString::number(value));
    }
}


void WebsockServer::programChange(QString program, QStringList nextPrograms) {   
    for (auto c : m_clients) {
        c->sendTextMessage("SET_PROGRAM\n"+program+"\n"+nextPrograms.join('\n'));
    }
}

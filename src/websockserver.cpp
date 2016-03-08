#include "websockserver.h"

#include "QtWebSockets/qwebsocketserver.h"
#include "QtWebSockets/qwebsocket.h"
#include <QtCore/QDebug>

QT_USE_NAMESPACE

/*
 *
 * - Start new program:
 *   - Open window
 *   - Show list of programs
 *   - Start or cancel
 *   - Start

*/

WebsockServer::WebsockServer(quint16 port, QObject *parent) :
    QObject(parent),
    m_pWebSocketServer(new QWebSocketServer(QStringLiteral("Echo Server"),
                                            QWebSocketServer::NonSecureMode, this)),
    m_clients()
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

    m_clients << pSocket;
}

void WebsockServer::processTextMessage(QString message)
{
    QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
    qDebug() << "Message received:" << message;
    qDebug() << message.split(" ");
    if (pClient) {
        pClient->sendTextMessage("OK "+message);
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
}

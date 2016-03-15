#include "websockserver.h"

#include "QtWebSockets/qwebsocketserver.h"
#include "QtWebSockets/qwebsocket.h"
#include <QtCore/QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include "ecoradio.h"

QT_USE_NAMESPACE

WebsockServer::WebsockServer(Ecoradio &ecoradio, quint16 port, QObject *parent)
    : QObject(parent)
    , m_ecoradio(ecoradio)
    , m_pWebSocketServer(new QWebSocketServer(QStringLiteral("Ecoradio Server"),
                                              QWebSocketServer::NonSecureMode, this))
    , m_clients()
{
    if (m_pWebSocketServer->listen(QHostAddress::Any, port)) {
        qDebug() << "WebsockServer listening on port" << port;
        connect(m_pWebSocketServer.get(), &QWebSocketServer::newConnection,
                this, &WebsockServer::onNewConnection);
        connect(m_pWebSocketServer.get(), &QWebSocketServer::closed, this, &WebsockServer::closed);
    } else {
        qWarning() << "Cannot open port" << port;
        throw WebsockServerException();
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
    m_ecoradio.clientConnected(pSocket);
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
    } else {
        qWarning() << "SERVER IGNORED MESSAGE" << message;
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
    emit m_ecoradio.clientDisconnected(pClient);
}


void WebsockServer::vumeter(int channel, int value) {
    broadCastTextMessage(QString("VU\n") + QString::number(channel)+ '\n' + QString::number(value));
}


void WebsockServer::programChange(std::shared_ptr<Program> current,
                                  std::vector<std::shared_ptr<Program>> next)
{
    assert(current.get());
    QByteArray ba;
    QTextStream ts(&ba);
    ts <<__FUNCTION__<< endl;
    QJsonDocument doc;
    QJsonObject root = toJson(current, next);
    doc.setObject(root);
    ts << doc.toJson();
    ts.flush();
    broadCastTextMessage(ba);
    qDebug() << "server sent programChange";
}


void WebsockServer::currentSong(QString currentSong) {
    QByteArray ba;
    QTextStream ts(&ba);
    ts <<__FUNCTION__<<endl;
    ts << currentSong <<endl;
    ts.flush();
    broadCastTextMessage(ba);
}

void WebsockServer::nextSong(QString nextSong) {
    qDebug() << "server sets nextSong" << nextSong;
    QByteArray ba;
    QTextStream ts(&ba);
    ts <<__FUNCTION__<<endl;
    ts << nextSong;
    ts.flush();
    broadCastTextMessage(ba);
}

void WebsockServer::currentPos(float pos, float length) {
    //    qDebug() << __FUNCTION__ << pos;
    QByteArray ba;
    QTextStream ts(&ba);
    ts << __FUNCTION__<<endl;
    ts << pos<<endl;
    ts << length <<endl;
    ts.flush();
    broadCastTextMessage(ba);
}

void WebsockServer::broadCastTextMessage(const QString &msg) {
    for (auto c : m_clients) {
        c->sendTextMessage(msg);
    }
}

void WebsockServer::broadCastTextMessage(const QByteArray &ba) {
    broadCastTextMessage(QString::fromLocal8Bit(ba));
}

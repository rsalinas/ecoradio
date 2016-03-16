#include "radiostub.h"

#include <QDebug>

#include <QStringList>

RadioStub::RadioStub(const QUrl &url, QObject *parent) :
    QObject(parent), m_websocket()
{
    qDebug() << __FUNCTION__ << url;
    qRegisterMetaType<std::shared_ptr<ProgramTime> >();
    qRegisterMetaType<QList<std::shared_ptr<ProgramTime>> >();
    connect(&m_websocket, &QWebSocket::connected, this, &RadioStub::onConnected);
    connect(&m_websocket, &QWebSocket::disconnected, this, &RadioStub::disconnected);
    m_websocket.open(url);
    qDebug() << "connected?";
}

void RadioStub::disconnected() {
    qDebug() << __FUNCTION__;
    qDebug() << m_websocket.closeReason() << m_websocket.closeCode() << m_websocket.errorString();
    emit connectionClosed();
}


void RadioStub::onConnected()
{
    qDebug() << "WebSocket connected";
    connect(&m_websocket, &QWebSocket::textMessageReceived,
            this, &RadioStub::onTextMessageReceived);
    connect(&m_websocket, &QWebSocket::binaryMessageReceived,
            this, &RadioStub::onBinaryMessageReceived);
    m_websocket.sendTextMessage(QStringLiteral("USER user"));
}

void RadioStub::onBinaryMessageReceived(QByteArray message)
{
    QDataStream ds(&message, QIODevice::ReadOnly);
    QString type;
    ds >> type;
    if (type == "program") {

//        FolderProgram fp()
    }

}

void RadioStub::onTextMessageReceived(QString message)
{
    QTextStream ts(&message);
    QString cmd = ts.readLine();

//    qDebug() << "Message received:" << message << cmd;
    if (cmd == "VU") {
        int channel = ts.readLine().toInt();
        int value = ts.readLine().toInt();
//        qDebug() << "vumeter" << channel << value;
        emit vuMeterUpdate(channel, value);
    } else if (cmd == "PRGLIST") {
//        qDebug() << "program list!";
//        QStringList programs;
//        for (size_t i = 1; i < lsplit.size(); ++i) {
//            programs.push_back(lsplit[i]);
//        }
//        emit programListReady(programs);
    } else if (cmd  == "programChange") {
        auto rest = ts.readAll().toLocal8Bit();
        auto doc = QJsonDocument::fromJson(rest);
        auto current = programFromJson(doc.object()["current"].toObject());
        auto next = programListFromJson(doc.object()["next"].toArray());
        emit newProgram(current, next);
    } else if (cmd == "nextSong") {
        emit nextSong(ts.readLine());
    } else if (cmd == "currentSong") {
        emit currentSong(ts.readLine());
    } else if (cmd == "currentPos") {
//        qDebug() << "CURRENT POS" << message;
        auto pos = ts.readLine().toFloat();
        auto total = ts.readLine().toFloat();
        emit currentPos(pos, total);
    } else {
        qWarning () << "BAD MESSAGE RECEIVED: "<< message;
    }

}


bool RadioStub::startProgram(ProgramTime program, QString title, int delay) {
    m_websocket.sendTextMessage(QStringLiteral("START_PROGRAM ")+QString::number(0)); //FIXME
    return true;
}

bool RadioStub::endProgram() {
    m_websocket.sendTextMessage(QStringLiteral("END_PROGRAM"));
    return true;
}

bool RadioStub::pause() {
    m_websocket.sendTextMessage(QStringLiteral("PAUSE"));
    return true;
}

bool RadioStub::resume() {
    m_websocket.sendTextMessage(QStringLiteral("RESUME"));
    return true;
}

bool RadioStub::getPrograms() {
    m_websocket.sendTextMessage(QStringLiteral("GET_PROGRAMS"));
    return true;
}

bool RadioStub::ptt(bool onair) {
    QString msg(QStringLiteral("PTT ")+QString::number(onair));
    return m_websocket.sendTextMessage(msg) == msg.size();
}

bool RadioStub::skipSong() {
    QString msg(__FUNCTION__);
    return m_websocket.sendTextMessage(msg) == msg.size();
}

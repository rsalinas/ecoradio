#include "radiostub.h"

#include <QDebug>

#include <QStringList>

RadioStub::RadioStub(const QUrl &url, QObject *parent) :
    QObject(parent), m_websocket()
{
    qDebug() << __FUNCTION__ << url;
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
    m_websocket.sendTextMessage(QStringLiteral("USER user"));
}

void RadioStub::onTextMessageReceived(QString message)
{
    qDebug() << "Message received:" << message;
    //    m_webSocket.close();
    QStringList lsplit = message.split('\n');
    QStringList split(lsplit[0].split(' '));
    if (split[0] == "VU") {
        qDebug() << "vumeter";
        int channel = split[1].toInt();
        int value = split[2].toInt();
        emit vuMeterUpdate(channel, value);
    } else if (split[0] == "PRGLIST") {
        qDebug() << "program list!";
        QStringList programs;
        for (size_t i = 1; i < lsplit.size(); ++i) {
            programs.push_back(lsplit[i]);
        }
        emit programListReady(programs);
    } else if (split[0]  == "SET_PROGRAM") {
        QStringList programs;
        if (lsplit.size()<2 )
            return;
        auto current = lsplit[1];
        for (size_t i = 2; i < lsplit.size(); ++i) {
            programs.push_back(lsplit[i]);
        }
        emit newProgram(current, programs);
    }

}


bool RadioStub::startProgram(int id) {
    m_websocket.sendTextMessage(QStringLiteral("START_PROGRAM ")+QString::number(id));
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
    m_websocket.sendTextMessage(QStringLiteral("PTT ")+QString::number(onair));
    return true;
}


bool RadioStub::skipSong() {
    m_websocket.sendTextMessage(__FUNCTION__);
    return true;
}



#include "streamsrc.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QTimer>

StreamSrc::~StreamSrc() {
    qDebug() << __FUNCTION__;
}

void ping() {
    qDebug() << __FUNCTION__;
}

StreamSrc::StreamSrc(const QString &url, std::shared_ptr<Fifo> fifo) :
    m_nam(this), m_fifo(fifo)
{
    qDebug() << __FUNCTION__ << url;
    QNetworkRequest req(url);
    m_reply = m_nam.get(req);
    QObject::connect(m_reply, SIGNAL(readyRead()), this,  SLOT(streamReadyRead()));
    QObject::connect(m_reply, SIGNAL(finished()), this,  SLOT(streamFinished()));
    QTimer::singleShot(0, 0, &ping);
    qDebug() << __FUNCTION__ << "returning";
}

void StreamSrc::streamReadyRead() {
    auto bytes = m_reply->bytesAvailable();
    char buffer[bytes];
    qint64 n = m_reply->peek(buffer, bytes);
    if (n>0) {
        int n = m_fifo->write(buffer, n);
        qDebug() << "m_fifo->write(buffer, n) -> " << n;
        if (n > 0) {
            qDebug() << __FUNCTION__ << "Confirm write to fifo" << n;
            m_reply->read(buffer, n);
        }
        qDebug() << __FUNCTION__ << n << "after write, available in FIFO:" <<m_fifo->bytesAvailable();
    }
}

void StreamSrc::streamFinished() {
    qDebug() << __FUNCTION__ ;
    for (const auto &i : m_reply->rawHeaderList()) {
        qDebug() << i << m_reply->rawHeader(i);
    }
    auto cookie = m_reply->header(QNetworkRequest::KnownHeaders::CookieHeader);
    QNetworkRequest req(m_reply->header(QNetworkRequest::KnownHeaders::LocationHeader).toString());
    req.setHeader(QNetworkRequest::KnownHeaders::CookieHeader, cookie);
    m_reply->deleteLater();
    m_reply = m_nam.get(req);
    QObject::connect(m_reply, SIGNAL(readyRead()), this,  SLOT(streamReadyRead()));
    QObject::connect(m_reply, SIGNAL(finished()), this,  SLOT(streamFinished()));
}

void StreamSrc::networkError(QNetworkReply::NetworkError error) {
    qDebug() << "error " << error;
}

#include "streamsrc.h"
#include <QNetworkRequest>
#include <QNetworkReply>

StreamSrc::~StreamSrc() {
    qDebug() << __FUNCTION__;
}

StreamSrc::StreamSrc(const QString &url) : SoundSource(url), m_decoder(&m_fifo), m_nam(this)
{
    qDebug() << __FUNCTION__ << url;
    QNetworkRequest req(url);
    m_reply = m_nam.get(req);
    QObject::connect(m_reply, SIGNAL(readyRead()), this,  SLOT(streamReadyRead()));
    QObject::connect(m_reply, SIGNAL(finished()), this,  SLOT(streamFinished()));
}

int StreamSrc::readPcm(char * buf, const size_t length) {
    qDebug() << __FUNCTION__;
}

void StreamSrc::streamReadyRead() {
    char buffer[1024*128];
    qint64 n = m_reply->read(buffer, sizeof(buffer));
    qDebug() << __FUNCTION__ << n;
    if (n>0) {
        m_fifo.write(buffer, n);
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

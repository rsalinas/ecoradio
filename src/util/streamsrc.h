#pragma once

#include "fifo.h"
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

#include <memory>

class StreamSrc : public QObject
{
    Q_OBJECT
public:
    StreamSrc(const QString &url, std::shared_ptr<Fifo> fifo);
    virtual ~StreamSrc();

public slots:
    void streamReadyRead();
    void streamFinished();
    void networkError(QNetworkReply::NetworkError error);

private:
    QNetworkAccessManager m_nam;
    QNetworkReply * m_reply;
    std::shared_ptr<Fifo> m_fifo;
};

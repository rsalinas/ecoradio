#pragma once

#include "soundsource.h"

#include "fifo.h"
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include "mpg123wrap.h"

class StreamSrc : public SoundSource
{
    Q_OBJECT
public:
    StreamSrc(const QString &url);
    virtual int readPcm(char * buf, const size_t length);
    virtual     ~StreamSrc();

private:
    QNetworkAccessManager m_nam;
    QNetworkReply * m_reply;
    Fifo m_fifo;
    Mpg123 m_decoder;


public slots:
    void streamReadyRead();
    void streamFinished();
    void networkError(QNetworkReply::NetworkError error);
};

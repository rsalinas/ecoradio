#pragma once

#include <QObject>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>


class HttpReader : public QObject {
    Q_OBJECT
public:
    HttpReader(const QUrl &url) :
        m_reply(m_nam.get(QNetworkRequest (url)))
    {

        QObject::connect(m_reply, SIGNAL(readyRead()), this, SLOT(readyRead()));
        QObject::connect(m_reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(error(QNetworkReply::NetworkError)));
    }

    QString readAll();

private slots:
    void error(QNetworkReply::NetworkError code);

 void readyRead();

private:
    QNetworkAccessManager m_nam;
    QNetworkReply * m_reply;
    QByteArray result;
};

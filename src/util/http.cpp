#include "http.h"

#include <QEventLoop>

QString HttpReader::readAll() {
    QEventLoop eventLoop;
    QObject::connect(m_reply, SIGNAL(finished()), &eventLoop, SLOT(quit()));
    qDebug() <<        eventLoop.exec() ;
    return result;
}
void HttpReader::error(QNetworkReply::NetworkError code) {
    qDebug() << "error: " << code << m_reply->errorString();
}
void HttpReader::readyRead() {
    qDebug() <<  __FUNCTION__;
    result.append(m_reply->readAll());
}

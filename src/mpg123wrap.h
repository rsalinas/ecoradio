#pragma once

#include "soundsource.h"
#include <exception>
#include <mpg123.h>
#include <QUrl>

class MemHandle;
class QNetworkAccessManager;
class QNetworkReply;


class Mpg123 : public SoundSource {
    Q_OBJECT
public:
    class Mpg123Exception : public std::exception {
    };

    Mpg123(QIODevice *dev);
    Mpg123(const QUrl &url);
    Mpg123(std::shared_ptr<QIODevice> dev);
    Mpg123(const QString &file);
    ~Mpg123();
    int readPcm(char *buf, const size_t length) override;
    int goTo(int millis) override;
    int lengthMillis() override;
    int currentMillis() override;

signals:
    void ready();

public slots:

    void readyRead();
    void streamFinished();


private:
    void postInit();
    int initMh();
    mpg123_handle *mh;

    QNetworkAccessManager * m_nam = nullptr;
    QNetworkReply * m_reply = nullptr;    
    std::shared_ptr<QIODevice> m_devholder;
    MemHandle * m_filemh = nullptr;
    bool m_initialized = false;
    bool m_complete = false;
};

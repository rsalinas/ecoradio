#pragma once


#include <csignal>
#include "sinks/sink.h"
#include <exception>
#include <memory>
#include <QProcess>
#include <QIODevice>
#include <QSettings>
#include <QDebug>

struct shout;
struct _util_dict;

class OggFwd : public QIODevice
{
public:
    struct Metadata {
        Metadata();
        void add(const QString &key, const QString &value);
        ~Metadata();

        Q_DISABLE_COPY(Metadata)
        friend class OggFwd;
    private:
        struct _util_dict * const self = nullptr;
    };

    struct Config
    {
        QString hostName;
        int port;
        QString passwd;
        QString mount;
        QString description;
        QString genre;
        QString name;
        QString url;
        QString user;
        Config() {

        }

        Config(const QSettings &settings)
            : hostName(settings.value("hostname").toString())
            , port(settings.value("port").toInt())
            , mount(settings.value("mount").toString())
            , passwd(settings.value("passwd").toString())
        {
               qDebug() << hostName << port << mount << passwd;
//               QSettings ss("test.ini", QSettings::Format::NativeFormat);
//               ss.setValue("hostname", "pepe");
//               ss.sync();;
        }

    };
    OggFwd(const Config &config);
    virtual ~OggFwd();
    //    virtual int writeBytes(const char * buf, int length);
    virtual qint64 readData(char *data, qint64 maxlen) override ;
    virtual qint64 writeData(const char *data, qint64 len) override;


        void setMetadata(const Metadata &metadata);


    class OggFwdException : public std::exception
    {
    public:
        OggFwdException(const QString &msg);
        const char * what() const _GLIBCXX_USE_NOEXCEPT override {
            return m_msg.c_str();
        }

    private:
        std::string m_msg;
    };

private:
    struct shout *shout;
    size_t m_total = 0;
};


class ProcessOggFwd
{
public:
    ProcessOggFwd();

private:
    QProcess m_process;
};



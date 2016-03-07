#pragma once


#include <csignal>
#include <sink.h>
#include <exception>
#include <memory>
#include <QProcess>
#include <QIODevice>

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



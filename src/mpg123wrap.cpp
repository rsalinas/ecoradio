#include "mpg123wrap.h"

#include <QDebug>
#include "streamsrc.h"
#include <QTimer>
#include <QNetworkRequest>
#include <QNetworkReply>


class MemHandle {
public:
    MemHandle(QIODevice * dev)  : m_dev(dev)
    {
        qDebug() << __FUNCTION__;
    }

    ssize_t read (char *buf, size_t length) {
        auto ret = m_dev->read(buf, length);;
        return ret;

    }

    off_t lseek(off_t offset, int whence) {
        qDebug() << __FUNCTION__ << offset << whence;
        switch (whence ) {
        case SEEK_SET:
            break;
        case SEEK_CUR:
            offset = m_dev->pos() + offset;
            break;
        case SEEK_END:
            offset = m_dev->size() + offset;
            break;
        }
        auto ret = m_dev->seek(offset);
        if (ret)
            return offset;
        else {
            return -1;
        }
    }

    ~MemHandle() {
        qDebug() << __FUNCTION__;
    }

    size_t bytesAvailable() {
        return m_dev->bytesAvailable();
    }


    static ssize_t r_read (void *h, void *buf, size_t length) {
        return static_cast<MemHandle*>(h)->read(static_cast<char*>(buf), length);
    }

    static off_t r_lseek(void *h, off_t offset, int whence) {
        return static_cast<MemHandle*>(h)->lseek(offset, whence);
    }

    static void cleanup(void* h) {
        delete static_cast<MemHandle*>(h);
    }


    //private:
    QIODevice * m_dev;
};

int initLibrary() {
    qDebug() << __FUNCTION__;
    int err = mpg123_init();
    if (err == MPG123_OK) {
        atexit(&mpg123_exit);
    }
    return err;
}


mpg123_handle *commonInit() {
    static int  initStatus = initLibrary();
    if (initStatus  != MPG123_OK)  {
        qFatal("cannot init mpg123");
    }

    int err;
    mpg123_handle *mh;
    if(err != MPG123_OK || (mh = mpg123_new(NULL, &err)) == NULL)
    {
        fprintf(stderr, "Basic setup goes wrong: %s", mpg123_plain_strerror(err));
        throw Mpg123::Mpg123Exception();
    }

    /* Simple hack to enable floating point output. */
    //    if(argc >= 4 && !strcmp(argv[3], "f32"))
    mpg123_param(mh, MPG123_ADD_FLAGS, MPG123_FORCE_RATE, 0.);
    mpg123_param(mh, MPG123_ADD_FLAGS, MPG123_FORCE_STEREO, 0.);

    const long * rates;
    size_t rate_count;
    int enc = MPG123_ENC_SIGNED_16;
    mpg123_rates(&rates, &rate_count);
    for(int i=0; i<rate_count; ++i) {
        qDebug() << "rate: " << rates[i];
        mpg123_format(mh, rates[i], MPG123_MONO|MPG123_STEREO, enc);
    }

    mpg123_param(mh, MPG123_ADD_FLAGS, MPG123_AUTO_RESAMPLE|MPG123_FORCE_STEREO, 0.);
    mpg123_param(mh, MPG123_FORCE_RATE, 44100, 0.);
    qDebug() << "returning mh";
    return mh;
}

int Mpg123::initMh() {
    qDebug() << __FUNCTION__;
    mpg123_replace_reader_handle(mh, MemHandle::r_read, MemHandle::r_lseek, MemHandle::cleanup);

    if (mpg123_open_handle(mh, m_filemh) != MPG123_OK) {
        fprintf( stderr, "Trouble with mpg123: %s\n", mpg123_strerror(mh) );
        throw Mpg123Exception();
    }
    postInit();
    return 0;
}

void Mpg123::readyRead() {
    if (m_filemh->m_dev->bytesAvailable() > 512) {
        emit ready();
    }

}

void Mpg123::streamFinished() {
    qDebug() << __FUNCTION__;

    for (const auto &i : m_reply->rawHeaderList()) {
        qDebug() << i << m_reply->rawHeader(i);
    }
    auto cookie = m_reply->header(QNetworkRequest::KnownHeaders::CookieHeader);
    auto location = m_reply->header(QNetworkRequest::KnownHeaders::LocationHeader).toString();
    if (location.size()) {
        qDebug() << "Location: " << location;
        QNetworkRequest req(location);
        req.setHeader(QNetworkRequest::KnownHeaders::CookieHeader, cookie);
        sender()->deleteLater();
        m_reply = m_nam->get(req);
        m_filemh->m_dev = m_reply;
        QObject::connect(m_reply, SIGNAL(readyRead()), this,  SLOT(readyRead()));
        QObject::connect(m_reply, SIGNAL(finished()), this,  SLOT(streamFinished()));
    } else {
        qDebug() << "COMPLETE!";
        m_complete = true;
    }


}

Mpg123::Mpg123(const QUrl &url)
    : SoundSource(url.url())
    , mh(commonInit())
{
    m_nam = new QNetworkAccessManager(this);
    m_reply = m_nam->get(QNetworkRequest(url));
    m_filemh = new MemHandle(m_reply);
    connect(m_reply, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(m_reply, SIGNAL(finished()), this, SLOT(streamFinished()));
    qDebug() << "Download started" << m_reply->error();
}

Mpg123::Mpg123(QIODevice * dev) :
    SoundSource("dev"),
    mh(commonInit()),
    m_filemh(new MemHandle(dev))
{
    qDebug() << "delaying start of filemh";
}

Mpg123::Mpg123(std::shared_ptr<QIODevice> dev) : SoundSource("dev"), mh(commonInit()), m_devholder(dev), m_filemh(new MemHandle(dev.get()))
{
    qDebug() << "delaying start of filemh";
}


Mpg123::Mpg123(const QString &file) : SoundSource(file), mh(commonInit())
{
    qDebug() << "Mpg123 opening " << file;
    postInit();

    if (mpg123_open(mh, file.toStdString().c_str()) != MPG123_OK) {
        fprintf( stderr, "Trouble with mpg123: %s\n", mpg123_strerror(mh) );
        throw Mpg123Exception();
    }
}

void Mpg123::postInit() {
    qDebug() << __FUNCTION__;
    /* Peek into track and get first output format. */
    qDebug() << "mpg123_getformat...";
    long rate = 0;
    int  channels = 0, encoding = 0;
    if (mpg123_getformat(mh, &rate, &channels, &encoding) != MPG123_OK ) {
        fprintf( stderr, "Trouble with mpg123: %s\n", mpg123_strerror(mh) );
        throw Mpg123Exception();
    }
    qDebug() << rate << channels << encoding << " in file";

    rate = 44100;
    channels = 2;

    if(encoding != MPG123_ENC_SIGNED_16 && encoding != MPG123_ENC_FLOAT_32)
    { /* Signed 16 is the default output format anyways; it would actually by only different if we forced it.
         So this check is here just for this explanation. */
        fprintf(stderr, "Bad encoding: 0x%x!\n", encoding);
        throw Mpg123Exception();
    }
    /* Ensure that this output format will not change (it could, when we allow it). */
    qDebug() << "format";
    mpg123_format_none(mh);
    if (MPG123_OK != mpg123_format(mh, rate, channels, MPG123_ENC_SIGNED_16)) {
        qFatal("Cannot set format");
    }

    qDebug() << "mpg123_outblock(mh)" << mpg123_outblock(mh);
    m_initialized = true;
    emit ready();
}

int Mpg123::readPcm(char *buf, const size_t length)
{
    qDebug() << m_filemh->bytesAvailable() << m_complete;
    if (m_closed) {
        qDebug() << "mp3 closed";
        return -1;
    }
    if (m_filemh ) {
        if (m_reply->error()) {
            qDebug() << "Network error";
            return -1;
        }
        if (!m_initialized ) {
            if (m_filemh->bytesAvailable() < 512 && ! m_complete) {
                qDebug() << "available: " << m_filemh->bytesAvailable();
                return 0;
            }
            if (initMh() < 0)
                return -1;
        }

        if (!m_complete && m_filemh->bytesAvailable() < 64*1024) {
            qDebug() << "mp3 streamer has less than 64kB, returning 0 WAV bytes" << m_filemh->bytesAvailable();
            return 0;
        }
    }

    size_t done = 0;
    int  err = mpg123_read( mh, reinterpret_cast<unsigned char*>(buf), length, &done);
    switch (err) {
    case MPG123_OK:
        return done;
    case MPG123_DONE:
        qDebug() << "MPG123_DONE" << done << mpg123_plain_strerror(err);
        close();
        return done;
    case MPG123_ERR:
        qDebug() << "MPG123_ERR" << done <<  mpg123_strerror(mh)  ;
        close();
    default:
        return -1;
    }
}

Mpg123::~Mpg123() {
    mpg123_close(mh);
    mpg123_delete(mh);
}

int lengthMillis();
int currentMillis();

int Mpg123::currentMillis() {
    return mpg123_tell(mh) * 1000 / 44100.0; //FIXME
}

int Mpg123::lengthMillis() {
    return mpg123_length(mh) *1000 / 44100; //FIXME
}


int Mpg123::goTo(int millis) {
    mpg123_seek(mh, 44100*millis / 1000, 0);
}

#include "mpg123wrap.h"

#include <cstdio>
#include <QDebug>

#include <mpg123.h>


Mpg123::Mpg123(const QString &file) : SoundSource(file)
{
    qDebug() << "Mpg123 opening " << file;
    size_t buffer_size = 0;

    int  channels = 0, encoding = 0;
    int  err  = mpg123_init();
    if(err != MPG123_OK || (mh = mpg123_new(NULL, &err)) == NULL)
    {
        fprintf(stderr, "Basic setup goes wrong: %s", mpg123_plain_strerror(err));
        throw Mpg123Exception();
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

    /* Let mpg123 work with the file, that excludes MPG123_NEED_MORE messages. */
    qDebug() << "open";
    long rate = 0;

    if(    mpg123_open(mh, file.toStdString().c_str()) != MPG123_OK
           /* Peek into track and get first output format. */
           || mpg123_getformat(mh, &rate, &channels, &encoding) != MPG123_OK )
    {
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
        qFatal("Cannot set format %d", err);
    }

    qDebug() << "mpg123_outblock(mh)" << mpg123_outblock(mh);
}

int Mpg123::readPcm(char *buf, const size_t length)
{
    if (m_closed) {
        return -1;
    }
    size_t done = 0;
    int  err = mpg123_read( mh, reinterpret_cast<unsigned char*>(buf), length, &done );
    /* We are not in feeder mode, so MPG123_OK, MPG123_ERR and MPG123_NEW_FORMAT are the only possibilities.
           We do not handle a new format, MPG123_DONE is the end... so abort on anything not MPG123_OK. */
    /*} */
    //    while (err==MPG123_OK);

    //    if(err != MPG123_DONE)
    //        fprintf( stderr, "Warning: Decoding ended prematurely because: %s\n",
    //                 err == MPG123_ERR ? mpg123_strerror(mh) : mpg123_plain_strerror(err) );
    switch (err) {
    case MPG123_OK:
        return done;
    case MPG123_DONE:
        qDebug() << "MPG123_DONE" << done;
        close();
        return done;
    case MPG123_ERR:
        qDebug() << "MPG123_ERR" << done;
        close();
    default:
        return -1;
    }
}


Mpg123::~Mpg123() {
    mpg123_close(mh);
    mpg123_delete(mh);
    mpg123_exit();
}

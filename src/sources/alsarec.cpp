#include "alsarec.h"

#include <QDateTime>
#include <QDebug>
#include "util.h"


AlsaRec::AlsaRec(const QString &deviceName) : SoundSource(__FUNCTION__)
{
    unsigned int rate = 44100;

    int err;
    if ((err = snd_pcm_open (&capture_handle, deviceName.toStdString().c_str(), SND_PCM_STREAM_CAPTURE, 0)) < 0) {
        throw AlsaException("cannot open audio device  "+deviceName , err);
    }


    snd_pcm_hw_params_t *hw_params;

    if ((err = snd_pcm_hw_params_malloc (&hw_params)) < 0) {
        throw AlsaException("cannot allocate hardware parameter structure", err);
    }
    std::shared_ptr<snd_pcm_hw_params_t> hw_paramsGuard(hw_params, snd_pcm_hw_params_free);

    if ((err = snd_pcm_hw_params_any (capture_handle, hw_params)) < 0) {
        fprintf (stderr, "cannot initialize hardware parameter structure (%s)\n",
                 snd_strerror (err));
        exit (1);
    }

    if ((err = snd_pcm_hw_params_set_access (capture_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
        fprintf (stderr, "cannot set access type (%s)\n",
                 snd_strerror (err));
        exit (1);
    }

    if ((err = snd_pcm_hw_params_set_format (capture_handle, hw_params, format)) < 0) {
        fprintf (stderr, "cannot set sample format (%s)\n",
                 snd_strerror (err));
        exit (1);
    }

    if ((err = snd_pcm_hw_params_set_rate_near (capture_handle, hw_params, &rate, 0)) < 0) {
        fprintf (stderr, "cannot set sample rate (%s)\n",
                 snd_strerror (err));
        exit (1);
    }

    if ((err = snd_pcm_hw_params_set_channels (capture_handle, hw_params, 2)) < 0) {
        fprintf (stderr, "cannot set channel count (%s)\n",
                 snd_strerror (err));
        exit (1);
    }

    if ((err = snd_pcm_hw_params (capture_handle, hw_params)) < 0) {
        fprintf (stderr, "cannot set parameters (%s)\n",
                 snd_strerror (err));
        exit (1);
    }

    if ((err = snd_pcm_prepare (capture_handle)) < 0) {
        fprintf (stderr, "cannot prepare audio interface for use (%s)\n",
                 snd_strerror (err));
        exit (1);
    }
}

int AlsaRec::readPcm(char * buf, const size_t length)
{
    AutoTimeMeter tm(__FUNCTION__);

    if (m_status == Finished) {
        qDebug() << "AlsaRec was closed";
        return -1;
    }

    int buffer_frames = length/(snd_pcm_format_width(format) / 8 * 2);
    int err;
    if ((err = snd_pcm_readi (capture_handle, buf, buffer_frames)) != buffer_frames) {
        qDebug("read from audio interface failed (%s)\n",
               snd_strerror (err));
        return -1;
    }
    return length;
}
AlsaRec::~AlsaRec() {
    AutoTimeMeter tm(__FUNCTION__);
    snd_pcm_close (capture_handle);
}

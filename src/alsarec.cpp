#include "alsarec.h"


#include "pcm_player.h"

AlsaRec::AlsaRec(const QString &deviceName) : Decoder(__FUNCTION__)
{
    unsigned int rate = 44100;

    int err;
    if ((err = snd_pcm_open (&capture_handle, deviceName.toStdString().c_str(), SND_PCM_STREAM_CAPTURE, 0)) < 0) {
        qFatal("cannot open audio device %s (%s)", deviceName.toStdString().c_str(), snd_strerror (err));
        throw new PcmPlayerException;
    }

    fprintf(stdout, "audio interface opened\n");
    snd_pcm_hw_params_t *hw_params;

    if ((err = snd_pcm_hw_params_malloc (&hw_params)) < 0) {
        fprintf (stderr, "cannot allocate hardware parameter structure (%s)\n",
                 snd_strerror (err));
        exit (1);
    }

    fprintf(stdout, "hw_params allocated\n");

    if ((err = snd_pcm_hw_params_any (capture_handle, hw_params)) < 0) {
        fprintf (stderr, "cannot initialize hardware parameter structure (%s)\n",
                 snd_strerror (err));
        exit (1);
    }

    fprintf(stdout, "hw_params initialized\n");

    if ((err = snd_pcm_hw_params_set_access (capture_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
        fprintf (stderr, "cannot set access type (%s)\n",
                 snd_strerror (err));
        exit (1);
    }

    fprintf(stdout, "hw_params access setted\n");

    if ((err = snd_pcm_hw_params_set_format (capture_handle, hw_params, format)) < 0) {
        fprintf (stderr, "cannot set sample format (%s)\n",
                 snd_strerror (err));
        exit (1);
    }

    fprintf(stdout, "hw_params format setted\n");

    if ((err = snd_pcm_hw_params_set_rate_near (capture_handle, hw_params, &rate, 0)) < 0) {
        fprintf (stderr, "cannot set sample rate (%s)\n",
                 snd_strerror (err));
        exit (1);
    }

    fprintf(stdout, "hw_params rate setted\n");

    if ((err = snd_pcm_hw_params_set_channels (capture_handle, hw_params, 2)) < 0) {
        fprintf (stderr, "cannot set channel count (%s)\n",
                 snd_strerror (err));
        exit (1);
    }

    fprintf(stdout, "hw_params channels setted\n");

    if ((err = snd_pcm_hw_params (capture_handle, hw_params)) < 0) {
        fprintf (stderr, "cannot set parameters (%s)\n",
                 snd_strerror (err));
        exit (1);
    }

    fprintf(stdout, "hw_params setted\n");

    snd_pcm_hw_params_free (hw_params);

    fprintf(stdout, "hw_params freed\n");

    if ((err = snd_pcm_prepare (capture_handle)) < 0) {
        fprintf (stderr, "cannot prepare audio interface for use (%s)\n",
                 snd_strerror (err));
        exit (1);
    }

    fprintf(stdout, "audio interface prepared\n");

}

int AlsaRec::readPcm(char * buf, const size_t length)
{
    if (m_closed) {
        qDebug() << "AlsaRec closed";
        return -1;
    }

    int buffer_frames = length/(snd_pcm_format_width(format) / 8 * 2);
    int err;
    if ((err = snd_pcm_readi (capture_handle, buf, buffer_frames)) != buffer_frames) {
        qFatal("read from audio interface failed (%s)\n",
               snd_strerror (err));
    }
    return length;
}
AlsaRec::~AlsaRec() {
    snd_pcm_close (capture_handle);
}

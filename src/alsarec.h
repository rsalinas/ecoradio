#pragma once

#include "decoder.h"

#include <alsa/asoundlib.h>

class AlsaRec : public Decoder {
public:
    AlsaRec(const QString &deviceName);
    virtual ~AlsaRec();
    virtual int readPcm(char * buf, const size_t length);

private:
    snd_pcm_format_t format = SND_PCM_FORMAT_S16_LE;
    snd_pcm_t *capture_handle;
};

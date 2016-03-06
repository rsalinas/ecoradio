#pragma once

#include "soundsource.h"

#include <alsa/asoundlib.h>
#include <string>

class AlsaException : public std::exception{
public:
    AlsaException(const QString &cause) : m_cause(cause) {
    }

    AlsaException(const QString &cause, int err) : AlsaException(cause+ " - "+snd_strerror (err)) {
    }

    virtual const char* what() const _GLIBCXX_USE_NOEXCEPT {
        return m_cause.toStdString().c_str();
    }

private:
    QString m_cause;
};

class AlsaRec : public SoundSource {
public:
    AlsaRec(const QString &deviceName);
    virtual ~AlsaRec();
    virtual int readPcm(char * buf, const size_t length);

private:
    snd_pcm_format_t format = SND_PCM_FORMAT_S16_LE;
    snd_pcm_t *capture_handle;
};

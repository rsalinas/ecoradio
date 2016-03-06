#pragma once

#include "soundsource.h"
#include <exception>
#include <mpg123.h>

class Mpg123 : public SoundSource {
public:
    class Mpg123Exception : public std::exception {
    };

    Mpg123(const QString &file);
    ~Mpg123();
    int readPcm(char *buf, const size_t length);

private:
    mpg123_handle *mh;
};

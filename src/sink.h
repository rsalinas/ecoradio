#pragma once

#include "format.h"
#include <cstdio>

class SndSink
{
public:

    virtual ~SndSink() {
    }

    virtual bool writePcm(char * buffer, int length) = 0;

protected:
    const SndFormat m_format;
};

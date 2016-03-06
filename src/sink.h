#pragma once

#include <cstdio>

class SndSink
{
public:
    struct Format
    {
        const int rate;
        const int channels;
        const int sampleSizeBits;
        const int bufferSize;
        Format(int bits = 16, int channels = 2 , int rate = 44100, int millis=100) :
            sampleSizeBits(bits),
            channels(channels),
            rate(rate),
            bufferSize(bits/2*rate*channels*millis/1000)  {

        }      
    };

    virtual ~SndSink() {
    }

    virtual bool writePcm(char * buffer, int length) = 0;

protected:
    const Format m_format;
};

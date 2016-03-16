#pragma once

struct SndFormat
{
    const int rate;
    const int channels;
    const int sampleSizeBits;
    const int bufferSize;
    SndFormat(int bits = 16, int channels = 2 , int rate = 44100, int millis=100)
        : rate(rate)
        , channels(channels)
        , sampleSizeBits(bits)
        , bufferSize(bits/2*rate*channels*millis/1000)
    {
    }
};


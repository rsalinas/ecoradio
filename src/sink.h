#pragma once

class SndSink
{
public:
    virtual ~SndSink() = 0;
    virtual ssize_t writePcm(char * buffer, int length) = 0;
};

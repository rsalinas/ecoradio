#pragma once

#include "sink.h"
#include <ao/ao.h>

#include <exception>

class AoSink : public SndSink
{
public:
    class AoSinkException : public std::exception
    {
    };
    AoSink(const SndFormat &m_format);
    ~AoSink();
    bool writePcm(char *buffer, int length) override;

private:
    ao_sample_format m_format;
    size_t buf_size;
    ao_device *device = nullptr;
};

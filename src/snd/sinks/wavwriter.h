#pragma once

#include "sink.h"

#include <QString>

#include <sndfile.hh>

class WavWriter : public SndSink {
public:
    WavWriter(const QString &filename);
    bool writePcm(char *buffer, int length);
    ~WavWriter();

private:
    SndfileHandle m_sf;
};

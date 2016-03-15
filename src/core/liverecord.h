#pragma once

#include <common/program.h>

class WavWriter;

class LiveProgramRecording {
public:
    LiveProgramRecording(const LiveProgram &p);
    ~LiveProgramRecording();

    void compress();

private:
    std::shared_ptr<WavWriter> m_wavWriter;
};

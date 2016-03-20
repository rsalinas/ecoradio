#pragma once

#include <common/program.h>

class WavWriter;

class LiveProgramRecording {
public:
    LiveProgramRecording(const LiveProgram &p, const QString &filename);
    ~LiveProgramRecording();

    bool compress();

    std::shared_ptr<WavWriter> getWriter() {
        return m_wavWriter;
    }

private:
    std::shared_ptr<WavWriter> m_wavWriter;
};

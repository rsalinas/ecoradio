#include "liverecord.h"

#include <snd/sinks/wavwriter.h>


LiveProgramRecording::LiveProgramRecording(const LiveProgram &p)
{
    QString file = "directo.wav"; // FIXME
    m_wavWriter = std::make_shared<WavWriter>(file);
}

LiveProgramRecording::~LiveProgramRecording()
{
}

bool LiveProgramRecording::compress()
{
    return false;
}

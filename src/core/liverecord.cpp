#include "liverecord.h"

#include <snd/sinks/wavwriter.h>


LiveProgramRecording::LiveProgramRecording(const LiveProgram &p,
                                           const QString &filename)
    : m_wavWriter(std::make_shared<WavWriter>(filename))
{
}

LiveProgramRecording::~LiveProgramRecording()
{
}

bool LiveProgramRecording::compress()
{
    return false;
}

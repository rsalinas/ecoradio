#include "gtest/gtest.h"

#include <QDebug>
#include "snd/sinks/wavwriter.h"
#include "snd/sources/soundsource.h"

TEST(WavWriter, WavWriter)
{
    SinWave sw;
    WavWriter ww("wavwriter.wav");
    char buffer[44100*2*2];
    memset(buffer, 0, sizeof(buffer));
    ASSERT_TRUE(sw.readPcm(buffer, sizeof(buffer)));
    ASSERT_TRUE(ww.writePcm(buffer, sizeof(buffer)));
}

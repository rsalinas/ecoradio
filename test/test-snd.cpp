#include "gtest/gtest.h"

#include "decoder.h"
#include "pcm_player.h"

#include <QBitArray>

TEST (DecoderTest, DISABLED_Decoder0) {
    Mp3Decoder d("test.mp3");
    SinWave swg(440.0);
    SinWave swg2(340.0);
    PcmPlayer player;
    char buffer[player.buf_size];
    char buffer2[player.buf_size];
    size_t n = swg.readPcm(buffer, sizeof(buffer));
    size_t n2 = swg2.readPcm(buffer2, sizeof(buffer2));
    ASSERT_GT(n, 0);
    for (int i=0; i < 2; i++) {
        player.copyFirst(buffer);
        player.add(buffer2);
        player.writeMix();
//        player.write(buffer2, n2);
    }
}


TEST (DecoderTest, Mp3) {
    Mp3Decoder d("beep.mp3");
    PcmPlayer player;
    SinWave swg2(440.0);
    char buffer[player.buf_size];
    char buffer2[player.buf_size];

    for (int i=0; i < 200; i++) {
        size_t n = d.readPcm(buffer, sizeof(buffer));
        qDebug() << "readPcm said " << n;
        swg2.readPcm(buffer2, sizeof(buffer2));
        ASSERT_GT(n, 0);
//        player.write(buffer, sizeof(buffer));
        player.copyFirst(buffer);
//        player.add(buffer2);
        player.writeMix();
//        player.write(buffer2, n2);
    }
}

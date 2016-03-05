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
}


TEST (DecoderTest, DISABLED_Mp3) {
    Mp3Decoder d("beep.mp3");
    PcmPlayer player;
    SinWave swg2(440.0);
}


TEST(PlayerThread, DISABLED_PlayerThread ){
    PcmPlayer player;
    std::shared_ptr<Decoder> sin = std::make_shared<SinWave>(440.0);
    sin->setFadeIn(1000);
    player.addStream(sin);
    std::shared_ptr<Decoder> d = std::make_shared<Mp3Decoder>("test.mp3");
    player.addStream(d);
    sleep(3);
    sin->stopFadeOut(1000);
    sin->waitEnd();
    sin = std::make_shared<SinWave>(2440.0);
    sin->setFadeIn(500);
    sleep(2);
    d->stopFadeOut(1000);
    d->waitEnd();
    player.addStream(sin);
    sleep(2);
    sin->stopFadeOut(1000);
    player.waitEnd();
}


TEST(PlayerThread, RecordPlayerThread ){
    PcmPlayer player;
    std::shared_ptr<Decoder> sin = std::make_shared<Arecord>();
    sleep(3);
//    sin->setFadeIn(1000);
    player.addStream(sin);
    std::shared_ptr<Decoder> d = std::make_shared<Mp3Decoder>("test.mp3");
//    player.addStream(d);
//    sleep(3);
    player.waitEnd();
}

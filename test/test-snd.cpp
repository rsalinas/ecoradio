#include "gtest/gtest.h"

#include "decoder.h"
#include "pcm_player.h"
#include "alsarec.h"
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


TEST(PlayerThread, DISABLED_RecordPlayerThread ){
    PcmPlayer player;
    std::shared_ptr<Decoder> line= std::make_shared<AlsaRec>("default");
    line->setFadeIn(1000);   //empezará con un fundido
    player.addStream(line);
    std::shared_ptr<Decoder> mp3 = std::make_shared<Mp3Decoder>("beep.mp3");
    sleep(3);  //con esto le damos tiempo al descompresor a ir trabajando
    line->stopFadeOut(1000);   //hará un fundido de bajada y parará
    mp3->setFadeIn(1000);
    player.addStream(mp3);
    player.waitEnd();         //espera a que terminen todos los flujos
}

TEST(PlayerThread, ArecordPlayerThread ){
    PcmPlayer player;
    std::shared_ptr<Decoder> line= std::make_shared<ThreadingDecoder>(
                std::unique_ptr<Decoder>(new Arecord()), 10);
//    line->setFadeIn(1000);   //empezará con un fundido
    player.addStream(line);
    sleep(5);
    line->stopFadeOut(1000);
//    std::shared_ptr<Decoder> mp3 = std::make_shared<Mp3Decoder>("beep.mp3");
//    sleep(3);  //con esto le damos tiempo al descompresor a ir trabajando
//    line->stopFadeOut(1000);   //hará un fundido de bajada y parará
//    mp3->setFadeIn(1000);
//    player.addStream(mp3);
    player.waitEnd();         //espera a que terminen todos los flujos
}

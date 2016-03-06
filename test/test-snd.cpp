#include "gtest/gtest.h"

#include "soundsource.h"
#include "pcm_player.h"
#include "alsarec.h"
#include <QBitArray>
#include <QCoreApplication>
#include <QTimer>
#include <QObject>

#include "oggfwd.h"
#include "mpg123wrap.h"

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
    std::shared_ptr<SoundSource> sin = std::make_shared<SinWave>(440.0);
    sin->setFadeIn(1000);
    player.addStream(sin);
    std::shared_ptr<SoundSource> d = std::make_shared<Mp3Decoder>("test.mp3");
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
    std::shared_ptr<SoundSource> line= std::make_shared<AlsaRec>("default");
    line->setFadeIn(1000);   //empezará con un fundido
    player.addStream(line);
    std::shared_ptr<SoundSource> mp3 = std::make_shared<Mp3Decoder>("beep.mp3");
    sleep(3);  //con esto le damos tiempo al descompresor a ir trabajando
    line->stopFadeOut(1000);   //hará un fundido de bajada y parará
    mp3->setFadeIn(1000);
    player.addStream(mp3);

    player.waitEnd();         //espera a que terminen todos los flujos
}

class SilenceListener : public QObject {

    Q_OBJECT
public slots:
    void silenceStarted() {
        qDebug() << __FUNCTION__ << "class";
    }
    void silenceFinished() {
        qDebug() << __FUNCTION__ << "class";
    }
};


class PlayerFixture :  public ::testing::Test::Test {
public:

protected:
    SilenceListener sl;
};

void silenceStarted() {
    qDebug() << __FUNCTION__;
}
void silenceFinished() {
    qDebug() << __FUNCTION__;
}

TEST_F(PlayerFixture, ArecordPlayerThread ){
    try {
        PcmPlayer player;
        QObject::connect(&player, SIGNAL(silenceStarted()), &sl, SLOT(silenceStarted()));
        QObject::connect(&player, SIGNAL(silenceFinished()), &sl, SLOT(silenceFinished()));
        QObject::connect(&player, (&PcmPlayer::silenceStarted), silenceStarted);
        QObject::connect(&player, (&PcmPlayer::silenceFinished), silenceFinished);

        std::shared_ptr<SoundSource> line;
        //        std::shared_ptr<SoundSource> mp3 = std::make_shared<Mpg123>("/home/rsalinas/Sync/enconstruccio/Dalactus - Follar mola.mp3");
        std::shared_ptr<SoundSource> mp3b = std::make_shared<Mpg123>("/home/rsalinas/Sync/enconstruccio/Dalactus - Follar mola.mp3");
        //        std::shared_ptr<SoundSource> tone = std::make_shared<SinWave>(440.0);
        std::shared_ptr<SoundSource> mp3 = std::make_shared<Mpg123>("beep.mp3");
        player.addStream(mp3);
        player.waitEnd();
        return;
        sleep(1);
        //    player.addStream(mp3b);
        //    player.addStream(tone);


        if (false)
            line= std::make_shared<ThreadingDecoder>(
                        std::unique_ptr<SoundSource>(new Arecord()), 10);
        else
            line= std::make_shared<AlsaRec>("default");
        line->setFadeIn(1000);   //empezará con un fundido
        player.addStream(line);
        sleep(5);
        //    tone->stopFadeOut(2000);
        line->stopFadeOut(1000);
        sleep(3);  //con esto le damos tiempo al descompresor a ir trabajando
        line->stopFadeOut(1000);   //hará un fundido de bajada y parará
        //    mp3->setFadeIn(1000);
        line->waitEnd();
        line.reset();
        mp3->stopFadeOut(2000);
        mp3b->stopFadeOut(5000);

        player.waitEnd();         //espera a que terminen todos los flujos
        sleep(2);
    } catch (const std::exception &e) {
        qFatal("Exception: %s", e.what());
    } catch (...) {
        qFatal("Exception");
    }
}


#include "test-snd.moc"

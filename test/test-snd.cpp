#include "gtest/gtest.h"

#include "aosink.h"
#include "soundsource.h"
#include "mixer.h"
#include "alsarec.h"
#include <QBitArray>
#include <QCoreApplication>
#include <QTimer>
#include <QObject>
#include "oggfwd.h"
#include <QFile>

#include "ogg_encoder.h"

#include "oggfwd.h"
#include "mpg123wrap.h"

//https://www.xiph.org/vorbis/doc/v-comment.html

TEST(OggEncTest, DISABLED_OggEncTest)
{
    std::unique_ptr<QFile> file(new QFile("salida.ogg"));
    ASSERT_TRUE(file->open(QFile::WriteOnly));
    OggEncoder encoder(std::move(file));
    Mpg123 decoder("test.mp3");

    while (true) {
        char buffer[4096];
        int n = decoder.readPcm(buffer, sizeof(buffer));
        if (n <= 0)
            break;
        encoder.writePcm(buffer, n);
    }

    qDebug() << "Finished writing";
}

TEST(OggEncTest, DISABLED_OggEncSendTest)
{
    OggFwd::Config fwdConfig;
    fwdConfig.hostName = "vps66370.ovh.net";
    fwdConfig.port = 8000;
    fwdConfig.mount = "/tests.ogg";
    fwdConfig.passwd = "ecoradio";

    std::unique_ptr<OggFwd> output(new OggFwd(fwdConfig));
    ASSERT_TRUE(output->open(QFile::WriteOnly));
    OggFwd::Metadata metadata;
    metadata.add("ARTIST", "Artist");
    metadata.add("TITLE", "Title");
    output->setMetadata(metadata);
    OggEncoder encoder(std::move(output));
    Mpg123 decoder("test.mp3");



    while (true) {
        char buffer[4096];
        int n = decoder.readPcm(buffer, sizeof(buffer));
        if (n <= 0)
            break;
        encoder.writePcm(buffer, n);
    }


    qDebug() << "Finished writing";
}



TEST(OggFwdTest, DISABLED_OggFwdTest)
{
    OggFwd::Config fwdConfig;
    fwdConfig.hostName = "vps66370.ovh.net";
    fwdConfig.port = 8000;
    fwdConfig.mount = "/tests.ogg";
    fwdConfig.passwd = "ecoradio";

    OggFwd fwd(fwdConfig);
    OggFwd::Metadata metadata;
    metadata.add("ARTIST", "Artist");
    metadata.add("TITLE", "Title");
    fwd.setMetadata(metadata);
    sleep (5);

    QFile file("test.ogg");
    ASSERT_TRUE(file.open(QFile::ReadOnly));
    char buffer[65536];
    while (file.seek(0)) {

        qint64 n;
        while (n = file.read(buffer, sizeof(buffer)), n > 0) {
            ASSERT_EQ(n, fwd.write(buffer, n));
        }
    }


    qDebug() << "all right";
}

TEST (DecoderTest, DISABLED_Decoder0) {
    Mp3Decoder d("test.mp3");
    SinWave swg(440.0);
    SinWave swg2(340.0);
    Mixer player;
    char buffer[player.buf_size];
    char buffer2[player.buf_size];
    size_t n = swg.readPcm(buffer, sizeof(buffer));
    size_t n2 = swg2.readPcm(buffer2, sizeof(buffer2));
    ASSERT_GT(n, 0);
}


TEST (DecoderTest, DISABLED_Mp3) {
    Mp3Decoder d("beep.mp3");
    Mixer player;
    SinWave swg2(440.0);
}


TEST(PlayerThread, DISABLED_PlayerThread ){
    Mixer player;
    std::shared_ptr<SoundSource> sin = std::make_shared<SinWave>(440.0);
    sin->setFadeIn(1000);
    player.addSource(sin);
    std::shared_ptr<SoundSource> d = std::make_shared<Mp3Decoder>("test.mp3");
    player.addSource(d);
    sleep(3);
    sin->stopFadeOut(1000);
    sin->waitEnd();
    sin = std::make_shared<SinWave>(2440.0);
    sin->setFadeIn(500);
    sleep(2);
    d->stopFadeOut(1000);
    d->waitEnd();
    player.addSource(sin);
    sleep(2);
    sin->stopFadeOut(1000);
    player.waitEnd();
}



TEST(PlayerThread, DISABLED_RecordPlayerThread ){
    Mixer player;
    std::shared_ptr<SoundSource> line= std::make_shared<AlsaRec>("default");
    line->setFadeIn(1000);   //empezará con un fundido
    player.addSource(line);
    std::shared_ptr<SoundSource> mp3 = std::make_shared<Mp3Decoder>("beep.mp3");
    sleep(3);  //con esto le damos tiempo al descompresor a ir trabajando
    line->stopFadeOut(1000);   //hará un fundido de bajada y parará
    mp3->setFadeIn(1000);
    player.addSource(mp3);

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
        SndFormat format;
        Mixer player(format);
        player.addSink(std::make_shared<AoSink>(format));
        OggFwd::Config fwdConfig;
        fwdConfig.hostName = "vps66370.ovh.net";
        fwdConfig.port = 8000;
        fwdConfig.mount = "/tests.ogg";
        fwdConfig.passwd = "ecoradio";

        std::unique_ptr<OggFwd> output(new OggFwd(fwdConfig));
        OggFwd::Metadata metadata;
        metadata.add("ARTIST", "Artist");
        metadata.add("TITLE", "Title");
        output->setMetadata(metadata);
        qDebug() << "casi";
        auto encoder = std::make_shared<OggEncoder>(std::move(output));


        player.addSink(encoder);


        QObject::connect(&player, SIGNAL(silenceStarted()), &sl, SLOT(silenceStarted()));
        QObject::connect(&player, SIGNAL(silenceFinished()), &sl, SLOT(silenceFinished()));
        QObject::connect(&player, (&Mixer::silenceStarted), silenceStarted);
        QObject::connect(&player, (&Mixer::silenceFinished), silenceFinished);

        std::shared_ptr<SoundSource> line;
        std::shared_ptr<SoundSource> mp3 = std::make_shared<Mpg123>("/home/rsalinas/Sync/enconstruccio/Dalactus - Follar mola.mp3");
//        std::shared_ptr<SoundSource> mp3 = std::make_shared<Mpg123>("beep.mp3");
//        auto srcMetadata = mp3->getMetadata();
        std::shared_ptr<SoundSource> mp3b = std::make_shared<Mpg123>("/home/rsalinas/Sync/enconstruccio/Dalactus - Follar mola.mp3");
        //        std::shared_ptr<SoundSource> tone = std::make_shared<SinWave>(440.0);
        //        std::shared_ptr<SoundSource> mp3 = std::make_shared<Mpg123>("beep.mp3");
        player.addSource(mp3);
        player.waitEnd();
        sleep(1);
        //    player.addStream(mp3b);
        //    player.addStream(tone);


        if (false)
            line= std::make_shared<ThreadingDecoder>(
                        std::unique_ptr<SoundSource>(new Arecord()), 10);
        else
            line= std::make_shared<AlsaRec>("default");
        line->setFadeIn(1000);   //empezará con un fundido
        player.addSource(line);
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

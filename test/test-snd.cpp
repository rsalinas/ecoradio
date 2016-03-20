#include "gtest/gtest.h"

#include <QBitArray>
#include <QCoreApplication>
#include <QFile>
#include <QObject>
#include <QTimer>

#include "snd/mixer.h"
#include "snd/oggfwd.h"
#include "snd/oggfwd.h"
#include "snd/sinks/aosink.h"
#include "snd/sinks/ogg_encoder.h"
#include "snd/sources/alsarec.h"
#include "snd/sources/mpg123wrap.h"
#include "snd/sources/soundsource.h"
#include "snd/sources/process_src.h"
#include "util/rssparser.h"
#include "util/util.h"

//https://www.xiph.org/vorbis/doc/v-comment.html

const SndFormat c_format;

OggFwd::Config getIcecastConfig() {
    OggFwd::Config  fwdConfig;
    fwdConfig.hostName = "vps66370.ovh.net";
    fwdConfig.port = 8000;
    fwdConfig.mount = "/tests.ogg";
    fwdConfig.passwd = "ecoradio";
    return fwdConfig;
}


TEST(SimpleMix, SimpleMix0) {
    auto s0 = std::make_shared<SinWave>(440.0);
    auto s1 = std::make_shared<SinWave>(840.0);

    Mixer player;
    player.addSink(std::make_shared<AoSink>(c_format));
    player.start();
    qDebug() << "fadein 440";
    s0->setSilence();
    player.addSource(s0);
    s1->setSilence();
    player.addSource(s1);

    s0->fadeIn(1000);
    for (int i = 0; i < 5 ; i++) {
        sleep(2);
        s0->fadeOut(1000, SoundSource::FadeAction::WillSilence);
        s1->fadeIn(1000);
        sleep(2);
        s0->fadeIn(1000);
        s1->fadeOut(1000, SoundSource::FadeAction::WillSilence);
    }
    s0->stopFadeOut(1000);
    s1->stopFadeOut(1000);
    player.waitEnd();
    exit(69);
}


TEST(SimpleMix, SimpleMix) {
    auto s0 = std::make_shared<SinWave>(440.0);
    auto s1 = std::make_shared<SinWave>(840.0);

    Mixer player;
    player.addSink(std::make_shared<AoSink>(c_format));
    player.start();
    qDebug() << "fadein 440";
    s0->fadeIn(1000);
//    s1->setFadeIn(1000);
    player.addSource(s0);
    sleep(1);
    qDebug() << "fadeout 440";
    s0->fadeOut(1000, SoundSource::FadeAction::WillSilence);
    sleep(2);
    qDebug() << "fadein 840";
    player.addSource(s1);
    sleep(2);
    qDebug() << "fadeout 840";
    s1->fadeOut(1000, SoundSource::FadeAction::WillStop);
    qDebug() << "fadein 440";
    s0->fadeIn(1000);
    sleep(2);
    s0->fadeOut(1000, SoundSource::FadeAction::WillStop);
//    s1->close();s
    player.waitEnd();
    exit(69);
}

TEST(OggEncTest, OggEncTest)
{
    std::unique_ptr<QFile> file(new QFile("salida.ogg"));
    ASSERT_TRUE(file->open(QFile::WriteOnly));
    OggEncoder encoder(std::move(file));
    Mpg123 decoder("samples/long.mp3");
    TimeMeter tm;

    size_t bytes = 0;

    while (true) {
        char buffer[4096];
        int n = decoder.readPcm(buffer, sizeof(buffer));
        bytes +=n;
        //        ASSERT_GE(n, 0);
        if (n <= 0)
            break;
        ASSERT_TRUE(encoder.writePcm(buffer, n));
    }
    qDebug() << "Finished writing"  <<
                decoder.lengthMillis()/tm.ellapsed() << "x";
}

TEST(OggEncTest, OggEncSendTest)
{
    std::unique_ptr<OggFwd> output(new OggFwd(getIcecastConfig()));
    ASSERT_TRUE(output->open(QFile::WriteOnly));
    OggFwd::Metadata metadata;
    metadata.add("ARTIST", "Artist");
    metadata.add("TITLE", "Title");
    output->setMetadata(metadata);
    OggEncoder encoder(std::move(output));
    Mpg123 decoder("samples/beep.mp3");

    while (true) {
        char buffer[4096];
        int n = decoder.readPcm(buffer, sizeof(buffer));
        if (n <= 0)
            break;
        ASSERT_TRUE(encoder.writePcm(buffer, n));
    }
}



TEST(OggFwdTest, OggFwdTest)
{
    OggFwd fwd(getIcecastConfig());
    OggFwd::Metadata metadata;
    metadata.add("ARTIST", "Artist");
    metadata.add("TITLE", "Title");
    fwd.setMetadata(metadata);
    sleep (5);

    QFile file("samples/system-ready.ogg");
    ASSERT_TRUE(file.open(QFile::ReadOnly));
    char buffer[64*1024];
    if (file.seek(0)) {
        qint64 n;
        while (n = file.read(buffer, sizeof(buffer)), n > 0) {
            ASSERT_EQ(n, fwd.write(buffer, n));
        }
    }
}

TEST (DecoderTest, Decoder0) {
    Mp3Decoder d("samples/beep.mp3");
    SinWave swg(440.0);
    SinWave swg2(340.0);
    Mixer player;
    char buffer[player.getBufferSize()];
    char buffer2[player.getBufferSize()];
    size_t n = swg.readPcm(buffer, sizeof(buffer));
    size_t n2 = swg2.readPcm(buffer2, sizeof(buffer2));
    ASSERT_EQ(n, sizeof(buffer));
    ASSERT_EQ(n2, sizeof(buffer2));
}


TEST (DecoderTest, Mp3PlusSin) {
    auto mp3 = std::make_shared<Mp3Decoder>("samples/beep.mp3");
    ASSERT_TRUE(mp3->waitForReadyReady(1000));
    Mixer mixer;
    mixer.addSink(std::make_shared<AoSink>(c_format));
    auto sin = std::make_shared<SinWave>(440.0);
    mixer.addSource(mp3);
    mixer.addSource(sin);
    mixer.start();
    sleep(3);
    mp3->stopFadeOut(1000);
    sleep(2);
    sin->stopFadeOut(1000);
}


TEST(PlayerThread, PlayerThread ){
    Mixer mixer;
    std::shared_ptr<SoundSource> sin = std::make_shared<SinWave>(440.0);
    sin->fadeIn(1000);
    mixer.addSink(std::make_shared<AoSink>(c_format));
    mixer.start();
    mixer.addSource(sin);
    std::shared_ptr<SoundSource> d = std::make_shared<Mp3Decoder>("samples/long.mp3");
    mixer.addSource(d);
    sleep(3);
    sin->stopFadeOut(1000);
    sin->waitEnd();
    sin = std::make_shared<SinWave>(2440.0);
    sin->fadeIn(500);
    sleep(2);
    d->stopFadeOut(1000);
    d->waitEnd();
    mixer.addSource(sin);
    sleep(2);
    sin->stopFadeOut(1000);
    mixer.waitEnd();
}



TEST(PlayerThread, DISABLED_RecordPlayerThread ){
    Mixer player;
    std::shared_ptr<SoundSource> line= std::make_shared<AlsaRec>("default");
    line->fadeIn(1000);   //empezará con un fundido
    player.addSource(line);
    std::shared_ptr<SoundSource> mp3 = std::make_shared<Mp3Decoder>("beep.mp3");
    sleep(3);  //con esto le damos tiempo al descompresor a ir trabajando
    line->stopFadeOut(1000);   //hará un fundido de bajada y parará
    mp3->fadeIn(1000);
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

auto longMp3 = "/home/rsalinas/Sync/enconstruccio/Dalactus - Follar mola.mp3";

TEST(Mp3, DISABLED_Mp3Qfile) {

    auto f = "beep.mp3";
    //    QFile qfile(f);
    //    ASSERT_TRUE(qfile.open(QFile::ReadOnly));
    //    std::shared_ptr<SoundSource> mp3 = std::make_shared<Mpg123>(&qfile);
    std::shared_ptr<SoundSource> mp3 = std::make_shared<Mpg123>(f);
    char buf[4096];
    qDebug() << mp3->readPcm(buf, sizeof(buf));
    qDebug()<< "finished test1";
}




TEST_F(PlayerFixture, DISABLED_ArecordPlayerThread ){
    try {
        SndFormat format;
        Mixer player(format);
        player.addSink(std::make_shared<AoSink>(format));

        if (false) {
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
        }



        QObject::connect(&player, SIGNAL(silenceStarted()), &sl, SLOT(silenceStarted()));
        QObject::connect(&player, SIGNAL(silenceFinished()), &sl, SLOT(silenceFinished()));
        QObject::connect(&player, (&Mixer::silenceStarted), silenceStarted);
        QObject::connect(&player, (&Mixer::silenceFinished), silenceFinished);
        player.start();



        if (false)
            for (int i=0; i < 50 ; i++) {
                auto m = std::make_shared<Mpg123>(longMp3);
                m->stopFadeOut(500);
                player.addSource(m);
            }

        if (false) {
            QFile qfile(longMp3);
            ASSERT_TRUE(qfile.open(QFile::ReadOnly));
            std::shared_ptr<SoundSource> mp3 = std::make_shared<Mpg123>(&qfile);

            //        std::shared_ptr<SoundSource> mp3 = std::make_shared<Mpg123>(longMp3);
            //        std::shared_ptr<SoundSource> mp3 = std::make_shared<Mpg123>("beep.mp3");
            //        auto srcMetadata = mp3->getMetadata();
            //        std::shared_ptr<SoundSource> mp3b = std::make_shared<Mpg123>("/home/rsalinas/Sync/enconstruccio/Dalactus - Follar mola.mp3");
            //        std::shared_ptr<SoundSource> tone = std::make_shared<SinWave>(440.0);
            //        std::shared_ptr<SoundSource> mp3 = std::make_shared<Mpg123>("beep.mp3");
            player.addSource(mp3);
            sleep(2);
            mp3->stopFadeOut(1000);
            player.waitEnd();
        }

        if (true) {
            RssParser rss("rss/podcast-linterna-diogenes_fg_f136870_filtro_1.xml");
            auto list = rss.getStreams();
            if (list.size()) {
                qDebug() << list.front();
                AutoTimeMeter atm("mp3");
                SynSleep sl0;
                auto ss = std::make_shared<Mpg123>(QUrl("http://vps66370.ovh.net/~rsalinas/beep.mp3" /*list.front()*/));
                qDebug() << "Waiting for MP3 to connect";
                sl0.sleep(ss.get(), SIGNAL(ready()));
                qDebug() << atm.ellapsed() << "Waiting for MP3 to connect. CONNECTED";

                SynSleep sl;
                player.addSource(ss);
                sl.sleep(&player, SIGNAL(sourceFinished(std::shared_ptr<SoundSource> s)));
                qDebug() << atm.ellapsed() << "finished track";

                //                            player.waitEnd();
            }
        }
        return;
        //    player.addStream(mp3b);
        //    player.addStream(tone);


        if (false) {
            std::shared_ptr<SoundSource> line;
            if (false)
                line= std::make_shared<Arecord>();
            else
                line= std::make_shared<AlsaRec>("default");
            line->fadeIn(1000);   //empezará con un fundido
            player.addSource(line);
            sleep(5);
            line->stopFadeOut(1000);
            sleep(3);  //con esto le damos tiempo al descompresor a ir trabajando
            line->stopFadeOut(1000);   //hará un fundido de bajada y parará
            line->waitEnd();
            line.reset();
        }

        //    tone->stopFadeOut(2000);

        //    mp3->setFadeIn(1000);


        player.waitEnd();         //espera a que terminen todos los flujos
        sleep(2);
    } catch (const std::exception &e) {
        FAIL() << "Exception" << e.what();
    } catch (...) {
        FAIL() << "Exception";
    }
}
TEST(Mp3, DISABLED_Mp3) {
    Mpg123 m("test.mp3");
}



class TestWrapper : public QObject {
    Q_OBJECT
public:
    void run() {
        QCoreApplication::instance()->exit(RUN_ALL_TESTS());
    }
};


int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    TestWrapper tw;
    ::testing::InitGoogleTest(&argc, argv);
    QTimer::singleShot(0, &tw, &TestWrapper::run);

    return app.exec();
}



#include "test-snd.moc"

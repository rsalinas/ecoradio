#include "gtest/gtest.h"

#include "mixer.h"
#include "scheduler.h"
#include "soundsource.h"
#include "file_scan.h"
#include "decoder.h"

#include <QMap>
#include <QTextStream>
#include <QDir>
#include <QDebug>
#include <QBitArray>

TEST (SchedulerTest, DISABLED_Scheduler0) {
    Scheduler s;
}


TEST (PlayTest, DISABLED_PlayFile) {
    FileSoundSource s("test.mp3");
    s.play();
}

static const QDir repositoryDir("/home/rsalinas/Sync/");
static const QString vistosFile = "/tmp/vistos.txt";
static const QString indexFile = "/tmp/all";

TEST(MixerTest, SimplePlayer) {
    Traverse t(repositoryDir.absolutePath() , vistosFile , indexFile);
    PcmPlayer player;

    while (true) {
        std::shared_ptr<Decoder> mp3 = std::make_shared<Mp3Decoder>(repositoryDir.absoluteFilePath(t.getRandom()));
        mp3->skip(2000);
        mp3->setFadeIn(2000);
        player.addStream(mp3);
        sleep(2);
        mp3->stopFadeOut(2000);

    }
}


TEST(MixerTest, DISABLED_BasicMixerTest) {
    std::unique_ptr<Traverse> t;

    qDebug() << QFile(indexFile).remove();
    qDebug() << QFile(vistosFile).remove();
    t.reset(new Traverse(repositoryDir.absolutePath() , vistosFile , indexFile));
    auto n= t->size();
    QMap<QString, int> map;
    const int reps=3;
    for (int rep=0; rep < reps ; rep++) {
        for (int i=0; i < n ; i++) {
            if (i==1) {
                t.reset();
                t.reset(new Traverse(repositoryDir.absolutePath() , vistosFile , indexFile));
            }
            auto f = t->getRandom();
            map[f]++;
        }
    }
    t.reset();
    for (auto e : map.keys()) {
        ASSERT_EQ(reps, map.value(e));

    }
}



TEST(Mixing, Mixing) {
    Mixer m;
    FileSoundSource s0("test.mp3");
    FileSoundSource s1("test.mp3");
//    m.play(s0);
//    m.play(s1);
}


TEST(Mixing, DISABLED_SwitchLine) {
    Mixer m;
    FileSoundSource s0("test.mp3");
    LiveSoundSource s1;

    s0.play();
    sleep(3);
    s1.play();
    sleep(3);
    s0.play();
    sleep(3);

//    m.play(s0);
//    m.play(s1);
}

TEST(DbTest, DbTest) {
    Scheduler s;
}

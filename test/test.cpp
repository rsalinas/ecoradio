#include "gtest/gtest.h"

#include "scheduler.h"
#include "soundsource.h"
#include "file_scan.h"
#include "soundsource.h"
#include "pcm_player.h"

#include <QMap>
#include <QTextStream>
#include <QDir>
#include <QDebug>
#include <QBitArray>

TEST (SchedulerTest, DISABLED_Scheduler0) {
    Scheduler s;
}

static const QDir repositoryDir("/home/rsalinas/Sync/enconstruccio");
static const QString vistosFile = "/tmp/vistos.txt";
static const QString indexFile = "/tmp/all";

TEST(MixerTest, SimplePlayer) {
    Traverse t(repositoryDir.absolutePath() , vistosFile , indexFile);
    t.rescan();
    PcmPlayer player;

    while (true) {
        std::shared_ptr<SoundSource> mp3 = std::make_shared<Mp3Decoder>(repositoryDir.absoluteFilePath(t.getRandom()));
//        mp3->skip(2000);
        mp3->setFadeIn(2000);
        player.addSource(mp3);

//        mp3->stopFadeOut(2000);
        mp3->waitEnd();

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

TEST(DbTest, DbTest) {
    Scheduler s;
}

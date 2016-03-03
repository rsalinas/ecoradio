#include "gtest/gtest.h"

#include "mixer.h"
#include "scheduler.h"
#include "soundsource.h"
#include "file_scan.h"

#include <QMap>
#include <QTextStream>
#include <QDir>
#include <QDebug>
#include <QBitArray>

TEST (SchedulerTest, Scheduler0) {
    Scheduler s;
}


TEST (PlayTest, PlayFile) {
    FileSoundSource s("test.mp3");
    s.play();
}


TEST(MixerTest, BasicMixerTest) {
    std::unique_ptr<Traverse> t;
    const QString dir = "/home/rsalinas/Sync/";
    //= "/tmp/t";
    const QString vistos = "/tmp/vistos.txt";
    const QString index = "/tmp/all";
    qDebug() << QFile(index).remove();
    qDebug() << QFile(vistos).remove();
    t.reset(new Traverse(dir , vistos , index));
    auto n= t->size();
    QMap<QString, int> map;
    const int reps=3;
    for (int rep=0; rep < reps ; rep++) {
        for (int i=0; i < n ; i++) {
            if (i==1) {
                t.reset();
                t.reset(new Traverse(dir , vistos , index));
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

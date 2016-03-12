#include "gtest/gtest.h"

#include "util/rssparser.h"
#include <QDir>
#include "util/streamsrc.h"
#include <QDebug>
#include <QCoreApplication>
#include <QTimer>

void hallo() {
    qDebug() << __FUNCTION__<< 2;
}


TEST(TestSched, TestSched)
{
    QEventLoop loop;
    QDir dir("rss/");
    std::unique_ptr<StreamSrc> ss;
    for (auto f : dir.entryList(QStringList() << "*.xml")) {
        qDebug() << "Processing " << f;
        RssParser rp(dir.absoluteFilePath(f));
        QTimer::singleShot(0, 0, &hallo);

        auto streams = rp.getStreams();
        for (auto url : streams) {
            qDebug() << "  "<< url;
            ss.reset(new StreamSrc (url, nullptr));
//ss.reset(new StreamSrc ("http://www.upv.es"));
            break;
        }
        break;

    }

    qDebug() << "pre loop exec";
    while (loop.exec()) {
        qDebug() << "running events";
    }
    qDebug() << "fin";
//    QCoreApplication::instance()->exec();
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    ::testing::InitGoogleTest(&argc, argv);
    int ret = RUN_ALL_TESTS();

    return app.exec();
}


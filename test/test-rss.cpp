#include "gtest/gtest.h"

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QTextStream>
#include <QTimer>
#include <QBuffer>

#include "snd/sources/mpg123wrap.h"
#include "util/rssparser.h"
#include "util/http.h"

TEST(TestRss, TestRssFromFiles)
{
    QDir dir("rss/");
    auto xmlFiles = dir.entryList(QStringList() << "*.xml");
    ASSERT_NE(0, xmlFiles.size());
    for (auto f : xmlFiles) {
        qDebug() << "Processing " << f;
        RssParser rp(dir.absoluteFilePath(f));
        auto streams = rp.getStreams();
        ASSERT_NE(0, streams.size());
        for (auto url : streams) {
            qDebug() << "  "<< url;
        }
    }
}

TEST(TestRss, HttpTestGood) {
    HttpReader hr(QUrl("http://www.google.com"));
    auto result = hr.readAll();
    ASSERT_NE(0, result.size());
}
TEST(TestRss, HttpTestBad) {
    HttpReader hr(QUrl("http://localhost/MISSING"));
    auto result = hr.readAll();
    ASSERT_EQ(0, result.size());
}

TEST(TestRss, TestRssFromUrl)
{
    QFile list("samples/rss-list.txt");
    ASSERT_TRUE(list.open(QFile::ReadOnly));
    QTextStream s(&list);
    while (!s.atEnd()) {
        QString l = s.readLine();
        if (!l.size())
            continue;
        qDebug() << "LINEA" << l;
        QByteArray ba = (HttpReader((l))).readAll().toLocal8Bit();
        RssParser rp(new QBuffer(&ba));
        auto streams = rp.getStreams();
        qDebug() << streams;
        ASSERT_NE(0, streams.size());
        auto src = std::make_shared<Mpg123>(QUrl(streams.front()));
        char buf[4096];
        int n;
        do {
            QCoreApplication::instance()->processEvents();
            n = src->readPcm(buf, sizeof(buf));
            qDebug() << n;
            usleep(500*1000);
        } while (n == 0);

        break;
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

#include "test-rss.moc"

#include "gtest/gtest.h"

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QTextStream>
#include <QTimer>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>

#include "snd/sources/mpg123wrap.h"
#include "util/rssparser.h"

class HttpReader {
public:
    QString readAll(const QUrl &url) {
        QNetworkAccessManager NAManager;
        QNetworkRequest request(url);
        QNetworkReply *reply = NAManager.get(request);
        QEventLoop eventLoop;
//        QObject::connect(reply, SIGNAL()
//        QObject::connect(reply, SIGNAL(finished()), &eventLoop, SLOT(quit()));
        eventLoop.exec();
        std::cout << "finished" << std::endl; //request finished here
    }
};


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
QString getUrl(QUrl url) {
//    abort();
    return "";
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
        RssParser rp(getUrl(QUrl(l)));
        auto streams = rp.getStreams();
        qDebug() << streams;
//        auto src = std::make_shared<Mpg123>(l);



    }
}


int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    ::testing::InitGoogleTest(&argc, argv);
    int ret = RUN_ALL_TESTS();

    return app.exec();
}



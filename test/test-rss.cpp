#include "gtest/gtest.h"

#include "rssparser.h"
#include <QDir>

#include <QDebug>
TEST(TestSched, TestSched)
{
    QDir dir("rss/");
    for (auto f : dir.entryList(QStringList() << "*.xml")) {
        qDebug() << "Processing " << f;
        RssParser rp(dir.absoluteFilePath(f));
        auto streams = rp.getStreams();
//        break;
    }
}

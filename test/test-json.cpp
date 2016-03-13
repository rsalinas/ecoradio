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
#include "common/program.h"
#include "core/scheduler.h"
#include <QJsonDocument>
#include <QJsonObject>


TEST(TestJson, ProgramJson)
{
    Scheduler sched("radio.sqlite");
    auto current = sched.getCurrent();
    auto next = sched.getNext();
    QByteArray s;
    {
        next.push_back(current);
        next.push_back(current);
        QJsonDocument doc;

        QJsonObject o;
        o = toJson(current, next);
        o.insert("cmd", "programStatus");
        doc.setObject(o);
        s = doc.toJson();
        qDebug().noquote() << s;
    }
    {
        auto doc = QJsonDocument::fromJson(s);
        qDebug() << doc;
        auto cmd = doc.object()["cmd"].toString();
        qDebug() << "CMD was" << cmd;
        auto readp = programFromJson(doc.object()["current"].toObject());
        qDebug() << "current: " << *readp;
        ASSERT_EQ(*current, *readp);
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

#include "test-json.moc"
